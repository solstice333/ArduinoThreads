#include "os.h"
#include "globals.h"

#define DEBUG 1
#define DEBUG_CS 1
#define DEBUG_TS 0

#if DEBUG
   static uint8_t row = 0;
#endif

// global system_t variable to track register contents belonging to threads
static system_t system_threads;

//This interrupt routine is automatically run every 10 milliseconds
ISR(TIMER0_COMPA_vect) {
   //The following statement tells GCC that it can use registers r18-r27, 
   //and r30-31 for this interrupt routine.  These registers (along with
   //r0 and r1) will automatically be pushed and popped by this interrupt 
   // routine.
   asm volatile ("" : : : "r18", "r19", "r20", "r21", "r22", "r23", "r24", \
                 "r25", "r26", "r27", "r30", "r31");                        






/*
#if DEBUG   
   set_cursor(++row, 1);
   print_string("current_thread: ");
   print_int(system_threads.current_thread);
   set_cursor(++row, 1);
   print_string("SP inside ISR: ");
   print_int(SP);
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[4]);
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[5]);
#endif
   */

   uint8_t old_id = system_threads.current_thread;
   uint8_t new_id = get_next_thread();

#if DEBUG
   // set_cursor(++row, 1);
   // print_string("new_id: ");
   // print_int(new_id);
   set_cursor(row = 1, 1);
   print_string("entering context_switch here: ");
#endif

   uint8_t *old_tos = system_threads.thread_list[old_id].tos = SP + 1;
   uint8_t *new_tos = system_threads.thread_list[new_id].tos;
   context_switch(new_tos, old_tos);

#if DEBUG
   set_cursor(++row, 1);
   print_string("Coming back into ISR from context_switch!");
   exit(0);
#endif

   system_threads.current_thread = new_id;













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

/*
 * Switches between threads. |new_tp| is the new top of stack to point to. 
 * |old_tp| is the old top of stack pointed to. Note that the stack pointer
 * needs to be set up prior to the call to context_switch to take into account
 * the automatic push of the return address which will need to be loaded back
 * into the PC by the end of context_switch()
 */
__attribute__((naked)) void context_switch(uint16_t* new_tp, uint16_t* old_tp) {
#if DEBUG_CS
   set_cursor(++row, 1);
   print_string("SP position before assigning new value to SP: ");
   print_int(SP);
#endif

   SP = (uint8_t *)old_tp - PC_OFFSET - 1;

#if DEBUG_CS
   set_cursor(++row, 1);
   print_string("Back to os_start");
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[1]);
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[2]);

   set_cursor(++row, 1);
   print_string("To blink");
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[3]);
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[4]);

   set_cursor(++row, 1);
   print_string("Current SP position: ");
   print_int(SP);
#endif

   asm volatile("push r2");
   asm volatile("push r3");
   asm volatile("push r4");
   asm volatile("push r5");
   asm volatile("push r6");
   asm volatile("push r7");
   asm volatile("push r8");
   asm volatile("push r9");
   asm volatile("push r10");
   asm volatile("push r11");
   asm volatile("push r12");
   asm volatile("push r13");
   asm volatile("push r14");
   asm volatile("push r15");
   asm volatile("push r16");
   asm volatile("push r17");
   asm volatile("push r28");
   asm volatile("push r29");

#if DEBUG_CS
   set_cursor(++row, 1);
   print_string("SP position after pushing: ");
   print_int(SP);
#endif

   SP = (uint8_t *)new_tp - 1;

#if DEBUG_CS
   set_cursor(++row, 1);
   print_string("SP position after switching to new_tp: ");
   print_int(SP);
#endif

   asm volatile("pop r29");
   asm volatile("pop r28");
   asm volatile("pop r17");
   asm volatile("pop r16");
   asm volatile("pop r15");
   asm volatile("pop r14");
   asm volatile("pop r13");
   asm volatile("pop r12");
   asm volatile("pop r11");
   asm volatile("pop r10");
   asm volatile("pop r9");
   asm volatile("pop r8");
   asm volatile("pop r7");
   asm volatile("pop r6");
   asm volatile("pop r5");
   asm volatile("pop r4");
   asm volatile("pop r3");
   asm volatile("pop r2");

#if DEBUG_CS
   set_cursor(++row, 1);
   print_string("Printing out SP before return: ");
   print_int(SP);

   set_cursor(++row, 1);
   print_string("Back to os_start");
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[1]);
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[2]);

   set_cursor(++row, 1);
   print_string("To blink");
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[3]);
   set_cursor(++row, 1);
   print_hex(((uint8_t *)SP)[4]);
#endif

   asm volatile("ret");
}

/*
 * Starts multithreading by enabling interrupts
 */
__attribute__((naked)) void thread_start(void) {
   sei(); //enable interrupts - leave as the first statement  
   start_system_timer();
   SP += 2;

#if DEBUG_TS
   set_cursor(++row, 1);
   print_string("SP before ret in thread_start: ");
   print_int(SP); 
   set_cursor(++row, 1);
   print_hex(((uint8_t*)SP)[1]);
   set_cursor(++row, 1);
   print_hex(((uint8_t*)SP)[2]);
#endif

   asm volatile("ret");
}

void os_init() {
   int i;

   system_threads.current_thread = ETHREAD;
   for (i = 0; i < MAX_THREADS; i++)
      system_threads.thread_list[i].active = 0;

   system_threads.active_threads_count = 0;
   system_threads.uptime = 0;
   cli();
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
   open_thread->stack_usage = 20;
   open_thread->stack_size = stack_size;

   open_thread->end = malloc(stack_size);
   open_thread->base = open_thread->end + stack_size - 1;
   open_thread->tos = open_thread->base - 1;
   open_thread->args = args;
   open_thread->active = 1;

   ++system_threads.active_threads_count;

   uint8_t *byte_ptr = &address;
   open_thread->tos[0] = byte_ptr[1];  // storing address in big endian format
   open_thread->tos[1] = byte_ptr[0];  // storing address in big endian format
   open_thread->tos -= M_OFFSET; // pushing registers
}

void os_start() {
   system_threads.current_thread = get_next_thread();

   if (system_threads.current_thread == ETHREAD) {
      print_string("Error: No active thread can be found");
      return;
   }

   thread_t *curr_thread = 
    &system_threads.thread_list[system_threads.current_thread];

   curr_thread->tos += M_OFFSET; // popping off registers
   SP = curr_thread->tos - 1; // setting up SP for context_switch
   context_switch(curr_thread->tos - M_OFFSET - 2, curr_thread->tos);

   thread_start();
}

uint8_t get_next_thread() {
   uint8_t next;
   uint8_t save = system_threads.current_thread;

   if (system_threads.current_thread >= MAX_THREADS)
      system_threads.current_thread = MAX_THREADS - 1;

   for (next = (system_threads.current_thread + 1) % MAX_THREADS;
    next != system_threads.current_thread && 
    !system_threads.thread_list[next].active; 
    next = (next + 1) % MAX_THREADS) 
      ;

   if (next == system_threads.current_thread && 
    !system_threads.thread_list[next].active) {
      system_threads.current_thread = save;
      return ETHREAD;
   }

   return next;
}
