// PIC16F886 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR21V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include "lcd.h"
#include "adc.h"

#define WHEEL_PERIMETER 1.07 //m
#define MAGNET_NUM 12

#define _XTAL_FREQ 20000000

static unsigned long overflow;
static unsigned long count;

void interrupt itrpt(void)
{
  if(TMR1IF){
    TMR1IF = 0;
    overflow ++;
  }
  if(INTF){
    INTF = 0;
    
    count = overflow << 16;
    count += ((unsigned)TMR1H * 256) + (unsigned)TMR1L;
    if(overflow > 100)
      count = 0;
    overflow = 0;
    TMR1H = 0;
    TMR1L = 0;
  }
}


void main()
{
  static char* msg;
  static unsigned long freq, temp; // 0...4294967295
  double speed, rpm;
  unsigned char buf[13];
  // ポートの設定
  TRISA = 0b11101111;
  TRISB = 0b00111111;
  TRISC = 0b00000001;
  PORTA = 0x00;
  PORTB = 0x00;
  PORTC = 0x00;
  // アナログの設定
  ANSEL  = 0b11111111;  // 使用する
  ANSELH  = 0b00101111;
  
  nRBPU = 0;    // PORTBをプルアップする。 

  TMR1CS = 0;
  T1OSCEN = 0;
  TMR1GE = 0;
  TMR1IF = 0;
  TMR1IE = 1;
  TMR1ON = 1;
  T1CONbits.T1CKPS = 0b00;
  
  INTEDG = 1; //立ち上がりエッジRB0/INT
  INTF = 0;
  INTE = 1; //enable interrupt RB0/INT

  adc_init();
  
//   ＬＣＤ（液晶モニタ）の初期化 
  lcd_init();
  lcd_puts("DATA  LOGGER  v1");
  __delay_ms(1000);
  lcd_clear();
  
  PEIE = 1;
  GIE = 1;
  while (1) {
    if(overflow > 100)
      count = 0;
    
    rpm = 5 * 1000000 * 60 / count / MAGNET_NUM * 2;
    lcd_goto(0x00);
    lcd_puts_ltoa(buf, 13, rpm, 1);
    lcd_puts("rpm");
    
    speed = rpm * WHEEL_PERIMETER * 60 / 1000;
    
    lcd_goto(0x40);
    lcd_puts_ltoa(buf, 8, (long)speed, 1);
    lcd_puts("km/h");
  }
}