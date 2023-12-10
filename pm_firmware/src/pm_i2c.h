/****************************************************************************************
pm_i2c.h: Include file for pm_i2c.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021
*****************************************************************************************/


#ifndef PM_I2C_H
#define PM_I2C_H


void pm_i2c_configure(void);
enum status_code pm_i2c_read_response_packet(uint16_t, uint32_t *, uint16_t);
enum status_code pm_i2c_write_command_packet(uint16_t, uint8_t *, uint16_t, uint8_t);
enum status_code pm_i2c_write_command_read_response(uint16_t, uint8_t *, uint16_t, uint32_t *, uint16_t, uint8_t);

enum status_code pm_i2c_command_read_reg(uint16_t, uint16_t, uint8_t , uint8_t *, uint16_t);
enum status_code pm_i2c_command_write_reg(uint16_t , uint8_t , uint8_t *, uint16_t );

#endif	// PM_I2C_H


