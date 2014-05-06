#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "globals.h"
#include "os.h"
#include "synchro.h"

#define DEBUG 1

#define TOTAL_COLORS 7
#define COL_WIDTH 12
#define STACKSIZE 4 * (sizeof(regs_context_switch) + sizeof(regs_interrupt))
#define BUF_SIZE 10
#define SEED 4

int buffer[BUF_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Shared buffer
int consumed = 0;   // Value consumed
mutex_t buffer_lock;   // Mutex to be used for the buffer
mutex_t print_lock; // Only one thread printing to screen at once 
mutex_t is_producing; // locked if producing
semaphore_t empty;  // Semaphore for checking empty spots in buffer
semaphore_t full;   // Semaphore for checking full spots in buffer
int p_sleep = 100;  // Production rate related
int c_sleep = 100;  // Consuming rate related

/*
 * Filler thread. Use this as the default thread when no other threads are
 * running. As of program 3, it is unsafe to run any thread without this
 * filler thread function
 */
void filler();

/*
 * Blinks the on-board LED for |t| milliseconds
 */
void blink();

/*
 * Sends strings to the host machine which can be seen with the screen 
 * command
 */
void display_stats();

/*
 * Producer thread
 */
void producer();

/*
 * Consumer thread
 */
void consumer();

/*
 * Prints bounded buffer
 */
void display_bounded_buffer();

int main() {
   os_init();
   serial_init();
   clear_screen();

   mutex_init(&buffer_lock);
   mutex_init(&print_lock);
   mutex_init(&is_producing);
   sem_init(&empty, BUF_SIZE);
   sem_init(&full, 0);

   create_thread(blink, NULL, STACKSIZE);
   create_thread(filler, NULL, STACKSIZE);
   create_thread(producer, NULL, STACKSIZE);
   create_thread(consumer, NULL, STACKSIZE);
   create_thread(display_stats, NULL, STACKSIZE);
   create_thread(display_bounded_buffer, NULL, STACKSIZE);

   os_start();
   return 0;
}

void filler() {
   while (true)
      ;
}

void blink() {
   DDRB |= 1 << 5;

   while (true) {
      PORTB |= 1 << 5;  // on
      mutex_lock(&is_producing);
      PORTB &= ~(1 << 5); // off
      mutex_unlock(&is_producing);
   }
}

void display_stats() {
   system_t *sys_threads = get_system_stats();
   clear_screen();

   while (true) {
      uint8_t input = read_byte();
      if (input != 255) {
         switch (input) {
            // production rate 'a' - faster, 'z' - slower
            case 'a':
               if (p_sleep)
                  p_sleep -= 10; 
               break;
            case 'z':
               p_sleep += 10; 
               break;

            // consumer rate 'k' - faster, 'm' - slower
            case 'k':
               if (c_sleep)
                  c_sleep -= 10;
               break;
            case 'm':
               c_sleep += 10;
               break;
         }
      }

      mutex_lock(&print_lock);

      set_color(WHITE);
      set_cursor(1, 1);
      print_string("uptime: ");
      print_int32(sys_threads->uptime_s);
   
      set_cursor(2, 1);
      print_string("ips: ");
      print_int(sys_threads->interrupts_per_sec);

      set_cursor(3, 1);
      print_string("threads: ");
      print_int(sys_threads->num_threads);

      int i;
      uint8_t col = 1;
      for (i = 0; i < MAX_THREADS; i++) {
         if (sys_threads->thread_list[i].active) {
            set_color(i%TOTAL_COLORS + RED);

            set_cursor(5, col + i*COL_WIDTH);
            print_string("id: ");
            print_int(sys_threads->thread_list[i].thread_id); 

            set_cursor(6, col + i*COL_WIDTH);
            print_string("fpc: 0x");
            print_hex(sys_threads->thread_list[i].thread_pc);

            set_cursor(7, col + i*COL_WIDTH);
            print_string("ipc: 0x");
            print_hex(sys_threads->thread_list[i].interrupted_pc);

            set_cursor(8, col + i*COL_WIDTH);
            print_string("usg: ");
            print_int(sys_threads->thread_list[i].stack_usage);

            set_cursor(9, col + i*COL_WIDTH);
            print_string("size: ");
            print_int(sys_threads->thread_list[i].stack_size);

            set_cursor(10, col + i*COL_WIDTH);
            print_string("tos: 0x");
            print_hex(sys_threads->thread_list[i].tos);

            set_cursor(11, col + i*COL_WIDTH);
            print_string("bs: 0x");
            print_hex(sys_threads->thread_list[i].base);

            set_cursor(12, col + i*COL_WIDTH);
            print_string("end: 0x");
            print_hex(sys_threads->thread_list[i].end);

            set_cursor(13, col + i*COL_WIDTH);
            print_string("rps: ");
            print_int(sys_threads->thread_list[i].run_per_second);
         }
      }

      mutex_unlock(&print_lock);

      thread_sleep(25);
   }
}

void producer() {
   srand(SEED);
   int in = 0;

   while (true) {
      sem_wait(&empty);
      mutex_lock(&buffer_lock);
      mutex_lock(&is_producing);

      buffer[in] = rand() % 90 + 10;
      in = (in + 1) % BUF_SIZE;

      thread_sleep(p_sleep);

      mutex_unlock(&is_producing);
      mutex_unlock(&buffer_lock);
      sem_signal_swap(&full);
   }
}

void consumer() {
   int out = 0;

   while (true) {
      sem_wait(&full);
      mutex_lock(&buffer_lock);

      thread_sleep(c_sleep);

      consumed = buffer[out];
      out = (out + 1) % BUF_SIZE;

      mutex_unlock(&buffer_lock);
      sem_signal_swap(&empty);
   }
}

void display_bounded_buffer() {
   int i;

   while (true) {
      mutex_lock(&print_lock);
      set_color(WHITE);

      set_cursor(15, 1);
      print_string("buffer: ");

      for (i = 0; i < BUF_SIZE; i++) {
         print_int(buffer[i]);
         print_string(" ");
      }

      set_cursor(16, 1);
      print_string("consumed: ");
      print_int(consumed);

      set_cursor(17, 1);
      print_string("prod rate: 1 item / ");
      print_string("       ");
      set_cursor(17, 21);
      print_int(p_sleep * 10);
      print_string(" ms");

      set_cursor(18, 1);
      print_string("cons rate: 1 item / ");
      print_string("       ");
      set_cursor(18, 21);
      print_int(c_sleep * 10);
      print_string(" ms");

      mutex_unlock(&print_lock);

      thread_sleep(25);
   }
}
