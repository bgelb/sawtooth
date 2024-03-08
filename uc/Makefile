CC=avr-gcc
CFLAGS=-g -Os --short-enums -mmcu=attiny102 -B ./atpack/gcc/dev/attiny102 -isystem ./atpack/include
LOPTS=-g -mmcu=attiny102 -B ./atpack/gcc/dev/attiny102 -isystem ./atpack/include
CDEPS=attiny102.h
OBJCOPY=avr-objcopy

all: sawtooth.hex

%.o: %.c $(CDEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

%.elf: %.o
	$(CC) $(LOPTS) -o $@ $<

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

clean:
	rm -f *.o *.elf *.hex
