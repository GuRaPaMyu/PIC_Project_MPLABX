#define _XTAL_FREQ 8000000
#define INTERCEPT -84-30 //for calibration
#define TANGENT 25 //for calibration
#define REF_mV 2405 //ADC reference voltage
#define RES 1024 //resolution of ADC bits
#define RES_mV REF_mV/RES //resolution of ADC voltage

// PIC16F88 Configuration Bit Settings

// 'C' source line config statements
#include <xc.h>
#include <pic16f88.h>
#include <math.h>
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
char sprint_double(char *str, int str_length, double val, int digit);

void main(void)
{
    init();
    double fwd, ref;
    double fwd_watt, ref_watt;
    double fwd_sqrt, ref_sqrt;
    double swr;
    char str[6];

    while(1)
    {
        fwd = adc_read(1);
        ref = adc_read(1);

        //bits -> Voltage -> dBm -> Watt
        fwd_watt = pow(10, (TANGENT * fwd * RES_mV - INTERCEPT)/10);
        ref_watt = pow(10, (TANGENT * ref * RES_mV - INTERCEPT)/10);

        //fwd_sqrt = sqrt(fwd_watt);
        //ref_sqrt = sqrt(ref_watt);

        swr = (fwd_sqrt + ref_sqrt)/(fwd_sqrt - ref_sqrt);

        lcd_goto(0x00);
        __delay_us(50);
        sprint_double(str, 6, fwd_watt, 2);
        lcd_puts(str);
        __delay_us(50);
        lcd_puts(" / ");
        __delay_us(50);
        sprint_double(str, 6, ref_watt, 2);
        lcd_puts(str);
        __delay_us(50);
        
        lcd_goto(0x40);
        __delay_us(50);
        lcd_puts("SWR: ");
        sprint_double(str, 6, swr, 2);
        lcd_puts(str);
    }
}

void init()
{
    CMCON = 0b00000111; //cut off analog comparator just add this one
    TRISA = 0b00000011; //ra0 and ra1 as input
    TRISB = 0b00000000;
    PORTA = 0b00000000;
    PORTB = 0b00000000;
    ANSEL = 0b00001111;
    INTCON=0; // purpose of disabling the interrupts.

    lcd_init();
    adc_init();

    __delay_ms(2);
    lcd_goto(0x00);	// select first line
    __delay_us(50);
    lcd_puts("  POWER &  SWR  ");
    lcd_goto(0x40);	// Select second line
    __delay_us(50);
    lcd_puts("      METER     ");
    __delay_ms(30);
}

char sprint_double(char *str, int str_length, double val, int digit)
{
  int i,s_flag = 0;

  for(i=0;i<digit;i++)
  {
    val *= 10;
  }
  *(str + str_length - 1) = '\0';
  str_length--;

  for(i=str_length ;i>0;i--)
  {
    if(digit == 0)
    {
      *(str + i - 1) = 0x2e;
    }else{
      if((val != 0) && (s_flag == 0))
      {
        *(str + i - 1) = (int)val % 10 + 0x30;
      }else{
        *(str + i - 1) = 0x20;
        s_flag = 1;
      }
      val = (int)val / 10;
    }
    digit--;
  }
  
  return str;
}