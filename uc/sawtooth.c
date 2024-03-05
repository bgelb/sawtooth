#include "attiny102.h"

void main() {

  unsigned char buf;

  // set clock prescaler to x1
  CCP = 0xd8;
  CLKPSR = 0;

  // enable serial port
  UBRR0H = 0;
  UBRR0L = 51;
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  UCSR0C = (3<<UCSZ00);

  buf = 0;
  while (1) {
    if(UCSR0A & (1<<RXC0)) {
      buf = UDR0;
    }
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = buf;//++;
  }

}
