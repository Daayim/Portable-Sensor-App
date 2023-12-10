/***************************************************************************
pm_power.h:  Include file for pm_power.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	October 2022
****************************************************************************/


#ifndef ALTIMETER_POWER_H
#define ALTIMETER_POWER_H

void pm_power_configure_wakeup_en(void);

void pm_power_normal_power_mode(void);
void pm_power_low_power_mode(void);


#endif	// ALTIMETER_POWER_H