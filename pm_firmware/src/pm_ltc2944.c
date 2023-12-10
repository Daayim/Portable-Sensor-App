/****************************************************************************************
pm_ltc2944.c:   power module (PM) LTC2944 Battery Gas Gauge
	functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	December 2021

Note(s):
- The Linear Technology Corp. (LTC) battery gas gauge part number is LTC2944IDD
- It's slave address is 1100100
*****************************************************************************************/


#include <delay.h>
#include <status_codes.h>
#include <stdint.h>
#include "pm_i2c.h"
#include "pm_ltc2944.h"
#include "pm_usart.h"



/****************************************************************************************
Local variable(s)
*****************************************************************************************/

// I2C address
static const uint16_t ltc2944_address = 0x64;

// Registers
static const uint8_t status_register = 0x00;
static const uint8_t control_register = 0x01;
static const uint8_t accumulated_charge_msb_register = 0x02;
static const uint8_t voltage_msb_register = 0x08;
static const uint8_t current_msb_register = 0x0e;
static const uint8_t temperature_msb_register = 0x14;

// Conversion constants
static const double m = 4096.0;
static const double qlsb_default = 0.34e-3;
static const double tfs = 510.0;
static const double vfsi = 64.0;
static const double vfsv = 70.8;

// Sense resistor
static const double rsense = 15e-3;

// Battery capacity in mAh
static double battery_capacity = 5200.0;


/****************************************************************************************
Function to initialize the LTC2944 battery gas gauge
*****************************************************************************************/
enum status_code pm_ltc2944_init(void)
{
	//char response[128];
	enum status_code status;
	uint8_t command_bytes[4];
	uint8_t repeated_start;
	uint32_t data;
	
	// Read the control register
	command_bytes[0] = control_register;
	repeated_start = 1;

	status = pm_i2c_write_command_read_response(ltc2944_address, command_bytes, 1, &data, 1, repeated_start);	
	
	//if (status == STATUS_OK)
	//{
		//sprintf(response, "pm_ltc2944_init: control register = 0x%x\r\n", (uint8_t)data);
		//pm_usart_send_pc_message(response);
	//}
	//else
	if (status != STATUS_OK)
	{
		//retry initialization
		status = pm_i2c_write_command_read_response(ltc2944_address, command_bytes, 1, &data, 1, repeated_start);
		if (status != STATUS_OK)
		{
			return (status);
		}
	}

	// Set control register B[0] to 1 to temporarily shut down the analog section before writing
	//	to the accumulated charge registers
	command_bytes[0] = control_register;
	command_bytes[1] = (uint8_t)data | 0x01;
	repeated_start = 0;
	status = pm_i2c_write_command_packet(ltc2944_address, command_bytes, 2, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}

	// Assume full charge by writing the appropriate value to the accumulated charge registers
	// My 12 V battery capacity is 5200 mAh
	// The LTC2944 accumulated charge registers maximum reading is 0xffff giving:
	double max_charge = 1000.0 * (double)0xffff * qlsb_default * m * 50e-3 / (rsense * 4096.0);
	uint16_t accumulated_charge_register_value = (uint16_t)((double)0xffff * battery_capacity / max_charge + 0.5);

	command_bytes[0] = accumulated_charge_msb_register;
	command_bytes[1] = (accumulated_charge_register_value >> 8) & 0xff;
	command_bytes[2] = accumulated_charge_register_value & 0xff;
	repeated_start = 0;
	status = pm_i2c_write_command_packet(ltc2944_address, command_bytes, 3, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}

	return (status);

}	// End of pm_ltc2944_init


/****************************************************************************************
Function to read the LTC2944 battery gas gauge
*****************************************************************************************/
enum status_code pm_ltc2944_read(double *v, double *c, double *t, double *charge, uint8_t *status_value)
{
	char response[128];
	enum status_code status;
	uint8_t command_bytes[4];
	uint8_t repeated_start;
	uint32_t data;

