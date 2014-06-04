#include <stdlib.h>
#include <avr/io.h>
#include <assert.h>
#include "globals.h"
#include "os.h"
#include "SdInfo.h"
#include "SdReader.h"
#include "synchro.h"

#define DEBUG 1
#define STACKSIZE 4 * (sizeof(regs_context_switch) + sizeof(regs_interrupt))

int playback_global = 0;
int read_global = 0;

/*
 * Audio playback thread
 */
void playback();

/*
 * SD card read thread
 */
void read();

/*
 * Console display thread
 */
void display();

/*
 * Idle thread
 */
void idle();

int main(void) {
   uint8_t sd_card_status;

   // initialize the card with fast clock. If this does not work, try
   // sdInit(1) for a slower clock
   sd_card_status = sdInit(0);   

   serial_init(); 
   if (!sd_card_status) {
      print_string("Error: Failed to initialize SD card");
      exit(1);
   }

   start_audio_pwm();
   os_init();
   clear_screen();

   create_thread(playback, NULL, STACKSIZE);
   create_thread(read, NULL, STACKSIZE);
   create_thread(display, NULL, STACKSIZE);
   create_thread(idle, NULL, STACKSIZE);
   os_start();

   return 0;
}

void playback() {
   while (true) {
      playback_global = !playback_global;
      thread_sleep(1);
   }
}

void read() {
   while (true) {
      read_global = !read_global;
      thread_sleep(2);
   }
}

void display() {
   while (true) {
      set_cursor(1, 1);
      print_string("playback_global: ");
      print_int(playback_global);
      print_string(" ");

      set_cursor(2, 1);
      print_string("read_global: ");
      print_int(read_global);
      print_string(" ");

      thread_sleep(10999);
   }
}

void idle() {
   while (true)
      ;
}
