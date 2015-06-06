//pwm test module

#define _XTAL_FREQ 8000000

#include <xc.h>
#include <pic16f88.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"

// CONFIG1
#pragma config FOSC = EXTRCIO   // Oscillator Selection bits (EXTRC oscillator; port I/O function on RA6/OSC2/CLKO)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB3      // CCP1 Pin Selection bit (CCP1 function on RB3)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)

void pwm_set_duty(double duty_ratio);
int init(void);
void pwm_init(unsigned long freq, unsigned char T2presc);

int main (void)
{
	init();
	pwm_init(1000, 1);
	init_comms();
	char str[5] = {0};
	int duty = 0;
	int cnt,i;
	
	while(1)
	{
		pwm_set_duty(duty / 10);
		cnt = 0;
		do{
			str[cnt] = getche();
			cnt ++;
		}while(str[cnt -1] != 0x0d);
		
		duty = atoi(str);
		printf("set to %d\r\n", duty);
		printf("PR2: %x\r\n", PR2);
		printf("freq: %d\r\n", (unsigned int)(_XTAL_FREQ / ((PR2+1) * 4)));
		
		/* fuwafuwa
		if(i>=100)
		cnt = 0;
		else if(i<=0)
		cnt = 1;
		if(cnt > 0)
		pwm_set_duty(i++);
		else if(cnt <= 0)
		pwm_set_duty(i--);
		
		__delay_ms(20);
		*/
	}	
	return 0;
}

int init(void)
{
	OSCCON = 0b01110010;
	TRISA = 0x00;
	TRISB = 0x00;			// output changes on LED
	CMCON = 0b00000111;
	PORTA = 0x00;
	PORTB = 0b00000010;
	T2CON = 0x00;
}
 
void pwm_set_duty(double duty_ratio)
{
	unsigned short reg;
	reg = (unsigned short)duty_ratio * (PR2 + 1) / 25;	//duty_ratio / 100 * (PR2 + 1) * 4 @ dutyratio = Tduty / Tpwm
	CCPR1L =  reg >> 2;
	CCP1CON &= 0x0F;
	CCP1CON |= (reg << 4) & 0x30;
}

void pwm_init(unsigned long freq, unsigned char T2presc)  //0 is presc 1, 1 is presc 4, 3 is presc 16
{
	T2CON &= 0xFC;
	switch(T2presc)
	{
		case 1 :	T2CON |= 0x00;
							break;
		case 2 :	T2CON |= 0x01;
							break;
		case 16:	T2CON |= 0x02;
							break;
		default:	T2CON |= 0x00;
	}
	
	PR2 = (unsigned char)(_XTAL_FREQ / freq / 4 - 1);
	CCPR1L = 0;
	OPTION_REGbits.PS = 0b101; //128cnt prescaler
	TMR2ON = 1; //enable timer2 module
	CCP1CON = 0x0C; //set PWM mode
}