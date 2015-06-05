

void indicate_modedata(mode_data)

int mode_switching
{	
	lcd_goto(0x30);
	
	switch(mode_switching)
	{
		case 0 : lcd_puts("LSB");
				break;
			
		case 1 : lcd_puts("USB");
				break;
				
		case 1 : lcd_puts("AM");
				break;
			
		case 2 : lcd_puts("CW");
				break;
				
		case 3 : lcd_puts("RTTY");
				break;
			
		case 4 : lcd_puts("FM");
				break;
			
		default : lcd_puts("?");		

	}
	
}

