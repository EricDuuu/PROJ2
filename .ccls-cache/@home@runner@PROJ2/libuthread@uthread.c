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

typedef enum { READY, RUNNING, BLOCKED, ZOMBIE } state;
/* typedef void (*uthread_func_t)(void *arg) */

struct uthread_tcb {
  int status;
  ucontext_t *context;
  void *stack;
  int t_id;
};

/* global queues */
static queue_t ready_processes;
static queue_t blocked_processes;

static struct uthread_tcb *idle;
static struct uthread_tcb *currT;

struct uthread_tcb *uthread_current(void) {
  return currT;
}

void uthread_yield(void) {
  
  if(queue_length(ready_processes)>0){
    struct uthread_tcb *oldThread = malloc(sizeof(struct uthread_tcb));
    struct uthread_tcb *runThread = malloc(sizeof(struct uthread_tcb));
    currT->status = READY;
    queue_enqueue(ready_processes, currT);
    queue_dequeue(ready_processes, (void **)&runThread);
    oldThread = currT;
    currT = runThread;
    runThread->status = RUNNING;
    uthread_ctx_switch(oldThread->context, runThread->context);
  }
  
}

void uthread_exit(void) { 
  currT->status = ZOMBIE;
  uthread_ctx_switch(currT->context, idle->context); 
}

int uthread_create(uthread_func_t func, void *arg) {
  struct uthread_tcb *newT = malloc(sizeof(struct uthread_tcb));
  newT->status = READY;
  newT->stack = uthread_ctx_alloc_stack();
  newT->context = malloc(sizeof(ucontext_t));
  uthread_ctx_init(newT->context, newT->stack, func, arg);
  queue_enqueue(ready_processes, newT);
  
  return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg) {
  idle = malloc(sizeof(struct uthread_tcb));
  idle->context = malloc(sizeof(ucontext_t));
  idle->status = RUNNING;

  ready_processes = queue_create();
  uthread_create(func, arg);
  
  if (preempt)
    preempt_enable();
  // return 0;

  while (queue_length(ready_processes) > 0) {
    struct uthread_tcb *runThread = malloc(sizeof(struct uthread_tcb));
    queue_dequeue(ready_processes, (void **)&runThread);
    currT = runThread;
    idle->status = READY;
    runThread->status = RUNNING;
    uthread_ctx_switch(idle->context, runThread->context);
  }
  idle->status = RUNNING;
  return 0;
}

void uthread_block(void) { /* TODO Phase 4 */
}

void uthread_unblock(struct uthread_tcb *uthread) { /* TODO Phase 4 */
  queue_delete(blocked_processes, uthread);
}
