#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "os.h"

/*
 * Queue struct designed to hold thread_t pointers
 */
typedef struct Queue {
   thread_t **queue;
   int head, tail;
   int queue_size, queue_size_max;
} Queue;

/*
 * Queue constructor. |size| specifies the max size of the queue. 
 */
Queue *Queue_create(int size);

/*
 * Push thread_t *element pointed to by |element| to back of queue |q|
 */
bool Queue_push(Queue *q, thread_t *element);

/*
 * Pop front thread_t* element of queue |q|. Returns 0 if queue is empty.
 */
thread_t *Queue_pop(Queue *q);

/*
 * Peek front thread_t *element of queue |q|. Returns 0 if queue is empty.
 */
void *Queue_peek(Queue *q);

/*
 * Get the number of elements inside the queue |q|
 */
int Queue_size(Queue *q);

/* 
 * Check if queue |q| is empty. Returns true if empty, false otherwise
 */
bool Queue_empty(Queue *q);

/* 
 * Queue destructor. Destroys queue |q|
 */
void Queue_destroy(Queue *q);

#endif
