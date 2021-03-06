/* 
 * File:   main.c
 * Author: YudaiHashimoto
 *
 * Created on September 2, 2015, 4:55 PM
 */

#include <xc.h>

#define _XTAL_FREQ 8000000

// ???????????????
#pragma config FOSC     = INTOSC   // ??????????(INTOSC)
#pragma config WDTE     = OFF      // ???????????????(OFF)
#pragma config PWRTE    = ON       // ??ON??64ms??????????????(ON)
#pragma config MCLRE    = OFF      // ??????????????????????(RA3)??????(OFF)
#pragma config CP       = OFF      // ?????????????????(OFF)
#pragma config CPD      = OFF      // ??????????????(OFF)
#pragma config BOREN    = ON       // ????????????ON(ON)
#pragma config CLKOUTEN = OFF      // CLKOUT????RA4????????(OFF)
#pragma config IESO     = OFF      // ????????????????????(OFF)
#pragma config FCMEN    = OFF      // ???????????(OFF)

// ???????????????
#pragma config WRT    = OFF        // Flash??????????(OFF)
#pragma config PLLEN  = OFF        // ???????32MHz????????(OFF)
#pragma config STVREN = ON         // ?????????????????????????????(ON)
#pragma config BORV   = HI         // ????????????(2.5V)??(HI)
#pragma config LVP    = OFF        // ?????????????????(OFF)

int main(void)
{
     OSCCON     = 0b01110010 ;   // ???????8??????
     OPTION_REG = 0b00000000 ;   // ????I/O???????????????
     ANSELA     = 0b00000000 ;   // ??????????????????I/O??????
     TRISA      = 0b11101100 ;   // ???RA1(SCL)/RA2(SDA)????(RA3?????)
     WPUA       = 0b00000000 ;   // RA1/RA2???????????????
     PORTA      = 0b00010000 ;   // ????????(??LOW???)
     
     __delay_ms(10);

     while(1)
     {
         while(1)
         {
             while(RA5 == 0)
                 continue;
             __delay_ms(5);
             if(RA5 == 1)
                 break;
         }

         RA4 = 0;

         RA1 = 1;
         __delay_ms(300);
         RA1 = 0;
         __delay_ms(300);
         RA1 = 1;
         __delay_ms(300);
         RA1 = 0;
         __delay_ms(300);
         RA1 = 1;
         __delay_ms(300);
         RA1 = 0;
         __delay_ms(300);
         RA1 = 1;
         __delay_ms(300);
         RA1 = 0;
         __delay_ms(300);

         if(RA5 == 0)
         {
             PORTA = 0b00010000;
         }else{
             PORTA = 0b00100011;
         }
         
         while(1)
         {
             while(RA5 == 1)
                 continue;
             __delay_ms(5);
             if(RA5 == 0)
                 break;
         }

         PORTA = 0b00010000;
     }

    return 0;
}

