#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

#define CHILD_CPU 1
#define PARENT_CPU 0

/* Running one unit time */
#define UNIT_T() { volatile unsigned long i; for(i=0;i<1000000UL;i++); } 

struct process {
	char name[32];
	int readyTime;
	int execTime;
	pid_t pid;
};

long int assignProc(int pid, int core);

long int execProc(struct process proc);

long int decideProc(int flag, int pid);

#endif
