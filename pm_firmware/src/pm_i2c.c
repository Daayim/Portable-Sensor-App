/****************************************************************************************
pm_i2c.c:   power module (PM) I2C functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021

Note(s):
- Main PM board is configured to be an I2C master
- The slave devices are:
-------------------------------------------------------------------
Device (Manufacturer)					Part Number		I2C Address
-------------------------------------------------------------------
Accelerometer							MC3416			1001100x
	(mCube Inc.)
Battery gas gauge						LTC2944			1100100x
	(Linear Technology Corp.)
Internal pressure / temperature sensor	MS5637-02BA03	1110110x
	(Measurement Specialties Inc.)
-------------------------------------------------------------------
----------------------------------------------
SAML21J18B
Pin		I/O		PM board pin	Function
----------------------------------------------
35		PA16	I2C_SDA			SERCOM1/PAD[0]
36		PA17	I2C_SCL			SERCOM1/PAD[1]
*****************************************************************************************/


#include <i2c_master.h>

#include "pm_i2c.h"
#include "pm_usart.h"
#include "pm_gpio.h"


/****************************************************************************************
Local variable(s)
*****************************************************************************************/

static struct i2c_master_module i2c_master_module_struct;
static struct i2c_master_packet packet;
static uint8_t read_buffer[4];

//static uint8_t read_reg_buffer[6];

//static const int timeout = 1000;


/****************************************************************************************
Function to configure the Main PM I2C
*****************************************************************************************/
void pm_i2c_configure(void)
{
	struct i2c_master_config i2c_master_config_struct;
	
	i2c_master_get_config_defaults(&i2c_master_config_struct);
	
	//i2c_master_config_struct.buffer_timeout = 10000;
	i2c_master_config_struct.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;
	i2c_master_config_struct.pinmux_pad1 = PINMUX_PA17C_SERCOM1_PAD1;

	i2c_master_init(&i2c_master_module_struct, SERCOM1, &i2c_master_config_struct);

	i2c_master_enable(&i2c_master_module_struct);

	packet.ten_bit_address = false;
	packet.high_speed = false;
	packet.hs_master_code = 0x0;

}	// End of pm_i2c_configure


/****************************************************************************************
Function to read a response packet from an I2C device
*****************************************************************************************/
enum status_code pm_i2c_read_response_packet(uint16_t address, uint32_t *data, uint16_t num_bytes)
{
	char response[128];
	enum status_code status;
	//uint16_t count;
	uint16_t i;
	uint16_t num_bits;

	packet.address = address;
	packet.data = read_buffer;
	packet.data_length = num_bytes;

	//count = 0;
	while (1)
	{
		status = i2c_master_read_packet_wait(&i2c_master_module_struct, &packet);
		if (status == STATUS_OK)
		{
			break;
		}
		else
		{
			sprintf(response, "pm_i2c_read_response_packet: status = 0x%x!\r\n", status);
			pm_usart_send_pc_message(response);

			//i2c_master_reset(&i2c_master_module_struct);

			return (status);

			//count++;
			//if (count == timeout)
			//{
				//status = STATUS_ERR_TIMEOUT;
				//return (status);
			//}
		}
	}

	*data = 0;
	num_bits = 8 * (num_bytes - 1);
	for (i = 0; i < num_bytes; i++)
	{
		*data |= read_buffer[i] << num_bits;
		num_bits -= 8;
	}

	return (status);

}	// End of pm_i2c_read_response_packet


/****************************************************************************************
Function to write a command packet to an I2C device
*****************************************************************************************/
enum status_code pm_i2c_write_command_packet(uint16_t address, uint8_t *command_bytes, uint16_t num_bytes, uint8_t repeated_start)
{
	char response[128];
	enum status_code status;
	//uint16_t count;

	packet.address = address;
	packet.data = command_bytes;
	packet.data_length = num_bytes;

