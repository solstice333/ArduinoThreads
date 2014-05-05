#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "globals.h"
#include "os.h"
#include "synchro.h"

#define DEBUG 1
#define TOTAL_COLORS 7
#define COL_WIDTH 11
#define STACKSIZE 4 * (sizeof(regs_context_switch) + sizeof(regs_interrupt))
#define BUF_SIZE 5

static int buffer = 0;  // Shared buffer
static mutex_t buffer_lock;   // Mutex to be used for the buffer
static semaphore_t empty;
static semaphore_t full; 

/*
 * Blinks the on-board LED for |t| milliseconds
 */
void blink(uint16_t t);

/*
 * Sends strings to the host machine which can be seen with the screen 
 * command
 */
void stats();

/*
 * Filler thread. Use this as the default thread when no other threads are
 * running. As of program 3, it is unsafe to run any thread without this
 * filler thread function
 */
void filler();

/*
 * Producer thread
 */
void producer();

/*
 * Consumer thread
 */
void consumer();

/*
 * Prints any debug information
 */
void debug_print();

int main() {
   uint16_t t = 500;   // arg for blink

   os_init();
   serial_init();
   clear_screen();

   mutex_init(&buffer_lock);
   sem_init(&empty, 1);
   sem_init(&full, 0);

   create_thread(blink, &t, STACKSIZE + sizeof(t));
   create_thread(filler, NULL, STACKSIZE);
   create_thread(producer, NULL, STACKSIZE);
   create_thread(consumer, NULL, STACKSIZE);
   create_thread(stats, NULL, STACKSIZE);

   os_start();
   return 0;
}

void blink(uint16_t t) {
   t = t/10;
   DDRB |= 1 << 5;

   while (true) {
      PORTB &= ~(1 << 5); // off
      thread_sleep(t);
      PORTB |= 1 << 5;  // on
      thread_sleep(t);
   }
}

void stats() {
   system_t *sys_threads = get_system_stats();
   clear_screen();

   while (true) {
      cli();

      thread_sleep(50);
      set_color(WHITE);

      set_cursor(1, 1);
      print_string("System Stats: ");

      set_cursor(2, 1);
      print_string("System Time: ");
      print_int32(sys_threads->uptime_s);
   
      set_cursor(3, 1);
      print_string("Interrupts Per Second: ");
      print_int(sys_threads->interrupts_per_sec);

      set_cursor(4, 1);
      print_string("Number Of Threads In The System: ");
      print_int(sys_threads->num_threads);

      set_cursor(7, 1);
      print_string("Per Thread Information: ");

      int i;
      uint8_t col = 1;
      for (i = 0; i < MAX_THREADS; i++) {
         if (sys_threads->thread_list[i].active) {
            set_color(i%TOTAL_COLORS + RED);

            set_cursor(8, col + i*COL_WIDTH);
            print_string("id: ");
            print_int(sys_threads->thread_list[i].thread_id); 

            set_cursor(9, col + i*COL_WIDTH);
            print_string("pc: 0x");
            print_hex(sys_threads->thread_list[i].thread_pc);

            set_cursor(10, col + i*COL_WIDTH);
            print_string("usg: ");
            print_int(sys_threads->thread_list[i].stack_usage);

            set_cursor(11, col + i*COL_WIDTH);
            print_string("size: ");
            print_int(sys_threads->thread_list[i].stack_size);

            set_cursor(12, col + i*COL_WIDTH);
            print_string("tos: 0x");
            print_hex(sys_threads->thread_list[i].tos);

            set_cursor(13, col + i*COL_WIDTH);
            print_string("bs: 0x");
            print_hex(sys_threads->thread_list[i].base);

            set_cursor(14, col + i*COL_WIDTH);
            print_string("end: 0x");
            print_hex(sys_threads->thread_list[i].end);
         }
      }

      thread_t *id_wl;
      set_color(WHITE);

      set_cursor(18, 1);
      print_string("thread id currently on buffer_lock waitlist: ");
      if (Queue_empty(buffer_lock.waitlist))
         print_string(" ");
      else {
         id_wl = Queue_peek(buffer_lock.waitlist);
         print_int(id_wl->thread_id);
      }

      set_cursor(19, 1);
      print_string("thread id currently on sem full waitlist: ");
      if (Queue_empty(full.waitlist))
         print_string(" ");
      else {
         id_wl = Queue_peek(full.waitlist);
         print_int(id_wl->thread_id);
      }

      set_cursor(20, 1);
      print_string("thread id currently on sem empty waitlist: ");
      if (Queue_empty(empty.waitlist))
         print_string(" ");
      else {
         id_wl = Queue_peek(empty.waitlist);
         print_int(id_wl->thread_id);
      }

      set_cursor(21, 1);
      print_string("thread states: ");
      for (i = 0; i < MAX_THREADS; i++) {
         print_int(system_threads.thread_list[i].t_state); 
         print_string(" ");
      }

      sei();
   }
}

void filler() {
   while (true)
      ;
}

void producer() {
   srand(2);

   while (true) {
      sem_wait(&empty);
      mutex_lock(&buffer_lock);

      buffer = rand() % 90 + 10;

      mutex_unlock(&buffer_lock);
      sem_signal(&full);

      thread_sleep(100);
   }
}

void consumer() {
   while (true) {
      sem_wait(&full);
      mutex_lock(&buffer_lock);

      cli();
      set_cursor(17, 1);
      set_color(WHITE);
      print_string("Buffer contents ");
      print_string(": ");
      print_int(buffer);
      sei();

      mutex_unlock(&buffer_lock);
      sem_signal(&empty);

      thread_sleep(100);
   }
}
