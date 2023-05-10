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
sigset_t ss;

void alarm_handler(int signum){
    (void) signum; 
	uthread_yield();
}

void preempt_disable(void)
{
	/* Ignore SIGVTALRM */
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &ss, NULL);

}

void preempt_enable(void)
{
	/* ENABLE SIGVTALRM */
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void preempt_start(bool preempt)
{
    (void) preempt;
	/* TODO Phase 4 */
	/* Set up handler for alarm */
	struct sigaction sa;
	sa.sa_handler = &alarm_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGVTALRM, &sa, NULL);

		
	/* Configure the timer to fire an alarm 100 times per second */
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = HZ * HZ;  // 10000 microseconds = 0.01 seconds
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = HZ * HZ;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

void preempt_stop(void)
{
	/* TODO Phase 4 */

	/* Restore the previous action for SIGVTALRM */
	struct sigaction sa;
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGVTALRM, &sa, NULL);

		
	/* Configure the timer to fire an alarm 100 times per second */
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;  // 10000 microseconds = 0.01 seconds
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}