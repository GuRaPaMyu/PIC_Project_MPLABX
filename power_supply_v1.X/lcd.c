/*
 *	LCD interface example
 *	Uses routines from delay.c
 *	This code will interface to a standard LCD controller
 *	like the Hitachi HD44780. It uses it in 4 bit mode, with
 *	the hardware connected as follows (the standard 14 pin
 *	LCD connector is used):
 *
 *	PORTD bits 0-3 are connected to the LCD data bits 4-7 (high nibble)
 *	PORTA bit 3 is connected to the LCD RS input (register select)
 *	PORTA bit 1 is connected to the LCD EN bit (enable)
 *
 *	To use these routines, set up the port I/O (TRISA, TRISD) then
 *	call lcd_init(), then other routines as required.
 *
 */

#ifndef _XTAL_FREQ
 // Unless specified elsewhere, 4MHz system frequency is assumed
 #define _XTAL_FREQ 8000000
#endif

#ifndef LCD_PIN_SET
 #define LCD_RS RA6
 #define LCD_RW RA6
 #define LCD_EN RA5
 #define LCD_DATA PORTB
#endif

#define	LCD_STROBE()	((LCD_EN = 1),(LCD_EN = 0))

#include	<xc.h>
#include	"lcd.h"

/* write a byte to the LCD in 4 bit mode */

void lcd_write(unsigned char c)
{
	__delay_us(40);
	LCD_DATA = (( c >> 4 ) & 0x0F ) | (LCD_DATA & 0xF0);
	LCD_STROBE();
	LCD_DATA = ( c & 0x0F ) | (LCD_DATA & 0xF0);
	LCD_STROBE();
}

/*
 * 	Clear and home the LCD
 */

void lcd_clear(void)
{
	LCD_RS = 0;
	lcd_write(0x01);
	__delay_ms(2);
}

/* write a string of chars to the LCD */

void lcd_puts(const char * s)
{
	LCD_RS = 1;	// write characters
	while(*s)
	lcd_write(*s++);
}

/* write one character to the LCD */

void lcd_putch(char c)
{
	LCD_RS = 1;	// write characters
	lcd_write( c );
}


/*
 * Go to the specified position
 */

void lcd_goto(unsigned char pos)
{
	LCD_RS = 0;
	lcd_write(0x80+pos);
}

/* initialise the LCD - put into 4 bit mode */
void lcd_init()
{
	char	 init_value;
	init_value = 0x3;
	LCD_RS = 0;
	LCD_EN = 0;
	LCD_RW = 0;

	__delay_ms(40);	// wait 15mSec after power applied,
	LCD_DATA = init_value;
	LCD_STROBE();
	__delay_ms(5);
	LCD_STROBE();
	__delay_us(200);
	LCD_STROBE();
	__delay_us(200);
	LCD_DATA = 2;	// Four bit mode
	LCD_STROBE();

	lcd_write(0x28); // Set interface length
	lcd_write(0b00001100); // Display On, Cursor off
	lcd_clear();	// Clear screen
	lcd_write(0x6); // Set entry Mode
}

void lcd_cmd(char cmd)
{
	LCD_RS = 0;
	lcd_write(cmd);
}

void lcd_putint(char *buf, int val, int base, int str_length, char space_f)
{
	unsigned int v;
	char c;
	int length = str_length;
	int minus_f = 0;

	if(val < 0)
	{
		minus_f = 1;
		*buf = '-';
		buf ++;
		length --;
		str_length --;
		val = -val;
	}

	v = val;
	while(length > 1)
	{
		v /= base;
		buf++;
		length --;
	}

	*buf = 0;

	do
	{
		if((val == 0) && (length != 1))
		{
			if(space_f == 1)
				c = 0x20;
			else
				c = 0x30;
		}else{
			c = val % base;
			val /= base;
			if(c >= 10)
			c += 'A'-'0'-10;
			c += '0';
		}
		buf --;
		length ++;
		*buf = c;
	}while(length < str_length);

	if(minus_f == 1)
	{
		buf --;
		length ++;
		str_length ++;
	}

	lcd_puts(buf);
}