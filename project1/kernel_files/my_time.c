//333

#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/timer.h>

asmlinkage long sys_mytime(void) {
    static const long constant = 1000000000;
    struct timespec t;
    getnstimeofday(&t);
    return t.tv_sec * constant + t.tv_nsec;
}
