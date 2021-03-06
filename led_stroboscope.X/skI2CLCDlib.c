/*******************************************************************************
*  skI2CLCDlib - ???????????????                                *
*             ST7032i???????(8??x2??16??x2?)?????????? *
*                                                                              *
*    LCD_Init       - ????????????                                 *
*    LCD_Clear      - ???????????????????????           *
*    LCD_SetCursor  - ?????????????????????????       *
*    LCD_Putc       - ??????????????????                     *
*    LCD_Puts       - ?????????????????                       *
*    LCD_CreateChar - ?????????????????                       *
*       (???Vr2.00????)                                                 *
*    LCD_Contrast   - ?????????????????                       *
*    LCD_IconClear  - ??????????????                             *
*    LCD_IconOnOff  - ?????????ON/OFF?????                       *
*    LCD_PageSet       - ???????????????                        *
*    LCD_PageClear     - ????????????????????              *
*    LCD_PageSetCursor - ?????????????????????????    *
*    LCD_PageNowNo     - ??????????????????                  *
*                                                                              *
*    ???__delay_us() and __delay_ms() ????????? "skI2CLCDlib.h" ? *
*         "#define _XTAL_FREQ 8000000"??????????                     *
*         8MHz???CPU??????????????????                    *
* ============================================================================ *
*  VERSION DATE        BY                    CHANGE/COMMENT                    *
* ---------------------------------------------------------------------------- *
*  1.00    2013-07-25  ?????(????)  Create                            *
*  1.01    2014-08-29  ?????(????)  18F25K22????(delay_ms????)*
*  1.02    2015-02-02  ?????(????)  64MHz????((delay_ms????)  *
*  2.00    2015-03-01  ?????(????)  ST7032i??????LCD???      *
* ============================================================================ *
*  PIC 12F1822 16F1827 18F25K22 (????????????PIC??OK?????)  *
*  MPLAB IDE(V8.63) MPLAB X(v2.15)                                             *
*  MPLAB(R) XC8 C Compiler Version 1.00/1.32                                   *
*******************************************************************************/
#include <xc.h>
#include "skI2Clib.h"
#include "skI2CLCDlib.h"


static int LCD_ColumnSu ;          // ???????????????(8/16)
static int LCD_NowPage ;           // ????????????????????
static unsigned int FuncSet_DT ;   // "function set"?????????
static unsigned int ContSet_DT ;   // "Contrast set"?????????

