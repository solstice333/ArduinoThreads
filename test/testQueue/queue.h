#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
   false, true
} bool;

typedef struct Queue {
   void *queue;
   int head, tail;
   int queue_size, queue_size_max, element_size;
} Queue;

Queue *Queue_create(int size, int element_size);
bool Queue_push(Queue *q, void *element);
void *Queue_pop(Queue *q);
void *Queue_peek(Queue *q);
int Queue_size(Queue *q);
bool Queue_empty(Queue *q);
void Queue_destroy(Queue *q);

#define Queue_print(q, type) {\
   Queue *__q = (q);\
   int __i;\
   for (__i = __q->head; __i != __q->tail; __i = ++__i % __q->queue_size_max)\
      printf("element %d: %d\n", __i, ((type *)__q->queue)[__i]);\
   printf("element %d: %d\n", __i, ((type *)__q->queue)[__i]);\
}\

#endif
