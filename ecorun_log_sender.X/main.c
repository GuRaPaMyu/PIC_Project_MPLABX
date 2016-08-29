/*
＜周波数カウンター＞
■機能
sw1:プリスケーラの切り替え 
・sw1=11/1 
・sw1=01/8 
sw2:ゲートタイムの切り替え
・sw2=11秒
・sw2=00.1秒
sw3:-455kHzの有無切り替え 
・sw3=1-0kHz
・sw3=0-455kHz 
sw4:表示レンジの切り替え
・sw4=1Hz表示
・sw4=0kHz表示
■コンフィグ設定
LVP_OFF
MCLR_OFF
WDT_OFF
EXTCLK
■ピンアサイン 
Pin-01ＬＣＤ：Ｄ５ 
Pin-02ＬＣＤ：Ｄ４ 
Pin-03ＬＥＤ
Pin-04未使用
Pin-05Ｖｓｓ（ＧＮＤ） 
Pin-06プリスケーラの切替ＳＷ 
Pin-07ゲートタイムの切替ＳＷ 
Pin-08－４５５ｋＨｚの切替ＳＷ 
Pin-09表示レンジの切替ＳＷ 
Pin-10ＬＣＤ：ＲＳ
Pin-11ＬＣＤ：ＷＲ
Pin-12信号入力
Pin-13ＬＣＤ：ＥＮ
Pin-14Ｖｄｄ（＋５Ｖ） 
Pin-15未使用
Pin-16クロック入力（２０ＭＨｚ入力）
Pin-17ＬＣＤ：Ｄ７
Pin-18ＬＣＤ：Ｄ６ 
*/
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

#define GATETIME_100MSEC  10
#define GATETIME_1SEC   1
#define PRESC 0 //prescalar disable
#define GTIME 0 //getetime 1sec
#define WHEEL_PERIMETER 1
#define MAGNET_NUM 12

#define _XTAL_FREQ 20000000

static unsigned int MeasurementCnt;

void interrupt itrpt(void)
{
  TMR2IF = 0;
  
  MeasurementCnt--;
  if (MeasurementCnt == 0) {
    TMR1ON = 0; // ゲートを閉める。 
    TMR2ON = 0; // TIMER2を停止する。 
  }
}

unsigned  long  FreqMeasurement(unsigned char gateTime)
{
  static unsigned long freq;
  // TIMER1の設定
  TMR1IF = 0;
  TMR1L = 0;
  TMR1H = 0;
  // TIMER2の設定
  TMR2IF = 0;
  switch (gateTime) {
  case GATETIME_1SEC:
    MeasurementCnt = 1221;
    TMR2 = 0x4C;    // 312500=1/((1/20000000) * 4 * 16)
              // 0x4C=256-(312500-(256*1220))
    break;
  case GATETIME_100MSEC:
    MeasurementCnt = 123;
    TMR2 = 0xEE;    // 31250=0.1/((1/20000000) * 4 * 16)
              // 0xEE=256-(31250-(256*122))
    break;
  }
  //
  freq = 0;
  // 割り込みを許可する。 
  PEIE = 1;
  GIE = 1;
  // 開始 
  TMR2ON = 1; //タイマを開始する。
  
  TMR1ON = 1; //ゲートを開ける。
  // 測定 
  while (TMR2ON != 0) {
    if (TMR1IF == 1) {
      TMR1IF = 0;
      freq++;
    }
  }
  if (TMR1IF == 1) {
    TMR1IF = 0;
    freq++;
  }
  //換算 
  freq = freq * 65536;
  freq = freq + ((unsigned)TMR1H * 256) + (unsigned)TMR1L;

  return (freq);
}

void main()
{
  static char* msg;
  static unsigned long freq, temp; // 0...4294967295
  double speed, rpm;
  unsigned char buf[13], prescaler, gateTime;
  // ポートの設定
  TRISA = 0b11101111;
  TRISB = 0b00111111;
  TRISC = 0b00000001;
  PORTA = 0x00;
  PORTB = 0x00;
  PORTC = 0x00;
  // アナログの設定
  ANSEL  = 0b11111111;  // 使用する
  ANSELH  = 0b00111111;
  
  nRBPU = 0;    // PORTBをプルアップする。 
  // TIMER2の設定
  TMR2IE = 1;
  TMR2IF = 0;
  TOUTPS0 = 0;
  TOUTPS1 = 0;
  TOUTPS2 = 0;
  TOUTPS3 = 0;
  TMR2ON = 0;
  T2CKPS0 = 1;
  T2CKPS1 = 1;
  TMR2 = 0;
  // TIMER1の設定
  TMR1GE = 0;
  TMR1IE = 0;
  TMR1IF = 0;
  T1CKPS0 = 0;
  T1CKPS1 = 0;
  T1OSCEN = 0;
  T1INSYNC = 1;
  TMR1CS = 1;
  TMR1ON = 0;
  TMR1L = 0;
  TMR1H = 0;
  // 変数の初期化 
  prescaler = 1;
  gateTime = GATETIME_100MSEC;

  adc_init();
  
//   ＬＣＤ（液晶モニタ）の初期化 
  lcd_init();
  lcd_puts("Freq Counter");
  __delay_ms(1000);
  lcd_clear(); 
  
  while (1) {
    // 周波数の測定 
    freq = FreqMeasurement(gateTime);
    //換算 
    freq = freq * prescaler * gateTime;

    // プリスケーラの切り替え
    if (PRESC == 0) {
      T1CKPS0 = 0;
      T1CKPS1 = 0;
      prescaler = 1;
      msg = "1/1 ";
    } else {
      T1CKPS0 = 1;
      T1CKPS1 = 1;
      prescaler = 8;
      msg = "1/8 ";
    }

    // ゲートタイムの切り替え
    if (GTIME == 0) {
      gateTime = GATETIME_1SEC;
      msg = "1sec   ";
    } else {
      gateTime = GATETIME_100MSEC;
      msg = "0.1sec ";
    }
    
    lcd_goto(0x00);
    lcd_puts_ltoa(buf, 13, freq, 1);
    lcd_goto(0x0C);
    lcd_puts("Hz");
    
    rpm = freq / MAGNET_NUM * 2 * 60;
    speed = freq / 3600 * WHEEL_PERIMETER / MAGNET_NUM;
    
    lcd_goto(0x40);
    lcd_puts_ltoa(buf, 8, (long)rpm, 1);
    lcd_puts("rpm");

//    
//    double hoge;
//    hoge = adc_read(0);
//    hoge = 5000 / 1024 * hoge;
//    hoge = hoge - 400;
//    hoge = hoge / 19.5 * 1000;
//    lcd_goto(0x40);
//    lcd_puts_ltoa(buf, , (long)hoge, 1);
  }
}