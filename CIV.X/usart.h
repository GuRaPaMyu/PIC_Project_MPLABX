#ifndef _SERIAL_H_
#define _SERIAL_H_

#define BAUD 1200
#define NINE 0     /* Use 9bit communication? FALSE=8bit */
#define SPDIF 32

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 20000000
#endif

#define HIGH_SPEED 0

#if NINE == 1
#define NINE_BITS 0x40
#else
#define NINE_BITS 0
#endif

#if HIGH_SPEED == 1
#define SPEED 0x4
#else
#define SPEED 0
#endif

#define RX_PIN TRISB2
#define TX_PIN TRISB5

/* Serial initialization */
#define init_comms()\
	RX_PIN = 1;	\
	TX_PIN = 1;		  \
	SPBRG = SPDIF;     	\
	RCSTA = (NINE_BITS|0x90);	\
	TXSTA = (SPEED|NINE_BITS|0x20)

void putch(unsigned char);
unsigned char getch(void);
unsigned char getche(void);

#endif
