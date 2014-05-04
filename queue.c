#include "queue.h"

Queue *Queue_create(int size) {
   Queue *q = malloc(sizeof(Queue));
   q->queue = malloc(size * sizeof(thread_t *));
   q->head = 0;
   q->tail = -1;
   q->queue_size = 0;
   q->queue_size_max = size;
   return q;
}

bool Queue_push(Queue *q, thread_t *element) {
   if (q->queue_size < q->queue_size_max) {
      q->tail = ++q->tail % q->queue_size_max;
      q->queue[q->tail] = element;
      ++q->queue_size;
      return true;
   }
   return false;
}

thread_t *Queue_pop(Queue *q) {
   if (q->queue_size > 0) {
      thread_t *popped = q->queue[q->head];
      q->head = ++q->head % q->queue_size_max;
      --q->queue_size;
      return popped;
   }
   return 0;
}

void *Queue_peek(Queue *q) {
   if (q->queue_size > 0)
      return q->queue[q->head];
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
