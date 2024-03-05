#ifndef _AVR_SFR_DEFS_H_
#include <avr/sfr_defs.h>
#endif

#define CCP     _SFR_IO8(0x3c)
#define CLKPSR  _SFR_IO8(0x36)
#define CLKPS   0

#define UDR0    _SFR_IO8(0x08)

/* Combine UBRR0L and UBRR0H */
#define UBRR0   _SFR_IO16(0x09)

#define UBRR0L  _SFR_IO8(0x09)
#define UBRR0H  _SFR_IO8(0x0a)

#define UCSR0D  _SFR_IO8(0x0b)
#define SFDE0   5
#define RXS0    6
#define RXSIE0  7

#define UCSR0C  _SFR_IO8(0x0c)
#define UCPOL0  0
#define UCSZ00  1
#define UCSZ01  2
#define USBS0   3
#define UPM00   4
#define UPM01   5
#define UMSEL00 6
#define UMSEL01 7

#define UCSR0B  _SFR_IO8(0x0d)
#define TXB80   0
#define RXB80   1
#define UCSZ02  2
#define TXEN0   3
#define RXEN0   4
#define UDRIE0  5
#define TXCIE0  6
#define RXCIE0  7

#define UCSR0A  _SFR_IO8(0x0e)
#define MPCM0   0
#define U2X0    1
#define UPE0    2
#define DOR0    3
#define FE0     4
#define UDRE0   5
#define TXC0    6
#define RXC0    7

