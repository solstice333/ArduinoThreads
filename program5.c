#include <stdlib.h>
#include <avr/io.h>
#include <assert.h>
#include "globals.h"
#include "os.h"
#include "SdInfo.h"
#include "SdReader.h"
#include "synchro.h"

#define DEBUG 1

int main(void) {
   uint8_t sd_card_status;

   sd_card_status = sdInit(0);   //initialize the card with fast clock
                                 //if this does not work, try sdInit(1)
                                 //for a slower clock
   serial_init(); 

#if DEBUG
   print_string("sd_card_status: ");
   print_int(sd_card_status);
#endif

   start_audio_pwm();
   os_init();
   
   return 0;
}
