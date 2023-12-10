/****************************************************************************************
pm_eeprom.h: Include file for pm_eeprom.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	September 2022
*****************************************************************************************/



#ifndef PM_EEPROM_H_
#define PM_EEPROM_H_

void eeprom_configure(uint16_t *, uint16_t *, uint16_t *);

void eeprom_write_settings(uint16_t, uint16_t, uint16_t);

void eeprom_read_settings(uint16_t *, uint16_t *, uint16_t *);

#endif /* PM_EEPROM_H_ */