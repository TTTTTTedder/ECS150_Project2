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

// int blocked = 0;
struct uthread_tcb {
    uthread_ctx_t ctx; // context for the thread. We will set context during thread creation
    char* stack; // We will initialize this during thread creation
};

queue_t readyThreads; // Keeps track of all ready threads (not blocked)

struct uthread_tcb* current_thread;

struct uthread_tcb *uthread_current(void) {
	return current_thread;
}

void uthread_yield(void) {
    preempt_disable();

    struct uthread_tcb* prev_thread = current_thread;
    queue_enqueue(readyThreads, current_thread);
    queue_dequeue(readyThreads, (void**)&current_thread); // Get next thread into current_thread
    struct uthread_tcb* next_thread = current_thread; // Assign next thread as dequeue'd thread
    uthread_ctx_switch(&(prev_thread->ctx), &(next_thread->ctx));
}

void uthread_exit(void) {
    preempt_disable();
    uthread_ctx_destroy_stack(current_thread->stack);
    struct uthread_tcb* prev_thread = current_thread;
    queue_dequeue(readyThreads, (void**)&current_thread);
    struct uthread_tcb* next_thread = current_thread;
    uthread_ctx_switch(&(prev_thread->ctx), &(next_thread->ctx));
}

int uthread_create(uthread_func_t func, void *arg) {
    // First, create a TCB for thread. Set state to ready, initialize stack, initialize context

	struct uthread_tcb* thread = malloc(sizeof(struct uthread_tcb));

    if (thread == NULL) {
        return -1;
    }

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

int uthread_start(uthread_func_t func, void *arg) {
    /* IDEA:
     * Create a thread
     * switch context to this thread
     * then, this thread is run
     * in the infinite loop, call uthread_yield(), so it looks for other runnable threads
     * after the infinite loop, call uthread_exit()
     */

    preempt_start();

	// queue_t threads was declared as a global variable. Initialize it here
    readyThreads = queue_create();

	// Assign everything that a thread needs
    // Create an "idle" thread
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

void uthread_block(void) {
    preempt_disable();
    // blocked = 1;
    struct uthread_tcb* prev_thread = current_thread;
    queue_dequeue(readyThreads, (void**)&current_thread); // Get next thread into current_thread
    struct uthread_tcb* next_thread = current_thread; // Assign next thread as dequeue'd thread
    uthread_ctx_switch(&(prev_thread->ctx), &(next_thread->ctx));
}

void uthread_unblock(struct uthread_tcb *uthread) {
    preempt_disable();
    queue_enqueue(readyThreads, uthread);
    uthread_yield();
    preempt_enable();
}
