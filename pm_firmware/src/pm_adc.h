/****************************************************************************************
pm_adc.h: Include file for pm_adc.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	February 2022
*****************************************************************************************/


#ifndef PM_ADC_H
#define PM_ADC_H


void pm_adc_configure(void);
enum status_code pm_adc_read(float *);


#endif	// PM_ADC_H


