#define _XTAL_FREQ 1000000
#define TIMER_INTERVAL (int)(0xFFFF - (0.001 * _XTAL_FREQ / 4)) //set default timer setting
#define BUTTON1 RA0
#define BUTTON2 RA1
#define BUTTON3 RA2

#include <xc.h>
#include <pic16f1827.h>
#include "i2c.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

void Init(void);
void Timer1ON(void);
void Timer1Stop(void);
int ButtonPush(void);
void I2CLcdInit(void);
void I2CLcdPuts(char *str);
void I2CLcdGoto(unsigned char dist);
void I2CLcdSendCmd(unsigned char cmd);
void interrupt itrpt(void);

unsigned int timerMitute;
unsigned int intrCounter;

int main (void)
{
  Init();
  i2c_Open(0x7C, 0);
  I2CLcdInit();

  while(1)
  {
  i2c_SendByte(0xC0);
  i2c_PutString("test", 4);
  i2c_SendByte(0xC0);
  }
  return 0; 
}

void Init(void)
{
  OSCCON = 0b01011010;  //OSC set to 1MHz

  PORTA = 0x00;
  TRISA = 0x00;
  ANSELA = 0x00;
  timerMitute = 0;
  intrCounter = 0;
  TMR1 = TIMER_INTERVAL;
  PEIE = 1;
  GIE = 1;
}

void I2CLcdInit(void)
{
  i2c_SendByte(0x38);
  __delay_us(30);
  i2c_SendByte(0x39);
  __delay_us(30);
  i2c_SendByte(0x14);
  __delay_us(30);
  i2c_SendByte(0x70);
  __delay_us(30);
  i2c_SendByte(0x56);
  __delay_us(30);
  i2c_SendByte(0x6C);
  __delay_ms(200);
  i2c_SendByte(0x38);
  __delay_us(30);
  i2c_SendByte(0x0C);
  __delay_us(30);
  i2c_SendByte(0x01);
  __delay_ms(2);
  i2c_SendByte(0x80);
}

//10ms interrupt(timer)
static void interrupt intr(void)
{
  if (TMR1GIF == 1)
	{
      TMR1 = TIMER_INTERVAL;  //reset to default value

      intrCounter ++;

      if (intrCounter >= 1000)
	    {
        if(timerMitute >= 0xFFFF)
	      {
          Timer1Stop();
          timerMitute = 0;
		    }else{
          timerMitute ++;
        }

        intrCounter = 0;
      }

        TMR1GIF = 0;
    }   
}

void Timer1ON(void)
{
  TMR1 = TIMER_INTERVAL;
  TMR1GIE = 1;
  TMR1ON = 1;
}

void Timer1Stop(void)
{
  TMR1ON = 0;
}

int ButtonPush(void)
{
  int push_value = 0; //1:left 2:right 3:ok 4:left and right button pushed
  while(BUTTON1 && BUTTON2 && BUTTON3)
    continue;

  do{
    if(push_value != 4)
    {
      if(BUTTON1 == 0)
        push_value = 1;
      if(BUTTON2 == 0)
        push_value = 2;
      if(BUTTON3 == 0)
        push_value = 3;
    }   
    if((BUTTON1 == 0)&&(BUTTON2 == 0))
      push_value = 4;
  }while(!(BUTTON1 && BUTTON2 && BUTTON3));

  return push_value;
}
void I2CLcdPuts(char *str)
{

}

void I2CLcdGoto(unsigned char dist)
{

}

void I2CLcdSendCmd(unsigned char cmd)
{

}