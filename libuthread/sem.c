#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"
#include "uthread.h"

#define ERROR -1;
#define SUCCESS 0;
struct semaphore {
	size_t count; // maximum internal count
	queue_t waitingOnSemaphore; // holds all threads waiting on this semaphore
};

sem_t sem_create(size_t count) {
	struct semaphore* sem = malloc(sizeof(count)); // memory allocate
	if(sem == NULL) { // if error
		return NULL;
	}
	sem->count = count; // define the origin semaphore count
	sem->waiting = 0;
	sem->waitingOnSemaphore = queue_create();
	return sem;
}

int sem_destroy(sem_t sem) {
	if(sem == NULL || queue_length(sem->waitingOnSemaphore) != 0){ // if error
		return ERROR;
	}
	queue_destroy(sem->waitingOnSemaphore);
	free(sem); // deallocate
	return SUCCESS; // success
}

int sem_down(sem_t sem) {
	// idea: when sem->count == 0, block the thread
	// kind of running uthread_yield() and run the next thread

	if(sem == NULL){ // if error
		return ERROR;
	}
	preempt_disable();

	while(sem->count == 0) {
		struct uthread_tcb* current_thread = uthread_current();
		queue_enqueue(sem->waitingOnSemaphore, current_thread);
		uthread_block();
	}
	sem->count --;

	preempt_enable();

	return SUCCESS;
}

int sem_up(sem_t sem) {
	if(sem == NULL){ // if error
		return ERROR;
	}
	preempt_disable();
	sem->count++;
	if (queue_length(sem->waitingOnSemaphore) != 0) {
		struct uthread_tcb* unblock_thread;
		queue_dequeue(sem->waitingOnSemaphore, (void**)&unblock_thread);
		uthread_unblock(unblock_thread);
	}
	preempt_enable();

	return SUCCESS;
}
