#define _XTAL_FREQ 8000000
#define INTERCEPT -84-30 //for calibration
#define TANGENT 25 //for calibration
#define REF_mV 5000 //ADC reference voltage
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

// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)

void init();
void lcd_puts_ltoa(char *buf, int buf_size, int val);

void main(void)
{
    init();
    unsigned int fwd, ref;
    double fwd_watt, ref_watt;
    double fwd_sqrt, ref_sqrt;
    double swr=0;
    char str[6];
    
    while(1)
    {
        fwd = adc_read(0);
        ref = adc_read(1);
        
        //bits -> Voltage -> dBm -> Watt
        fwd_watt = pow(10, (TANGENT * fwd * RES_mV - INTERCEPT)/10);
        ref_watt = pow(10, (TANGENT * ref * RES_mV - INTERCEPT)/10);
        fwd_watt = 100;
        ref_watt = 10;
        
        fwd_sqrt = pow(fwd_watt, 0.5);
        ref_sqrt = pow(ref_watt, 0.5);
        //fwd_sqrt = sqrt(fwd_watt);
        //ref_sqrt = sqrt(ref_watt);

        swr = (fwd_sqrt + ref_sqrt)/(fwd_sqrt - ref_sqrt);
        
        lcd_goto(0x00);
        __delay_us(50);
        lcd_puts_ltoa(str, 4, (unsigned int)fwd_watt);
        __delay_us(50);
        lcd_puts(" / ");
        __delay_us(50);
        lcd_puts_ltoa(str, 4, (unsigned int)ref_watt);
        __delay_us(50);
        
        lcd_goto(0x40);
        __delay_us(50);
        lcd_puts("SWR: ");
        lcd_puts_ltoa(str, 5, (unsigned int)(swr*100));
        
        __delay_ms(100);
    }
}

void init()
{
    CMCON = 0b00000111; //cut off analog comparator just add this one
    INTCON = 0; // purpose of disabling the interrupts.
    OSCCON = 0b01111010;
    TRISA = 0b00000011; //ra0 and ra1 as input
    TRISB = 0b00000000;
    PORTA = 0b00000000;
    PORTB = 0b00000000;
    ANSEL = 0b00000011;

    lcd_init();
    adc_init();

    __delay_ms(2);
    lcd_goto(0x00);	// select first line
    __delay_us(50);
    lcd_puts("  POWER &  SWR  ");
    lcd_goto(0x40);	// Select second line
    __delay_us(50);
    lcd_puts("      METER     ");
    __delay_ms(1500);
    lcd_clear();
}

void lcd_puts_ltoa(char *buf, int buf_size, int val)
{
  char ascii_array[10] = {0x30,0x31,0x32,0x33,0x34,
              0x35,0x36,0x37,0x38,0x39};
  char zero_flag = 0;
  char cnt = 0;
  
  buf[buf_size - 1] = 0x00;
  buf_size --;
  while(buf_size > 0)
  {
    if(zero_flag == 0)
    {
      buf[buf_size - 1] = ascii_array[val%10];
      val /= 10;
      buf_size --;
    }else{
      buf[buf_size - 1] = 0x20;
      val /= 10;
      buf_size --;
    }
    cnt ++;
    val == 0 ? zero_flag=1 : zero_flag=0;
  }
  lcd_puts(buf);
}