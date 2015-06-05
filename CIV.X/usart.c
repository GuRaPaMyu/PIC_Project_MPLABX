#include <xc.h>
#include <stdio.h>
#include "usart.h"
#include "lcd.h"

static int rate_num = 0;

void putch(unsigned char byte) 
{
	/* output one byte */
	while(!TXIF)	/* set when register is empty */
		continue;
	TXREG = byte;
}

unsigned char getch() {
	/* retrieve one byte */
	while(!RCIF)
	{
		if(FERR)
		{
		FERR = 0;
			switch(rate_num)
			{
			case 0 : SPBRG = 15;
					rate_num++;
					break;
											
			case 1 : SPBRG = 31;
					rate_num++;
					break;
					
			case 2 : SPBRG = 64;
					rate_num++;
					break;
					
			case 3 : SPBRG = 129;
					rate_num++;
					break;
					
			case 4 : SPBRG = 254;
					rate_num++;
					break;
								
			case 5 : SPBRG = 1040;
					rate_num = 0;
					break;

			default : SPBRG = 15;
					rate_num = 0;
			}
		}
	}
	return RCREG;
}

unsigned char getche(void)
{
	unsigned char c;
	putch(c = getch());
	return c;
}
