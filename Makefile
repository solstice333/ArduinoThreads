DEV=/dev/ttyACM0
CFLAGS=-w

#Compile and Flash the Arduino
#Be sure to change the device (the argument after -P) to match the device on your computer
#On Windows, change the argument after -P to appropriate COM port
program2: os.c os.h globals.h serial.c program2.c
	avr-gcc $(CFLAGS) -mmcu=atmega328p -DF_CPU=16000000 -O2 -o program2.elf\
	 program2.c serial.c os.c
	avr-objcopy -O ihex program2.elf program2.hex
	avr-size program2.elf
	avrdude -pm328p -P $(DEV) -c arduino -F -u -U flash:w:program2.hex

#remove build files
clean:
	rm -fr *.elf *.hex *.o
