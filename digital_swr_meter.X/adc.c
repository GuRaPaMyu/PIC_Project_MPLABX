#ifndef _XTAL_FREQ
	#define _XTAL_FREQ 8000000
#endif

#include <xc.h>
#include "adc.h"

void adc_init(void)
{
	ADCON0 &= 0x05;
	ADCON0bits.ADCS = 0b10;
	ADCON1bits.ADFM = 1; //use 2bit of ADRESH
	ADCON1bits.ADCS2 = 0;
	ADCON1bits.VCFG = 0b00;
	ADON = 1;
}

int adc_read(unsigned char channel)
{
	channel &= 0x07;	// truncate channel to 3 bits
	ADCON0 &= 0xC5;
	ADCON0 |=(channel << 3);
	__delay_us(10);	
	GO = 1;	// initiate conversion on the selected channel
	while(GO)
		continue;
	
	return (ADRESH << 8) | ADRESL;
}

void adc_itrpt_init(void)
{
	INTCON=0;	// purpose of disabling the interrupts.
	ADIE = 1;
	ADIF = 0;
	PEIE = 1;
	GIE = 1;
}

void adc_itrpt_go(unsigned char channel)
{
	channel &= 0x07;	// truncate channel to 3 bits
	ADCON0 &= 0xC5 | (channel << 3);
	GO=1;	// initiate conversion on the selected channel
}

int adc_itrpt_read(void)
{
	ADIE = 0;
	ADIF = 0;
	return (ADRESH << 8) | ADRESL;
}