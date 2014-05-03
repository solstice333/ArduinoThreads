#include "queue.h"

Queue *Queue_create(int size, int element_size) {
   Queue *q = malloc(sizeof(Queue));
   q->queue = malloc(size * element_size);
   q->head = 0;
   q->tail = -1;
   q->queue_size = 0;
   q->queue_size_max = size;
   q->element_size = element_size;
   return q;
}

bool Queue_push(Queue *q, void *element) {
   if (q->queue_size < q->queue_size_max) {
      q->tail = ++q->tail % q->queue_size_max;
      memmove(q->queue + q->tail * q->element_size, element, q->element_size);
      ++q->queue_size;
      return true;
   }
   return false;
}

void *Queue_pop(Queue *q) {
   if (q->queue_size > 0) {
      void *popped = malloc(q->element_size);
      memmove(popped, q->queue + q->head * q->element_size, q->element_size);
      q->head = ++q->head % q->queue_size_max;
      --q->queue_size;
      return popped;
   }
   return 0;
}

void *Queue_peek(Queue *q) {
   if (q->queue_size > 0) {
      void *peeked = malloc(q->element_size);
      memmove(peeked, q->queue + q->head * q->element_size, q->element_size);
      return peeked;
   }
   return 0;
}

int Queue_size(Queue *q) {
   return q->queue_size;
}

bool Queue_empty(Queue *q) {
   if (!q->queue_size)
      return true;
   return false;
}

void Queue_destroy(Queue *q) {
   free(q->queue);
   free(q);
}
