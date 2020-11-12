#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

#define IDLE 0
#define RUNNING 1
#define READY 2
#define EXITED 3

int blocked = 0;

struct uthread_tcb {
	int state; // states are described above
    uthread_ctx_t ctx; // context for the thread. We will set context during thread creation
    char* stack; // We will initialize this during thread creation
};

// queue_t threads_MAIN;
queue_t readyThreads;
queue_t waitingThreads;

struct uthread_tcb* current_thread;

struct uthread_tcb *uthread_current(void)
{
	return current_thread;
}

void uthread_yield(void)
{
    preempt_disable();

    struct uthread_tcb* prev_thread = current_thread;
    if (blocked == 0) {
        queue_enqueue(readyThreads, current_thread);
    }
    blocked = 0;
    queue_dequeue(readyThreads, (void**)&current_thread);
    struct uthread_tcb* next_thread = current_thread;
    uthread_ctx_switch(&(prev_thread->ctx), &(next_thread->ctx));

    preempt_enable();
}

void uthread_exit(void)
{
	free(current_thread);
	uthread_yield();
}

int uthread_create(uthread_func_t func, void *arg)
{
    // First, create a TCB for thread. Set state to ready, initialize stack, initialize context
    
	struct uthread_tcb* thread = malloc(sizeof(struct uthread_tcb));

    if (thread == NULL) {
        return -1;
    }

    thread->state = RUNNING;
    thread->stack = (char*)uthread_ctx_alloc_stack();
    preempt_disable();
	if (uthread_ctx_init(&(thread->ctx), thread->stack, func, arg) == -1) { // type conversion because function returns void*
        return -1;
    }
    preempt_enable();
    // Now that the thread is set up and is ready to run, we can push it into the queue
    preempt_disable();
    queue_enqueue(readyThreads, thread);
    preempt_enable();
    return 0;
}

int uthread_start(uthread_func_t func, void *arg)
{
    /* IDEA:
     * Create a thread
     * switch context to this thread
     * then, this thread is run
     * in the infinite loop, call uthread_yield(), so it looks for other runnable threads
     * after the infinite loop, call uthread_exit()
     */
    // queue_t threads was declared as a global variable. Initialize it here

    preempt_start();

    readyThreads = queue_create();
    waitingThreads = queue_create();

	// Assign everything that a thread needs
    // Create an "idle" thread

    // Create idle thread
    struct uthread_tcb* idle_thread = malloc(sizeof(struct uthread_tcb));
    current_thread = idle_thread;

    uthread_create(func, arg);

    // Infinite loop until no more threads are ready to run in the system
    while (queue_length(readyThreads) != 0) {
        uthread_yield();
    }

    preempt_stop();
	return 0;
}

void uthread_block(void)
{   
    preempt_disable();
    blocked = 1;
	uthread_yield();
    preempt_enable();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
    preempt_disable();
    queue_enqueue(readyThreads, uthread);
    uthread_yield();
    preempt_enable();
}

// 1 2 3 4 5 6 7
// target 5
// 2 3 4 5 6 7 | temp =1
// 3 4 5 6 7 | temmp = 1 2
// ...
// 5 6 7 | temp = 1 2 3 4
// take out 5
// 6 7 |temp =1 2 3 4 
// temp = 1 2 3 4 6 7
// waiting.dequeue(elements left)
// queue.delete(waitingThread)
// queue create (waiting thread)
// waitingThread = temp
// O(n)