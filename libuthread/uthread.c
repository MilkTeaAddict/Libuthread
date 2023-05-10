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

//#define UTHREAD_STACK_SIZE 32768

int IDLE = 0;
int READY = 1;
int RUNNING = 2;   //  maybe remove later
int BLOCKED = 3;
int EXITED = 4;

queue_t ready_queue;
queue_t exit_queue;


struct uthread_tcb {
	int state;
	uthread_func_t func;
	void *arg;
	uthread_ctx_t *ctx;
	void *stack;
	int tid;
};

struct uthread_tcb *runningThread;
struct uthread_tcb *idleThread;     // mgiht not need to be global, could remvoe late

struct uthread_tcb *uthread_current(void)
{
	return runningThread;

}

void uthread_yield(void)
{
	//printf("yield\n");
	/* TODO Phase 2 */

   // preempt_disable();
    
	struct uthread_tcb *previousThread = uthread_current();
	if(previousThread->state == READY){
		queue_enqueue(ready_queue, (void*) previousThread);
	}
	else if(previousThread->state == EXITED){

		queue_enqueue(exit_queue, (void*) previousThread);
	}
	
	
  	struct uthread_tcb *nextThread;

  	queue_dequeue(ready_queue, (void**)&runningThread);

  	nextThread = uthread_current();

	if(queue_length(ready_queue) == 0){
		uthread_ctx_switch(previousThread->ctx, idleThread->ctx);
	}

  	uthread_ctx_switch(previousThread->ctx, nextThread->ctx);

   // preempt_enable();
}

void uthread_exit(void)
{
	//printf("exit\n");
	/* TODO Phase 2 */
	runningThread->state = EXITED;
	uthread_yield();
}

int uthread_create(uthread_func_t func, void *arg)
{
	//printf("create\n");
	// create new thread
	struct uthread_tcb *new_thread = (struct uthread_tcb*) malloc(sizeof(struct uthread_tcb));
	new_thread->state = READY;
	new_thread->stack = uthread_ctx_alloc_stack();
	new_thread->ctx = malloc(sizeof(uthread_ctx_t));
	int newCtxStatus = uthread_ctx_init(new_thread->ctx, new_thread->stack, func, arg);

	/*Fails if initialization of ctx goes wrong*/ 
	if (newCtxStatus != 0 ) {
		return -1;
	}

	preempt_disable();
    
	queue_enqueue(ready_queue, (void*) new_thread);
    
    preempt_enable();
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
    if (preempt) {
        preempt_start(preempt);
    }
	
	// create two global queues to store all ready threads and threads that have exited
	ready_queue = queue_create();
	exit_queue = queue_create();

    
	/* Create IDLE thread to come back to and keep executing */
	idleThread = (struct uthread_tcb*) malloc(sizeof(struct uthread_tcb));
	idleThread->state = READY;
	idleThread->ctx = malloc(sizeof(uthread_ctx_t));
	idleThread->stack = uthread_ctx_alloc_stack(); // maybe not be needed
	
	/*Fails if initialization of ctx goes wrong*/
	int idleCtxStatus = uthread_ctx_init(idleThread->ctx, idleThread->stack, NULL, NULL);
	if(idleCtxStatus != 0) {
		return -1;
	}

	/* Create First Thread */
	uthread_create(func, arg);
	runningThread = idleThread;
	//printf("queu len: %d\n", queue_length(ready_queue));

	/* Queue to run through all threads */
	while(queue_length(ready_queue) > 0) {
		//printf("Waiting for thread\n");
		uthread_yield();
	}

	if (preempt) {
		preempt_stop();
	}

	return 0;

}

void uthread_block(void)
{
 	/* TODO Phase 3 */
	struct uthread_tcb *askingThread = uthread_current();
	askingThread->state = BLOCKED;

}

void uthread_unblock(struct uthread_tcb *uthread)
{

// 	/* TODO Phase 3 */
	uthread->state = READY;
	queue_enqueue(ready_queue, (void*) uthread);
}

