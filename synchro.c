#include "synchro.h"

#define DEBUG 1

static bool test_and_set(bool *lock) {
   bool rv = *lock;
   *lock = true;
   return rv;
}

void mutex_init(mutex_t *m) {
   m->lock = false;
   m->waitlist = Queue_create(8);
}

void mutex_lock(mutex_t *m) {
   while(test_and_set(&m->lock)) {  // acquire
      thread_t *this_thread = 
       &system_threads.thread_list[system_threads.current_thread];

      this_thread->t_state = THREAD_WAITING;
      Queue_push(m->waitlist, this_thread);

#if DEBUG
      set_cursor(5, 1);
      print_string("thread id added onto waitlist: ");
      print_int(this_thread->thread_id);
#endif

      yield();
   }
}

void mutex_unlock(mutex_t *m) {
   m->lock = false;  // release
   if (!Queue_empty(m->waitlist)) {
      thread_t *next_thread = Queue_pop(m->waitlist);
      next_thread->t_state = THREAD_READY;

#if DEBUG
      set_cursor(4, 1);
      print_string("thread id removed off of waitlist: ");
      print_int(next_thread->thread_id);
#endif
   }
}

