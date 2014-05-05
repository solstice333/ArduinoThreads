#include "synchro.h"

#define DEBUG 1

static bool test_and_set(bool *lock) {
   cli();
   bool rv = *lock;
   *lock = true;
   sei();
   return rv;
}

void mutex_init(mutex_t *m) {
   m->lock = false;
   m->waitlist = Queue_create(8);
}

void mutex_lock(mutex_t *m) {
   while(test_and_set(&m->lock)) {  // acquire
      cli();

      thread_t *this_thread = 
       &system_threads.thread_list[system_threads.current_thread];
      this_thread->t_state = THREAD_WAITING;
      Queue_push(m->waitlist, this_thread);

      sei();
      yield(get_next_thread());
   }
}

void mutex_unlock(mutex_t *m) {
   m->lock = false;  // release
   if (!Queue_empty(m->waitlist)) {
      thread_t *next_thread = Queue_pop(m->waitlist);
      next_thread->t_state = THREAD_READY;
   }
}

void sem_init(semaphore_t *s, int8_t value) {
   s->n = value;
   s->waitlist = Queue_create(8);
}

void sem_wait(semaphore_t *s) {
   s->n--;
   if (s->n < 0) {
      cli();

      thread_t *this_thread =
       &system_threads.thread_list[system_threads.current_thread];
      this_thread->t_state = THREAD_WAITING;
      Queue_push(s->waitlist, this_thread);

      sei();
      yield(get_next_thread());
   }
}

void sem_signal(semaphore_t *s) {
   s->n++;
   if (s->n <= 0) {
      thread_t *next_thread = Queue_pop(s->waitlist);
      next_thread->t_state = THREAD_READY;
   }
}

void sem_signal_swap(semaphore_t *s) {
   s->n++;
   if (s->n <= 0) {
      thread_t *next_thread = Queue_pop(s->waitlist); 
      next_thread->t_state = THREAD_READY;
      yield(next_thread->thread_id);
   }
}
