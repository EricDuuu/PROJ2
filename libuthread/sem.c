#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

/*
 * Purpose: the struct stores two data values
 * It holds the semaphore resource count
 * and the queue of blocked threads
 */
struct semaphore {
  int count;
  queue_t blocked;
};

/* Allocates the semaphor struct to manage the round robin queue */
sem_t sem_create(size_t count) {
  preempt_disable();
  struct semaphore *sem = malloc(sizeof(struct semaphore));
  if (!sem)
    return NULL;
  sem->blocked = queue_create();
  sem->count = count;
  preempt_enable();

  return sem;
}

/* Deallocate memory allocated to the semaphor struct safely */
int sem_destroy(sem_t sem) {
  preempt_disable();
  if (!sem)
    return -1;
  if (queue_destroy(sem->blocked) == -1 || queue_length(sem->blocked) != 0)
    return -1;
  free(sem);
  preempt_enable();

  return 0;
}

/* Reduces the sempahore resource count by 1 or blocks the thread if no
 * resources available */
int sem_down(sem_t sem) {
  if (!sem)
    return -1;
  else if (sem->count == 0) {
    struct uthread_tcb *cur_tcb = uthread_current();
    queue_enqueue(sem->blocked, cur_tcb);
    uthread_block();
  } else
    --sem->count;
  return 0;
}

/* Increases the semaphore resource count by 1 or dequeues a blocked thread if
 * it exists*/
int sem_up(sem_t sem) {
  if (!sem)
    return -1;
  if (queue_length(sem->blocked) > 0) {
    struct uthread_tcb *front;
    if (queue_dequeue(sem->blocked, (void **)&front) == -1) {
      return -1;
    }
    uthread_unblock(front);
  } else
    ++sem->count;
  return 0;
}
