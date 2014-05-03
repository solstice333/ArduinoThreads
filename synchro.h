#ifndef SYNCHRO_H
#define SYNCHRO_H

#include "globals.h"
#include "queue.h"
#include "os.h"

typedef struct mutex_t {
   bool lock;
   Queue *waitlist;
} mutex_t;

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);

/*
TODO implement semaphore_t
typedef struct semaphore_t {
} semaphore_t;

void sem_init(struct semaphore_t *s, int8_t value);
void sem_wait(struct semaphore_t *s);
void sem_signal(struct semaphore_t *s);
void sem_signal_swap(struct semaphore_t *s);
*/

#endif
