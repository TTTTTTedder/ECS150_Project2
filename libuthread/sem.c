#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"
#include "uthread.h" // Added by Zhengyu

#define ERROR -1;
#define SUCCESS 0;
struct semaphore {
	size_t count; // maximum internal count
	int waiting; // 1 if threads are blocked, 0 if no more thread waiting
	// struct uthread_tcb* uthread; // *** Not sure if we can add uthread like this ***
	queue_t waitingOnSemaphore;
};

// current_thread = uthread_current();
// queue_enqueue(sem->waiting, current_thread);

sem_t sem_create(size_t count)
{
	struct semaphore* sem = malloc(sizeof(count)); // memory allocate
	if(sem == NULL){ // if error
		return NULL;
	}
	sem->count = count; // define the origin semaphore count
	sem->waiting = 0;
	sem->waitingOnSemaphore = queue_create();
	return sem;
}

int sem_destroy(sem_t sem)
{
	if(sem == NULL || sem->waiting != 0){ // if error
		return ERROR;
	}
	free(sem); // deallocate
	return SUCCESS; // success
}

int sem_down(sem_t sem)
{
	struct uthread_tcb* current_thread = uthread_current();
	if(sem == NULL){ // if error
		return ERROR;
	}

	// idea: when sem->count = 0, block the thread
	// kind of running uthread_yield() and run the next thread
	
	// question: we are decreasing the semaphore here
	// but not sure if we should call uthread_block() here
	if(sem->count > 0){
		sem->count--;
	} else if(sem->count == 0){
		queue_enqueue(sem->waitingOnSemaphore, current_thread);
		uthread_block();
	}
	return SUCCESS;
}

int sem_up(sem_t sem)
{
	if(sem == NULL){ // if error
		return ERROR;
	}
	sem->count++;
	if (queue_length(sem->waitingOnSemaphore) != 0) {
		struct uthread_tcb* unblock_thread;
		queue_dequeue(sem->waitingOnSemaphore, (void**)&unblock_thread);
		uthread_unblock(unblock_thread);
	}
	// idea: increment by one, and wake up one of the waiting threads if any

	// using uthread_unblock() here?

	// question: not sure if it is the correct way to define a uthread 
	// in a semaphore sturcture.
	// question: as we are using a queue to store the order of threads
	// which means each node in the queue is a thread, then are we 
	// still able to include the uthread pointer at the beginning of this
	// file?

	// uthread_unblock();
	return SUCCESS;
}