#Program 3#

##Description##
Program 3 - Basis of mini Arduino OS
Authors: Kevin Navero, Brian Truong

##How To Use##
To compile program from the terminal,

```
$ sudo make program3
$ sudo screen /dev/ttyACM0 115200   # replace /dev/ttyACM0 with correct
                                    # device/serial port
```

To adjust production rate using 'a' to speed up or 'z' to slow down. To adjust 
consumption rate, use 'k' to speed up, and 'm' to slow down.

##Notes##

1. Double check the build command for the "program" target and make sure
the -P argument points to the correct device/serial port

2. There is instability in running only one thread due to the nature of how
get_next_thread() works. When running only one thread, if that thread goes to 
sleep, get_next_thread() will return ETHREAD with the assumption that there 
are no threads to run and the system will deadlock. One resolution to this is 
to create a filler thread that just spins until the sleeping thread wakes up. 
In general, always have one thread active and ready/running.

3. Do not use _delay_ms(). Use thread_sleep() instead

4. Almost running out of space in the data segment to store global variables
and string literals? Adding in a few more print_string()'s or global variables
in program3.c causes the program to act unpredictably.

##Known Bugs##

1. Adjusting the production rate and consumption rate rapidly may cause the 
program to completely reset unpredictably.

2. Production rate and consumption rate does not go slower than 1 item per
2500 ms without wrapping around to 40 ms.
