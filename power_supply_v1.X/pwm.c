#include <xc.h>

#define _XTAL_FREQ 8000000

void pwm_init(unsigned long freq, unsigned char T2presc)  //0 is presc 1, 1 is presc 4, 3 is presc 16
{
	T2CON &= 0xFC;
	switch(T2presc)
	{
		case 1 :	T2CON |= 0x00;
							break;
		case 2 :	T2CON |= 0x01;
							break;
		case 16:	T2CON |= 0x02;
							break;
		default:	T2CON |= 0x00;
	}
	
	PR2 = (unsigned char)(_XTAL_FREQ / freq / 4 - 1);
	CCPR1L = 0;
	OPTION_REGbits.PS = 0b101; //128cnt prescaler
	TMR2ON = 1; //enable timer2 module
	CCP1CON = 0x0C; //set PWM mode
}

void pwm_set_duty(double duty_ratio)
{
	unsigned short reg;
	reg = (unsigned short)duty_ratio * (PR2 + 1) / 25;	//duty_ratio / 100 * (PR2 + 1) * 4 @ dutyratio = Tduty / Tpwm
	CCPR1L =  reg >> 2;
	CCP1CON &= 0x0F;
	CCP1CON |= (reg << 4) & 0x30;
}