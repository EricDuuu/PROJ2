#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* REMINDER struct queue* = queue_t */
/**/

typedef struct node *node_t;

struct node {
  void *data;
  struct node *next;
};

struct queue {
  struct node *first;
  struct node *last;
  int length;
};

queue_t queue_create(void) {
  queue_t newQueue = malloc(sizeof(struct queue));

  newQueue->first = NULL;
  newQueue->last = NULL;
  newQueue->length = 0;

  return newQueue;
}

int queue_destroy(queue_t queue) {
  if (!queue || queue->first)
    return -1;

  free(queue);
  return 0;
}

int queue_enqueue(queue_t queue, void *data) {
  /* Allocate and initialize a new element in queue */
  node_t newNode = malloc(sizeof(struct node));
  newNode->data = data;
  newNode->next = NULL;

  if (!queue || !data || !newNode)
    return -1;
  else if (queue->length == 0) { /* Case: newly created queue */
    queue->first = newNode;
    queue->last = newNode;
  } else { /* Case: enqueue to last of the linked list */
    queue->last->next = newNode;
    queue->last = queue->last->next;
  }

  ++queue->length;
  return 0;
}

int queue_dequeue(queue_t queue, void **data) {
  if (queue->first || !data || !queue)
    return -1;

  node_t current = queue->first;
  data = &current->data;
  queue->first = queue->first->next;

  free(current);

  /* Case: last element popped */
  if (!queue->first)
    queue->last = queue->first;

  --queue->length;
  return 0;
}

/* finds node, then deletes node at any point in linked list */
int queue_delete(queue_t queue, void *data) {
  if (!queue || !data)
    return -1;

  node_t fastNode = queue->first;
  node_t slowNode = queue->first;

  if (fastNode && fastNode->data == data) {
    queue->first = fastNode->next;
    if (!queue->first)
      queue->last = queue->first;
    free(fastNode);
  } else {
    while (fastNode && fastNode->data != data) {
      slowNode = fastNode;
      fastNode = fastNode->next;
    }
    if (!fastNode)
      return -1;

    if (!fastNode->next)
      queue->last = slowNode;

    slowNode->next = fastNode->next;
    free(fastNode);
  }

  --queue->length;
  return 0;
}

int queue_iterate(queue_t queue, queue_func_t func) {
  if (!queue || !func || !queue->first)
    return -1;

  node_t current = queue->first;
  while (current) {
    (*func)(queue, current->data);
    current = current->next;
  }
  return 0;
}

int queue_length(queue_t queue) {
  if (!queue)
    return -1;
  return queue->length;
}
