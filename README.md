#Program 2#

##Description##
Program 2 - Basis of mini Arduino OS
Authors: Kevin Navero, Brian Truong

##How To Use##
To compile program 2 from the terminal,

```
$ sudo make program_2
$ sudo screen /dev/ttyACM0 115200   # replace /dev/ttyACM0 with correct
                                    # device/serial port
```
Notes: 

1. Double check the build command for the "program" target and make sure
the -P argument points to the correct device/serial port

2. In os.c and program2.c, a |DEBUG| #define exists. When this is set to 0,
the program is stable, however the stats() function just prints out a string
literal "foofoo " to the screen. When |DEBUG| is set to 1, stats() attempts to 
print out system statistics to the screen, but the system becomes unstable. It
seems like avr-gcc is having issues with either auto-pushing, auto-popping,
or both within the ISR.

3. The update time on the screen output is a delay of 1 second, so you might
have to wait awhile before you see everything on the screen
