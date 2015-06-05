/*
	For Debug unit
	
	usart debug
	lcd debug
	etc...
*/

//indicate argument variable
//and delay a few second

extern void lcd_indicate(char test);

//scan RX data of RS-232C
//and write to the lcd one by one

extern void scan_ex(void);

//transform hex code to variable data

extern char hex2char(char hex);


extern void data_div(char freq, char *dived);