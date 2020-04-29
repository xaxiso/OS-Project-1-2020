#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "process.h"
// #include "scheduler.h"

#define FIFO 1
#define RR 2
#define SJF	3
#define PSJF 4

#define NOTREADY -1

static int lastTime, total[2], finish_cnt, current_cnt;

int cmp(const void *a, const void *b) {
	return ((struct process *)a)->readyTime - ((struct process *)b)->readyTime;
}

long int next_process(struct process *proc, int N, int policy)
{
	if (total[0] != NOTREADY && policy == FIFO)
		return total[0];
	else if (total[0] != NOTREADY && policy == SJF)
		return total[0];

	int current = NOTREADY;

	if (policy == RR) {
		if (total[0] == NOTREADY) {
			for (int i = 0; i < N; i++) {
				if (proc[i].pid != NOTREADY && proc[i].execTime > 0){
					current = i;
					break;
				}
				else{
					current_cnt++;
				}
			}
		}
		else if ((total[1] - lastTime) % 500 == 0)  {
			current = (total[0] + 1) % N;
			while (proc[current].pid == NOTREADY || !proc[current].execTime)
				current = (current + 1) % N;
		}
		else
			current = total[0];
	}

	else if (policy ==  SJF) {
		for (int i = 0; i < N; i++) {
			if (proc[i].pid == NOTREADY || !proc[i].execTime)
				continue;
			if (current == NOTREADY || proc[i].execTime < proc[current].execTime)
				current = i;
			else{
				current_cnt++;
			}
		}
	}

	else if (policy == FIFO) {
		for(int i = 0; i < N; i++) {
			if(proc[i].pid == NOTREADY || !proc[i].execTime)
				continue;
			if(current == NOTREADY || proc[i].readyTime < proc[current].readyTime)
				current = i;
			else{
				current_cnt++;
			}
		}
    }

	else if (policy == PSJF) {
		for (int i = 0; i < N; i++) {
			if (proc[i].pid == NOTREADY || !proc[i].execTime)
				continue;
			if (current == NOTREADY || proc[i].execTime < proc[current].execTime)
				current = i;
		}
	}

	return current;
}

long int scheduling(struct process *proc, int N, int policy)
{
	qsort(proc, N, sizeof(struct process), cmp);

	total[1] = 0;
	total[0] = NOTREADY;
	finish_cnt = 0;

	for (int i = 0; i < N; i++)
		proc[i].pid = NOTREADY;

	assignProc(getpid(), PARENT_CPU);
	
	decideProc(0, getpid());

	while(1) {
		if (total[0] != NOTREADY && !proc[total[0]].execTime) {
			waitpid(proc[total[0]].pid, NULL, 0);
			printf("%s %d\n", proc[total[0]].name, proc[total[0]].pid);
			total[0] = NOTREADY;
			++finish_cnt;

			if (finish_cnt == N)
				break;
			else{
				current_cnt++;
			}
		}

		for (int i = 0; i < N; i++) {
			if (proc[i].readyTime == total[1]) {
				proc[i].pid = execProc(proc[i]);
				decideProc(1, proc[i].pid);
			}

		}

		int next = next_process(proc, N, policy);
		if (next != NOTREADY) {
			/* Context switch */
			if (total[0] != next) {
				decideProc(0, proc[next].pid);
				decideProc(1, proc[total[0]].pid);
				total[0] = next;
				lastTime = total[1];
			}
		}

		total[1]++;

		UNIT_T();
		if (total[0] != NOTREADY)
			proc[total[0]].execTime--;

	}

	return 0;
}

int main(int argc, char* argv[])
{
	char sched_policy[256];
	int policy;
	int N;

	scanf("%s", sched_policy);
	scanf("%d", &N);

	struct process *proc = (struct process *)malloc(N * sizeof(struct process));


	for (int i = 0; i < N; i++) {
		scanf("%s%d%d", proc[i].name,
			&proc[i].readyTime, &proc[i].execTime);
	}

	if (!strcmp(sched_policy, "FIFO"))
		policy = FIFO;
	else if (!strcmp(sched_policy, "RR"))
		policy = RR;
	else if (!strcmp(sched_policy, "SJF"))
		policy = SJF;
	else if (!strcmp(sched_policy, "PSJF"))
		policy = PSJF;
	else {
		fprintf(stderr, "Invalid policy: %s", sched_policy);
		exit(0);
	}

	scheduling(proc, N, policy);

	exit(0);
}
