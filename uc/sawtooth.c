#include <avr/io.h>

#define INIT_CKSUM unsigned char cka; unsigned char ckb;
#define ZERO_CKSUM cka=0; ckb=0;
#define UPDATE_CKSUM(X) cka+=X; ckb+=cka;
#define CHECKA_CKSUM(X) cka^=X;
#define CHECKB_CKSUM(X) ckb^=X;
#define VALID_CKSUM (cka==0x0 && ckb==0x0)

void main() {

  unsigned char buf;
  unsigned char plen;
  unsigned char i;
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
  INIT_CKSUM

  // set clock prescaler to x1 (for 8MHz clock)
  CCP = 0xd8;
  CLKPSR = 0;

  // enable serial port
  UBRRH = 0;
  UBRRL = 51; // 9600 baud w/ 8MHz clock
  UCSRB = (1<<RXEN)|(1<<TXEN);
  UCSRC = (3<<UCSZ0);

  state = sSync1;
  while (1) {
    while(!(UCSRA & (1<<RXC)));
    buf = UDR;
    switch(state) {
      case sSync1:
        if (buf == 0xb5) state = sSync2;
      break;
      case sSync2:
        ZERO_CKSUM
        if (buf == 0x62) state = sClass;
        else state = sSync1;
      break;
      case sClass:
        UPDATE_CKSUM(buf)
        class = buf;
        state = sId;
      break;
      case sId:
        UPDATE_CKSUM(buf)
        id = buf;
        state = sLen1;
      break;
      case sLen1:
        UPDATE_CKSUM(buf)
        plen = buf;
        state = sLen2;
      break;
      case sLen2:
        UPDATE_CKSUM(buf)
        if(buf==0 && plen>0) { i=0; state = sPayload; }
        else if (buf==0) state = sCheckA;
        else state = sSync1; // give up if payload > 255 bytes
      break;
      case sPayload:
        UPDATE_CKSUM(buf)
        if (i==8) qerr_ptr[0] = buf;
        else if (i==9) qerr_ptr[1] = buf;
        else if (i==10) qerr = qerr/150 + 128;
        i++;
        if (i==plen) state = sCheckA;
      break;
      case sCheckA:
        CHECKA_CKSUM(buf)
        state = sCheckB;
      break;
      case sCheckB:
        CHECKB_CKSUM(buf)
        if (class==13 && id==1 && VALID_CKSUM) {
          while ( !( UCSRA & (1<<UDRE)) );
          UDR = (unsigned char) qerr_ptr[0];
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
