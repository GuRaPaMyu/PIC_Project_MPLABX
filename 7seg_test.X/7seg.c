#define _XTAL_FREQ 4000000

#include <xc.h>
#include <pic16f648a.h>

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

void seg_light(int num);

int main (void)
{
	TRISA = 0b00000001;
	TRISB = 0b00000000;
	CMCON = 0b00000111; //cut off analog comparator just add this one
	PORTB = 0b00000000;
	
	int button = 0;     //puch button counter
	
	while(1)
	{
	seg_light(button);
	
		if(RA0 == 0)
		{
		button ++;
		__delay_ms(200);
			if(button >= 10)
			{
			button = button - 10;
			}	
		}	
	}
	return 0;
}

void seg_light(int num)
{
	switch(num)
	{
		case 0: PORTB = 0b00010001;
				break;
		case 1: PORTB = 0b10011111;
				break;
		case 2: PORTB = 0b00110010;
				break;
		case 3: PORTB = 0b00010110;
				break;
		case 4: PORTB = 0b10011100;
				break;
		case 5: PORTB = 0b01010100;
				break;
		case 6: PORTB = 0b01010000;
				break;
		case 7: PORTB = 0b00011111;
				break;
		case 8: PORTB = 0b00010000;
				break;
		case 9: PORTB = 0b00010100;
				break;
		default : PORTB = 0b11111111;
	}
}