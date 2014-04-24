#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "globals.h"
#include "os.h"

static volatile uint32_t system_time;

/*
 * Blinks the on-board LED for |t| milliseconds
 */
void blink(uint16_t t);

/*
 * Sends strings to the host machine which can be seen with the screen 
 * command
 */
// TODO make this print stats later
void stats();

int main() {
   uint16_t t = 500;   // arg for blink
   system_time = 0;
    
    
   serial_init();
    
   /*clear_screen();
   _delay_ms(500);
   set_cursor(1, 1);
   set_color(YELLOW);
   print_string("Entering Main");
   _delay_ms(500);
   clear_screen();*/

   os_init();
   create_thread(blink, &t, sizeof(regs_context_switch) + 
    sizeof(regs_interrupt) + sizeof(t));
   create_thread(stats, NULL, sizeof(regs_context_switch) +
    sizeof(regs_interrupt)); 
   os_start();
    
   return 0;
}

void blink(uint16_t t) {
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

void stats() {
   system_t *system_stats = get_system_stats();
   clear_screen();
   _delay_ms(500);
   set_cursor(1, 1);
   set_color(YELLOW);
   print_string("Entering Stats Loop");
   _delay_ms(500);
   clear_screen();
    
   while (1) {
      _delay_ms(500);
      set_color(RED);
      set_cursor(2, 1);
      //print_string("foofoo ");
      print_string("System time: ");
      system_time += system_stats->uptime;
      print_int32(system_time / MS_TO_SEC);
      print_string(" seconds.");
      /*_delay_ms(1000);
      set_color(GREEN);
      set_cursor(2, 1);
      print_string("Number of interrupts: ");
      print_int32(system_stats->numInterrupts);
      _delay_ms(1000);
      clear_screen();*/
       
   }
}
