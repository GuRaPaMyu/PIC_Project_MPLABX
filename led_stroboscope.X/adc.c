#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif

#include <xc.h>
#include "adc.h"

void adc_init(void) {
    ADCON0 = 0x00;
    ADCON1bits.ADCS = 0b110;
    ADCON1bits.ADFM = 1; //use 2bit of ADRESH
    ADCON1bits.ADCS2 = 0;
    ADCON1bits.ADPREF = 0b11;
    ADON = 1;
}

int adc_read(unsigned char channel) {
    channel &= 0x07; // truncate channel to 3 bits
    ADCON0 &= 0x03;
    ADCON0 |= (channel << 3);
    __delay_us(10);
    GO = 1; // initiate conversion on the selected channel
    while (GO)
        continue;

    return (ADRESH << 8) | ADRESL;
}