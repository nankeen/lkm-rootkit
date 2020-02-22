#include "cmd.h"

static int parse_argv(char *cmd, char ***dest) {
  int argc = 0;
  char *ptr, *token;
  ptr = cmd;
  printk(KERN_INFO "Parsing arguments for %s\n", cmd);
  // TODO: Implement support for quotes
  while ((token = strsep(&ptr, " ")) != NULL) {
    printk(KERN_INFO "Token: %s\n", token);
    (*dest)[argc] = token;
    argc++;
  }
  printk(KERN_INFO "Argc: %d\n", argc);
  return argc;
}

static void cmd_userspace_shell(char *cmd, size_t n)
{
  char **argv = kmalloc(sizeof(char *) * n, GFP_USER);
  printk(KERN_INFO "Running shell command\n");
  parse_argv(cmd + strlen(CMD_SHELL) + 1, &argv);
  printk(KERN_INFO "Parsing successful\n");
  call_usermodehelper(argv[0], argv, NULL, UMH_NO_WAIT);
  printk(KERN_INFO "Ran command\n");
  kfree(argv);
}

int handle_cmd(char *cmd, size_t len)
{
  if (strncmp(cmd, CMD_SHELL, strlen(CMD_SHELL)) == 0) {
    cmd_userspace_shell(cmd, len);
  }
  return 0;
}
