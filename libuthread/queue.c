#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "queue.h"

/* REMINDER struct queue* = queue_t */

typedef struct node *node_t;

struct node {
  void *data;
  node_t next;
};

struct queue {
  node_t first;
  node_t last;
  int length;
};

/* Allocates empty queue and initializes struct members */
queue_t queue_create(void) {
  queue_t newQueue = malloc(sizeof(struct queue));
  newQueue->first = NULL;
  newQueue->last = NULL;
  newQueue->length = 0;
  return newQueue;
}

/*
 * Deallocates just the queue element, all elements should be deallocated
 * in respective delete/dequeue operations
 */
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
  if (!queue || queue->length <= 0)
    return -1;

  node_t current = queue->first;
  *data = (current->data);
  queue->first = queue->first->next;
  free(current);

  /* Case: last element popped */
  if (!queue->first)
    queue->last = queue->first;

  --queue->length;
  return 0;
}

/* Finds node, then deletes node at any point in linked list */
int queue_delete(queue_t queue, void *data) {
  if (!queue || !data || queue->length <= 0)
    return -1;

  node_t current = queue->first;

  if (current->data == data) { /* Case: first node found */
    if (!current->next)
      queue->first = queue->last = NULL;
    else
      queue->first = queue->first->next;
    free(current);
    --queue->length;
    return 0;
  }

  while (current->next) { /* Case: node in rest of queue */
    if (current->next->data == data) {
      node_t temp = current->next;
      current->next = current->next->next;
      --queue->length;
      if (!current->next) /* Case: last node deleted */
        queue->last = current;
      free(temp);
      return 0;
    }
    current = current->next;
  }
  return -1;
}

int queue_iterate(queue_t queue, queue_func_t func) {
  if (!queue || !func || !queue->first)
    return -1;

  // Two pointers to point at our node in case of deletion
  node_t current = queue->first;
  node_t current_safe = NULL;
  while (current) {
    current_safe = current->next;
    func(queue, current->data);
    current = current_safe;
  }

  return 0;
}

int queue_length(queue_t queue) {
  if (!queue)
    return -1;
  return queue->length;
}
