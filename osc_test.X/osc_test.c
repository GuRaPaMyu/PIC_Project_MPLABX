#define _XTAL_FREQ 8000000

#include <xc.h>
#include <stdio.h>
#include <pic12f1822.h>
#include "usart.h"
#include "adc.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)


void init(void);
int adc_result;
void interrupt adc_test(void);

int main (void)
{
	init();
	adc_init();
	
	__delay_ms(1000);
	while(1)
	{
		ADIE = 1;
		adc_itrpt_go(1);
		printf("resu  :%d\r\n", adc_read(2));
	}
	return 0;
}

void init(void)
{
	OSCCON = 0b01110010; //32MHz (x4PLL)
	INTCON=0;	// purpose of disabling the interrupts.
	ANSELA = 0b00000110;
	TRISA = 0b00000110;
	PORTA = 0x00;

	init_comms();	// set up the USART - settings defined in usart.h
	adc_init();
	adc_itrpt_init();
}

void interrupt adc(void)
{
	if(ADIF)
	{
	ADIE = 0;
	printf("result:%d \r\n", adc_itrpt_read());
	}
}