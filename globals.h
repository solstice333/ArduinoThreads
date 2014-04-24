//Global defines

#ifndef GLOBALS_H
#define GLOBALS_H

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

//place defines and prototypes here
#define DECBASE 10
#define HEXSTR 7
#define HEXBASE 16
#define STR32 11
#define NAMELEN 12
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37
#define MAX_ROW 25
#define MAX_COL 80
#define TWENTY_FIVE_MS 25
#define FIVE_HUNDRED_MS 500

void serial_init();
uint8_t byte_available();
uint8_t read_byte();
uint8_t write_byte(uint8_t b);
void print_string(char* s);
void print_int32(uint32_t i);
void print_int(uint16_t i);
void print_hex(uint16_t i);
void print_hex32(uint32_t i);
void set_cursor(uint8_t row, uint8_t col);
void set_color(uint8_t color);
void clear_screen();

#endif
