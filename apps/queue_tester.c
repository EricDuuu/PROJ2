#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"
#include <queue.h>

#define TEST_ASSERT(assert)                                                    \
  do {                                                                         \
    printf("ASSERT: " #assert " ... ");                                        \
    if (assert) {                                                              \
      printf("PASS\n");                                                        \
    } else {                                                                   \
      printf("FAIL\n");                                                        \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

/* Create */
void test_create(void) {
  fprintf(stderr, "*** TEST create ***\n");
  TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void) {
  int data = 3, *ptr;
  queue_t q;

  fprintf(stderr, "*** TEST queue_simple ***\n");

  q = queue_create();
  queue_enqueue(q, &data);
  queue_dequeue(q, (void **)&ptr);
  TEST_ASSERT(ptr == &data);
}

static void iterator_inc(queue_t q, void *data) {
  int *a = (int *)data;

  if (*a == 42)
    queue_delete(q, data);
  else
    *a += 1;
}

void test_iterator(void) {
  queue_t q;
  int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
  size_t i;

  fprintf(stderr, "*** TEST test_iterator ***\n");

  /* Initialize the queue and enqueue items */
  q = queue_create();
  for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
    queue_enqueue(q, &data[i]);

  /* Increment every item of the queue, delete item '42' */
  queue_iterate(q, iterator_inc);
  TEST_ASSERT(data[0] == 2);
  TEST_ASSERT(queue_length(q) == 9);
}

void test_no_members(void) {
  int i = 10;
  fprintf(stderr, "*** TEST test_no_members ***\n");
  queue_t q = queue_create();

  /*
   * enqueue and dequeue checking validity of deletion and
   * insert
   */
  queue_enqueue(q, &i);
  queue_dequeue(q, (void **)&i);

  /* Checks if the queue is properly empty */
  TEST_ASSERT(queue_iterate(q, iterator_inc) == -1);
  TEST_ASSERT(queue_dequeue(q, (void **)&i) == -1);
  TEST_ASSERT(queue_delete(q, (void **)&i) == -1);
  TEST_ASSERT(queue_length(q) == 0);
}

void test_endequeue(void) {
  queue_t q;
  int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
  size_t i;

  fprintf(stderr, "*** TEST test_iterator ***\n");

  int dataLen = (sizeof(data) / 4) + 1;
  void *dataCopy[dataLen];
  /* Initialize the queue and enqueue items */
  q = queue_create();
  for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
    queue_enqueue(q, &data[i]);
    queue_dequeue(q, (void **)&dataCopy[i]);
  }
  TEST_ASSERT(queue_length(q) == 0);
}

int main(void) {
  test_create();
  test_queue_simple();
  test_iterator();
  test_no_members();
  test_endequeue();

  return 0;
}
