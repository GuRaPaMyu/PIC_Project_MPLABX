#include <xc.h>
#include "eeprom.h"

int eeprom_write_char(unsigned char address, unsigned char data)
{
	while(WR)
		continue;
		
	EEDAT = data;
	EEADR = address;
	EECON2 = 0x55;
	EECON2 = 0xAA;
	
	WR = 1;
	
	if(WREN == 0)
	return 1;

	while(WR)
		continue;
		
	EEIF = 0;
	if(WRERR == 0)
		return 0;
	else
		return 1;
}

char eeprom_read_char(unsigned char address)
{
	EEADR = address;
	
	while(WR)
		continue;
	
	RD = 1;

	while(RD)
		continue;

	return EEDAT;
}

char eeprom_verify(char address, char data)
{
	if(eeprom_read_char(address) == data)
		return 0;
	else
		return 1;
}

void eeprom_init(char eepgd, char wren)
{
	EEDAT = 0x00;
	EEDATH = 0x00;
	EEADR = 0x00;
	EEADRH = 0x00;
	EECON1 = 0x00;
	if(eepgd)
		EEPGD = 1;
	if(wren)
		WREN = 1;
}

int eeprom_write_int(char address, int data)
{
	intsep.twobyte = data;
	if(eeprom_write_char(address, intsep.byte[0]))
		return 1;
	if(eeprom_write_char(address + 1, intsep.byte[1]))
		return 1;
	return 0;
}

int eeprom_read_int(char address)
{
	intsep.byte[0] = eeprom_read_char(address);
	intsep.byte[1] = eeprom_read_char(address + 1);
	return intsep.twobyte;
}