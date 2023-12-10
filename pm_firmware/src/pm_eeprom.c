/****************************************************************************************
pm_eeprom.c:   power module (PM) EEPROM EMULATION control functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	September 2022
	Note(s):
	
- Based on altimeter_eeprom.c
- According to "Quick Start Guide for the Emulated EEPROM Module - Basic Use Case"
http://asf.atmel.com/docs/latest/samr21/html/asfdoc_sam0_eeprom_basic_use_case.html

"The device's fuses must be configured to reserve a sufficient number of
FLASH memory rows for use by the EEPROM emulator service, before the service
can be used. That is: NVMCTRL_FUSES_EEPROM_SIZE has to be set to 
0x6 in the fuse setting, then there will be 1 pages size for
EEPROM."

"The device's fuses was configured at 0x4 due to error"

*****************************************************************************************/

#include <eeprom.h>
#include <string.h>
#include "pm_eeprom.h"
#include "pm_usart.h"

#define EEPROM_DATA_LENGTH 6




/***************************************************************************
// Local variable(s)
****************************************************************************/

// Buffer to hold EEPROM data
static uint8_t eeprom_data[EEPROM_DATA_LENGTH];

// Parameter locations within the EEPROM data buffer
static const uint8_t x_coord_offset = 0;
static const uint8_t y_coord_offset = 2;
static const uint8_t z_coord_offset = 4;



/***************************************************************************
Function to configure the EEPROM emulator
****************************************************************************/
void eeprom_configure(uint16_t *x_coord, uint16_t *y_coord, uint16_t *z_coord)
{
	// Initialize the EEPROM emulator service
	enum status_code retval = eeprom_emulator_init();
	if (retval == STATUS_OK)
	{
		
		pm_usart_send_pc_message("eeprom_configure: STATUS_OK\r\n");
		pm_usart_send_pc_message("eeprom_configure: Have to read settings\r\n");

		// EEPROM emulation service was successfully initialized
		eeprom_read_settings(x_coord, y_coord, z_coord);
	}
	else if (retval == STATUS_ERR_NO_MEMORY)
	{
		pm_usart_send_pc_message("eeprom_configure: STATUS_ERR_NO_MEMORY\r\n");
	}
	else
	{
		// STATUS_ERR_BAD_FORMAT  Emulated EEPROM memory is corrupt or not formatted
		// or
		// STATUS_ERR_IO          EEPROM data is incompatible with this version or scheme of the EEPROM emulator

		pm_usart_send_pc_message("eeprom_configure: STATUS_ERR_BAD_FORMAT\r\n");
		pm_usart_send_pc_message("eeprom_configure: or\r\n");
		pm_usart_send_pc_message("eeprom_configure: STATUS_ERR_IO\r\n");

		pm_usart_send_pc_message("eeprom_configure: Have to erase and initialize\r\n");

		// Erase the emulated EEPROM memory (assume it is unformatted or irrecoverably corrupt)
		eeprom_emulator_erase_memory();
		retval = eeprom_emulator_init();
		if (retval == STATUS_OK)
		{
			pm_usart_send_pc_message("eeprom_configure: STATUS_OK 2\r\n");
			pm_usart_send_pc_message("eeprom_configure: Have to write settings\r\n");

			eeprom_write_settings(*x_coord, *y_coord, *z_coord);
		}
	}
}	// End of eeprom_configure


/***************************************************************************
Local function to read the settings stored in the EEPROM emulator
****************************************************************************/
void eeprom_read_settings(uint16_t *x_coord, uint16_t *y_coord, uint16_t *z_coord)
{
	
	enum status_code status = eeprom_emulator_read_buffer(0, eeprom_data, EEPROM_DATA_LENGTH);
	
	if(status == STATUS_OK )
	{
		*x_coord = eeprom_data[x_coord_offset];
		*x_coord|= (uint16_t)(eeprom_data[x_coord_offset + 1]) << 8;
		
		*y_coord = eeprom_data[y_coord_offset];
		*y_coord|= (uint16_t)(eeprom_data[y_coord_offset + 1]) << 8;
		
		*z_coord = eeprom_data[z_coord_offset];
		*z_coord|= (uint16_t)(eeprom_data[z_coord_offset + 1]) << 8;	
	}
	else
	{
		pm_usart_send_pc_message("eeprom_configure: STATUS_ERR_READ_EEPROM\r\n");
	}

	

}	// End of eeprom_read_settings



/***************************************************************************
Function to save the settings to the EEPROM emulator
****************************************************************************/
void eeprom_write_settings(uint16_t x_coord, uint16_t y_coord, uint16_t z_coord)
{
	uint16_t i;

	for (i=0; i<EEPROM_DATA_LENGTH; i++)
	{
		eeprom_data[i] = 0;
	}

	eeprom_data[x_coord_offset] = (uint8_t) x_coord;
	eeprom_data[x_coord_offset + 1] = (uint8_t)(x_coord >> 8);
	
	eeprom_data[y_coord_offset] = y_coord;
	eeprom_data[y_coord_offset + 1] = (uint8_t)(y_coord >> 8);
	
	eeprom_data[z_coord_offset] = z_coord;
	eeprom_data[z_coord_offset + 1] = (uint8_t)(z_coord >> 8);

	enum status_code status = eeprom_emulator_write_buffer(0, eeprom_data, EEPROM_DATA_LENGTH);
	
	if(status != STATUS_OK)
	{
		pm_usart_send_pc_message("eeprom_configure: STATUS_ERR_WRITE_EEPROM_BUFFER\r\n");	
	}
	
	status = eeprom_emulator_commit_page_buffer();
	
	if(status != STATUS_OK)
	{
		pm_usart_send_pc_message("eeprom_configure: STATUS_ERR_WRITE_EEPROM_COMMIT\r\n");
	}

}	// End of eeprom_write_settings

