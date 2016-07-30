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

// PIC16F88 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>
#include <pic16f88.h>
#include "lcd.h"
#include <stdlib.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)

#define GATETIME_100MSEC  10
#define GATETIME_1SEC   1
#define PRESC 0
#define GTIME 1

#define _XTAL_FREQ 20000000

static unsigned int MeasurementCnt;
void ltoa_lcd(char *buf, int buf_size, long val, char div);

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
  //  Delay
  __delay_ms(100);
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
  unsigned char buf[13], prescaler, gateTime;
  // アナログの設定
  ANSEL  = 0b00000000;  // 使用しない。
  // ポートの設定
  TRISA  = 0b11100000;
  TRISB  = 0b01001000;
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
  TMR1IE = 0;
  TMR1IF = 0;
  T1RUN = 0;
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
  
  // ＬＣＤ（液晶モニタ）の初期化 
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
    if (PRESC == 1) {
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
    if (GTIME == 1) {
      gateTime = GATETIME_1SEC;
      msg = "1sec   ";
    } else {
      gateTime = GATETIME_100MSEC;
      msg = "0.1sec ";
    }
    
    lcd_goto(0x00);
    ltoa_lcd(buf, 13, freq, 1);
    lcd_goto(0x0C);
    lcd_puts("Hz");
    
    __delay_ms(50);
  }
}

void ltoa_lcd(char *buf, int buf_size, long val, char div)
{
  char ascii_array[10] = {0x30,0x31,0x32,0x33,0x34,
              0x35,0x36,0x37,0x38,0x39};
  char zero_flag = 0;
  char cnt = 0;
  
  buf[buf_size - 1] = 0x00;
  buf_size --;
  while(buf_size > 0)
  {
    if(zero_flag == 0)
    {
      buf[buf_size - 1] = ascii_array[(val%10 & 0x0F)];
      val /= 10;
      buf_size --;
    }else{
      buf[buf_size - 1] = 0x20;
      val /= 10;
      buf_size --;
    }
    if(((cnt % 3) == 2) && (val != 0) && (div == 1))
    {
      buf[buf_size - 1] = 0x2e;  //0x2c is comma,0x2e is decimal
      buf_size --;
    }
    cnt ++;
    val == 0 ? zero_flag=1 : zero_flag=0;
  }
  lcd_puts(buf);
}