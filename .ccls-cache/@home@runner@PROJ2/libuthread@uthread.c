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

typedef enum { READY, RUNNING, BLOCKED, DEAD } state;
/* typedef void (*uthread_func_t)(void *arg) */

/* thread control block */
struct uthread_tcb {
  int status;
  ucontext_t *context;
  void *stack;
  int t_id;
};

/* global queues */
static queue_t ready_processes;
static queue_t dead_processes;
static struct uthread_tcb *idle;
static struct uthread_tcb *currT;

int t_id = 0;

/* returns the current running thread tcb */
struct uthread_tcb *uthread_current(void) {
  return currT;
}

/* Frees the allocated tcb */
void uthread_destroy(queue_t dead, void *thread) {
  queue_delete(dead, thread);
  free(thread);
}

/*
 * Stops current running thread, runs first available thread in the queue,
 * and saves the old thread by pushing back onto queue
 */
void uthread_yield(void) {
  preempt_disable();
  struct uthread_tcb *oldThread = uthread_current();
  struct uthread_tcb *newThread;

  if (oldThread->status == RUNNING)
    oldThread->status = READY;

  queue_dequeue(ready_processes, (void **)&newThread);

  /* Once the thread is done, it automatically exits and sets status to DEAD */
  if (oldThread->status == READY)
    queue_enqueue(ready_processes, oldThread);

  currT = newThread;
  newThread->status = RUNNING;

  uthread_ctx_switch(oldThread->context, newThread->context);
}

/* Called in uthread_ctx_bootstrap to execute thread then exit safely */
void uthread_exit(void) {
  preempt_disable();
  /* Deallocate memory and move onto next thread by uthread_yield() */
  struct uthread_tcb *current = uthread_current();
  current->status = DEAD;
  free(current->context);
  free(current->stack);
  queue_enqueue(dead_processes, current);
  uthread_yield();
}

/* Allocate a new thread and it's members */
int uthread_create(uthread_func_t func, void *arg) {

  struct uthread_tcb *newT = malloc(sizeof(struct uthread_tcb));

  if (!newT)
    return -1;

  preempt_disable();
  newT->status = READY;
  newT->stack = uthread_ctx_alloc_stack();
  newT->context = malloc(sizeof(ucontext_t));
  newT->t_id = t_id++;

  if (uthread_ctx_init(newT->context, newT->stack, func, arg) != 0)
    return -1;

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

  if (!idle || !idle->context)
    return -1;

  idle->status = RUNNING;
  idle->t_id = t_id++;
  currT = idle;

  /* initialize the queue and create our initial thread */
  ready_processes = queue_create();
  dead_processes = queue_create();
  if (uthread_create(func, arg) != 0)
    return -1;

  /* update appropriate statuses */
  idle->status = RUNNING;

  /* while loop until the there are no more processes */
  while (queue_length(ready_processes) > 0)
    uthread_yield();

  if (preempt)
    preempt_stop();

  queue_iterate(dead_processes, uthread_destroy);

  free(currT->context);
  free(currT->stack);
  free(currT);
  queue_destroy(ready_processes);
  queue_destroy(dead_processes);

  return 0;
}

/* Functions to block threads in queue implemented in semaphor */
void uthread_block(void) {
  currT->status = BLOCKED;
  uthread_yield();
}

/* Functions to unblock threads in queue implemented in semaphor */
void uthread_unblock(struct uthread_tcb *uthread) {
  uthread->status = READY;
  queue_enqueue(ready_processes, uthread);
}

