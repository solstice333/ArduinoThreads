#ifndef OS_H
#define OS_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "globals.h"
#include "os_util.h"

#define MAX_THREADS 8
#define M_OFFSET 18  // offset for manually saved registers
#define A_OFFSET 18  // offset for automatically saved registers
#define PC_OFFSET 2  // offset related to automatic push of pc
#define ARGS 2       // number of args
#define ETHREAD 255  // error thread id
#define INIT_SIZE 20 // initial stack size
#define REMAINING 15 // remaining spots in stack during create_thread()
#define GARBAGE_SIZE 32 // garbage size for first context switch
#define SEC 100   // expected interrupts per second

// enum for thread states
typedef enum {
   THREAD_RUNNING, THREAD_READY, THREAD_SLEEPING, THREAD_WAITING 
} thread_state;

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
   uint8_t padding[16]; //stack pointer is pointing to 1 byte below the top of the stack

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
   uint16_t interrupted_pc;
   uint8_t stack_usage;
   uint16_t stack_size;
   uint8_t *tos;
   uint8_t *base;
   uint8_t *end;
   bool active;
   thread_state t_state;
   uint16_t interrupt_slept;
   uint32_t run_count;
   uint16_t run_per_second;
} thread_t;

// system_t contains a list of all threads running and a pointer to the
// current thread running
typedef struct system_t {
   thread_t thread_list[MAX_THREADS];
   uint8_t active_threads_count;
   uint8_t current_thread; 
   uint32_t interrupts;
   uint32_t uptime_s;
   uint8_t interrupts_per_sec;
   uint8_t num_threads;
} system_t;

// global system_t variable to track register contents belonging to threads
extern volatile system_t system_threads;

/*
 * Initialize the os
 */
void os_init();

/*
 * Creates a thread given the starting address of the function to be ran on
 * a separate thread |addresss|, the pointer to the first argument (stored
 * contiguously on the runtime stack) |args|, and the |stack_size| given
 * in bytes. |stack_size| needs to be at least sizeof(regs_context_switch) + 
 * sizeof(regs_interrupt) + sizeof(<any parameters belonging to the thread
 * function>), all multiplied by 4.
 */
void create_thread(uint16_t address, void* args, uint16_t stack_size);

/*
 * Starts the os
 */
void os_start();

/*
 * Gets the next thread. Returns the thread id of the next thread if 
 * successful. Returns ETHREAD otherwise. 
 */
uint8_t get_next_thread();

/*
 * DEPRECATED
 * Returns a pointer to |system_threads|. Use only as read-only. As of
 * program 3, system stats has been made available as an extern global
 * variable via |system_threads| so that the synchronization functions can
 * freely manipulate them. This function is thus deprecated.
 */
system_t *get_system_stats();

/*
 * Immediately calls context_switch to exit out of thread and switch to
 * thread with id |next_thread|. Used primarily when thread state transitions 
 * to THREAD_SLEEPING or THREAD_WAITING to prevent deadlocking. The thread
 * belonging to id |next_thread| is switched to THREAD_RUNNING. The caller
 * is responsible for setting the state of the previous thread. 
 */
void yield(uint8_t next_thread);

/*
 * Puts the thread to sleep for |ticks| number of interrupts
 */
void thread_sleep(uint16_t ticks);

#endif
