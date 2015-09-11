#define _XTAL_FREQ 32000000

#include <xc.h>
#include <pic12f1822.h>

// CONFIG1 4x PLL enabled
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

char turn_left(char output);
char turn_right(char output);

char stepping_state = 0b0011;
int direct_sign = 0;

void interrupt step(void);
void init (void);

int main(void)
{
	init ();
	
	while(1)
	{
            //
            //stepping_state = turn_left(stepping_state);
            //PORTA = ((stepping_state & 0x03) | ((stepping_state << 2) & 0x30)) | (PORTA & 0x0C);
            //__delay_ms(1);
            //
	}
		
	return 0;
}

char turn_left(char output)
{
	output = (output << 1);
	if(output & 0x10)
	output ++;
	
	return output;
}

char turn_right(char output)
{
	output = (output >> 1);
	if(CARRY)
	output |= 0b00001000;
	
	return output;
}

void interrupt step(void)
{
	if(INTF)
	{
		if(~RA3)
		stepping_state = turn_right(stepping_state);
		else
		stepping_state = turn_left(stepping_state);
		
		PORTA = ((stepping_state & 0x03) | ((stepping_state << 2) & 0x30)) | (PORTA & 0x0C);
		INTF = 0;
	}
}

void init (void)
{
	OSCCONbits.SCS = 0b00; //use clock detaminate
	OSCCONbits.IRCF = 0b1110; //set to 8MHz
	ANSELA = 0b00000000;
	TRISA = 0b00001100;	//RA3 is input only. RA3 is direct. RA2(INT) is interrupt by clock
	PORTA = ((stepping_state & 0x03) | ((stepping_state << 2) & 0x30));
	ADON = 0;
	INTEDG = 0;
	INTE = 1;
	GIE = 1;
}