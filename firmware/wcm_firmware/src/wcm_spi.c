/****************************************************************************************
wcm_spi.c: Marine Mammal Detection (MMD) Wireless Communication Module (WCM) SPI functions

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022

Note(s):
- MMD WCM board is configured to be an SPI slave

------------------------------------------------
SAML21E17B
Pin		I/O		WCM board pin	Function			
------------------------------------------------
21		PA22	SPI_MISO		SERCOM5/PAD[0]		
22		PA23	SPI_SCK			SERCOM5/PAD[1]		
23		PA24	SPI_CS			SERCOM5/PAD[2]		
24		PA25	SPI_MOSI		SERCOM5/PAD[3]		

*****************************************************************************************/


#include <spi.h>
#include <spi_interrupt.h>
#include "wcm_spi.h"
#include "wcm_usart.h"
#include "wcm_config_codes.h"

/****************************************************************************************
Local variable(s)
*****************************************************************************************/

volatile bool transfer_complete = false;
static struct spi_module spi_module_struct;


/****************************************************************************************
Local function(s)
*****************************************************************************************/

static void spi_slave_callback(struct spi_module *const);


/****************************************************************************************
Function to configure the MMD WCM SPI
*****************************************************************************************/
void wcm_spi_configure(uint8_t mode)
{
	static bool bFirst = true;

	//spi_slave_configure();
	//spi_slave_callback_configure();

	struct spi_config spi_config_struct;

	spi_get_config_defaults(&spi_config_struct);

	spi_config_struct.mode = SPI_MODE_SLAVE;
	spi_config_struct.mode_specific.slave.preload_enable = true;
	spi_config_struct.mode_specific.slave.frame_format = SPI_FRAME_FORMAT_SPI_FRAME;
	spi_config_struct.mux_setting = SPI_SIGNAL_MUX_SETTING_D;

	spi_config_struct.pinmux_pad0 = PINMUX_PA22D_SERCOM5_PAD0;
	spi_config_struct.pinmux_pad1 = PINMUX_PA23D_SERCOM5_PAD1;
	spi_config_struct.pinmux_pad2 = PINMUX_PA24D_SERCOM5_PAD2;
	spi_config_struct.pinmux_pad3 = PINMUX_PA25D_SERCOM5_PAD3;

	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		spi_disable(&spi_module_struct);
	}

	spi_init(&spi_module_struct, SERCOM5, &spi_config_struct);

	spi_register_callback(&spi_module_struct, spi_slave_callback, SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_module_struct, SPI_CALLBACK_BUFFER_TRANSCEIVED);

	spi_enable(&spi_module_struct);
	
	if (mode == MODE_DISABLED){
		wcm_usart_send_pc_message("spi disabled!\r\n");
		spi_disable(&spi_module_struct);
	}

}	// End of wcm_spi_configure


/****************************************************************************************
Function to start an SPI read
*****************************************************************************************/
enum status_code wcm_spi_start_read(uint8_t *tx_buffer, uint8_t *rx_buffer, int buffer_length)
{
	enum status_code status;

	transfer_complete = false;

//	spi_read_buffer_job(&spi_module_struct, rx_buffer, buffer_length, 0x00);	
	status = spi_transceive_buffer_job(&spi_module_struct, tx_buffer, rx_buffer, buffer_length);

	return (status);

}	// End of wcm_spi_start_read


/****************************************************************************************
Function to return the transfer_complete variable
*****************************************************************************************/
bool wcm_spi_transfer_complete(void)
{
	return (transfer_complete);
	
}	// End of wcm_spi_transfer_complete


/****************************************************************************************
SPI slave callback function
*****************************************************************************************/
static void spi_slave_callback(struct spi_module *const module)
{
	transfer_complete = true;

}	// End of spi_slave_callback