	//count = 0;
	while (1)
	{
		if (repeated_start == 0)
		{
			status = i2c_master_write_packet_wait(&i2c_master_module_struct, &packet);
		}
		else
		{
			status = i2c_master_write_packet_wait_no_stop(&i2c_master_module_struct, &packet);
		}
		if (status == STATUS_OK)
		{
			break;
		}
		else
		{
			sprintf(response, "pm_i2c_write_command_packet: status = 0x%x!\r\n", status);
			pm_usart_send_pc_message(response);

			//i2c_master_reset(&i2c_master_module_struct);

			break;

			//count++;
			//if (count == timeout)
			//{
				//break;
			//}
		}
	}

	return (status);

}	// End of pm_i2c_write_command_packet


/****************************************************************************************
Function to write a command to and read a response from an I2C device
*****************************************************************************************/
enum status_code pm_i2c_write_command_read_response(uint16_t address,
													uint8_t *command_bytes, uint16_t num_command_bytes,
													uint32_t *data, uint16_t num_response_bytes,
													uint8_t repeated_start)
{
	enum status_code status;
	//uint16_t i;
	//uint16_t num_bits;
	char response[128];

	status = pm_i2c_write_command_packet(address, command_bytes, num_command_bytes, repeated_start);
	if (status != STATUS_OK)
	{
		sprintf(response, "pm_i2c_write_command_read_response: (1) status = 0x%x!\r\n", status);
		pm_usart_send_pc_message(response);

		return (status);
	}

	status = pm_i2c_read_response_packet(address, data, num_response_bytes);
	if (status != STATUS_OK)
	{
		sprintf(response, "pm_i2c_write_command_read_response: (2) status = 0x%x!\r\n", status);
		pm_usart_send_pc_message(response);

		return (status);
	}

	//*data = 0;
	//num_bits = 8 * (num_bytes - 1);
	//for (i = 0; i < num_bytes; i++)
	//{
		//*data |= read_buffer[i] << num_bits;
		//num_bits -= 8;
	//}

	return (status);

}	// End of pm_i2c_write_command_read_response



/****************************************************************************************
Function to write a command to MC3416 Register from an I2C device
*****************************************************************************************/


enum status_code pm_i2c_command_read_reg(uint16_t address, uint16_t num_command_bytes, uint8_t reg_address, uint8_t *data, uint16_t num_response_bytes)
{
	uint8_t* command_reg = &reg_address;
	
	enum status_code status;
	char response[128];
	
	uint8_t no_stop =1;
	
	status = pm_i2c_write_command_packet(address, command_reg, num_command_bytes, no_stop);
	
	if (status != STATUS_OK)
	{
		sprintf(response, "pm_i2c_command_read_reg_response: (1) status = 0x%x!\r\n", status);
		pm_usart_send_pc_message(response);

		return (status);
	}

	packet.address = address;
	packet.data = data;
	packet.data_length = num_response_bytes;
	
	while (1)
	{
		status = i2c_master_read_packet_wait(&i2c_master_module_struct, &packet);
		if (status == STATUS_OK)
		{
			break;
		}
		else
		{
			sprintf(response, "pm_i2c_command_read_response: (2) status = 0x%x!\r\n", status);
			pm_usart_send_pc_message(response);

			return (status);
		}
	}
	return (status);

}	// End of pm_i2c_command_read_reg


/****************************************************************************************
Function to read a command to MC3416 Register from an I2C device
*****************************************************************************************/


enum status_code pm_i2c_command_write_reg(uint16_t slave_address, uint8_t reg_address,
uint8_t *command_bytes, uint16_t num_bytes)
{
	//Create write command. 8 MSB set for register address and 8 LSB for command data
	uint8_t full_address[2];
	full_address[0] = reg_address;
	full_address[1] = *command_bytes;;
	
	char response[128];
	enum status_code status;

	packet.address = slave_address;
	packet.data = full_address;
	packet.data_length = num_bytes;
	
	
	while (1)
	{
		status = i2c_master_write_packet_wait(&i2c_master_module_struct, &packet);
		if(status != STATUS_OK)
		{
			sprintf(response, "pm_i2c_write_command_reg: status = 0x%x!\r\n", status);
			pm_usart_send_pc_message(response);

			//i2c_master_reset(&i2c_master_module_struct);

			break;

		}
		if (status == STATUS_OK)
		{
			break;
		}
		
	}
	
	return (status);

}	// End of pm_i2c_write_command_reg


