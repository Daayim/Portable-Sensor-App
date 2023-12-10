/****************************************************************************************
pm_ms5637.c:   power module (PM) MS5637 pressure /
	temperature sensor functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	December 2021

Note(s):
- The Measurement Specialties Inc. (TE Connectivity) pressure / temperature sensor part
	number is MS5637-02BA03
- It's slave address is 1110110
*****************************************************************************************/


#include <delay.h>
#include <math.h>
#include <status_codes.h>
#include "pm_i2c.h"
#include "pm_ms5637.h"
#include "pm_usart.h"

#include "pm_gpio.h"


/****************************************************************************************
Local variable(s)
*****************************************************************************************/

static uint16_t c[7];
static uint32_t d1;
static uint32_t d2;
static const uint16_t ms5637_address = 0x76;


/****************************************************************************************
Local function(s)
*****************************************************************************************/

static enum status_code ms5637_adc_read(uint32_t *);
static enum status_code ms5637_convert_d1(void);
static enum status_code ms5637_convert_d2(void);
static enum status_code ms5637_prom_read(void);
static enum status_code ms5637_read_d1_d2(void);
static enum status_code ms5637_reset(void);


/****************************************************************************************
Local function to read the MS5637 ADC
*****************************************************************************************/
static enum status_code ms5637_adc_read(uint32_t *adc_value)
{
	enum status_code status;
	uint8_t command;
	uint8_t repeated_start;
	uint32_t data;

	command = 0x00;
	repeated_start = 0;
	status = pm_i2c_write_command_read_response(ms5637_address, &command, 1, &data, 3, repeated_start);
	if (status != STATUS_OK)
	{
		return (status);
	}

	*adc_value = data;

	delay_ms(20);

	return (status);

}	// End of ms5637_adc_read


/****************************************************************************************
Local function to convert the MS5637 uncompensated pressure (D1)
*****************************************************************************************/
static enum status_code ms5637_convert_d1(void)
{
	enum status_code status;
	uint8_t command;
	uint8_t repeated_start;

	// Initiate pressure conversion (D1) with OSR = 4096
	command = 0x48;
	repeated_start = 0;
	status = pm_i2c_write_command_packet(ms5637_address, &command, 1, repeated_start);
	
	// ADC conversion time with OSR = 4096 is 8.22 ms
	delay_ms(20);

	return (status);

}	// End of ms5637_convert_d1


/****************************************************************************************
Local function to convert the MS5637 uncompensated temperature (D2)
*****************************************************************************************/
static enum status_code ms5637_convert_d2(void)
{
	enum status_code status;
	uint8_t command;
	uint8_t repeated_start;

	// Initiate temperature conversion (D2) with OSR = 4096
	command = 0x58;
	repeated_start = 0;
	status = pm_i2c_write_command_packet(ms5637_address, &command, 1, repeated_start);

	// ADC conversion time with OSR = 4096 is 8.22 ms
	delay_ms(20);

	return (status);

}	// End of ms5637_convert_d2


/****************************************************************************************
Local function to read the MS5637 pressure sensor PROM
*****************************************************************************************/
static enum status_code ms5637_prom_read(void)
{
	enum status_code status;
	int i;
	uint8_t command;
	uint8_t repeated_start;
	uint32_t data;

	// CRC
	command = 0xa0;
	repeated_start = 0;
	status = pm_i2c_write_command_read_response(ms5637_address, &command, 1, &data, 2, repeated_start);
	if (status != STATUS_OK)
	{
		pm_usart_send_pc_message("pm_ms5637_prom_read: Could not read CRC!\r\n");

		return (status);
	}
	c[0] = (data >> 12) & 0x0f;

	delay_ms(20);

	// Calibration coefficients
	for (i = 1; i <= 6; i++)
	{
		command = 0xa0 + (i << 1);
		status = pm_i2c_write_command_read_response(ms5637_address, &command, 1, &data, 2, repeated_start);
		if (status != STATUS_OK)
		{
			pm_usart_send_pc_message("pm_ms5637_prom_read: Could not read calibration coefficient!\r\n");

			return (status);
		}
		//else
		//{
			//pm_usart_send_pc_message("pm_ms5637_prom_read: Calibration coefficient read\r\n");
		//}
		c[i] = data;

		delay_ms(20);
	}

	return (status);

}	// End of ms5637_prom_read


/****************************************************************************************
Local function to read the uncompensated pressure and temperature from the MS5637
pressure sensor
*****************************************************************************************/
static enum status_code ms5637_read_d1_d2(void)
{
	enum status_code status;
	uint32_t adc_value;

	status = ms5637_convert_d1();
	if (status != STATUS_OK)
	{
		pm_usart_send_pc_message("ms5637_read_d1_d2: Could not convert D1!\r\n");

		return (status);
	}

	status = ms5637_adc_read(&adc_value);
	if (status != STATUS_OK)
	{
		pm_usart_send_pc_message("ms5637_read_d1_d2: Could not read ADC (1)!\r\n");

		return (status);
	}

	d1 = adc_value;

	status = ms5637_convert_d2();
	if (status != STATUS_OK)
	{
		pm_usart_send_pc_message("ms5637_read_d1_d2: Could not convert D2!\r\n");

		return (status);
	}

