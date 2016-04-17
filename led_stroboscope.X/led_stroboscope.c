//pwm test module

#define _XTAL_FREQ 32000000

#include <xc.h>
#include <pic12f1822.h>
#include <stdio.h>
#include <stdlib.h>
#include "skI2Clib.h"
#include "skI2CLCDlib.h"
#include "adc.h"
#include "usart.h"

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

void pwm_set_duty(double duty_ratio);
int init(void);
void pwm_init(unsigned long freq, unsigned char T2presc);

int main(void) {
    init();
    pwm_init(1000, 1);
    char str[5] = {0};
    int duty = 0;

    LCD_SetCursor(0, 0);
    LCD_Puts("-JP7FKF-");
    LCD_SetCursor(0, 1);
    LCD_Putc("hogehoge");

    while (1) {
        pwm_set_duty(duty / 10);
        printf("set to %d\r\n", duty);
        printf("PR2: %x\r\n", PR2);
        printf("freq: %d\r\n", (unsigned int) (_XTAL_FREQ / ((PR2 + 1) * 4)));
    }
    return 0;
}

int init(void) {
    OSCCON = 0b01110010;
    WPUA = 0b00000000;
    TRISA = 0b00010111;
    CMCON = 0b00000111;
    PORTA = 0x00;
    ANSELA = 0b00010000;
    APFCON = 0b00000001;
    T2CON = 0x00;

    adc_init();
    InitI2C_Master(1);
    LCD_Init(LCD_NOT_ICON,32,LCD_VDD3V,8);
}

void pwm_set_duty(double duty_ratio) {
    unsigned short reg;
    reg = (unsigned short) duty_ratio * (PR2 + 1) / 25; //duty_ratio / 100 * (PR2 + 1) * 4 @ dutyratio = Tduty / Tpwm
    CCPR1L = reg >> 2;
    CCP1CON &= 0x0F;
    CCP1CON |= (reg << 4) & 0x30;
}

void pwm_init(unsigned long freq, unsigned char T2presc) //0 is presc 1, 1 is presc 4, 3 is presc 16
{
    T2CON &= 0xFC;
    switch (T2presc) {
        case 1: T2CON |= 0x00;
            break;
        case 2: T2CON |= 0x01;
            break;
        case 16: T2CON |= 0x02;
            break;
        default: T2CON |= 0x00;
    }

    PR2 = (unsigned char) (_XTAL_FREQ / freq / 4 - 1);
    CCPR1L = 0;
    OPTION_REGbits.PS = 0b101; //128cnt prescaler
    TMR2ON = 1; //enable timer2 module
    CCP1CON = 0x0C; //set PWM mode
}