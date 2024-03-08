#include "attiny102.h"

void main() {

  unsigned char buf;
  unsigned char plen;
  unsigned char i;
  unsigned char cka;
  unsigned char ckb;
  unsigned char class;
  unsigned char id;
  int16_t qerr;
  unsigned char * qerr_ptr = (unsigned char *)&qerr;
  typedef enum {
    sSync1,
    sSync2,
    sClass,
    sId,
    sLen1,
    sLen2,
    sPayload,
    sCheckA,
    sCheckB
  } state_t;

  unsigned char state;

  // set clock prescaler to x1 (for 8MHz clock)
  CCP = 0xd8;
  CLKPSR = 0;

  // enable serial port
  UBRR0H = 0;
  UBRR0L = 51; // 9600 baud w/ 8MHz clock
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  UCSR0C = (3<<UCSZ00);

  state = sSync1;
  while (1) {
    while(!(UCSR0A & (1<<RXC0)));
    buf = UDR0;
    switch(state) {
      case sSync1:
        if (buf == 0xb5) state = sSync2;
      break;
      case sSync2:
        if (buf == 0x62) state = sClass;
        else state = sSync1;
      break;
      case sClass:
        class = buf;
        state = sId;
      break;
      case sId:
        id = buf;
        state = sLen1;
      break;
      case sLen1:
        plen = buf;
        state = sLen2;
      break;
      case sLen2:
        if(buf==0 && plen>0) { i=0; state = sPayload; }
        else if (buf==0) state = sCheckA;
        else state = sSync1; // give up if payload > 255 bytes
      break;
      case sPayload:
        if (i==8) qerr_ptr[0] = buf;
        else if (i==9) qerr_ptr[1] = buf;
        else if (i==10) qerr = qerr/150 + 128;
        i++;
        if (i==plen) state = sCheckA;
      break;
      case sCheckA:
        if (class==13 && id==1) {
          while ( !( UCSR0A & (1<<UDRE0)) );
          UDR0 = (unsigned char) qerr_ptr[0];
        }
        state = sCheckB;
      break;
      case sCheckB:
        if (class==13 && id==1) {
          while ( !( UCSR0A & (1<<UDRE0)) );
          UDR0 = (unsigned char) qerr_ptr[1];
        }
        state = sSync1;
      break;

      // should never reach
      default:
        state = sSync1;
      break;
    }
  }

}
