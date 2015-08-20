//CCCV power surpply ver.1

#define _XTAL_FREQ 8000000

#include <xc.h>
#include <pic16f88.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"
#include "adc.h"
#include "pwm.h"
#include "lcd.h"

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

int init(void);
void interrupt itrpt(void);

unsigned char encoder_last = 0;
unsigned char digit = 0;
unsigned char i_v_button = 0;
unsigned char i_v_state = 0;

int main (void)
{
    double v_limit = 0, i_limit = 0;
    double v_now = 0, i_now = 0;
    int duty = 0;

    init();
    
    __delay_ms(1000);
    
    while(1)
    {
        //TODO set channel applopreately
        v_now = adc_read(1);
        i_now = adc_read(2);

        if((v_now > v_limit) || (i_now > i_limit))
            duty --;
        else
            duty ++;

        //set switching duty
        CCPR1L =  duty >> 2;
	CCP1CON &= 0x0F;
	CCP1CON |= (duty << 4) & 0x30;
        //
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
	PORTB = 0x00;
	T2CON = 0x00;
        //TODO set TRIS and PORT to use ADC
        adc_init();
        pwm_init(1000, 1); //TODO set presc applopreately
        RBIE = 1;
        GIE = 1;

        encoder_last = PORTB & 0x30; //todo set encoder pin applopreately(nowRB4,RB5)
}

void set_switching_duty(short switch_duty)
{
	CCPR1L =  switch_duty >> 2;
	CCP1CON &= 0x0F;
	CCP1CON |= (switch_duty << 4) & 0x30;
}

void interrupt itrpt(void)
{
    if(RBIF)
    {
        RBIE = 0;
        if((encoder_last ^ (PORTB & 0x30)) == 0b00010000)
        {
            if((encoder_last - PORTB) > 0)
            {
                //left
            }else{
                //right
            }
        }else if((encoder_last ^ (PORTB & 0x30)) == 0b00100000){
            if(encoder_last - PORTB > 0)
            {
                //right
            }else{
                //left
            }
        }else if(PORTB & 0b01000000){
            //change digit(TODO more safety(chataling etc))
            digit ++;
            if(digit > 3)
                digit -= 4;
        }else if(PORTB & 0b10000000){
            //change i_v_button
            i_v_state ^= 1;
        }
        RBIF = 0;
        RBIE = 1;
    }
}