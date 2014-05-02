#include <stdio.h>
#include "queue.h"

int main() {
   Queue *q = Queue_create(5, sizeof(int));
   printf("q->queue: %d\n", q->queue);
   printf("q->head: %d\n", q->head);
   printf("q->tail: %d\n", q->tail);
   printf("q->queue_size: %d\n", q->queue_size);
   printf("q->queue_size_max: %d\n", q->queue_size_max);
   printf("q->element_size: %d\n", q->element_size);

   printf("\n\npush tests: \n\n");
   int x = 1;
   Queue_push(q, &x);
   x = 2;
   Queue_push(q, &x);
   x = 3;
   Queue_push(q, &x);
   x = 4;
   Queue_push(q, &x);
   x = 5;
   Queue_push(q, &x);
   x = 6;
   Queue_push(q, &x);
   x = 7;
   Queue_push(q, &x);
   x = 8;
   Queue_push(q, &x);
   Queue_print(q, int);

   printf("\n\npop tests: \n\n");
   int *val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);
   val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);
   Queue_print(q, int);

   printf("\n\ncircular tests: \n\n");
   x = 6;
   Queue_push(q, &x);
   x = 7;
   Queue_push(q, &x);

   printf("\n\nprinting queue (tail wraps): \n\n");
   Queue_print(q, int);

   x = 8;
   Queue_push(q, &x);

   printf("\n\nprinting queue: \n\n");
   Queue_print(q, int);

   val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);
   x = 8;
   Queue_push(q, &x);

   printf("\n\nprinting queue: \n\n");
   Queue_print(q, int);

   val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);
   val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);
   val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);
   x = 9;
   Queue_push(q, &x);
   val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);

   printf("\n\nprinting queue (head wraps): \n\n");
   Queue_print(q, int);

   printf("\n\nTry to pop past 0:\n\n");
   val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);
   val = Queue_pop(q);
   printf("popped value: %d\n", *val);
   free(val);
   // val = Queue_pop(q);
   // printf("popped value: %d\n", *val); // should cause seg fault

   printf("\n\npeek: \n\n");
   x = 10;
   Queue_push(q, &x); 
   x = 11;
   Queue_push(q, &x); 
   x = 12;
   Queue_push(q, &x); 

   printf("\n\nprinting queue (head wraps): \n\n");
   Queue_print(q, int);

   val = Queue_peek(q);
   printf("peeked value: %d\n", *val);
   free(val);
   val = Queue_peek(q);
   printf("peeked value: %d\n", *val);
   free(val);

   Queue_destroy(q);

   return 0;
}
