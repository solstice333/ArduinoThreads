#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "globals.h"
#include "os.h"

/*
 * Blinks the on-board LED for |t| milliseconds
 */
void blink(int t);

/*
 * Sends "Hello " to the host machine which can be seen with the screen 
 * command
 */
// TODO make this print stats later
void out();

int main() {
   int t = 500;   // arg for blink

   serial_init();
   os_init();
   create_thread(blink, &t, sizeof(regs_context_switch) + 
    sizeof(regs_interrupt) + sizeof(t));
   // create_thread(out);
   // os_start();
   return 0;
}

void blink(int t) {
   int i;

   while (1) {
      DDRB |= 1 << 5;
      PORTB &= ~(1 << 5);

      for (i = 0; i < t; i++)
         _delay_ms(1);

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
