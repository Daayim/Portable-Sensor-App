/****************************************************************************************
wcm_ms5637.h: Include file for wcm_ms5637.c

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	December 2022
*****************************************************************************************/


#ifndef WCM_MS5637_H
#define WCM_MS5637_H


void wcm_ms5637_get_calibration_coefficients(uint16_t *);
enum status_code wcm_ms5637_init(void);
enum status_code wcm_ms5637_read(uint32_t *, double *, uint32_t *, double *);


#endif	// WCM_MS5637_H


