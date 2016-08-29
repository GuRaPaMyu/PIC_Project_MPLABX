#ifndef _XTAL_FREQ
	#define _XTAL_FREQ 8000000
#endif

#include <xc.h>
#include "adc.h"

void adc_init(void)
{
  ADCON0bits.ADON = 1;
  ADCON0 &= 0x03;
  ADCON1bits.ADFM = 1; //use 2bit of ADRESH
  ADCON0bits.ADCS = 0b10;
  ADCON1bits.VCFG0 = 0;
  ADCON1bits.VCFG1 = 0;
}

int adc_read(unsigned char channel)
{
	channel &= 0x0F;	// truncate channel to 4 bits
	ADCON0 &= 0xC3;
	ADCON0 |=(channel << 2);
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