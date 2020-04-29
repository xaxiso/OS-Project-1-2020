#define _GNU_SOURCE
#include "process.h"
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define NOTREADY -1

long int decideProc(int flag, int pid)
{
	struct sched_param param;
	int current;
	
	param.sched_priority = 0;

	if(flag)
		current = sched_setscheduler(pid, SCHED_IDLE, &param);
	else
		current = sched_setscheduler(pid, SCHED_OTHER, &param);

	if (current < 0) {
		perror("sched_setscheduler");
		return NOTREADY;
	}

	return current;
}

long int execProc(struct process proc)
{
	int pid = fork();

	pid_t currentpid = getpid();

	if (pid == -1) {
		perror("fork");
		return -1;
	}

	if (!pid) {
		long start = syscall(333);

		for (int i = 0; i < proc.execTime; i++) {
			UNIT_T();
		}

		long end = syscall(333);
		syscall(334, currentpid, start, end);
		exit(0);
	}
	
	assignProc(pid, CHILD_CPU);

	return pid;
}

long int assignProc(int pid, int core)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
		
	if (core > sizeof(cpu_set_t)) {
		fprintf(stderr, "Core index error.");
		return NOTREADY;
	}

	if (sched_setaffinity(pid, sizeof(mask), &mask) == -1) {
		fprintf(stderr, "setaffinity ERROR\n");
		exit(1);
	}

	return 0;
}