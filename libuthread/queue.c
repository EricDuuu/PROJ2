#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue {
  void *data;
  struct queue *next;
};

queue_t queue_create(void) {
  struct queue *head = malloc(sizeof(struct queue));
  return head;
}

int queue_destroy(queue_t queue) {
  if (!queue || !queue->data)
    return -1;

  free(queue);

  return 0;
}

int queue_enqueue(queue_t queue, void *data) {
  if (!queue || !data)
    return -1;

  struct queue *newElement = malloc(sizeof(struct queue));

  // memory allocation error?
  if (!newElement) {
    free(newElement);
    return -1;
  }

  newElement->next = queue;
  queue = newElement;
}

int queue_dequeue(queue_t queue, void **data) { /* TODO Phase 1 */
}

int queue_delete(queue_t queue, void *data) { /* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func) { /* TODO Phase 1 */
}

int queue_length(queue_t queue) { /* TODO Phase 1 */
}
