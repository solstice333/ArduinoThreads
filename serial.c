#include <avr/io.h>
#include "globals.h"

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

/*
 * Writes each byte of the string until a null is reached.
 */
void print_string(char* s) {
    
    while (*s) {
        write_byte(*s++);
    }
}

/*
 * Reverses the order of a string
 */
static void reverse_string(char *org, int length) {
    char temp[STR32];
    char *ptr = temp;
    int strlen = length;
    
    while (--length >= 0) {
        memcpy(ptr++, org + length, sizeof(char));
    }
    
    memcpy(org, temp, strlen * sizeof(char));
}

/*
 * Converts an integer into a string, the contents of the string
 * is in reverse order.  Use reverse_string to reverse the string.
 */
static void int_to_string(uint32_t num, char *str) {
    uint32_t quo = num, rem = 0;
    int ndx = 0;
    
    //divide num by 10 until you get a quotient of 0
    //storing each reminder as a character of the string
    do {
        
        rem = quo % DECBASE;
        quo = quo / DECBASE;
        
        memset(str + ndx++, rem + '0', sizeof(char));
        
    } while (quo);
    
    memset(str + ndx, '\0', sizeof(char));
    reverse_string(str, ndx);
}

/*
 * Converts an integer into a string. Uses int_to_string
 */
void print_int32(uint32_t i) {
    char dec[STR32] = "";
    
    int_to_string(i, dec);
    print_string(dec);
}

/*
 * Uses print_int32
 */
void print_int(uint16_t i) {
    print_int32(i);
}

/*
 * Converts an integer into a string, the string is in hex format.
 */
void print_hex(uint16_t i) {
    uint16_t quo = i, rem = 0;
    int ndx = HEXSTR - 2;
    char hex[HEXSTR] = "0x0000";
    
    //divide num by 16 until you get a quotient of 0
    //storing each reminder as a character of the string
    do {
        
        rem = quo % HEXBASE;
        quo = quo / HEXBASE;
        
        if (rem == 10)
            hex[ndx] = 'A';
        else if (rem == 11)
            hex[ndx] = 'B';
        else if (rem == 12)
            hex[ndx] = 'C';
        else if (rem == 13)
            hex[ndx] = 'D';
        else if (rem == 14)
            hex[ndx] = 'E';
        else if (rem == 15)
            hex[ndx] = 'F';
        else
            hex[ndx] = rem + '0';
        
        ndx--;
        
    } while (quo);
    
    print_string(hex);
}

/*
 * Converts an integer into a string, the string is in hex format.
 */
void print_hex32(uint32_t i) {
    uint32_t quo = i, rem = 0;
    int ndx = STR32 - 2;
    char hex[STR32] = "0x0000";
    
    do {
        
        //divide num by 16 until you get a quotient of 0
        //storing each reminder as a character of the string
        rem = quo % HEXBASE;
        quo = quo / HEXBASE;
        
        if (rem == 10)
            hex[ndx] = 'A';
        else if (rem == 11)
            hex[ndx] = 'B';
        else if (rem == 12)
            hex[ndx] = 'C';
        else if (rem == 13)
            hex[ndx] = 'D';
        else if (rem == 14)
            hex[ndx] = 'E';
        else if (rem == 15)
            hex[ndx] = 'F';
        else
            hex[ndx] = rem + '0';
        
        ndx--;
        
    } while (quo);
    
    print_string(hex);
}

void set_cursor(uint8_t row, uint8_t col) {
    char rowStr[3] = "";
    char colStr[3] = "";
    
    //converts double digit numbers into strings
    int_to_string(row, rowStr);
    int_to_string(col, colStr);
    
    write_byte(0x1B);        //ESC
    write_byte('[');
    
    if (row < 10) {
        write_byte(rowStr[0]);
    }
    else {
        //need to write each byte for double digits
        write_byte(rowStr[0]);
        write_byte(rowStr[1]);
    }
    
    write_byte(';');
    
    if (col < 10) {
        write_byte(colStr[0]);
    }
    else {
        write_byte(colStr[0]);
        write_byte(colStr[1]);
    }
    
    write_byte('H');
}

void set_color(uint8_t color) {
    char colorStr[3] = "";
    
    int_to_string(color, colorStr);
    
    write_byte(0x1B);        //ESC
    write_byte('[');
    write_byte(colorStr[0]);
    write_byte(colorStr[1]);
    write_byte('m');
}

void clear_screen() {
    write_byte(0x1B);        //ESC
    write_byte('[');
    write_byte('2');
    write_byte('J');
}
