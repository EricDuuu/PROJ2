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

enum status { READY, RUNNING, BLOCKED, ZOMBIE };
/* typedef void (*uthread_func_t)(void *arg) */

struct uthread_tcb {
  int status;
  ucontext_t *context;
  void *stack;
  struct uthread_tcb *blocking;
  bool joined;
};

struct uthread_tcb *uthread_current(void) {
  /* TODO Phase 2/4 */
}

void uthread_yield(void) {}

void uthread_exit(void) {}

int uthread_create(uthread_func_t func, void *arg) {}

int uthread_run(bool preempt, uthread_func_t func, void *arg) {}

void uthread_block(void) { /* TODO Phase 4 */
}

void uthread_unblock(struct uthread_tcb *uthread) { /* TODO Phase 4 */
}
