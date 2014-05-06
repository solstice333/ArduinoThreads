#ifndef SYNCHRO_H
#define SYNCHRO_H

#include "globals.h"
#include "queue.h"
#include "os.h"

/*
 * Mutex struct
 */
typedef struct mutex_t {
   bool lock;
   Queue *waitlist;
} mutex_t;

/*
 * Initialize mutex |m|
 */
void mutex_init(mutex_t *m);

/*
 * Lock mutex |m|
 */
void mutex_lock(mutex_t *m);

/*
 * Unlock mutex |m|
 */
void mutex_unlock(mutex_t *m);

/*
 * Semaphore struct
 */
typedef struct semaphore_t {
   int n;
   Queue *waitlist;
} semaphore_t;

/*
 * Initialize semaphore |s| with |value|
 */
void sem_init(semaphore_t *s, int8_t value);

/*
 * Cause semaphore |s| to wait, and thus decrement the semaphore
 */
void sem_wait(semaphore_t *s);

/*
 * Signal sempahore |s|, and thus inrement the semaphore
 */
void sem_signal(semaphore_t *s);

/*
 * Signal semaphore |s|, inrement the semaphore, and swap directly to the
 * waiting thread at the front of the waitlist
 */
void sem_signal_swap(semaphore_t *s);

#endif
