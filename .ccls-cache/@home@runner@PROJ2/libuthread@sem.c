#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

struct semaphore {
  int count;
  struct queue *blocked;
};

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
  if (queue_destroy(sem->blocked) == -1)
    return -1;
  free(sem);
  return 0;
}

int sem_down(sem_t sem) { /* TODO Phase 3 */
}

int sem_up(sem_t sem) { /* TODO Phase 3 */
}
