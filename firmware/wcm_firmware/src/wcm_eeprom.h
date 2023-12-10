/****************************************************************************************
wcm_eeprom.h: Include file for wcm_eeprom.c

Written by:
	Daayim Asim, Engineering Student.
	Computer Engineer Student
	eSonar Inc.

Date:
	September 2022
*****************************************************************************************/



#ifndef WCM_EEPROM_H_
#define WCM_EEPROM_H_

void eeprom_configure(uint16_t *, uint16_t *, uint16_t *);

void eeprom_write_settings(uint16_t, uint16_t, uint16_t);

void eeprom_read_settings(uint16_t *, uint16_t *, uint16_t *);

#endif	// WCM_EEPROM_H_ 