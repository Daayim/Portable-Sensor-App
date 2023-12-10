/****************************************************************************************
wcm_adc.h: Include file for wcm_adc.c

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022
*****************************************************************************************/


#ifndef WCM_ADC_H
#define WCM_ADC_H

void wcm_bat_adc_configure(void);
void wcm_adc_configure(void);

enum status_code wcm_adc_read(float *);
enum status_code wcm_bat_adc_read(float *);


#endif	// WCM_ADC_H


