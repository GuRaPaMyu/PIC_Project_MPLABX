/*
	LanCableChecker ver 1.0
	edited by JP7FKF

	this program will run on PIC16F887
	this checker has 4mode(Straight cable, Cross over cable, Custom, Debug).
	please select mode you want to check.
	this program use lcd.h and eeprom.h which is in libraries folder.
*/

#define _XTAL_FREQ 8000000

#define LCD_PIN_SET
#define LCD_RS RE0
#define LCD_RW RA4
#define LCD_EN RA5
#define LCD_DATA PORTA

#define BUTTON1 RC2
#define BUTTON2 RC1
#define BUTTON3 RC0

#define SENDP PORTB
#define RCVP PORTD

#include <xc.h>
#include <pic16f887.h>
#include "lcd.h"
#include "eeprom.h"

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

void ModeSelect(void);
void SetCheckRow(void);
void SetNumberOfTimes(void);
void Confirm(void);
void CheckDo(void);
void DebugMode(void);
void ShowResult(void);
int ButtonPush(void);
void IndicadeSettingMode(int modenum);
void IndicateSelectedMode(char start);
void Setting(void);
void SetIntervalTime(void);

int state = 0;
int mode = 0;	//0:str 1:cross 2:custom 3:debug
int checkrow[8] = {1,2,3,4,5,6,7,8};
int times = 5;
int interval_time_ms = 20;
int ok_num;
int ng_num;

void main(void)
{

	OSCCON = 0b01110001;	//OSC : INTERNAL 8MHz
	ANSEL = 0x00;
	INTCON = 0x00;
	CCP1CON = 0x00;
	CCP2CON = 0x00;
	CM1CON0 = 0x00;
	CM2CON0 = 0x00;

	TRISA = 0x00;
	TRISB = 0x00;
	TRISC = 0x07;
	TRISD = 0xFF;
	TRISE = 0x00;

	PORTA = 0x00;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
	PORTE = 0x00;
	
	lcd_init();
	lcd_goto(0x00);
	lcd_puts("LanCable Checker");
	lcd_goto(0x40);
	lcd_puts("PSE PUSH ANY BTN");
	ButtonPush();
	
	eeprom_init(0,1);
	times = eeprom_read_int(0x00);
	interval_time_ms = eeprom_read_int(0x02);
		
	while(1)
	{
		switch(state)
		{
			case 0 :	ModeSelect();
								break;
			case 1 : 	SetCheckRow();
								break;
			case 2 :	SetNumberOfTimes();
								break;
			case 3 :	Confirm();
								break;
			case 4 :	CheckDo();
								break;
			case 5 :	DebugMode();
								break;
			case 6 :	ShowResult();
								break;
			case 7 :	Setting();
								break;
			default :	ModeSelect();
		}
	}
}

void ModeSelect(void)
{
	int button = 0;
	
	lcd_clear();
	lcd_goto(0x00);
	lcd_puts("  Mode  Select  ");
	IndicateSelectedMode(0x40);

	while((button != 3) && (button != 4))
	{
		button = ButtonPush();

		if(button == 1)
		{
			mode --;
			if(mode < 0)
				mode += 5;
			IndicateSelectedMode(0x40);
		}
		if(button == 2)
		{
			mode ++;
			if(mode > 4)
				mode -= 5;
			IndicateSelectedMode(0x40);
		}
	}
	if(button == 4)
		state = 7;
	else
		state = 1;
}
void Setting(void)
{
	int setmode = 0;
	int button = 0;

	lcd_clear();
	lcd_goto(0x00);
	lcd_puts("Settig Mode");
	lcd_goto(0x40);
	IndicadeSettingMode(setmode);

	while(button != 3)
	{
		button = ButtonPush();

		if(button == 1)
		{
			setmode ++;
			if(setmode > 0)
				setmode -= setmode;
		}
		if(button == 2)
		{
			setmode --;
			if(setmode < 0)
				setmode += 1;
		}
		lcd_goto(0x40);
		IndicadeSettingMode(setmode);
	}

	switch(setmode)
	{
		case 0 :	SetIntervalTime();
							break;
		default:	SetIntervalTime();

	}
	state = 0;
}	

