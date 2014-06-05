# DEV=/dev/ttyACM0
CFLAGS=-w

#Compile and Flash the Arduino
#Be sure to change the device (the argument after -P) to match the device on your computer
#On Windows, change the argument after -P to appropriate COM port


program5: globals.h os.c os.h os_util.c program5.c queue.c queue.h SdInfo.h\
 os_util.h SdReader.c SdReader.h serial.c synchro.c synchro.h WavePinDefs.h\
 ext2.h ext2reader.c ext2reader.h
	avr-gcc $(CFLAGS) -mmcu=atmega328p -DF_CPU=16000000 -O2 -o program5.elf\
	 os.c os_util.c program5.c queue.c SdReader.c serial.c synchro.c\
    ext2reader.c 
	avr-objcopy -O ihex program5.elf program5.hex
	avr-size program5.elf
	avrdude -pm328p -P $(DEV) -c arduino -F -u -U flash:w:program5.hex

#remove build files
clean:
	rm -fr *.elf *.hex *.o
