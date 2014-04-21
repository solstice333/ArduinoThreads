#include "os.h"
#include "globals.h"

#define DEBUG 0
#define DEBUG_CS 1

#define PRINT_ITER 1

#if DEBUG_CS
   static volatile int count = 1;
#endif

// global system_t variable to track register contents belonging to threads
static volatile system_t system_threads;

//This interrupt routine is automatically run every 10 milliseconds
ISR(TIMER0_COMPA_vect) {
   //The following statement tells GCC that it can use registers r18-r27, 
   //and r30-31 for this interrupt routine.  These registers (along with
   //r0 and r1) will automatically be pushed and popped by this interrupt 
   // routine.
   asm volatile ("" : : : "r18", "r19", "r20", "r21", "r22", "r23", "r24", \
                 "r25", "r26", "r27", "r30", "r31");                        

   uint8_t old_id = system_threads.current_thread;
   uint8_t new_id = get_next_thread();

   uint8_t *old_tos = system_threads.thread_list[old_id].tos = SP + 1;

   if (old_id == new_id)
      context_switch(old_tos - M_OFFSET - PC_OFFSET, old_tos);
   else {
      uint8_t *new_tos = system_threads.thread_list[new_id].tos;
      context_switch(new_tos, old_tos);
   }
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
 * |old_tp| is the old top of stack pointed to. Prior to calling context_switch,
 * the stack pointer should be pointing to a valid spot within the stack 
 * associated to |old_tp|. Secondly, if the stack associated to |new_tp| is 
 * new and has just been initialized by create_thread(), then it is assumed that
 * interrupts need to be enabled again, and thus, context_switch will call 
 * sei(). The |tos| pointers for the thread_t objects associated to |new_tp| 
 * and |old_tp| will also be updated given that |new_tp| associates to the 
 * thread stack immediately after |old_tp|'s associating thread stack or if
 * |old_tp| and |new_tp| point to the same thread stack. 
 */
__attribute__((naked)) void context_switch(uint16_t* new_tp, uint16_t* old_tp) {
// TODO move all C code out of here
#if DEBUG_CS
   if (count == PRINT_ITER) {
      set_cursor(5, 1);
      print_string("SP position before assigning old_tp: ");
      print_int(SP);
   }
#endif

   SP = (uint8_t *)old_tp - PC_OFFSET - 1;

#if DEBUG_CS
   if (count == PRINT_ITER) {
      set_cursor(6, 1);
      print_string("SP position after assigning old_tp: ");
      print_int(SP);
   }
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
   if (count == PRINT_ITER) {
      set_cursor(7, 1);
      print_string("SP position after pushing: ");
      print_int(SP);
   }
#endif

   // update current thread's stack |tos| to now
   thread_t *curr_thread = 
    &system_threads.thread_list[system_threads.current_thread];
   curr_thread->tos = SP + 1;
   curr_thread->stack_usage = curr_thread->base - curr_thread->tos + 1; 

   // switch SP to |new_tp|
   SP = (uint8_t *)new_tp - 1;

   // check if SP is within current thread's stack or next thread's stack
   // and update |tos| and |stack_usage| accordingly to how they would be
   // at the exit of context_switch()
   uint8_t curr_id = system_threads.current_thread;
   uint8_t next_id = get_next_thread();

   if (system_threads.thread_list[next_id].base > SP &&  // next thread
    system_threads.thread_list[next_id].end <= SP) {   
      curr_thread = &system_threads.thread_list[next_id];      
      curr_thread->tos = SP + M_OFFSET + PC_OFFSET + 1;
      system_threads.current_thread = curr_id = next_id;
      curr_thread->stack_usage = curr_thread->base - curr_thread->tos + 1;
   }
   else if (system_threads.thread_list[curr_id].base > SP &&   // same thread
    system_threads.thread_list[curr_id].end <= SP) {
      curr_thread = &system_threads.thread_list[curr_id];
      curr_thread->tos = SP + M_OFFSET + PC_OFFSET + 1;
      curr_thread->stack_usage = curr_thread->base - curr_thread->tos + 1;
   }

#if DEBUG_CS
   if (count == PRINT_ITER) {
      set_cursor(8, 1);
      print_string("SP position after switching to new_tp: ");
      print_int(SP);

      set_cursor(9, 1);
      print_string("base address for stats: ");
      print_int(system_threads.thread_list[1].base);
      set_cursor(10, 1);
      print_string("end address for stats: ");
      print_int(system_threads.thread_list[1].end);
      
      set_cursor(11, 1);
      print_string("base address for blink: ");
      print_int(system_threads.thread_list[0].base);
      set_cursor(12, 1);
      print_string("end address for blink: ");
      print_int(system_threads.thread_list[0].end);
   }
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
   curr_thread = &system_threads.thread_list[0];
   if (count++ == PRINT_ITER) {
      set_cursor(13, 1);
      print_string("Printing out SP before return: ");
      print_int(SP);
      set_cursor(14, 1);
      print_string("tos of blink thread stack: ");
      print_int(system_threads.thread_list[0].tos);
      set_cursor(15, 1);
      print_string("tos of stats thread stack: ");
      print_int(system_threads.thread_list[1].tos);
      set_cursor(16, 1);
      print_string("stack usage of blink thread stack: ");
      print_int(system_threads.thread_list[0].stack_usage);
      set_cursor(17, 1);
      print_string("stack usage of stats thread stack: ");
      print_int(system_threads.thread_list[1].stack_usage);
      set_cursor(18, 1);
      print_string("curr_thread->thread_id: ");
      print_int(curr_thread->thread_id);
      set_cursor(19, 1);
      print_string("curr_thread->new_thread: ");
      print_int(curr_thread->new_thread);
 
      exit(0);
   }
#endif

   // && curr_thread->thread_id) {
   if (system_threads.thread_list[system_threads.current_thread].new_thread) {
      system_threads.thread_list[system_threads.current_thread].new_thread = false;

#if DEBUG_CS
      if (count++ == PRINT_ITER) {
         set_cursor(20, 1);
         print_string("getting here in cs");
         exit(0);
      }
#endif
      thread_start();
   }

   asm volatile("ret");
}

/*
 * Starts multithreading by enabling interrupts
 */
__attribute__((naked)) void thread_start(void) {
   sei(); //enable interrupts - leave as the first statement  
   SP += PC_OFFSET;
   asm volatile("ret");
}

void os_init() {
   int i;

   system_threads.current_thread = ETHREAD;
   for (i = 0; i < MAX_THREADS; i++)
      system_threads.thread_list[i].active = false;

   system_threads.active_threads_count = 0;
   system_threads.uptime = 0;
   cli();
}

void create_thread(uint16_t address, void *args, uint16_t stack_size) {
   int idx;
   thread_t *open_thread;
   uint8_t *byte_ptr;

   // look for an inactive thread
   for (idx = 0; idx < MAX_THREADS && 
    system_threads.thread_list[idx].active; idx++) 
      ;

   if (idx == MAX_THREADS) {
      print_string("Error: Maximum amount of threads has been reached");
      return;
   }

   open_thread = &system_threads.thread_list[idx];
   open_thread->thread_id = idx;
   open_thread->thread_pc = address;
   open_thread->stack_usage = INIT_SIZE;
   open_thread->stack_size = stack_size;
   
   // base represents the bottom of the stack and 
   // end represents the top of the stack
   open_thread->end = malloc(stack_size);
   open_thread->base = open_thread->end + stack_size - 1;
   open_thread->tos = open_thread->base - 1;
   open_thread->args = args;
   open_thread->active = true;
   open_thread->new_thread = true;

   ++system_threads.active_threads_count;

   byte_ptr = &address;
   open_thread->tos[0] = byte_ptr[1];  // storing address in big endian format
   open_thread->tos[1] = byte_ptr[0];  // storing address in big endian format
   open_thread->tos -= M_OFFSET; // simulate pushing registers
}

void os_start() {
   system_threads.current_thread = get_next_thread();

   if (system_threads.current_thread == ETHREAD) {
      print_string("Error: No active thread can be found");
      return;
   }

   thread_t *curr_thread = 
    &system_threads.thread_list[system_threads.current_thread];

   curr_thread->tos += M_OFFSET; // simulate popping off registers
   SP = curr_thread->tos - 1; // setting up SP for context_switch
   context_switch(curr_thread->tos - M_OFFSET - PC_OFFSET, curr_thread->tos);

   start_system_timer();
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
