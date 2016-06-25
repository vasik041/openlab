OBJECTS=flwrclk.o

GCC_DIR =  c:/ti/gcc/bin
SUPPORT_FILE_DIRECTORY = c:/ti/gcc/include

DEVICE  = msp430f2013
CC      = $(GCC_DIR)/msp430-elf-gcc
OBJCOPY = $(GCC_DIR)/msp430-elf-objcopy
SIZE    = $(GCC_DIR)/msp430-elf-size
MSP430FLASHER = c:/ti/MSP430Flasher_1.3.6/MSP430Flasher

CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -O2 -g
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY) -T $(DEVICE).ld

all: ${OBJECTS}
	$(CC) $(CFLAGS) $(LFLAGS) $? -o $(DEVICE).elf
	$(SIZE) $(DEVICE).elf
	$(OBJCOPY) -O ihex $(DEVICE).elf $(DEVICE).hex

flash: all
	$(MSP430FLASHER) -v -w $(DEVICE).hex -z [VCC]

clean:
	rm -f $(DEVICE).elf $(DEVICE).lst $(DEVICE).hex $(OBJECTS) log/log.txt
	rmdir log
