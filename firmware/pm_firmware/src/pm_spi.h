/****************************************************************************************
pm_spi.h: Include file for pm_spi.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021
*****************************************************************************************/


#ifndef PM_SPI_H
#define PM_SPI_H


void pm_spi_configure(uint8_t);
enum status_code pm_spi_start_read(uint8_t *, uint8_t *, int);
bool pm_spi_transfer_complete(void);


#endif	// PM_SPI_H


