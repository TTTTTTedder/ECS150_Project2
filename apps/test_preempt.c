// The idea here is that we create an initial thread that creates a second thread
// The initial thread has an infinite while loop that tries to keep control of the CPU forever
// But because of preemption, thread 2 gets a chance to run and completes its execution

#include <stdio.h>
#include <stdlib.h>
#include <sem.h>
#include <uthread.h>

static void thread2(void *arg)
{
	printf("thread 2 finished executing...\n");
}

static void thread1(void *arg)
{
	uthread_create(thread2, NULL);
	while (1) {
        // Keeps doing the while loop... tries not to give up control of CPU
    }
}

int main(void)
{
	uthread_start(thread1, NULL);
	return 0;
}
