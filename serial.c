#include "globals.h"

/*
 * Converts |num| to its ASCII decimal value and stores it in string format 
 * (null-terminated) inside |container| assuming there is enough space within 
 * |container|. |type| can be uint8_t, uint16_t or uint32_t which describes 
 * the type of |num| and the data contents within the array |container|. Any
 * other type used will result in undefined behavior.
 */
#define numToAsciiDecimal(type, num, container) {\
   type __copy = (num), __quo = (num);\
   int __i, __length;\
   \
   for (__length = 0; __copy || !__length; __length++)\
      __copy /= 10;\
   \
   for (__i = __length - 1; __i >= 0; __i--) {\
      (container)[__i] = (__quo) % 10 + '0';\
      (__quo) /= 10;\
   }\
   \
   (container)[__length] = 0;\
}\

/*
 * Converts |num| to its ASCII hexadecimal value and stores it in string format
 * (null-terminated) inside |container| assuming there is enough space within 
 * |container|. |type| can be uint8_t, uint16_t, or uint32_t which describes 
 * the type of |num| and the data contents within the array |container|. Any 
 * other type used will result in undefined behavior.
 */
#define numToAsciiHex(type, num, container) {\
   type __copy = (num), __quo = (num), __rem;\
   int __i, __length;\
   \
   for (__length = 0; __copy || !__length; __length++)\
      __copy /= 16;\
   \
   for (__i = __length - 1; __i >= 0; __i--) {\
      __rem = (__quo) % 16;\
   \
      switch (__rem) {\
         case 10:\
            __rem = 'A';\
            break;\
         case 11:\
            __rem = 'B';\
            break;\
         case 12:\
            __rem = 'C';\
            break;\
         case 13:\
            __rem = 'D';\
            break;\
         case 14:\
            __rem = 'E';\
            break;\
         case 15:\
            __rem = 'F';\
            break;\
         default:\
            __rem += '0';\
      }\
      (container)[__i] = __rem;\
      (__quo) /= 16;\
   }\
   (container)[__length] = 0;\
}\

/*
 * Initialize the serial port.
 */
void serial_init() {
   uint16_t baud_setting;

   UCSR0A = _BV(U2X0);
   baud_setting = 16; //115200 baud

   // assign the baud_setting
   UBRR0H = baud_setting >> 8;
   UBRR0L = baud_setting;

   // enable transmit and receive
   UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
}

/*
 * Return 1 if a character is available else return 0.
 */
uint8_t byte_available() {
   return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

/*
 * Unbuffered read
 * Return 255 if no character is available otherwise return available character.
 */
uint8_t read_byte() {
   if (UCSR0A & (1 << RXC0)) return UDR0;
   return 255;
}

/*
 * Unbuffered write
 *
 * b byte to write.
 */
uint8_t write_byte(uint8_t b) {
   //loop until the send buffer is empty
   while (((1 << UDRIE0) & UCSR0B) || !(UCSR0A & (1 << UDRE0))) {}

   //write out the byte
   UDR0 = b;
   return 1;
}

void print_string(char *s) {
   while (*s)
      write_byte(*s++);
}

void print_int(uint16_t i) {
   uint16_t iAsciiDec[16], *runner = iAsciiDec;

   numToAsciiDecimal(uint16_t, i, iAsciiDec);
   while(*runner)
      write_byte(*runner++);
}

void print_int32(uint32_t i) {
   uint32_t iAsciiDec[16], *runner = iAsciiDec;

   numToAsciiDecimal(uint32_t, i, iAsciiDec);
   while(*runner)
      write_byte(*runner++);
}

void print_hex(uint16_t i) {
   uint16_t iAsciiHex[16], *runner = iAsciiHex;

   numToAsciiHex(uint16_t, i, iAsciiHex);
   while(*runner)
      write_byte(*runner++);
}

void print_hex32(uint32_t i) {
   uint32_t iAsciiHex[16], *runner = iAsciiHex;

   numToAsciiHex(uint32_t, i, iAsciiHex);
   while(*runner)
      write_byte(*runner++);
}

void set_cursor(uint8_t row, uint8_t col) {
   uint8_t rowAscii[3], colAscii[3], *runner;

   numToAsciiDecimal(uint8_t, row, rowAscii);
   numToAsciiDecimal(uint8_t, col, colAscii);

   write_byte(ESC);
   write_byte('[');

   runner = rowAscii;
   while (*runner)
      write_byte(*runner++);

   write_byte(';');

   runner = colAscii;
   while (*runner)
      write_byte(*runner++);

   write_byte('H');
}

void set_color(uint8_t color) {
   uint8_t asciiNum[3], *runner;
   numToAsciiDecimal(uint8_t, color, asciiNum);

   write_byte(ESC);
   write_byte('[');

   runner = asciiNum;
   while (*runner)
     write_byte(*runner++);

   write_byte('m');
}

void clear_screen() {
   write_byte(ESC);
   write_byte('[');
   write_byte('2');
   write_byte('J');
}
