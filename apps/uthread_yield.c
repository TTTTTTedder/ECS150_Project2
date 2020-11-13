/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <uthread.h>

void Preempt_test1(void *arg)
{
	sleep(10);
    printf("Print 1st\n");
    return;
}

void Preempt_test2(void *arg)
{
    uthread_create(Preempt_test1, NULL);

    printf("Print 2nd\n");
    uthread_yield();
    return;
}

void preempt_test(void)
{
    uthread_start(Preempt_test2,NULL);
}

int main(void)
{
	preempt_test();
	return 0;
}