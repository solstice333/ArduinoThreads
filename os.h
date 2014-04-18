#ifndef OS_H
#define OS_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "globals.h"

#define MAX_THREADS 8

//This structure defines the register order pushed to the stack on a
//system context switch.
typedef struct regs_context_switch {
   uint8_t padding; //stack pointer is pointing to 1 byte below the top of the stack

   //Registers that will be managed by the context switch function
   uint8_t r29;
   uint8_t r28;
   uint8_t r17;
   uint8_t r16;
   uint8_t r15;
   uint8_t r14;
   uint8_t r13;
   uint8_t r12;
   uint8_t r11;
   uint8_t r10;
   uint8_t r9;
   uint8_t r8;
   uint8_t r7;
   uint8_t r6;
   uint8_t r5;
   uint8_t r4;
   uint8_t r3;
   uint8_t r2;
   uint8_t pch;
   uint8_t pcl;
} regs_context_switch;

//This structure defines how registers are pushed to the stack when
//the system tick interrupt occurs.  This struct is never directly 
//used, but instead be sure to account for the size of this struct 
//when allocating initial stack space
typedef struct regs_interrupt {
   uint8_t padding; //stack pointer is pointing to 1 byte below the top of the stack

   //Registers that are pushed to the stack during an interrupt service routine
   uint8_t r31;
   uint8_t r30;
   uint8_t r27;
   uint8_t r26;
   uint8_t r25;
   uint8_t r24;
   uint8_t r23;
   uint8_t r22;
   uint8_t r21;
   uint8_t r20;
   uint8_t r19;
   uint8_t r18;
   uint8_t sreg; //status register
   uint8_t r0;
   uint8_t r1;
   uint8_t pch;
   uint8_t pcl;
} regs_interrupt;

// thread_t defines per-thread information
typedef struct thread_t {
   uint8_t thread_id;
   uint16_t thread_pc;
   size_t stack_usage;
   size_t stack_size;
   uint8_t *tos;
   uint8_t *base;
   uint8_t *end;
   void *args;
   uint8_t active;
} thread_t;

// system_t contains a list of all threads running and a pointer to the
// current thread running
typedef struct system_t {
   thread_t thread_list[MAX_THREADS];
   uint8_t active_threads_count;
   uint8_t current_thread; 
   uint32_t uptime;
} system_t;

/*
 * Initialize the os by intializing |system_threads| 
 * variable and turning off interrupts until os_start() is called
 */
void os_init();

/*
 * Creates a thread by allocating space in the heap to store data belonging
 * to that thread (i.e. contents in register) and updates the static global 
 * variable |system_threads|. |address| is the pointer to the function to 
 * be ran as a thread. |args| is a pointer to the beginning of a continguous
 * block of memory that defines the arguments to be sent into the function
 * starting at |address|. For example if main() called create_thread and
 * int x and y were declared and initialized in main and then passed into 
 * func(int arg1, int arg2), then |args| would be &x. |stack_size| is 
 * the total size of the thread stack which is the total of 
 * sizeof(regs_context_switch), sizeof(regs_interrupt), and 
 * sizeof(<all arguments passed into function located at |address|>).
 */
void create_thread(uint16_t address, void* args, uint16_t stack_size);

/*
 * Turns on interrupts to allow for context switches
 */
void os_start();

/*
 * Gets the next thread. Returns the thread id if successful. Returns
 * 255 otherwise. 
 */
uint8_t get_next_thread();

#endif

