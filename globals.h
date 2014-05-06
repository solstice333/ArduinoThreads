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
#include <string.h>

// boolean data type for readability
typedef enum {
   false, true
} bool;

/*
 * Initializes serial port for writing and reading VT100 escape sequences
 */
void serial_init();

/*
 * Checks if a byte is available. Returns 1 if a character is available,
 * otherwise returns 0
 */
uint8_t byte_available();

/*
 * Unbuffered read. Returns 255 if no character is available, otherwise
 * returns available character.
 */
uint8_t read_byte();

/*
 * Unbuffered write. |b| byte to write.
 */
uint8_t write_byte(uint8_t b);

/*
 * Prints string starting at |s|
 */
void print_string(char *s);

/*
 * Prints 8 bit or 16 bit int |i|
 */
void print_int(uint16_t i);

/*
 * Prints 32 bit int |i|
 */
void print_int32(uint32_t i);

/*
 * Sets cursor to row |row| and column |col|
 */
void set_cursor(uint8_t row, uint8_t col);

/*
 * Sets color |color|. Predefined colors are RED, GREEN, YELLOW, BLUE,
 * MAGENTA, CYAN, and WHITE
 */
void set_color(uint8_t color);

/*
 * Clears the screen
 */
void clear_screen();

#endif
