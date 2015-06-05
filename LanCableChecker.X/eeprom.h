#ifndef	_EEPROM_H_
	#define _EEPROM_H_

	union intsep
	{
		int twobyte;
		char byte[2];
	}intsep;

	extern int eeprom_write_char(char address, char data);
	extern char eeprom_read_char(char address);
	extern char eeprom_verify(char address, char data);
	extern void eeprom_init(char con1, char con2);
	extern int eeprom_write_int(char address, int data);
	extern int eeprom_read_int(char address);

#endif