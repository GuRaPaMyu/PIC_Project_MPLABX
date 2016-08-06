
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
#pragma config LVP = ON         // Low-Voltage Programming Enable bit (RB3/PGM pin has PGM function, Low-Voltage Programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)


#include <xc.h>
#include <pic16f88.h>
#include "adc.h"

void init();

void main(void)
{
  unsigned int sound_v;
  init();

  while(1)
  {
    sound_v = adc_read(0);

    if(sound_v < 9){
      PORTB = 0b11111111;
      RA6 = 1;
      RA7 = 1;
    }else if((9<=sound_v) && (sound_v < 14)){
      PORTB = 0b11111110;
      RA6 = 1;
      RA7 = 1;
    }else if((14<=sound_v) && (sound_v < 20)){
      PORTB = 0b11111100;
      RA6 = 1;
      RA7 = 1;
    }else if((20<=sound_v) && (sound_v < 28)){
      PORTB = 0b11111000;
      RA6 = 1;
      RA7 = 1;
    }else if((28<=sound_v) && (sound_v < 40)){
      PORTB = 0b11110000;
      RA6 = 1;
      RA7 = 1;
    }else if((40<=sound_v) && (sound_v < 56)){
      PORTB = 0b11100000;
      RA6 = 1;
      RA7 = 1;
    }else if((56<=sound_v) && (sound_v < 79)){
      PORTB = 0b11000000;
      RA6 = 1;
      RA7 = 1;
    }else if((79<=sound_v) && (sound_v < 112)){
      PORTB = 0b10000000;
      RA6 = 1;
      RA7 = 1;
    }else if((112<=sound_v) && (sound_v < 158)){
      PORTB = 0b00000000;
      RA6 = 1;
      RA7 = 1;
    }else if((158<=sound_v) && (sound_v < 224)){
      PORTB = 0b00000000;
      RA6 = 0;
      RA7 = 1;
    }else if(224<=sound_v){
      PORTB = 0b00000000;
      RA6 = 0;
      RA7 = 0;
    }else{
      PORTB = 0b01010101;
      RA6 = 1;
      RA7 = 0;
    }

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

  adc_init();
}