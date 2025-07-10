CPU_NAME := attiny85
CPU_CLOCK := 16000000UL
FUSE_L := 0xF1
FUSE_H := 0xDD
AVRDUDE := avrdude -c usbtiny -p $(CPU_NAME) -b 19200  -v

CPU_OPTIM := -Os
CPU_FLAGS := $(CPU_OPTIM) -mmcu=$(CPU_NAME) -DF_CPU=$(CPU_CLOCK)
WARNING_FLAGS := -Wall -Wextra -Wshadow -Wpointer-arith \
	-Wbad-function-cast -Wcast-align -Wsign-compare \
	-Waggregate-return -Wstrict-prototypes \
	-Wmissing-prototypes -Wmissing-declarations -Wunused
INCLUDE_FLAGS :=
CFLAGS := $(WARNING_FLAGS) $(CPU_FLAGS) $(INCLUDE_FLAGS)

SOURCE := src/main.c \
       src/uart.c \
	   src/temp_sensor.c \
	   src/onewire.c \
	   src/ds18b20.c \

OBJECTS := $(patsubst %.c, %.o, $(SOURCE))
TARGET := main

CC := avr-gcc
OBJCOPY := avr-objcopy

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"

# rule for programming fuse bits:
fuse:
	@[ "$(FUSE_H)" != "" -a "$(FUSE_L)" != "" ] || \
		{ echo "*** Edit Makefile and choose values for FUSE_L and FUSE_H!"; exit 1; }
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

all:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SOURCE}; \
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

flash:
		$(AVRDUDE) -U flash:w:${TARGET}.hex:i

clean:
		rm -f *.bin *.hex


#
# Rules
#

%.elf:
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%: %.o
	$(CC) $(CFLAGS) -o $@ $<
