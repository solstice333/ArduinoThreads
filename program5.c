#include <stdlib.h>
#include <avr/io.h>
#include <assert.h>
#include "globals.h"
#include "os.h"
#include "synchro.h"
#include "ext2reader.h"

#define PLAYBACK_DEBUG 1
#define READ_DEBUG 0

#define STACKSIZE 4 * (sizeof(regs_context_switch) + sizeof(regs_interrupt))

uint8_t p_buffer[256];
uint8_t c_buffer[256];

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

#if PLAYBACK_DEBUG
   create_thread(playback, NULL, STACKSIZE);
#elif READ_DEBUG
   create_thread(read, NULL, STACKSIZE);
#endif
   // create_thread(display, NULL, STACKSIZE);
   create_thread(idle, NULL, STACKSIZE);
   os_start();

   return 0;
}

void playback() {
   srand(452);

   while (true) {
      OCR2B = rand() % 256;   // this should sound like really strong rain
      thread_sleep(0);
   }
}

void read() {
   uint32_t *blocks = get_root();

   while (true) {
      list_entries(blocks);
   }
}

void display() {
   while (true) {
      print_string("foo ");
   }
}

void idle() {
   while (true)
      ;
}
