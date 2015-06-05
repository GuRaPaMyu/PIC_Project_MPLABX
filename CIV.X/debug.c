#include <xc.h>
#include "debug.h"
#include "usart.h"
#include "lcd.h"

#ifndef _XTAL_FREQ
	#define _XTAL_FREQ 4000000
#endif

#define	LCD_RS RB0
#define	LCD_RW RB1
#define LCD_EN RB3

#define LCD_DATA PORTA

#define	LCD_STROBE()	((LCD_EN = 1),(LCD_EN=0))


void lcd_indicate(char test)
{
	lcd_goto(0x0D);
	__delay_us(50);
	lcd_putch(test);
	__delay_ms(500);
}

void scan_ex(void)
{
	lcd_goto(0x40);
	char ad = 0x00;
	char test;
	char before_div;
	char dived_data[2];
	
	lcd_goto(0x00);
	
	while(1)
	{
	test = getch();
	
	data_div(test, dived_data);
	dived_data[0] = hex2char(dived_data[0]);
	dived_data[1] = hex2char(dived_data[1]);

	if(ad == 0x0F)
	ad = 0x40;
	else if(ad == 0x4F)
	ad = 0x00;
	else	
	ad++;
	lcd_goto(ad);

	lcd_putch(dived_data[0]);
	
	if(ad == 0x0F)
	ad = 0x40;
	else if(ad == 0x4F)
	ad = 0x00;
	else	
	ad++;
	lcd_goto(ad);
	
	lcd_putch(dived_data[1]);
	}
	
}

void data_div(char freq, char *dived)
{
	dived[0] = ((freq >> 4 ) & 0x0F);  //upper
	dived[1] = (freq & 0x0F);	     //lower
}

char hex2char(char val)
{
	char ascii_array[16] = {'0','1','2','3','4',
							'5','6','7','8','9',
							'A','B','C','D','E','F'};
							
	val = ascii_array[(val & 0x0F)];
	
	return val;
}