void SetCheckRow(void)
{
	int i;
	int button = 0;
	int cursor = 0;

	if((mode == 0) || (mode == 1) || (mode == 2))
	{
		for(i=0;i<8;i++)  //set straight cable row
		{
			checkrow[i] = i+1;
		}

		if(mode == 1)  //cross over cable mode
		{
			checkrow[0] = 3;
			checkrow[1] = 6;
			checkrow[2] = 1;
			checkrow[5] = 2;
		}
    if(mode == 2)  //full cross mode
    {
      checkrow[0] = 3;
			checkrow[1] = 6;
			checkrow[2] = 1;
			checkrow[5] = 2;
      checkrow[3] = 7;
			checkrow[4] = 8;
			checkrow[6] = 4;
			checkrow[7] = 5;
    }
	}else if(mode == 3)	//custom cable mode
	{
		lcd_clear();
		lcd_goto(0x00);
		lcd_puts("    12345678    ");
		lcd_goto(0x44);
		for(i=0;i<8;i++)
		{
			lcd_putch((char)(0x30 + checkrow[i]));
		}
		lcd_goto(0x44);
		lcd_cmd(0b00001111); // Display On, Cursor on blinking on

		while(button != 3 && button != 4)
		{
			button = ButtonPush();

			if(button == 1)
			{
				cursor ++;
				if(cursor > 7)
				cursor -= 8;
				lcd_goto(0x44 + cursor);
			}

			if(button == 2)
			{
				checkrow[cursor] ++;
				if(checkrow[cursor] > 8)
					checkrow[cursor] -= 8;
				lcd_putch((char)(0x30 + checkrow[cursor]));
				lcd_goto(0x44 + cursor);
			}
		}
		lcd_cmd(0b00001100); // Display On, Cursor off blinking off
	}
	if(button == 4)
		state = 0;
	else
		state = 2;
}

void SetNumberOfTimes(void)
{
	int i;
	char numstr[4];
	int button = 0;
	int cursor = 2;

	lcd_clear();
	lcd_goto(0x00);
	lcd_puts("SetNumberOfTimes");
	lcd_goto(0x40);
	lcd_putint(numstr, times, 10, 4, 0);
	lcd_goto(0x43);
	lcd_puts(" times");
	lcd_goto(0x40 + cursor);
	
	lcd_cmd(0b00001110); // Display On, Cursor on blinking off

	if(mode != 4)
	{
		while((button != 3) && (button != 4))
		{
			button = ButtonPush();

			if(button == 1)
			{
				cursor --;
				if(cursor < 0)
					cursor += 3;
				lcd_goto(0x40 + cursor);
			}

			/*  if you want to do inc. or dec. by left and right button, pse use this block instead of upper one.
			if(button == 1)
			{
				times --;
				
				if(times < 0)
					times += 1000;
					
				lcd_goto(0x40);
				lcd_putint(numstr, times, 10, 4);
			}
			*/

			if(button == 2)
			{
				if(cursor == 0)
				{
					if(times/100 == 9)
						times -= 1000;
					times += 100;
				}else if(cursor == 1){
					if((times / 10) % 10 == 9)
						times -= 100;
					times += 10;
				}else{
					if(times % 10 == 9)
						times -= 10;
					times ++;
				}

				lcd_goto(0x40);
				lcd_putint(numstr, times, 10, 4, 0);
				lcd_goto(0x40 + cursor);
			}
		}
	}

	lcd_cmd(0b00001100); // Display On, Cursor off blinking off
	eeprom_write_int(0x00, times);  //save to EEPROM
	
	if(button == 4)
		state = 0;
	else
		state = 3;
}

void Confirm(void)
{
	char chknum[4];
	lcd_clear();
	lcd_goto(0x00);
	lcd_puts("MODE:");

	switch(mode)
	{
		case 0 :	lcd_puts("STRAIGHT  ");
							break;
		case 1 :	lcd_puts("CROSS     ");
							break;
    case 2 :	lcd_puts("FULL CROSS");
          		break;
		case 3 :	lcd_puts("CUSTOM    ");
							break;
		case 4 :	lcd_puts("DEBUG     ");
							break;
		default:	lcd_puts("??????????");
	}

	if(mode != 4)
	{
		lcd_goto(0x46);
		lcd_putint(chknum, times, 10, 4, 1);
		lcd_puts(" TIMES");
	}

	if(ButtonPush() == 3)
	{
		state = 4;
		if(mode == 4)
			state = 5;
	}else{
		if(mode == 4)
			state = 0;
		else
			state = 2;
	}	
}

