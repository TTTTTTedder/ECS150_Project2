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
#define microseconds 1000000
 
// global variable so that preempt_start and preempt_stop can access it
struct sigaction new_action, old_action;
struct itimerval old_timer, new_timer;

void handler(int signum){
	// handling somethin
	if (signum == SIGVTALRM) {
		uthread_yield();
	}
	
	return;
} 


void preempt_disable(void)
{
	sigset_t block_alarm;
	sigemptyset(&block_alarm);
	sigaddset(&block_alarm, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &block_alarm, NULL);
}

void preempt_enable(void)
{
	sigset_t unblock_alarm;
	sigemptyset(&unblock_alarm);
	sigaddset(&unblock_alarm, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &unblock_alarm, NULL);
}
/*
 * preempt_start - Start thread preemption
 *
 * Configure a timer that must fire a virtual alarm at a frequency of 100 Hz and
 * setup a timer handler that forcefully yields the currently running thread.
 */
void preempt_start(void)
{
	// Idea:
	// Every 100 Hz (by alarm), write a handler that calls uthread_yield

	
	new_action.sa_handler = handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	if (sigaction(SIGVTALRM, &new_action, &old_action) == -1) {
		perror("sigaction");
		exit(1);
	}

	// We want to have a timer interrupt every 100Hz
	// 100 Hz = 0.01 seconds (T = 1/f)
	// "usec" uses microseconds
	// 0.01 seconds = 10,000 microseconds
	// There are 1000000 microseconds in a second
	// to get 0.01 seconds, divide 1000000/100 (microseconds / HZ)

	
	new_timer.it_interval.tv_usec = microseconds / HZ;
  	new_timer.it_interval.tv_sec = 0; // Don't have whole number of seconds
  	new_timer.it_value.tv_usec = microseconds / HZ;
  	new_timer.it_value.tv_sec = 0; // Don't have whole number of seconds

	if (setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer) == -1) {
		perror("setitimer");
		exit(1);
	}

}
/*
 * preempt_stop - Stop thread preemption
 *
 * Restore previous timer configuration, and previous action associated to
 * virtual alarm signals.
 */
void preempt_stop(void)
{
	if (sigaction(SIGVTALRM, &old_action, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	if (setitimer(ITIMER_VIRTUAL, &old_timer, NULL) == -1) {
		perror("setitimer");
		exit(1);
	}
}