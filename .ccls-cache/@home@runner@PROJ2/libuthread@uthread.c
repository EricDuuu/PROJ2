#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"
#include "uthread.h"

#define EXEC_AND_HANDLE(f, r, ...)                                             \
  do {                                                                         \
    if (f(__VA_ARGS__) != r) {                                                 \
      fprintf(stderr, "function: " #f "() failed in %s\n", __FILE__);          \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

typedef enum { READY, RUNNING, BLOCKED, DEAD } state;
/* typedef void (*uthread_func_t)(void *arg) */

struct uthread_tcb {
  int status;
  ucontext_t *context;
  void *stack;
  int t_id;
};

/* global queues */
static queue_t ready_processes;
static struct uthread_tcb *idle;
static struct uthread_tcb *currT;

int t_id = 0;

struct uthread_tcb *uthread_current(void) {
  return currT;
}

void uthread_yield(void) {
  preempt_disable();
  struct uthread_tcb *oldThread = uthread_current();
  struct uthread_tcb *newThread;

  if (oldThread->status == RUNNING)
    oldThread->status = READY;

  queue_dequeue(ready_processes, (void **)&newThread);

  /* Once the thread is done, it automatically exits */
  if (oldThread->status == READY)
    queue_enqueue(ready_processes, oldThread);

  currT = newThread;
  newThread->status = RUNNING;
  uthread_ctx_switch(oldThread->context, newThread->context);
  preempt_enable();
}

/* Called in uthread_ctx_bootstrap to execute thread then exit safely */
void uthread_exit(void) {
  /* Deallocate memory and move onto next thread by uthread_yield() */
  struct uthread_tcb *current = uthread_current();
  current->status = DEAD;
  free(current->context);
  free(current->stack);
  uthread_yield();
}

/* Allocate a new thread and it's members */
int uthread_create(uthread_func_t func, void *arg) {
  struct uthread_tcb *newT = malloc(sizeof(struct uthread_tcb));
  newT->status = READY;
  newT->stack = uthread_ctx_alloc_stack();
  newT->context = malloc(sizeof(ucontext_t));
  newT->t_id = t_id++;

  preempt_disable();

  uthread_ctx_init(newT->context, newT->stack, func, arg);
  queue_enqueue(ready_processes, newT);

  preempt_enable();
  return 0;
}

/*
 * This is the first function called for threads.
 * It takes in a option for preemption, function to
 * be run by the thread and a pointer to the arguments
 * of the function. An idle thread is intialized and
 * while more threads are ready to be run, we loop.
 * Finally, we return once all threads complete.
 */
int uthread_run(bool preempt, uthread_func_t func, void *arg) {
  if (preempt)
    preempt_start(preempt);

  // Creates the idle thread to later schedule for execution
  idle = malloc(sizeof(struct uthread_tcb));
  idle->context = malloc(sizeof(ucontext_t));
  idle->status = RUNNING;
  idle->t_id = t_id++;
  currT = malloc(sizeof(struct uthread_tcb));
  currT = idle;

  // initialize the queue and create our initial thread
  ready_processes = queue_create();
  uthread_create(func, arg);

  // update appropriate statuses
  idle->status = RUNNING;

  // while loop until the there are no more processes
  while (queue_length(ready_processes) > 0)
    uthread_yield();

  queue_destroy(ready_processes);

  return 0;
}

/* Used to block threads in queue implemented in semaphor */
void uthread_block(void) {
  currT->status = BLOCKED;
  uthread_yield();
}

/* Used to unblock threads in queue implemented in semaphor */
void uthread_unblock(struct uthread_tcb *uthread) {
  uthread->status = READY;
  queue_enqueue(ready_processes, uthread);
}
