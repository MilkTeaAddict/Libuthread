/*
 * Thread creation with preempt enabled
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <uthread.h>

void thread3(void *arg) {
  (void)arg;
  while (1) {
    printf("thread3\n");
    sleep(1);
  }
}

void thread2(void *arg) {
  (void)arg;

  uthread_create(thread3, NULL);

  while (1) {
    printf("thread2\n");
    sleep(1);
  }
}

void thread1(void *arg) {
  (void)arg;

  uthread_create(thread2, NULL);
  while (1) {
    printf("thread1\n");
    sleep(1);
  }
}

int main(void) {
  uthread_run(true, thread1, NULL);
  return 0;
}
