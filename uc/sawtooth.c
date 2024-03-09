#include <avr/io.h>

#define INIT_CKSUM unsigned char cka; unsigned char ckb;
#define ZERO_CKSUM cka=0; ckb=0;
#define UPDATE_CKSUM(X) cka+=X; ckb+=cka;
#define CHECKA_CKSUM(X) cka^=X;
#define CHECKB_CKSUM(X) ckb^=X;
#define VALID_CKSUM (cka==0x0 && ckb==0x0)

#define SPI_EN PORTB=0x2
#define SPI_DIS PORTB=0x0

#define CLK_RISE PORTA|=0x1
#define CLK_FALL PORTA&=~(0x1)
#define DATA1 PORTA|=0x2
#define DATA0 PORTA&=~(0x2)

#define SEND_BIT(X) if(X) DATA1; else DATA0; CLK_RISE; CLK_FALL;

//#define DEBUG_UART_TX 1

void main() {

  unsigned char buf;
  uint16_t plen;
  uint16_t i;
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
#ifdef UART_DEBUX_TX
  UCSRB = (1<<RXEN)|(1<<TXEN);
#else
  UCSRB = (1<<RXEN);
#endif
  UCSRC = (3<<UCSZ0);

  // set up SPI
  PORTA=0x00; // CLK, DAT low default
  PORTB=0x00; // SS is low by default
  DDRA=0x03; // PA0 = SCK, PA1 = DATA
  DDRB=0x02; // PB1 = SS

  state = sSync1;
  while (1) {
    while(!(UCSRA & (1<<RXC)));
    buf = UDR;
    switch(state) {
      case sSync1:
#ifdef DEBUG_UART_TX
          while ( !( UCSRA & (1<<UDRE)) );
          UDR = VALID_CKSUM ? (class<<4|id) : 0xff;
#endif
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
        plen += (((uint16_t)buf)<<8);
        i=0;
        if(plen>0) state = sPayload;
        else state = sCheckA;
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

          SPI_EN;
          SEND_BIT(qerr_ptr[0]>>7&0x1);
          SEND_BIT(qerr_ptr[0]>>6&0x1);
          SEND_BIT(qerr_ptr[0]>>5&0x1);
          SEND_BIT(qerr_ptr[0]>>4&0x1);
          SEND_BIT(qerr_ptr[0]>>3&0x1);
          SEND_BIT(qerr_ptr[0]>>2&0x1);
          SEND_BIT(qerr_ptr[0]>>1&0x1);
          SEND_BIT(qerr_ptr[0]&0x1);
          SPI_DIS;
#ifdef DEBUG_UART_TX
          while ( !( UCSRA & (1<<UDRE)) );
          UDR = (unsigned char) qerr_ptr[0];
#endif
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
