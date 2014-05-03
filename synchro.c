#include "synchro.h"

#define DEBUG 1

#if DEBUG
static bool test_and_set(bool *lock) {
   bool rv = *lock;
   *lock = true;
   return rv;
}

static void acquire(bool *lock) {
   while(test_and_set(lock)) {
      system_threads.thread_list[system_threads.current_thread].t_state =
       THREAD_WAITING;
   }
}

static void release(bool *lock) {
   *lock = false;
}

void mutex_init(mutex_t *m) {
   m = malloc(sizeof(mutex_t));   
   m->lock = 0;
   m->waitlist = Queue_create(8, sizeof(thread_t *));
}

void mutex_lock(mutex_t *m) {
   while(test_and_set(m)) // acquire
      ;
}

#endif
