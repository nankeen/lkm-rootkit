#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include "rootkit.h"

static int __init rkit_init(void)
{
  printk(KERN_INFO "Hello world!\n");
  // TODO: Start backdoor server
  // TODO: Hide backdoor server
  // TODO: Enable privesc service
  return 0;
}

static void __exit rkit_exit(void)
{
  printk(KERN_INFO "Goodbye world\n");
}

module_init(rkit_init);
module_exit(rkit_exit);
