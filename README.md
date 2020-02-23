# Rookit Kernel Module

Rootkit kernel module to be inserted as a backdoor.
Tested on Linux kernel version 4.5.

## TODO

- [x] Network backdoor
- [ ] Spawn reverse shell
- [ ] Priviledge Escalation
- [ ] Hidden port
- [ ] Hidden process
- [ ] Module persistence and stealth

## Debugging Environment

The kernel used to test this project is built with [buildroot](https://www.buildroot.org/).
Configure `BUILDROOT` in `Makefile` to point to the correct directory.

Set `KDIR` to the appropriate kernel version too.

If you wish to compile this for the system you are running, use the commented path instead.

## Compiling

Requires Linux headers, for Ubuntu use.

```
$ sudo apt install linux-header-`uname -r`
```

Compile with

```
$ make
```
