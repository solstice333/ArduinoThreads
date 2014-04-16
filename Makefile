DEV=/dev/ttyACM0

#Compile and Flash the Arduino
#Be sure to change the device (the argument after -P) to match the device on your computer
#On Windows, change the argument after -P to appropriate COM port
program_2: os.c os.h globals.h serial.c program_2.c
	avr-gcc -mmcu=atmega328p -DF_CPU=16000000 -O2 -o program_2.elf program_2.c\
    serial.c os.c
	avr-objcopy -O ihex program_2.elf program_2.hex
	avr-size program_2.elf
	avrdude -pm328p -P $(DEV) -c arduino -F -u -U flash:w:program_2.hex

#remove build files
clean:
	rm -fr *.elf *.hex *.o
