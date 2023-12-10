/****************************************************************************************
wcm_spi.h: Include file for wcm_spi.c

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022
*****************************************************************************************/


#ifndef WCM_SPI_H
#define WCM_SPI_H


void wcm_spi_configure(uint8_t);
enum status_code wcm_spi_start_read(uint8_t *, uint8_t *, int);
bool wcm_spi_transfer_complete(void);


#endif	// WCM_SPI_H


