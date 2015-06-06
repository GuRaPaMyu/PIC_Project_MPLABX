#define _XTAL_FREQ 8000000

#include <xc.h>
#include <stdio.h>
#include <pic16f88.h>
#include "usart.h"
#include "adc.h"

// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)


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
		printf("resu  :%d\r\n", adc_read(1));
	}
	return 0;
}

void init(void)
{
	CMCON = 0b00000111; //cut off analog comparator just add this one
	OSCCON = 0b01111010;
	INTCON=0;	// purpose of disabling the interrupts.
	ANSEL = 0b00000010;
	TRISA = 0b00000010;	//RA3 is input only. RA3 is direct. RA2(INT) is interrupt by clock
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