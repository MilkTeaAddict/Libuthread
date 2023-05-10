#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* TODO Phase 3 */
	// Number of resources 
	int count;
	// queue for waiting threads 
	queue_t blockedThreads;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	struct semaphore *s = malloc(sizeof(struct semaphore));

	if (s == NULL) {
        return NULL;
    }


	s->count = count;
	s->blockedThreads = queue_create();


	return s;
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
	if (sem == NULL || queue_length(sem->blockedThreads) != 0) {
        return -1;
    }

    free(sem);
    return 0;
}

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
	if(sem == NULL) {
		return -1;
	}
	// block current running thread if count drops to 0 and add it to the queue of blocked threads
    preempt_disable();
    
	if(sem->count == 0){
		uthread_block();
		struct uthread_tcb *askingThread = uthread_current();
		queue_enqueue(sem->blockedThreads, (void*) askingThread);
		uthread_yield();
	} else {
		sem->count--;
	}

    preempt_enable();

	return 0;
}

int sem_up(sem_t sem)
{
	//When a thread releases a semaphore which count was 0, it checks whether some other threads were currently waiting on it. 
	//In such case, the first thread of the waiting list can be unblocked, and be eligible to run later.

	if(sem == NULL) {
		return -1;
	} 

	/* TODO Phase 3 */

    preempt_disable();
    
	if(sem->count == 0 && queue_length(sem->blockedThreads) != 0){
		struct uthread_tcb *firstThreadInQueue;

		/* Grabs first thread in blocked threads if not empty*/
		queue_dequeue(sem->blockedThreads, (void**) &firstThreadInQueue);
		uthread_unblock(firstThreadInQueue);
	}
	else {
		sem->count++;
	}

    preempt_enable();

	return 0;
}