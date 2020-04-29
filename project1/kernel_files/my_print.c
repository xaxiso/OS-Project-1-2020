//334

#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_myprint(int pid, long start, long end) {
    static const long constant = 1000000000;
    printk(KERN_INFO "[Project1] %d %ld.%09ld %ld.%09ld", pid, start/constant, start%constant, end/constant, end%constant);
}
