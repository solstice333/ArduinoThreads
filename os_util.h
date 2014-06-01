#ifndef OS_UTIL_H
#define OS_UTIL_H

/*
 * Starts the system timer with 22KHz settings
 */
void start_system_timer();

/*
 * Run timer 2 in fast pwm mode and makes OC2B an output
 */
void start_audio_pwm();

#endif
