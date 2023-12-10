/****************************************************************************************
pm_ms5637.h: Include file for pm_ms5637.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	December 2021
*****************************************************************************************/


#ifndef PM_MS5637_H
#define PM_MS5637_H


void pm_ms5637_get_calibration_coefficients(uint16_t *);
enum status_code pm_ms5637_init(void);
enum status_code pm_ms5637_read(uint32_t *, double *, uint32_t *, double *);


#endif	// PM_MS5637_H


