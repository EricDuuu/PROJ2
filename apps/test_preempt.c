#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/uthread.h"
#include <uthread.h>

void thread2(void *arg) {
  (void)arg;

  printf("thread2\n");
}

void thread1(void *arg) {
  (void)arg;

  uthread_create(thread2, NULL);
  uthread_yield();
  while (1) {
  }
  printf("thread1\n");
  uthread_yield();
}

int main(void) {
  uthread_run(1, thread1, NULL);
  return 0;
}