// ???? x 10ms ???
void Wait10ms(unsigned int num)
{
     do {
          __delay_ms(10) ;    // 10ms??????????
     } while(--num != 0);
}
// ?27us???
void Wait27us(void)
{
     __delay_us(26) ;
}
// ???????????????
void command(unsigned char c)
{
     int  ans ;

     ans = I2C_Start(ST7032_ADRES,RW_0);     // ????????????????
     if (ans == 0) {
          // command word ???
          I2C_Send(0b10000000) ;             // control byte ???(???????)
          I2C_Send(c) ;                      // data byte ???
     }
     I2C_Stop() ;                            // ????????????????
     Wait27us() ;
}
/*******************************************************************************
*  LCD_Clear( )                                                                *
*    ???????????????????????                            *
*    ???Display Data RAM(2x40byte)?20H?????????????           *
*    ?????????????                                                *
*******************************************************************************/
void LCD_Clear(void)
{
     command(0x01) ;     // Clear Display : ?????20H???????????????col=0,row=0???
     __delay_us(1100) ;  // LCD???(1.08ms)????????
     command(0x02) ;     // Return Home   : ?????????????
     __delay_us(1100) ;  // LCD???(1.08ms)????????
     LCD_NowPage  = 0 ;  // ???????????????
}
/*******************************************************************************
*  LCD_SetCursor(col,row)                                                      *
*    ?????????????????????????                        *
*    ???Display Data RAM?2x40byte??????????(??2-line display)  *
*    ?????????????????????????????????????*
*                                                                              *
*    col : ?(?)?????????(0-39)                                      *
*    row : ?(?)?????????(0-1)                                       *
*******************************************************************************/
void LCD_SetCursor(int col, int row)
{
     int row_offsets[] = { 0x00, 0x40 } ;

     command(0x80 | (col + row_offsets[row])) ; // Set DDRAM Adddress : 00H-27H,40H-67H
}
/*******************************************************************************
*  LCD_Putc(c)                                                                 *
*    ??????????????????                                      *
*                                                                              *
*    c :  ????????????                                             *
*******************************************************************************/
void LCD_Putc(char c)
{
     int  ans ;

     ans = I2C_Start(ST7032_ADRES,RW_0);     // ????????????????
     if (ans == 0) {
          // command word ???
          I2C_Send(0b11000000) ;             // control byte ???(??????)
          I2C_Send(c) ;                      // data byte ???
     }
     I2C_Stop() ;                            // ????????????????
     Wait27us() ;
}
/*******************************************************************************
*  LCD_Puts(*s)                                                                *
*    ?????????????????                                        *
*    ?????NULL(0x00)???????????                                *
*                                                                              *
*    *s :  ??????????????????????????                *
*******************************************************************************/
void LCD_Puts(const char * s)
{
     int  ans ;

     ans = I2C_Start(ST7032_ADRES,RW_0);     // ????????????????
     if (ans == 0) {
          I2C_Send(0b01000000) ;             // control byte ???(??????)
          while(*s) {
               I2C_Send(*s++) ;              // data byte ???(????)
               Wait27us() ;
          }
     }
     I2C_Stop() ;                            // ????????????????
}
/*******************************************************************************
*  LCD_CreateChar(p,*dt)                                                       *
*    ?????????????????                                        *
*                                                                              *
*    p   : ?????????(?????????)(LCD?????0-15?16???) *
*    *dt : ??????????????????????????                *
********************************************************************************/
void LCD_CreateChar(int p,char *dt)
{
     int x ;

     x = I2C_Start(ST7032_ADRES,RW_0);     // ????????????????
     if (x == 0) {
          //  LCD?????????????????
          I2C_Send(0b10000000) ;             // control byte ???(???????)
          I2C_Send(0x40 | (p << 3)) ;
          Wait27us() ;
          //  LCD????????????
          I2C_Send(0b01000000) ;             // control byte ???(??????)
          for (x=0; x < 7; x++) {
               I2C_Send(*dt++) ;
               Wait27us() ;
          }
     }
     I2C_Stop() ;                            // ????????????????
}
/*******************************************************************************
*  LCD_IconClear( )                                                            *
*    ??????????????(??????LCD??)                         *
*******************************************************************************/
void LCD_IconClear(void)
{
     int x ;

     // ???????????????
     command(0x39) ;
     x = I2C_Start(ST7032_ADRES,RW_0);       // ????????????????
     if (x == 0) {
          // LCD??????????????
          I2C_Send(0b10000000) ;             // control byte ???(???????)
          I2C_Send(0x40) ;                   // data byte ???
          Wait27us() ;
          // ????????????????
          I2C_Send(0b01000000) ;        // control byte ???(??????)
          for (x=0x40 ; x<0x50 ; x++) {
               I2C_Send(0x00) ;
               Wait27us() ;
          }
     }
     I2C_Stop() ;                            // ????????????????
     // ???????????????
     command(FuncSet_DT) ;
}
/*******************************************************************************
*  LCD_IconOnOff(flag,dt)                                                      *
*    ?????????ON/OFF?????(??????LCD??)                   *
*    ????????????????????????????????????  *
*                                                                              *
*    flag : ?????????????????(1=ON 0=OFF)                     *
*    dt   : ???????????????????                             *
*           "skI2CLCDlib.h?define?????????                           *
*******************************************************************************/
void LCD_IconOnOff(int flag, unsigned int dt)
{
     int ans ;

     // ???????????????
     command(0x39) ;
     ans = I2C_Start(ST7032_ADRES,RW_0);     // ????????????????
     if (ans == 0) {
          // LCD??????????????
          I2C_Send(0b10000000) ;             // control byte ???(???????)
          I2C_Send(dt >> 8) ;                // data byte ???
          Wait27us() ;
          // ????????????????
          I2C_Send(0b11000000) ;             // control byte ???(??????)
          if (flag == 1) I2C_Send(dt & 0x00FF) ;
          else           I2C_Send(0x00) ;
     }
     I2C_Stop() ;                            // ????????????????
     Wait27us() ;
     // ???????????????
     command(FuncSet_DT) ;
}
/*******************************************************************************
*  ans = LCD_PageSet(no)                                                       *
*    ???????????????                                            *
*                                                                              *
*    no  : ???????????????(8x2??0-4?16x2??0-1)             *
*    ans : ????  ????????????????????                  *
*******************************************************************************/
int LCD_PageSet(int no)
{
     int ans, c , c2 ;

     ans = -1 ;
     if (LCD_NowPage != no) {      // ??????????????????????
          // ????????DDRAM???????
          c = no * LCD_ColumnSu ;
          // ????? - ????? ????
          c2 = c - (LCD_NowPage * LCD_ColumnSu) ;
          if (c < (40/LCD_ColumnSu)*LCD_ColumnSu) {    // ????????????????
               if (c2 > 0) {
                    // ??????????????????
                    for (c=0 ; c < c2 ; c++) command(0x18) ;
               } else {
                    // ??????????????????
                    c2 = c2 * -1 ;
                    for (c=0 ; c < c2 ; c++) command(0x1C) ;
               }
               LCD_NowPage = no ;  // ???????????????
               ans = 0 ;
          }
     }
     return ans ;
}
/*******************************************************************************
*  LCD_PageClear( )                                                            *
*    ????????????????????                                  *
*******************************************************************************/
void LCD_PageClear(void)
{
     int c , i ;

     // ????????DDRAM???????
     c = LCD_NowPage * LCD_ColumnSu ;
     // 1?????
     LCD_SetCursor(c, 0) ;
     for (i=0 ; i < LCD_ColumnSu ; i++) {
          LCD_Putc(0x20) ;
     }
     // 2?????
     LCD_SetCursor(c, 1) ;
     for (i=0 ; i < LCD_ColumnSu ; i++) {
          LCD_Putc(0x20) ;
     }
     LCD_SetCursor(c, 0) ;
}
/*******************************************************************************
*  LCD_PageSetCursor(col,row)                                                  *
*    ?????????????????????????                        *
*    ?????????????????????????????????????*
*                                                                              *
*    col : ?(?)?????????(8x2??0-7?16x2??0-15)                  *
*    row : ?(?)?????????(0-1)                                       *
*******************************************************************************/
void LCD_PageSetCursor(int col, int row)
{
     LCD_SetCursor((LCD_NowPage * LCD_ColumnSu) + col, row) ;
}
/*******************************************************************************
*  ans = LCD_PageNowNo()                                                       *
*    ??????????????????                                      *
*                                                                              *
*    ans : ????????????(8x2??0-4?16x2??0-1)                   *
*******************************************************************************/
int LCD_PageNowNo(void)
{
     return LCD_NowPage ;
}
/*******************************************************************************
*  LCD_Contrast(contrast)                                                      *
*    ?????????????????                                        *
*                                                                              *
*    contrast : LCD?????????????(????)                        *
*******************************************************************************/
void LCD_Contrast(int contrast)
{
     // ???????????????
     command(0x39) ;
     // ?????????????
     command(0x70 | (contrast & 0x0F)) ;
     // ?????????????
     ContSet_DT = (ContSet_DT & 0xFC) | ((contrast & 0x30) >> 4) ;
     command(ContSet_DT) ;
     // ???????????????
     command(FuncSet_DT) ;
}
/*******************************************************************************
*  LCD_Init(icon,contrast,bon,colsu)                                           *
*    ????????????                                                  *
*                                                                              *
*    icon     : ?????????????????(1=?? 0=????)          *
*    contrast : LCD?????????????(????)                        *
*    bon      : ?????????  1:ON(VDD=3.3V) 0:OFF(VDD=5.0V)             *
*    colsu    : LCD??????????????(?/??)                       *
*******************************************************************************/
void LCD_Init(int icon, int contrast, int bon, int colsu)
{
     unsigned int d ;

     LCD_ColumnSu = colsu ;                        // LCD????????
     Wait10ms(4) ;                                 // ?????40ms??????????
     FuncSet_DT = 0x38 ;
     command(FuncSet_DT);// function set           : ?????8?????2???????5x8???
     command(0x39) ;     // function set           : ???????????????
     command(0x14) ;     // Internal OSC frequency : ??????????OSC??????
     d = 0x70 | (contrast & 0x0F) ;                // (??4???)?????
     command(d) ;        // Contrast set           : ???????????
     d = 0x50 | ((contrast & 0x30) >> 4) ;         // (??2???)?????
     if (icon == 1) d = d | 0x08 ;                 // ???????????
     if (bon  == 1) d = d | 0x04 ;                 // ???????????
     ContSet_DT = d ;
     command(d) ;        // Contrast set           : ????????????????
     command(0x6C) ;     // Follower control       : ???????ON??????????
     Wait10ms(20) ;                                // ???????????(200ms)
     command(FuncSet_DT);// function set           : ???????????????
     command(0x0C) ;     // display control        : ?????ON????????OFF????????OFF
     command(0x06) ;     // entry mode set         : ?????????????????????
     LCD_Clear() ;       // Clear Display          : ???????????????
}

