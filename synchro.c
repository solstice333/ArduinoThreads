#include "synchro.h"

#define DEBUG 1

#if DEBUG
static bool test_and_set(bool *lock) {
   bool rv = *lock;
   *lock = true;
   return rv;
}

void mutex_init(mutex_t *m) {
   m->lock = false;
   m->waitlist = Queue_create(8, sizeof(thread_t *));
}

void mutex_lock(mutex_t *m) {
   while(test_and_set(&m->lock)) {  // acquire
      set_cursor(3, 1);
      print_string("inside mutex_lock");

      thread_t *this_thread = 
       &system_threads.thread_list[system_threads.current_thread];

      this_thread->t_state = THREAD_WAITING;
      Queue_push(m->waitlist, &this_thread);
      yield();
   }
}

void mutex_unlock(mutex_t *m) {
   m->lock = false;  // release
   thread_t *next_thread = Queue_pop(m->waitlist);
   next_thread->t_state = THREAD_READY;
   free(next_thread);
}

#endif
