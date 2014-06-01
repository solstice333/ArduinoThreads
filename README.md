#Program 5#

##Description##
Program 5 - Arduino OS with music playback

Authors: Kevin Navero, Brian Truong

##How To Use##
To compile program from the terminal,

```
$ ./buildAndScreen.sh <device serial port>
```
Example:

```
$ ./buildAndScreen.sh /dev/ttyACM0
```

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

##Known Bugs##
