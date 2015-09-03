#define _XTAL_FREQ 8000000

#define SEG_0 0b01110111
#define SEG_1 0b01000100
#define SEG_2 0b01011011
#define SEG_3 0b01011110
#define SEG_4 0b01101100
#define SEG_5 0b00111110
#define SEG_6 0b00111111
#define SEG_7 0b01010100
#define SEG_8 0b01111111
#define SEG_9 0b01111110

#define DIG1 RA3
#define DIG2 RA2
#define DIG3 RA7
#define DIG4 RA6

#define SEG_WAIT_MS 5
#define I_OFFSET 2
#define MED_VALUE 9

#include <xc.h>
#include <pic16f88.h>

// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)


void adc_init(void);
int adc_read(unsigned char channel);
void seg_light(int num);
void four_digit_seg(int val);
double median_value(double c[9]);

int main (void)
{
    	CMCON = 0b00000111; //cut off analog comparator just add this one
	OSCCON = 0b01111010;
	INTCON=0;	// purpose of disabling the interrupts.
	ANSEL = 0b00000011;

	TRISA = 0b00010011;
	TRISB = 0b00000000;
        PORTA = 0x00;
        PORTB = 0b00000000;

        adc_init();

        double volt = 0;
        double amp = 0;
        int i = 0;
        double buf[MED_VALUE];

	while(1)
	{
            if(RA4 == 1)
            {
                if(i < MED_VALUE)
                {
                    buf[i] = (double)adc_read(0);
                    i++;
                }else{
                    volt = median_value(buf) * 1.6;
                    i = 0;
                }
                four_digit_seg((short)(volt));
            }else if(RA4 == 0){
                if(i < MED_VALUE)
                {
                    buf[i] = (double)adc_read(1);
                    i++;
                }else{
                    amp = median_value(buf) * 1.5 - I_OFFSET;
                    i = 0;
                }
                four_digit_seg((short)(amp));
            }
        }
	return 0;
}

void four_digit_seg(int val)
{
    seg_light(val % 10);
    DIG1 = 1;
    __delay_ms(SEG_WAIT_MS);
    DIG1 = 0;
    seg_light((val / 10) % 10);
    DIG2 = 1;
    __delay_ms(SEG_WAIT_MS);
    DIG2 = 0;
    seg_light((val / 100) % 10);
    DIG3 = 1;
    __delay_ms(SEG_WAIT_MS);
    DIG3 = 0;
    seg_light((val / 1000) % 10);
    DIG4 = 1;
    __delay_ms(SEG_WAIT_MS);
    DIG4 = 0;
}

void seg_light(int num)
{
    switch(num)
    {
            case 0: PORTB = SEG_0;
                            break;
            case 1: PORTB = SEG_1;
                            break;
            case 2: PORTB = SEG_2;
                            break;
            case 3: PORTB = SEG_3;
                            break;
            case 4: PORTB = SEG_4;
                            break;
            case 5: PORTB = SEG_5;
                            break;
            case 6: PORTB = SEG_6;
                            break;
            case 7: PORTB = SEG_7;
                            break;
            case 8: PORTB = SEG_8;
                            break;
            case 9: PORTB = SEG_9;
                            break;
            default : PORTB = SEG_8;
    }
}

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

double median_value(double c[MED_VALUE])
{
    int i, j;
    double buf;

    for (j = 0; j < MED_VALUE - 1; j++) {
        for (i = 0; i < MED_VALUE - 1; i++) {
            if (c[i+1] < c[i]) {
                buf = c[i+1];
                c[i+1] = c[i];
                c[i] = buf;
            }
        }
    }
    return c[4];
}