/****************************************************************************************
pm_spi.c:   power module (PM) SPI functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021

Note(s):
- Main PM board is configured to be an SPI slave

----------------------------------------------
SAML21J18B
Pin		I/O		PM board pin	Function
----------------------------------------------
25		PB12	SPI1_MOSI		SERCOM4/PAD[0]
26		PB13	SPI1_SS0		SERCOM4/PAD[1]
27		PB14	SPI1_MISO		SERCOM4/PAD[2]
28		PB15	SPI1_SCLK		SERCOM4/PAD[3]

*****************************************************************************************/


#include <spi.h>
#include <spi_interrupt.h>
#include "pm_spi.h"
#include "pm_usart.h"
#include "pm_config_codes.h"


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
Function to configure the Main PM SPI
*****************************************************************************************/
void pm_spi_configure(uint8_t mode)
{
	static bool bFirst = true;

	//spi_slave_configure();
	//spi_slave_callback_configure();

	struct spi_config spi_config_struct;

	spi_get_config_defaults(&spi_config_struct);

	spi_config_struct.mode = SPI_MODE_SLAVE;
	spi_config_struct.mode_specific.slave.preload_enable = true;
	spi_config_struct.mode_specific.slave.frame_format = SPI_FRAME_FORMAT_SPI_FRAME;
	spi_config_struct.mux_setting = SPI_SIGNAL_MUX_SETTING_E;

	spi_config_struct.pinmux_pad0 = PINMUX_PB12C_SERCOM4_PAD0;
	spi_config_struct.pinmux_pad1 = PINMUX_PB13C_SERCOM4_PAD1;
	spi_config_struct.pinmux_pad2 = PINMUX_PB14C_SERCOM4_PAD2;
	spi_config_struct.pinmux_pad3 = PINMUX_PB15C_SERCOM4_PAD3;

	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		spi_disable(&spi_module_struct);
	}

	spi_init(&spi_module_struct, SERCOM4, &spi_config_struct);

	spi_register_callback(&spi_module_struct, spi_slave_callback, SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_module_struct, SPI_CALLBACK_BUFFER_TRANSCEIVED);

	spi_enable(&spi_module_struct);
	
	if (mode == MODE_DISABLED){
		pm_usart_send_pc_message("spi disabled!\r\n");
		spi_disable(&spi_module_struct);
	}

}	// End of pm_spi_configure


/****************************************************************************************
Function to start an SPI read
*****************************************************************************************/
enum status_code pm_spi_start_read(uint8_t *tx_buffer, uint8_t *rx_buffer, int buffer_length)
{
	enum status_code status;

	transfer_complete = false;

//	spi_read_buffer_job(&spi_module_struct, rx_buffer, buffer_length, 0x00);	
	status = spi_transceive_buffer_job(&spi_module_struct, tx_buffer, rx_buffer, buffer_length);

	return (status);

}	// End of pm_spi_start_read


/****************************************************************************************
Function to return the transfer_complete variable
*****************************************************************************************/
bool pm_spi_transfer_complete(void)
{
	return (transfer_complete);
	
}	// End of pm_spi_transfer_complete


/****************************************************************************************
SPI slave callback function
*****************************************************************************************/
static void spi_slave_callback(struct spi_module *const module)
{
	transfer_complete = true;

}	// End of spi_slave_callback


