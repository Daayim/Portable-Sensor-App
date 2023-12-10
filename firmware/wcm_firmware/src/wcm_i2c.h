/****************************************************************************************
wcm_i2c.h: Include file for wcm_i2c.c

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022
*****************************************************************************************/


#ifndef WCM_I2C_H
#define WCM_I2C_H


void wcm_i2c_configure(void);
enum status_code wcm_i2c_read_response_packet(uint16_t, uint32_t *, uint16_t);
enum status_code wcm_i2c_write_command_packet(uint16_t, uint8_t *, uint16_t, uint8_t);
enum status_code wcm_i2c_write_command_read_response(uint16_t, uint8_t *, uint16_t, uint32_t *, uint16_t, uint8_t);

enum status_code wcm_i2c_command_read_reg(uint16_t, uint16_t, uint8_t , uint8_t *, uint16_t);
enum status_code wcm_i2c_command_write_reg(uint16_t , uint8_t , uint8_t *, uint16_t );

#endif	// WCM_I2C_H


