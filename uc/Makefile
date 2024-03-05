CC=avr-gcc
CFLAGS=-g -Os -mmcu=avrtiny
LOPTS=-g -mmcu=avrtiny
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
