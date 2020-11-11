#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

void handler(int signum){
	// handling somethin
	switch(signum){
		case SIGALRM:
			printf("sigalrm\n");
			break;
		case SIGVTALRM:
			printf("sigvtalrm\n");
			break;
	}
	return;
} 


void preempt_disable(void)
{
	/* TODO Phase 4 */
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
}
/*
 * preempt_start - Start thread preemption
 *
 * Configure a timer that must fire a virtual alarm at a frequency of 100 Hz and
 * setup a timer handler that forcefully yields the currently running thread.
 */
void preempt_start(void)
{
	/* TODO Phase 4 */
	signal(SIGVTALRM, handler);

}
/*
 * preempt_stop - Stop thread preemption
 *
 * Restore previous timer configuration, and previous action associated to
 * virtual alarm signals.
 */
void preempt_stop(void)
{
	/* TODO Phase 4 */
}

