#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "globals.h"
#include "os.h"

/*
 * Blinks the on-board LED once per second
 */
void blink();

/*
 * Sends "Hello " to the host machine which can be seen with the screen 
 * command
 */
// TODO make this print stats later
void out();

int main() {
   os_init();
   create_thread(blink);
   create_thread(out);
   os_start();
   return 0;
}

void blink() {
   while (1) {
      DDRB |= 1 << 5;
      PORTB &= ~(1 << 5);
      _delay_ms(1000);
      DDRB |= 1 << 5;
      PORTB |= 1 << 5;
      _delay_ms(1000);
   }
}

void out() {
   serial_init();
   while (1) {
      _delay_ms(1000);
      print_string("Hello ");
   }
}
