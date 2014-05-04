#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "globals.h"
#include "os.h"
#include "synchro.h"

#define TOTAL_COLORS 7
#define COL_WIDTH 30
#define STACKSIZE 4 * (sizeof(regs_context_switch) + sizeof(regs_interrupt))
#define BUF_SIZE 5

/*
 * Shared buffer
 */
static int buffer[BUF_SIZE];

/*
 * Mutex to be used for the buffer
 */
static mutex_t buffer_lock;

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
 * filler thread
 */
void filler();

/*
 * producer thread
 */
void producer();

/*
 * consumer thread
 */
void consumer();

int main() {
   uint16_t t = 500;   // arg for blink
   memset(buffer, 0, BUF_SIZE * sizeof(int));

   os_init();
   serial_init();

   mutex_init(&buffer_lock);

   // create_thread(blink, &t, STACKSIZE + sizeof(t));
   // create_thread(stats, NULL, STACKSIZE);
   create_thread(filler, NULL, STACKSIZE);
   create_thread(producer, NULL, STACKSIZE);
   create_thread(consumer, NULL, STACKSIZE);

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
      _delay_ms(500);

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
            print_string("Thread ID: ");
            print_int(sys_threads->thread_list[i].thread_id); 

            set_cursor(9, col + i*COL_WIDTH);
            print_string("Thread PC: 0x");
            print_hex(sys_threads->thread_list[i].thread_pc);

            set_cursor(10, col + i*COL_WIDTH);
            print_string("Stack Usage: ");
            print_int(sys_threads->thread_list[i].stack_usage);

            set_cursor(11, col + i*COL_WIDTH);
            print_string("Total Stack Size: ");
            print_int(sys_threads->thread_list[i].stack_size);

            set_cursor(12, col + i*COL_WIDTH);
            print_string("Current Top Of Stack: 0x");
            print_hex(sys_threads->thread_list[i].tos);

            set_cursor(13, col + i*COL_WIDTH);
            print_string("Stack Base: 0x");
            print_hex(sys_threads->thread_list[i].base);

            set_cursor(14, col + i*COL_WIDTH);
            print_string("Stack End: 0x");
            print_hex(sys_threads->thread_list[i].end);
         }
      }
   }
}

void filler() {
   while (true)
      ;
}

void producer() {
   srand(2);

   while (true) {
      mutex_lock(&buffer_lock);

      set_cursor(1, 1);
      print_string("Writing to buffer ");
      print_int(system_threads.uptime_s);

      buffer[rand() % BUF_SIZE] = rand() % 90 + 10;

      mutex_unlock(&buffer_lock);
      _delay_ms(1000);
   }
}

void consumer() {
   while (true) {
      mutex_lock(&buffer_lock);

      set_cursor(2, 1);
      print_string("Buffer contents: ");

      int i;
      for (i = 0; i < BUF_SIZE; i++) {
         print_int(buffer[i]);
         print_string(" ");
      }

      // mutex_unlock(&buffer_lock);
      _delay_ms(1000);
   }
}
