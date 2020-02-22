
#ifndef RKIT_H
#define RKIT_H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define DEBUG 1

#ifdef DEBUG
#define debug(str, ...) 					        \
printk(KERN_DEBUG str, ##__VA_ARGS__);
#else
#define debug(str, ...)
#endif


MODULE_LICENSE("GPL");

#endif
