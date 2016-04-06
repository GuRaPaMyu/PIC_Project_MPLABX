#define _XTAL_FREQ 20000000
#define IC756


// PIC16F88 Configuration Bit Settings

// 'C' source line config statements
#include <xc.h>
#include <pic16f88.h>
#include "lcd.h"
#include "adc.h"

// PIC16F88 Configuration Bit Settings

// 'C' source line config statements
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable bit (RB3/PGM pin has PGM function, Low-Voltage Programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)

void init();

void main(void)
{
    init();
    while(1)
    {

    }
}


void init()
{
    CMCON = 0b00000111; //cut off analog comparator just add this one
    TRISA = 0b00000000;
    TRISB = 0b00000000;
    PORTA = 0b00000000;
    PORTB = 0b00000000;
    ANSEL = 0;
    INTCON=0;	// purpose of disabling the interrupts.

    lcd_init();
    __delay_ms(2);
    lcd_goto(0x00);	// select first line
    __delay_us(50);
    lcd_puts("**** IC-756 ****");
    lcd_goto(0x40);	// Select second line
    __delay_us(50);
    lcd_puts("--Freq Display--");
    __delay_ms(30);

    lcd_goto(0x00);
    __delay_us(50);
    lcd_puts("IC-756          ");
    __delay_us(50);
    lcd_goto(0x40);
    __delay_us(50);
    lcd_puts("Freq= ");
}
