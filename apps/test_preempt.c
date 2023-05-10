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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
#include <private.h>


void thread3(void *arg)
{
	(void)arg;

	uthread_yield();
	printf("thread3\n");
}

void thread2(void *arg)
{
	(void)arg;

	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread2\n");
}

void thread1(void *arg)
{
	(void)arg;
    //printf("1");
	uthread_create(thread2, NULL);
   // printf("2");
	uthread_yield();
   // printf("3");
	printf("thread1\n");
    //printf("4");
	uthread_yield();
    //printf("5");
}

int main(void)
{
    //for (int i = 0; i < 5; i++) {
        
    uthread_run(true, thread1, NULL);

    
	return 0;
}
