#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
  void *data;
  struct node *next;
};

struct queue {
  int queueLength;
  struct node *head, *tail;
};

queue_t queue_create(void) {
  /* allocate memory for new queue_t struct */
  queue_t queue = malloc(sizeof(struct queue));

  if (queue == NULL) {
    return NULL;
  }

  /* initialise the struct's fields to NULL and */
  queue->head = NULL;
  queue->tail = NULL;
  queue->queueLength = 0;
  return queue;
}

int queue_destroy(queue_t queue) {
  if (queue == NULL || queue->queueLength != 0) {
    return -1;
  }
  free(queue);
  return 0;
}

int queue_enqueue(queue_t queue, void *data) {
  /* check if queue or data are not NULL */
  if (queue == NULL || data == NULL) {
    return -1;
  }

  /* allocate memory to new node */
  struct node *newNode = (struct node *)malloc(sizeof(struct queue));

  /* check if allocation of new node was successful */
  if (newNode == NULL) {
    return -1;
  }
  newNode->data = data;
  /* printf("%d\n", newNode->data); */
  newNode->next = NULL;

  /* if queue is empty then set head to the new node else current tail's next
    becomes new node */
  if (queue->head == NULL) {
    queue->head = newNode;
  } else {
    queue->tail->next = newNode;
  }

  /* new node becomes tail of the queue */
  queue->tail = newNode;
  queue->queueLength++;

  return 0;
}

int queue_dequeue(queue_t queue, void **data) {
    
  /* check if queue or data are NULL or if queue is empty */
  if (queue == NULL || data == NULL || queue->queueLength == 0) {
    return -1;
  }

  struct node *old = queue->head;

  /* assign oldest item's data to the function 'data' parameter */
  *data = old->data;
  queue->head = old->next;

  /* if head is NULL then queue is down to a single node, so tail is NULL */
  if (queue->head == NULL) {
    queue->tail = NULL;
  }

  free(old);
  queue->queueLength--;

  return 0;
}

int queue_delete(queue_t queue, void *data) {
  /* check if queue or data are not NULL */
  if (queue == NULL || data == NULL) {
    return -1;
  }

  struct node *previousNode = NULL;
  struct node *currentNode = queue->head;

  while (currentNode) {
    // match detected
    if (currentNode->data == data) {
        
      /* if previous node existed, update its next node to be current's next,
       skipping over current node (to be removed) else head being removed, so
       head's next node will be current node's next node */
      if (previousNode) {
        previousNode->next = currentNode->next;
      } else {
        queue->head = currentNode->next;
      }

      /* if previous node's next is NULL (currentNode was tail, but it is
       removed), previous node is the new tail, so appropriately re-assign
       queue's tail */
      if (currentNode->next == NULL) {
        queue->tail = previousNode;
      }

      free(currentNode);
      queue->queueLength--;
      return 0;
    }
      
    // continue iterating if no match detected
    previousNode = currentNode;
    currentNode = currentNode->next;
  }
  return 0;
}

int queue_iterate(queue_t queue, queue_func_t func) {
  if (queue == NULL || func == NULL) {
    return -1;
  }

  struct node *currentNode = queue->head;
  struct node *nextNode = NULL;

  while (currentNode) {
    // preserves the currentNode's next node incase the func deletes currentNode
    nextNode = currentNode->next;
    func(queue, (currentNode->data));
    currentNode = nextNode;
  }
  return 0;
}

int queue_length(queue_t queue) {
  if (queue == NULL) {
    return -1;
  }
  return queue->queueLength;
}
