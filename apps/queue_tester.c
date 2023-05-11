#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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
  fprintf(stderr, "\n*** TEST create ***\n");

  TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void) {
  int data = 3, *ptr;
  queue_t q;

  fprintf(stderr, "\n*** TEST queue_simple ***\n");

  q = queue_create();
  queue_enqueue(q, &data);
  queue_dequeue(q, (void **)&ptr);
  TEST_ASSERT(ptr == &data);
}

/* Empty queue size */
void test_empty_queue_size(void) {
  queue_t q;

  fprintf(stderr, "\n*** TEST empty_queue_size ***\n");

  q = queue_create();
  TEST_ASSERT(queue_length(q) == 0);
}

/* Queue size after many enqueue */
void test_queue_size_many_enqueue(void) {
  int data1 = 1, data2 = 2, data3 = 3;
  queue_t q;

  fprintf(stderr, "\n*** TEST queue_size_many_enqueue ***\n");

  q = queue_create();
  queue_enqueue(q, &data1);
  queue_enqueue(q, &data2);
  queue_enqueue(q, &data3);
  TEST_ASSERT(queue_length(q) == 3);
}

/* Enqueue/Dequeue many */
void test_queue_many(void) {
  int data1 = 1, data2 = 2, data3 = 3, *ptr1, *ptr2, *ptr3;
  queue_t q;

  fprintf(stderr, "\n*** TEST queue_many ***\n");

  q = queue_create();
  queue_enqueue(q, &data1);
  queue_enqueue(q, &data2);
  queue_enqueue(q, &data3);
  queue_dequeue(q, (void **)&ptr1);
  queue_dequeue(q, (void **)&ptr2);
  queue_dequeue(q, (void **)&ptr3);
  TEST_ASSERT(ptr1 == &data1 && ptr2 == &data2 && ptr3 == &data3);
}

/* Dequeue from empty queue */
void test_error_dequeue_empty(void) {
  int *ptr;
  queue_t q;

  fprintf(stderr, "\n*** TEST error_dequeue_empty ***\n");

  q = queue_create();
  TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == -1);
}

/* Enqueue/Dequeue/Destroy/Length a NULL queue */
void test_error_queue_null(void) {
  int data = 3, *ptr;

  fprintf(stderr, "\n*** TEST error_queue_null ***\n");

  TEST_ASSERT(queue_enqueue(NULL, &data) == -1);
  TEST_ASSERT(queue_dequeue(NULL, (void **)&ptr) == -1);
  TEST_ASSERT(queue_destroy(NULL) == -1);
  TEST_ASSERT(queue_length(NULL) == -1);
}

/* Enqueue/Dequeue with NULL data */
void test_error_queue_null_data(void) {
  queue_t q;

  fprintf(stderr, "\n*** TEST error_queue_null_data ***\n");

  q = queue_create();
  TEST_ASSERT(queue_enqueue(q, NULL) == -1);
  TEST_ASSERT(queue_dequeue(q, NULL) == -1);
}

/* Queue destroyed */
void test_queue_destroyed(void) {
  int data = 1, *ptr;
  queue_t q;

  fprintf(stderr, "\n*** TEST queue_destroyed ***\n");

  q = queue_create();
  queue_enqueue(q, &data);
  queue_dequeue(q, (void **)&ptr);

  TEST_ASSERT(queue_destroy(q) == 0);
}

/* Callback function that increments items */
static void iterator_inc(queue_t q, void *data) {
  int *a = (int *)data;

  if (*a == 42) {
    queue_delete(q, data);
  } else {
    *a += 1;
  }
}

static void deleteHead(queue_t q, void *data) {
  int *a = (int *)data;

  if (*a == 2) {
    queue_delete(q, data);
  }
}

static void deleteTail(queue_t q, void *data) {
  int *a = (int *)data;

  if (*a == 10) {
    queue_delete(q, data);
  }
}

/* Interate over the queue and execute callback function*/
void test_iterator(void) {
  queue_t q;
  int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9}, *ptr;
  size_t i;

  /* Initialize the queue and enqueue items */
  q = queue_create();
  for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
    queue_enqueue(q, &data[i]);

  fprintf(stderr, "\n*** TEST iterator ***\n");

  /* Increment every item of the queue, delete item '42' */
  queue_iterate(q, iterator_inc);
  TEST_ASSERT(data[0] == 2);
  TEST_ASSERT(queue_length(q) == 9);

    
  /* Delete Head of queue, i.e. delete 2, thus queue's new head data is 3 */
  queue_iterate(q, deleteHead);
  queue_dequeue(q, (void **)&ptr);
  TEST_ASSERT(data[1] == *ptr); // expected: 3 == 3
  TEST_ASSERT(queue_length(q) == 7);
    
  /* Delete queue's tail */
  queue_iterate(q, deleteTail);
  TEST_ASSERT(queue_length(q) == 6);
}

int main(void) {
  test_create();
  test_queue_simple();
  test_empty_queue_size();
  test_queue_size_many_enqueue();
  test_queue_many();
  test_error_dequeue_empty();
  test_error_queue_null();
  test_error_queue_null_data();
  test_queue_destroyed();
  test_iterator();

  return 0;
}