	status = ms5637_adc_read(&adc_value);
	if (status != STATUS_OK)
	{
		pm_usart_send_pc_message("ms5637_read_d1_d2: Could not read ADC (2)!\r\n");

		return (status);
	}

	//pm_usart_send_pc_message("ms5637_read_d1_d2: read D2\r\n");

	d2 = adc_value;

	return (status);

}	// End of ms5637_read_d1_d2


/****************************************************************************************
Local function to reset the MS5637 pressure sensor
*****************************************************************************************/
static enum status_code ms5637_reset(void)
{
	enum status_code status;
	uint8_t command;
	uint8_t repeated_start;

	command = 0x1e;
	repeated_start = 0;

	status = pm_i2c_write_command_packet(ms5637_address, &command, 1, repeated_start);

	return (status);

}	// End of ms5637_reset


/****************************************************************************************
Function to return the MS5637 pressure sensor crc and calibration coefficients
*****************************************************************************************/
void pm_ms5637_get_calibration_coefficients(uint16_t *cc)
{
	int i;

	for (i = 0; i < 7; i++)
		cc[i] = c[i];

}	// End of pm_i2c_ms5637_get_calibration_coefficients


/****************************************************************************************
Function to initialize the MS5637 pressure sensor
*****************************************************************************************/
enum status_code pm_ms5637_init(void)
{
	enum status_code status;

	// Reset the MS5637 once after power-on
	status = ms5637_reset();
	if (status != STATUS_OK)
	{
		pm_usart_send_pc_message("pm_ms5637_init: Could not reset!\r\n");

		return (status);
	}
	
	// Read the MS5637 PROM to get the CRC and calibration coefficients
	status = ms5637_prom_read();
	if (status != STATUS_OK)
	{
		return (status);
	}

	return (status);

}	// End of pm_ms5637_init


/****************************************************************************************
Function to read the MS5637 pressure sensor
*****************************************************************************************/
enum status_code pm_ms5637_read(uint32_t *d1_arg, double *pressure, uint32_t *d2_arg, double *temperature)
{
	enum status_code status;

	// Temperature
	double dt;
	double temp;

	// Temperature compensated pressure
	double off;
	double sens;
	double p;

	// Second order temperature compensation
	double t2;
	double off2;
	double sens2;

	status = ms5637_read_d1_d2();

	//*****************
	// For testing only
	//c[1] = 46372;
	//c[2] = 43981;
	//c[3] = 29059;
	//c[4] = 27842;
	//c[5] = 31553;
	//c[6] = 28165;
	//
	//d1 = 6465444ul;
	//d2 = 8077636ul;
	//
	//status = STATUS_OK;
	// End of For testing only
	//************************

	if (status != STATUS_OK)
	{
		return (status);
	}

	*d1_arg = d1;
	*d2_arg = d2;
	
	// Calculate temperature
	// dT = D2 - TREF = D2 - C5 * 2^8
	// TEMP = 20 C + dT * TEMPSENS = 2000 + dT * C6 / 2^23
	dt = (double)d2 - (double)c[5] * pow(2, 8);
	temp = 2000.0 + dt * (double)c[6] / pow(2, 23);

	// Calculate temperature compensated pressure
	// OFF = OFFT1 + TCO * dT = C2 * 2^17 + (C4 * dT ) / 2^6
	// SENS = SENST1 + TCS * dT = C1 * 2^16 + (C3 * dT ) / 2^7
	// P = D1 * SENS - OFF = (D1 * SENS / 2^21 - OFF) / 2^15
	off = (double)c[2] * pow(2, 17) + (double)c[4] * dt / pow(2, 6);
	sens = (double)c[1] * pow(2, 16) + (double)c[3] * dt / pow(2, 7);
	//p = (d1 * sens / 2097152 - off) / 32768;

	// Second order temperature compensation
	if (temp < 2000.0)
	{
		double tmp;

		// Low temperature
		// T2 = 3 * dT^2 / 2^33
		// OFF2 = 61 * (TEMP - 2000)^2 / 2^4
		// SENS2 = 29 * (TEMP - 2000)^2 / 2^4
		t2 = 3.0 * dt * dt / pow(2, 33);
		tmp = (temp - 2000.0) * (temp - 2000.0) / pow(2, 4);
		off2 = 61.0 * tmp;
		sens2 = 29.0 * tmp;

		if (temp < -1500.0)
		{
			// Very low temperature
			// OFF2 = OFF2 + 17 * (TEMP + 1500)^2
			// SENS2 = SENS2 + 9 * (TEMP + 1500)^2
			tmp = (temp + 1500.0) * (temp + 1500.0);
			off2 += 17.0 * tmp;
			sens2 += 9.0 * tmp;
		}
	}
	else
	{
		// High temperature
		// T2 = 5 * dT^2 / 2^38
		// OFF2 = 0
		// SENS2 = 0
		t2 = 5.0 * dt * dt / pow(2, 38);
		off2 = 0.0;
		sens2 = 0.0;
	}

	temp -= t2;
	off -= off2;
	sens -= sens2;

	p = ((double)d1 * sens / pow(2, 21) - off) / pow(2, 15);

	*pressure = p / 100;
	*temperature = temp / 100;

	return (status);

}	// End of pm_ms5637_read


