// PIC16F88 Configuration Bit Settings

// 'C' source line config statements
#include <xc.h>
#include <pic16f88.h>
#include <math.h>
#include "lcd.h"
#include "adc.h"

void main(void)
{
  init();

  while(1)
  {
    sound_v = adc_read(0);

    if(sound_v < 9){
      PORTB = ~0b00000000;
      RA6 = 0;
      RA7 = 0;
    }else if((9<=sound_v) && (sound_v < 14)){
      PORTB = ~0b00000001;
      RA6 = 0;
      RA7 = 0;
    }else if((14<=sound_v) && (sound_v < 20)){
      PORTB = ~0b00000011;
      RA6 = 0;
      RA7 = 0;
    }else if((20<=sound_v) && (sound_v < 28)){
      PORTB = ~0b00000111;
      RA6 = 0;
      RA7 = 0;
    }else if((28<=sound_v) && (sound_v < 40)){
      PORTB = ~0b00001111;
      RA6 = 0;
      RA7 = 0;
    }else if((40<=sound_v) && (sound_v < 56)){
      PORTB = ~0b00011111;
      RA6 = 0;
      RA7 = 0;
    }else if((56<=sound_v) && (sound_v < 79)){
      PORTB = ~0b00111111;
      RA6 = 0;
      RA7 = 0;
    }else if((79<=sound_v) && (sound_v < 112)){
      PORTB = ~0b01111111;
      RA6 = 0;
      RA7 = 0;
    }else if((112<=sound_v) && (sound_v < 158)){
      PORTB = ~0b11111111;
      RA6 = 0;
      RA7 = 0;
    }else if((158<=sound_v) && (sound_v < 224)){
      PORTB = ~0b11111111;
      RA6 = 1;
      RA7 = 0;
    }else if(224<=sound_v){
      PORTB = ~0b11111111;
      RA6 = 1;
      RA7 = 1;
    }else{
      PORTB = ~0b01010101;
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

  lcd_init();
  adc_init();
}