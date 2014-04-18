#include "os.h"
#include "globals.h"

// global system_t variable to track register contents belonging to threads
static system_t system_threads;

//This interrupt routine is automatically run every 10 milliseconds
ISR(TIMER0_COMPA_vect) {
   //The following statement tells GCC that it can use registers r18-r27, 
   //and r30-31 for this interrupt routine.  These registers (along with
   //r0 and r1) will automatically be pushed and popped by this interrupt routine.
   asm volatile ("" : : : "r18", "r19", "r20", "r21", "r22", "r23", "r24", \
                 "r25", "r26", "r27", "r30", "r31");                        


   // TODO Call get_next_thread to get the thread id of the next thread to run
   // TODO Call context switch here to switch to that next thread
}

//Call this to start the system timer interrupt
void start_system_timer() {
   TIMSK0 |= _BV(OCIE0A);  //interrupt on compare match
   TCCR0A |= _BV(WGM01);   //clear timer on compare match

   //Generate timer interrupt every ~10 milliseconds
   TCCR0B |= _BV(CS02) | _BV(CS00);    //prescalar /1024
   OCR0A = 156;             //generate interrupt every 9.98 milliseconds
}

__attribute__((naked)) void context_switch(uint16_t* new_tp, uint16_t* old_tp) {
   // TODO implement context_switch second
}

__attribute__((naked)) void thread_start(void) {
   sei(); //enable interrupts - leave this as the first statement in thread_start()
   // TODO implement thread_start first
}

void os_init() {
   int i;

   system_threads.current_thread = 255;
   for (i = 0; i < MAX_THREADS; i++)
      system_threads.thread_list[i].active = 0;
   system_threads.active_threads_count = 0;
   system_threads.uptime = 0;
   asm volatile("cli");
}

void create_thread(uint16_t address, void *args, uint16_t stack_size) {
   int idx;
   for (idx = 0; idx < MAX_THREADS && 
    system_threads.thread_list[idx].active; idx++) 
      ;

   if (idx == MAX_THREADS) {
      print_string("Error: Maximum amount of threads has been reached");
      return;
   }

   thread_t *open_thread = &system_threads.thread_list[idx];
   open_thread->thread_id = idx;
   open_thread->thread_pc = address;
   open_thread->stack_usage = 0;
   open_thread->stack_size = stack_size;
   open_thread->tos = malloc(stack_size);
   open_thread->base = open_thread->tos;
   open_thread->end = open_thread->base + stack_size;
   open_thread->args = args;
   open_thread->active = 1;

   ++system_threads.active_threads_count;
}

void os_start() {
   system_threads.current_thread = get_next_thread();
   thread_start();
}

uint8_t get_next_thread() {
   int next;
   uint8_t save;

   if (system_threads.current_thread >= MAX_THREADS) {
      save = system_threads.current_thread;
      system_threads.current_thread = MAX_THREADS - 1;
   }

   for (next = (system_threads.current_thread + 1) % MAX_THREADS;
    next != system_threads.current_thread && 
    !system_threads.thread_list[next].active; 
    next = (next + 1) % MAX_THREADS)
      ;

   if (next == system_threads.current_thread && 
    !system_threads.thread_list[next].active) {
      system_threads.current_thread = save;
      return 255;
   }
   return next;
}
