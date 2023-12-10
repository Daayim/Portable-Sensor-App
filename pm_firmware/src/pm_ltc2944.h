/****************************************************************************************
pm_ltc2944.h: Include file for pm_ltc2944.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	December 2021
*****************************************************************************************/


#ifndef PM_LTC2944_H
#define PM_LTC2944_H


enum status_code pm_ltc2944_init(void);
enum status_code pm_ltc2944_read(double *, double *, double *, double *, uint8_t *);
//enum status_code pm_ltc2944_read_status(uint8_t *);


#endif	// PM_LTC2944_H


