//Global defines

#ifndef GLOBALS_H
#define GLOBALS_H

//place defines and prototypes here
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37
#define MY_NAME "Kevin Navero"
#define ESC 27

#include <avr/io.h>

// Implemented in serial.c
void serial_init();
uint8_t byte_available();
uint8_t read_byte();
uint8_t write_byte(uint8_t b);

void print_string(char *s);
void print_int(uint16_t i);
void print_int32(uint32_t i);
void set_cursor(uint8_t row, uint8_t col);
void set_color(uint8_t color);
void clear_screen();

#endif