	// Wake the device and and do a conversion
	// 0111 1000
	// Manual mode
	// Coulomb counter prescaling factor M = 4096
	// /ALCC pin disabled
	// Shutdown = 0
	// The ADC goes back to sleep after the conversions in manual mode
	command_bytes[0] = control_register;
	command_bytes[1] = 0x78;
	repeated_start = 0;
	status = pm_i2c_write_command_packet(ltc2944_address, command_bytes, 2, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}

	// For testing only ...
	//return (status);

	// Wait for voltage (48 ms max.), current (8 ms max.) and temperature (8 ms max.) conversions
	//delay_ms(33);
	//delay_ms(5);
	//delay_ms(5);
	delay_ms(100);

	//while (1)
	//{
		//delay_ms(400);
//
		//// Read the control register
		//command_bytes[0] = control_register;
		//status = pm_i2c_write_command_read_response(ltc2944_address, command_bytes, 1, &data, 1, repeated_start);
		//if (status == STATUS_OK)
		//{
			//sprintf(response, "pm_ltc2944_read: control register (2) = 0x%x\r\n", (uint8_t)data);
			//pm_usart_send_pc_message(response);
//
			//if ((data & 0xc0) == 0)
				//break;
		//}
		//else
		//{
			//return (status);
		//}
	//}

	// Read the voltage registers and calculate (V)
	// VSENSE = VFS(V) V * ADC / 65535.0
	command_bytes[0] = voltage_msb_register;
	repeated_start = 1;
	status = pm_i2c_write_command_read_response(ltc2944_address, command_bytes, 1, &data, 2, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}
	*v = vfsv * (double)data / 65535.0;

	//sprintf(response, "pm_ltc2944_read: voltage data = 0x%x\r\n", (uint16_t)data);
	//pm_usart_send_pc_message(response);

	// Read the current registers and calculate (A)
	// IBAT = (VFS(I) mV / RSENSE) * (ADC - 32767.0) / 32767.0
	command_bytes[0] = current_msb_register;
	status = pm_i2c_write_command_read_response(ltc2944_address, command_bytes, 1, &data, 2, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}
	*c = (vfsi * 1e-3 / rsense) * ((double)data - 32767.0) / 32767.0;

	sprintf(response, "pm_ltc2944_read: current data = 0x%x\r\n", (uint16_t)data);
	pm_usart_send_pc_message(response);

	// Read the temperature registers and calculate (degrees Celsius)
	command_bytes[0] = temperature_msb_register;
	status = pm_i2c_write_command_read_response(ltc2944_address, command_bytes, 1, &data, 2, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}
	*t = (double)data * tfs / 65535.0 - 273.15;

	//sprintf(response, "pm_ltc2944_read: temperature data = 0x%x\r\n", (uint16_t)data);
	//pm_usart_send_pc_message(response);

	// Read the accumulated charge registers and calculate (mAh)
	// qLSB = 0.340 mAh * (50 mohm / RSENSE) * (M / 4096)
	command_bytes[0] = accumulated_charge_msb_register;
	status = pm_i2c_write_command_read_response(ltc2944_address, command_bytes, 1, &data, 2, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}
	*charge = 1000.0 * (double)data * qlsb_default * m * 50e-3 / (rsense * 4096.0);

	//sprintf(response, "pm_ltc2944_read: charge data = 0x%x\r\n", (uint16_t)data);
	//pm_usart_send_pc_message(response);

	// Read the status register
	command_bytes[0] = status_register;
	status = pm_i2c_write_command_read_response(ltc2944_address, command_bytes, 1, &data, 1, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}
	*status_value = data;

	//sprintf(response, "pm_ltc2944_read: status data = 0x%x\r\n", (uint8_t)data);
	//pm_usart_send_pc_message(response);

	return (status);

}	// End of pm_ltc2944_read


///****************************************************************************************
//Function to read the LTC2944 status register
//*****************************************************************************************/
//enum status_code pm_ltc2944_read_status(uint8_t *status_value)
//{
	//enum status_code status;
	//uint32_t data;
	//
	//status = pm_i2c_write_command_read_response(ltc2944_address, 0x00, &data, 1, 1);
	//if (status != STATUS_OK)
	//{
		//return (status);
	//}
	//
	//*status_value = data & 0xff;
	//
	//return (status);
//
//}	// End of pm_ltc2944_read_status


