
// PIC16F88 Configuration Bit Settings

// 'C' source line config statements

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSCIO  //  (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF         // Low-Voltage Programming Enable bit (RB3/PGM pin has PGM function, Low-Voltage Programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)

#define PIN_DATA RA0
#define PIN_FQ_UD RA1
#define PIN_W_CLK RA2
#define PIN_RESET RA3
#define _XTAL_FREQ 8000000

#include <xc.h>
#include <pic16f88.h>

void init();
void wr_serial(unsigned char w0,double frequence);
void reset();
void set_freq(unsigned long  freq);

void main(void)
{
  init();
  reset();
  wr_serial(0x01, 30000000);

  while(1)
  {
    
  }
}

void init()
{
  CMCON = 0b00000111; //cut off analog comparator just add this one
  INTCON = 0; // purpose of disabling the interrupts.
  OSCCON = 0b01111010;
  TRISA = 0b00000000; //ra2 as input
  TRISB = 0b00000000;
  PORTA = 0b00000000;
  PORTB = 0b00000000;
  ANSEL = 0b00000000;
}

void set_freq(unsigned long  freq)
{
  // freq (delta phase)
  for (int i = 0; i < 32; i++) {
      PIN_DATA = (freq>>i & 1);
      PIN_W_CLK = 1;
      __delay_ms(1);
      PIN_W_CLK = 0;
      __delay_ms(1);
  }

  // control bits
  PIN_DATA = 1;
  PIN_W_CLK = 1;
  __delay_ms(1);
  PIN_W_CLK = 0;
  __delay_ms(1);
  PIN_DATA = 0;
  PIN_W_CLK = 1;
  __delay_ms(1);
  PIN_W_CLK = 0;
  __delay_ms(1);

  // powerdown
  PIN_DATA = 0;
  PIN_W_CLK = 1;
  __delay_ms(1);
  PIN_W_CLK = 0;
  __delay_ms(1);

  // phase
  for (int i = 0; i < 5; i++) {
      PIN_DATA = 0;
      PIN_W_CLK = 1;
      __delay_ms(1);
      PIN_W_CLK = 0;
      __delay_ms(1);
  }

  PIN_FQ_UD = 1;
  __delay_ms(1);
  PIN_FQ_UD = 0;
  __delay_ms(1);
}

void reset() {
  // ensure low
  PIN_DATA = 0;
  PIN_FQ_UD = 0;
  PIN_W_CLK = 0;

  // reset
  PIN_RESET = 1; 
  __delay_ms(1000);
  PIN_RESET = 0;
  __delay_ms(1000);

  // reset to serial mode
  // Pins of D0, D1 = 1, D2 = 0 for serial mode
  PIN_W_CLK = 1; 
  __delay_ms(1);
  PIN_W_CLK = 0;
  __delay_ms(1);

  PIN_FQ_UD = 1;
  __delay_ms(1);
  PIN_FQ_UD = 0;
  __delay_ms(1);
}
    
void wr_serial(unsigned char w0,double frequence)
{
  unsigned char i,w;
  long int y;
  double x;

  //Calculate the frequency of the HEX value
  x=4294967295/180;//Suitable for 180M Crystal  125-180 
  frequence=frequence/1000000;
  frequence=frequence*x;
  y=(long int)frequence;

  //write w4
  w=(y>>=0);
  for(i=0; i<8; i++)
  {
    PIN_DATA = ((w>>i)&0x01);
    __delay_ms(1);
    PIN_W_CLK = 1;
    __delay_ms(1);
    PIN_W_CLK = 0;
    __delay_ms(1);
  }
  //write w3
  w=(y>>8);
  for(i=0; i<8; i++)
  {
    PIN_DATA = ((w>>i)&0x01);
    __delay_ms(1);
    PIN_W_CLK = 1;
    __delay_ms(1);
    PIN_W_CLK = 0;
    __delay_ms(1);
  }
  //write w2
  w=(y>>16);
  for(i=0; i<8; i++)
  {
    PIN_DATA = ((w>>i)&0x01);
    __delay_ms(1);
    PIN_W_CLK = 1;
    __delay_ms(1);
    PIN_W_CLK = 0;
    __delay_ms(1);
  }
  //write w1
  w=(y>>24);
  for(i=0; i<8; i++)
  {
    PIN_DATA = ((w>>i)&0x01);
    __delay_ms(1);
    PIN_W_CLK = 1;
    __delay_ms(1);
    PIN_W_CLK = 0;
    __delay_ms(1);
  }
  //write w0
  w=w0;
  for(i=0; i<8; i++)
  {
    PIN_DATA = ((w>>i)&0x01);
    __delay_ms(1);
    PIN_W_CLK = 1;
    __delay_ms(1);
    PIN_W_CLK = 0;
    __delay_ms(1);
  }
  __delay_ms(1);
    PIN_FQ_UD = 1;
  __delay_ms(1);
  PIN_FQ_UD = 0;
  __delay_ms(1);
}