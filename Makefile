DEV=/dev/ttyACM0
CFLAGS=-w

#Compile and Flash the Arduino
#Be sure to change the device (the argument after -P) to match the device on your computer
#On Windows, change the argument after -P to appropriate COM port
program3: os.c os.h globals.h serial.c program3.c synchro.h synchro.c
	avr-gcc $(CFLAGS) -mmcu=atmega328p -DF_CPU=16000000 -O2 -o program3.elf\
	 program3.c serial.c os.c synchro.c
	avr-objcopy -O ihex program3.elf program3.hex
	avr-size program3.elf
	avrdude -pm328p -P $(DEV) -c arduino -F -u -U flash:w:program3.hex

#remove build files
clean:
	rm -fr *.elf *.hex *.o
