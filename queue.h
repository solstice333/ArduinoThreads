#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

// Queue struct
typedef struct Queue {
   void *queue;
   int head, tail;
   int queue_size, queue_size_max, element_size;
} Queue;

/*
 * Queue constructor. |size| specifies the max size of the queue. 
 * |element_size| specifices the element size
 */
Queue *Queue_create(int size, int element_size);

/*
 * Push element pointed to by |element| to back of queue |q|
 */
bool Queue_push(Queue *q, void *element);

// Pop front element of queue |q|. User needs to free the returned element.
void *Queue_pop(Queue *q);

// Peek front element of queue |q|. User needs to free the returned element. 
void *Queue_peek(Queue *q);

// Get the number of elements inside the queue |q|
int Queue_size(Queue *q);

// Check if queue |q| is empty. true if empty, false otherwise
bool Queue_empty(Queue *q);

// Queue destructor. Destroys queue |q|
void Queue_destroy(Queue *q);

#define Queue_print(q, type) {\
   Queue *__q = (q);\
   int __i;\
   for (__i = __q->head; __i != __q->tail; __i = ++__i % __q->queue_size_max)\
      printf("element %d: %d\n", __i, ((type *)__q->queue)[__i]);\
   printf("element %d: %d\n", __i, ((type *)__q->queue)[__i]);\
}\

#endif
