#define _XTAL_FREQ 20000000
#define IC7200


#ifdef IC7200
	#define CHKDATA 4
	#define FREQ_START 5
	#define ADDRESS 0x76
#endif
//#ifdef IC756
//	#define CHKDATA 4
//	#define FREQ_START 5
//	#define ADDRESS 0x50
//#else
//	#define CHKDATA 4
//	#define FREQ_START 5
//	#define ADDRESS 0x00
//#endif

/*
__CONFIG(FOSC_HS
		& WDTE_OFF & PWRTE_ON
		& MCLRE_ON & BOREN_OFF
		& LVP_OFF & CPD_OFF
		& CP_OFF);

__CONFIG(FCMEN_OFF & IESO_OFF);

*/


// PIC16F88 Configuration Bit Settings

// 'C' source line config statements
#include <xc.h>
#include <pic16f88.h>
#include "lcd.h"
#include "usart.h"
#include "debug.h"
#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF         // Low-Voltage Programming Enable bit (RB3/PGM pin has PGM function, Low-Voltage Programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = OFF        // Internal External Switchover bit (Internal External Switchover mode enabled

// prototype declaration
void ci_v_init();
void scanning_frequency_data();
void check_data();
void data_division(char freq, int num);
void swap_freq_data();
char trans_value(char val);
void indicate_frequency_data();
void indicate_mode();
void scan_ex();
void debug_unit(char debug_unit);

//variable declaration
static int status_num = 0;
static int flag = 0;
char receive_data[12];
char freq_data[8];
int data_num = 0;
int err_cnt = 0;
unsigned char recv_addr = 0x00;

void main(void)
{	
  ci_v_init();
	while(1)
	{
//	scan_ex(); 
		switch(status_num)
		{
			case 0 : 	ci_v_init();
						break;
						
			case 1 :	scanning_frequency_data();
						break;
									
			case 2 :	check_data();
						break;
						
			case 3 :	indicate_frequency_data();
						break;
						
			default:	ci_v_init();
		}
	}
}


void ci_v_init()
{
	if(flag == 0)
	{
	CMCON = 0b00000111; //cut off analog comparator just add this one
	TRISA = 0b00000000;
	TRISB = 0b00000100;
	PORTA = 0b00000000;
	PORTB = 0b00000000;
	ANSEL = 0;
	INTCON=0;	// purpose of disabling the interrupts.
	init_comms();	// set up the USART - settings defined in usart.h
	
	lcd_init();
	__delay_ms(2);
	lcd_goto(0x00);	// select first line
	__delay_us(50);
	lcd_puts("**** JP7IFH ****");
	lcd_goto(0x40);	// Select second line
	__delay_us(50);
	lcd_puts("- Freq Display -");
	__delay_ms(30);
	
	status_num = 1;
	
	}else{
	lcd_goto(0x00);
	__delay_us(50);
	lcd_puts("JP7IFH          ");
	__delay_us(50);
	lcd_goto(0x40);
	__delay_us(50);
	lcd_puts("Freq= ");
  recv_addr = receive_data[CHKDATA - 1];
  putch(0xFE);
	putch(0xFE);
	putch(recv_addr);
	putch(0x00);
	putch(0x04);
	putch(0xFD);
	status_num = 1;
	}
}

void scanning_frequency_data()
{
	data_num = 0;
	receive_data[data_num] = getch();
	
	if(receive_data[data_num] == 0xFE)
	{
		err_cnt = 0;
		while(receive_data[data_num] != 0xFD)
		{
		data_num++;
		receive_data[data_num] = getch();
		}
				
		status_num = 2;
	}else{
	
	/*
		if(err_cnt > 1)
		{
		lcd_indicate('4');
		accord_baudlate();
		err_cnt = 0;
		}else{ 
		lcd_indicate('5');
		err_cnt++;
		}
	*/
	status_num = 1;
	}
}

void check_data()
{
	
	if(receive_data[CHKDATA] == 0x00)  //check command number
	{
		swap_freq_data();
		
	}else if(receive_data[CHKDATA] == 0x01)
	{
		indicate_mode();
		
	}else{
	
	status_num = 1;
	}
}

void data_division(char freq, int num)
{
	freq_data[num] = ((freq >> 4 ) & 0x0F);  //upper
	freq_data[num+1] = (freq & 0x0F);	     //lower
}

char trans_value(char val)
{
	char ascii_array[10] = {0x30,0x31,0x32,0x33,0x34,
							0x35,0x36,0x37,0x38,0x39};
							
	val = ascii_array[(val & 0x0F)];
	
	return val;
}


void swap_freq_data()
{
	int space_flag;
	int i,j;

	for(i=0;i<4;i++)
	{
 	data_division(receive_data[FREQ_START + 3 - i],  i * 2);
 	}
 	
 	j = 0;
 	space_flag = 0;
	for(i=0;i<8;i++)
	{
		if((i == 2) || (i == 5))
		{
		receive_data[j] = 0x2e;  //0x2c is comma,0x2e is decimal
		j++;
		}
			
		receive_data[j] = trans_value(freq_data[i]); //set char instead of hex number
		j++;
	
		if((receive_data[i] == 0x30) && (space_flag == 0))  //change to space if the head number is 0
	 	{
	 	receive_data[i] = 0x20;
		}else{
		space_flag = 1;
		}
	
		if(j == 10)
		{
		receive_data[j] = 0x20;
		j++;
		}
	
	}
	status_num = 3;
}

void indicate_frequency_data()
{
	lcd_goto(0x46);	// Select freq expression place
	__delay_us(50);
	lcd_puts(receive_data);
	__delay_us(50);
	status_num = 1;

	if(flag == 0)
	{
	flag = 1;
	status_num = 0;
	}
}

void indicate_mode()
{	
	lcd_goto(0x07);
	__delay_us(50);
	lcd_puts("MODE:");
	__delay_us(50);
	
	lcd_goto(0x0C);
	
	switch(receive_data[CHKDATA + 1])
	{
		case 0 : lcd_puts(" LSB");
				break;
				
		case 1 : lcd_puts(" USB");
				break;
				
		case 2 : lcd_puts(" AM ");
				break;
				
		case 3 : lcd_puts(" CW ");
				break;
				
		case 4 : lcd_puts("RTTY");
				break;
				
		case 5 : lcd_puts(" FM ");
				break;
				
		default : lcd_puts("????");
	}
	status_num = 1;	
}