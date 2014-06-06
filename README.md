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

1. The threads aren't completely implemented, and thus there are a few 
DEBUG #defines in program5.c and ext2reader.c to help isolate testing. In 
program5.c, set PLAYBACK_DEBUG to 1 to isolate and run the playback thread.
The playback thread will just write random values between 0-255 inclusive
to the digital audio pin. Similarly, set READ_DEBUG to 1 to isolate and run the 
read thread. Between these two, only 1 #define should be set at one time. 

2. If READ_DEBUG is set to 1, there are three more #defines in ext2reader.c
that were used to help with incremental development and testing inside the 
get_root function - DEBUG_SB_BGDT, DEBUG_INO, and DEBUG_DENTRY. Among these 
three, only 1 #define should be set at one time. If DEBUG_SB_BGDT is set, it 
should show that the superblock and the bgdt can be read just fine. If the 
other two #defines are set, that's when instability begins to show, possibly
due to lack of memory.

3. There is instability in running only one thread due to the nature of how
get_next_thread() works. When running only one thread, if that thread goes to 
sleep, get_next_thread() will return ETHREAD with the assumption that there 
are no threads to run and the system will deadlock. One resolution to this is 
to create a filler thread that just spins until the sleeping thread wakes up. 
In general, always have one thread active and ready/running.

4. Do not use _delay_ms(). Use thread_sleep() instead

##Known Bugs##

1. Very bad memory issues causing unpredictability

2. Failure to read the SD card sometimes
