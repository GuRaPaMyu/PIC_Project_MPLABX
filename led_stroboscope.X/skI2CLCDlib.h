/*******************************************************************************
*  skI2CLCDlib.h - ??????????????????????????        *
*             ST7032i???????(8??x2??16??x2?)?????????? *
*                                                                              *
* ============================================================================ *
*  VERSION  DATE        BY                    CHANGE/COMMENT                   *
* ---------------------------------------------------------------------------- *
*  1.00     2013-07-25  ?????(????)  Create                           *
*  2.00     2015-03-01  ?????(????)  ST7032i??????LCD???     *
*******************************************************************************/
#ifndef _SKI2CLCDLIB_H_
#define _SKI2CLCDLIB_H_

#ifndef _XTAL_FREQ
 // Unless already defined assume 16MHz system frequency
 // This definition is required to calibrate __delay_us() and __delay_ms()
 #define _XTAL_FREQ 8000000  // ????PIC???????????????
#endif
#define ST7032_ADRES 0x3E     // I2C????LCD??????????

/******************************************************************************/
/*  ?????                                                                */
/******************************************************************************/
#define LCD_VDD3V                  1   // LCD????3.3V?????(????ON)
#define LCD_VDD5V                  0   // LCD????5.0V?????(????OFF)
#define LCD_USE_ICON               1   // ???????(?????LCD??)
#define LCD_NOT_ICON               0   // ?????????

/******************************************************************************/
/*  LCD?????????????(ST7032i????????)                    */
/******************************************************************************/
#define LCD_ICON_ANTENNA           0x4010    // ????
#define LCD_ICON_PHONE             0x4210    // ??
#define LCD_ICON_COMMUNICATION     0x4410    // ??
#define LCD_ICON_INCOMING          0x4610    // ??
#define LCD_ICON_UP                0x4710    // ?
#define LCD_ICON_DOWN              0x4708    // ?
#define LCD_ICON_LOCK              0x4910    // ?
#define LCD_ICON_KINSHI            0x4B10    // ??(???OFF?)
#define LCD_ICON_BATTERY_LOW       0x4D10    // ????????????
#define LCD_ICON_BATTERY_HALF      0x4D08    // ????????????
#define LCD_ICON_BATTERY_FULL      0x4D04    // ???????
#define LCD_ICON_BATTERY_EMPTY     0x4D02    // ??????????
#define LCD_ICON_HANAMARU          0x4F10    // ?????????(????????)

/******************************************************************************/
/*  ???????????                                                    */
/******************************************************************************/
void LCD_Clear(void) ;
void LCD_SetCursor(int col, int row) ;
void LCD_Putc(char c) ;
void LCD_Puts(const char * s) ;
void LCD_CreateChar(int p,char *dt) ;
void LCD_Init(int icon, int contrast, int bon, int colsu) ;
void LCD_Contrast(int contrast) ;
void LCD_IconClear(void) ;
void LCD_IconOnOff(int flag, unsigned int dt) ;
int  LCD_PageSet(int no) ;
void LCD_PageClear(void) ;
void LCD_PageSetCursor(int col, int row) ;
int  LCD_PageNowNo(void) ;
#endif