void CheckDo(void)
{
	int i,j,k;
	char result = 0x00;
	char expected = 0x00;
	char checked_num[4] = {0x00};
	int success_check = 0;

	ok_num = 0;
	ng_num = 0;

	lcd_clear();
	lcd_puts("Checking...");
	lcd_goto(0x40);
	for(i=0;i<times;i++)
	{
		success_check = 0;
		for(j=0;j<8;j++)	//the pin switch on each
		{
			SENDP = 1 << j;
			expected = 0x00;
			
			for(k=0;k<8;k++)	//searh selected pins which should be check and set 1 to expected
			{
				if(checkrow[k] - 1  == j)
					expected |= (1 << k);
			}
			for(k=0;k<interval_time_ms;k++)
				__delay_ms(1);
				
			result = RCVP;
			
			if(result == expected)
				success_check ++;
		}
		lcd_goto(0x0B);
		lcd_putch('(');
		lcd_putint(checked_num, i + 1, 10, 4, 1);
		lcd_putch(')');
		lcd_goto(0x40 + (ok_num + ng_num) % 17);
		if(success_check == 8)
		{
			ok_num ++;
			lcd_putch('!');
		}else{
			ng_num ++;
			lcd_putch('.');
		}
	}

	SENDP = 0x00;
	state = 6;
}

void DebugMode(void)
{
	char debug_arr[9] = {0x00};
	int i,j;
	char result = 0x00;
	
	lcd_clear();
	lcd_goto(0x00);
	lcd_puts("    12345678    ");
	
	while(BUTTON1 && BUTTON2 && BUTTON3)
	{
		for(i=0;i<9;i++)
		{
			debug_arr[i] = 0x00;
		}

		for(i=0;i<8;i++)
		{
			SENDP = 1 << i;
			
			for(j=0;j<interval_time_ms;j++)
				__delay_ms(1);
				
			result = RCVP;

			for(j=0;j<8;j++)
			{
				if((result >> j) %  2 == 1)
				{
					if(debug_arr[j] == 0x00)
						debug_arr[j] = 0x31 + i;
					else
						debug_arr[j] = 0x2A;
				}
			}
		}
		lcd_goto(0x44);
		lcd_puts(debug_arr);
	}
	ButtonPush();
	SENDP = 0x00;
	state = 0;
}

void ShowResult(void)
{
	char result_str[4];
	
	lcd_clear();
	lcd_goto(0x00);
	lcd_puts("Result");
	lcd_goto(0x08);
	lcd_putch('(');
	lcd_putint(result_str, (int)((ok_num / (ok_num + ng_num)*100)), 10 ,4, 1);
	lcd_puts("%)");
	lcd_goto(0x40);
	lcd_puts("OK:");
	lcd_putint(result_str, ok_num, 10, 4, 1);
	lcd_goto(0x48);
	lcd_puts("NG:");
	lcd_putint(result_str, ng_num, 10, 4, 1);
	
	ButtonPush();
	
	state = 0;
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

void IndicadeSettingMode(int modenum)
{
	switch(modenum)
	{
		case 0 :	lcd_puts("SET:ScanWait(ms)");
							break;
		default : lcd_puts("SET:???");
	}
}

void IndicateSelectedMode(char start)
{
	lcd_goto(start);
	switch(mode)
	{
		case 0 : 	lcd_puts("MODE:STRAIGHT  ");
							break;
		case 1 :	lcd_puts("MODE:CROSS     ");
							break;
    case 2 :	lcd_puts("MODE:FULL CROSS");
							break;
		case 3 :	lcd_puts("MODE:CUSTOM    ");
							break;
		case 4 :	lcd_puts("MODE:DEBUG     ");
							break;
		default :	lcd_puts("MODE:??????????");
	}
}

void SetIntervalTime(void)
{
	int button = 0;
	char str[4] = {0x00};
	
	lcd_clear();
	lcd_goto(0x00);
	lcd_puts("Set ScanWaitTime");
	lcd_goto(0x40);
	lcd_putint(str, interval_time_ms, 10, 4, 1);
	lcd_goto(0x44);
	lcd_puts("ms/scan");

	while(button != 3)
	{
		button = ButtonPush();

		if(button == 1)
		{
			interval_time_ms --;
			if(interval_time_ms < 1)
				interval_time_ms += 999;
		}
		if(button == 2)
		{
			interval_time_ms ++;
			if(interval_time_ms > 999)
				interval_time_ms -= interval_time_ms - 1;
		}
		lcd_goto(0x40);
		lcd_putint(str, interval_time_ms, 10, 4, 1);
	}

	eeprom_write_int(0x02, interval_time_ms);
}