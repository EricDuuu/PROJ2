#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

/*
// Purpose: the struct stores two data values
// It holds the semaphore resource count 
// and the queue of blocked threads
*/
struct semaphore {
  int count;
  queue_t blocked;
};

/*
// Purpose: sem_create 
*/
sem_t sem_create(size_t count) {
  struct semaphore *sem = malloc(sizeof(struct semaphore));
  if (!sem)
    return NULL;
  sem->blocked = queue_create();
  sem->count = count;
  return sem;
}

int sem_destroy(sem_t sem) {
  if (!sem)
    return -1;
  if (queue_destroy(sem->blocked) == -1 || queue_length(sem->blocked) != 0)
    return -1;
  free(sem);
  return 0;
}

int sem_down(sem_t sem) { /* TODO Phase 3 */
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

int sem_up(sem_t sem) {
  if (!sem)
    return -1;
  if (queue_length(sem->blocked) > 0) {
    struct uthread_tcb *front;
    if (queue_dequeue(sem->blocked, (void **)&front) == -1) {
      return -1;
    }
    uthread_unblock(front);
  }

  else
    ++sem->count;
  return 0;
}
