#include <stdio.h>

#define MAX_THREADS 8
#define ETHREAD 255

typedef enum {
   false, true
} bool;

typedef enum {
   THREAD_RUNNING, THREAD_READY, THREAD_SLEEPING, THREAD_WAITING
} thread_state;

typedef struct {
   bool active;
   thread_state t_state;
} thread_t;

typedef struct {
   int current_thread;
   thread_t thread_list[MAX_THREADS];
} system_t;

static system_t system_threads;

void init() {
   int i;
   for (i = 0; i < MAX_THREADS; i++) {
      system_threads.thread_list[i].active = false;
      system_threads.thread_list[i].t_state = THREAD_READY;
   }

   system_threads.current_thread = ETHREAD;
}

int get_next_thread() {
   int next;
   int save = system_threads.current_thread;

   if (system_threads.current_thread >= MAX_THREADS)
      system_threads.current_thread = MAX_THREADS - 1;

   for (next = (system_threads.current_thread + 1) % MAX_THREADS;
    next != system_threads.current_thread &&
    !system_threads.thread_list[next].active ||
    (system_threads.thread_list[next].t_state == THREAD_WAITING ||
    system_threads.thread_list[next].t_state == THREAD_SLEEPING);
    next = (next + 1) % MAX_THREADS)
      ;

   if (next == system_threads.current_thread &&
    !system_threads.thread_list[next].active) {
      system_threads.current_thread = save;
      return ETHREAD;
   }

   system_threads.thread_list[next].t_state = THREAD_READY;
   return next;
}

// tests
int main() {
   init();
   printf("get next thread: %d\n", get_next_thread());


   thread_t *t_list = system_threads.thread_list;

   t_list[0].active = true;

   t_list[1].active = true;
   t_list[1].t_state = THREAD_WAITING;

   t_list[3].active = true;

   t_list[5].active = true;
   t_list[5].t_state = THREAD_SLEEPING;

   
   int i;
   for (i = 0; i < 7; i++) 
      printf("get next thread: %d\n", system_threads.current_thread = 
       get_next_thread());

   return 0;
}

