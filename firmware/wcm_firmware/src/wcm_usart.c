/****************************************************************************************
wcm_usart.c: Marine Mammal Detection (MMD) Wireless Communication Module (WCM) USART functions

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022

Note(s):

-----------------------------------------------------------------
SAML21E17B
Pin		I/O		WCM board pin	Function			Atmel Start
-----------------------------------------------------------------
1		PA00	GPS_TX			SERCOM1/PAD[0]		USART_0 - TX
2		PA01	GPS_RX			SERCOM1/PAD[1]		USART_0 - RX
5		PA04	COM_TX			SERCOM0/PAD[0]		USART_1 - TX
6		PA05	COM_RX			SERCOM0/PAD[1]		USART_1 - RX
12		PA09	USB_RX			SERCOM2/PAD[1]		USART_2 - RX
13		PA10	USB_TX			SERCOM2/PAD[2]		USART_2 - TX
-----------------------------------------------------------------

- For now, using USB_TX and USB_RX for PC communications
*****************************************************************************************/


#include <string.h>
#include <usart.h>
#include "wcm_usart.h"
#include "wcm_config_codes.h"


/****************************************************************************************
// Local variable(s)
*****************************************************************************************/

static struct usart_module pc_usart_module_struct;
static struct usart_module gps_usart_module_struct;
static struct usart_module com_usart_module_struct;

static SercomUsart *pc_usart_hw;
static SercomUsart *gps_usart_hw;
static SercomUsart *com_usart_hw;


/****************************************************************************************
// Local function(s)
*****************************************************************************************/

static void pc_usart_configure(uint8_t mode);
static void gps_usart_configure(uint8_t mode);
static void com_usart_configure(uint8_t mode);

/****************************************************************************************
Function to configure the usart ports for WCM 
*****************************************************************************************/
void wcm_usart_configure(void)
{
	pc_usart_configure(MODE_ENABLED);
	gps_usart_configure(MODE_ENABLED);
	com_usart_configure(MODE_ENABLED);

}	// End of wcm_usart_configure

/****************************************************************************************
Function to configure the usart ports for WCM
*****************************************************************************************/
void wcm_usart_disable(void)
{
	gps_usart_configure(MODE_DISABLED);
	com_usart_configure(MODE_DISABLED);
	pc_usart_configure(MODE_DISABLED);
}	// End of wcm_usart_disable 


/***************************************************************************
Function to check for a command from the control computer
****************************************************************************/
enum status_code wcm_usart_check_for_pc_command(void)
{
	// Check if USART has new data
	if (pc_usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC)
	{
		return STATUS_OK;
	}

	return STATUS_BUSY;

}	// End of wcm_usart_check_for_pc_command

/***************************************************************************
Function to check for a data from the gps
****************************************************************************/
enum status_code wcm_usart_check_for_gps_data(void)
{
	// Check if USART has new data
	if (gps_usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC)
	{
		return STATUS_OK;
	}

	return STATUS_BUSY;

}	// End of wcm_usart_check_for_gps_data

/***************************************************************************
Function to check for a command from the Iridium module 
****************************************************************************/
enum status_code wcm_usart_check_for_com_data(void)
{
	// Check if USART has new data
	if (com_usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC)
	{
		return STATUS_OK;
	}

	return STATUS_BUSY;

}	// End of wcm_usart_check_for_com_data

/***************************************************************************
Function to send a message to the control computer
****************************************************************************/
void wcm_usart_send_pc_message(const char *message)
{
	usart_write_buffer_wait(&pc_usart_module_struct, (uint8_t *)message, strlen(message));

}	// End of wcm_usart_send_pc_message

/***************************************************************************
Function to send a command to the GPS
****************************************************************************/
void wcm_usart_send_gps_command(const char *command)
{
	usart_write_buffer_wait(&gps_usart_module_struct, (uint8_t *)command, strlen(command));

}	// End of wcm_usart_send_gps_command

/***************************************************************************
Function to send a command to the sat and cell
****************************************************************************/
void wcm_usart_send_com_command(const char *command)
{
	usart_write_buffer_wait(&com_usart_module_struct, (uint8_t *)command, strlen(command));

}	// End of wcm_usart_com_send_com_command

/****************************************************************************************
Local function to configure the SERCOM2 USART for communication with the control computer
*****************************************************************************************/
static void pc_usart_configure(uint8_t mode)
{
	static bool bFirst = true;

	struct usart_config usart_config_struct;

	usart_get_config_defaults(&usart_config_struct);

	usart_config_struct.baudrate                               = 38400;
	usart_config_struct.mux_setting                            = USART_RX_1_TX_2_XCK_3;
	usart_config_struct.pinmux_pad0                            = PINMUX_UNUSED;
	usart_config_struct.pinmux_pad1                            = PINMUX_PA09D_SERCOM2_PAD1;
	usart_config_struct.pinmux_pad2                            = PINMUX_PA10D_SERCOM2_PAD2;
	usart_config_struct.pinmux_pad3                            = PINMUX_UNUSED;
	usart_config_struct.transfer_mode                          = USART_TRANSFER_ASYNCHRONOUSLY;

	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		usart_disable(&pc_usart_module_struct);
	}
	
	
	usart_init(&pc_usart_module_struct, SERCOM2, &usart_config_struct);
	usart_enable(&pc_usart_module_struct);
	
	if (mode == MODE_DISABLED){
		wcm_usart_send_pc_message("pc usart disabled!\r\n");
		usart_disable(&pc_usart_module_struct);
	}

	// Get a pointer to the hardware module instance
	pc_usart_hw = &((&pc_usart_module_struct)->hw->USART);

}	// End of pc_usart_configure

/****************************************************************************************
Local function to configure the SERCOM1 USART for communication with the GPS
*****************************************************************************************/
static void gps_usart_configure(uint8_t mode)
{
	static bool bFirst = true;

	struct usart_config usart_config_struct;

	usart_get_config_defaults(&usart_config_struct);

	usart_config_struct.baudrate                               = 38400;
	usart_config_struct.mux_setting                            = USART_RX_1_TX_0_XCK_1;
	usart_config_struct.pinmux_pad0                            = PINMUX_PA00D_SERCOM1_PAD0;
	usart_config_struct.pinmux_pad1                            = PINMUX_PA01D_SERCOM1_PAD1;
	usart_config_struct.pinmux_pad2                            = PINMUX_UNUSED;
	usart_config_struct.pinmux_pad3                            = PINMUX_UNUSED;
	usart_config_struct.transfer_mode                          = USART_TRANSFER_ASYNCHRONOUSLY;

	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		usart_disable(&gps_usart_module_struct);
	}
	
	usart_init(&gps_usart_module_struct, SERCOM1, &usart_config_struct);
	usart_enable(&gps_usart_module_struct);		
	
	if (mode == MODE_DISABLED){
		wcm_usart_send_pc_message("gps usart disabled!\r\n");
		usart_disable(&gps_usart_module_struct);		
	}
	
	gps_usart_hw = &((&gps_usart_module_struct)->hw->USART);

}	// End of gps_usart_configure

/****************************************************************************************
Local function to configure the SERCOM0 USART for communication with the SAT/CELL
*****************************************************************************************/
static void com_usart_configure(uint8_t mode)
{
	static bool bFirst = true;

	struct usart_config usart_config_struct;

	usart_get_config_defaults(&usart_config_struct);

	usart_config_struct.baudrate                               = 38400;
	usart_config_struct.mux_setting                            = USART_RX_1_TX_0_XCK_1;
	usart_config_struct.pinmux_pad0                            = PINMUX_PA04D_SERCOM0_PAD0;
	usart_config_struct.pinmux_pad1                            = PINMUX_PA05D_SERCOM0_PAD1;
	usart_config_struct.pinmux_pad2                            = PINMUX_UNUSED;
	usart_config_struct.pinmux_pad3                            = PINMUX_UNUSED;
	usart_config_struct.transfer_mode                          = USART_TRANSFER_ASYNCHRONOUSLY;

	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		usart_disable(&com_usart_module_struct);
	}
	
	
	usart_init(&com_usart_module_struct, SERCOM0, &usart_config_struct);
	usart_enable(&com_usart_module_struct);
	
	if (mode == MODE_DISABLED){
		wcm_usart_send_pc_message("com usart disabled!\r\n");
		usart_disable(&com_usart_module_struct);
	}

	// Get a pointer to the hardware module instance
	com_usart_hw = &((&com_usart_module_struct)->hw->USART);

}	// End of com_usart_configure

/***************************************************************************
Function to wait for a command from the control computer
****************************************************************************/
bool wcm_usart_get_pc_command(char *command, int command_length)
{
	enum status_code retval;
	int i;
	uint16_t received_data;
	bool bCommandReceived;

	i = 0;
	received_data = 0;
	bCommandReceived = false;
	while (1)
	{
		retval = usart_read_wait(&pc_usart_module_struct, &received_data);
		if (retval == STATUS_OK)
		{
			if (received_data == '\n')
			{
				if (bCommandReceived)
				{
					break;
				}
				else
				{
					continue;
				}
			}

			command[i++] = (char)received_data;
			if (received_data == '\r')
			{
				command[i-1] = '\0';
				bCommandReceived = true;
				continue;
			}
			if (i == command_length)
			{
				// Null terminate the string
				command[i-1] = '\0';

				bCommandReceived = false;
				break;
			}
		}
		else if (retval == STATUS_BUSY)
		{
			continue;
		}
		else
		{
			if (retval == STATUS_ERR_BAD_FORMAT)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_BAD_FORMAT!\r\n");
			}
			else if (retval == STATUS_ERR_OVERFLOW)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_OVERFLOW!\r\n");
			}
			else if (retval == STATUS_ERR_BAD_DATA)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_BAD_DATA!\r\n");
			}
			else if (retval == STATUS_ERR_DENIED)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_DENIED!\r\n");
			}
			else if (retval == STATUS_ERR_PROTOCOL)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_PROTOCOL!\r\n");
			}
			else if (retval == STATUS_ERR_PACKET_COLLISION)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_PACKET_COLLISION!\r\n");
			}
			else
			{
				sprintf(command, "usart_read_wait UNKNOWN!\r\n");
			}

			bCommandReceived = false;
			break;
		}
	}

	return (bCommandReceived);

}	// End of wcm_usart_get_pc_command

/***************************************************************************
Function to wait for a data from gps
****************************************************************************/
bool wcm_usart_get_gps_data(char *command, int command_length)
{
	enum status_code retval;
	int i;
	uint16_t received_data;
	bool bCommandReceived;

	i = 0;
	received_data = 0;
	bCommandReceived = false;
	while (1)
	{
		retval = usart_read_wait(&gps_usart_module_struct, &received_data);
		if (retval == STATUS_OK)
		{
			if (received_data == '\n')
			{
				if (bCommandReceived)
				{
					break;
				}
				else
				{
					continue;
				}
			}

			command[i++] = (char)received_data;
			if (received_data == '\r')
			{
				command[i-1] = '\0';
				bCommandReceived = true;
				continue;
			}
			if (i == command_length)
			{
				// Null terminate the string
				command[i-1] = '\0';

				bCommandReceived = false;
				break;
			}
		}
		else if (retval == STATUS_BUSY)
		{
			continue;
		}
		else
		{
			if (retval == STATUS_ERR_BAD_FORMAT)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_BAD_FORMAT!\r\n");
			}
			else if (retval == STATUS_ERR_OVERFLOW)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_OVERFLOW!\r\n");
			}
			else if (retval == STATUS_ERR_BAD_DATA)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_BAD_DATA!\r\n");
			}
			else if (retval == STATUS_ERR_DENIED)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_DENIED!\r\n");
			}
			else if (retval == STATUS_ERR_PROTOCOL)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_PROTOCOL!\r\n");
			}
			else if (retval == STATUS_ERR_PACKET_COLLISION)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_PACKET_COLLISION!\r\n");
			}
			else
			{
				sprintf(command, "usart_read_wait UNKNOWN!\r\n");
			}

			bCommandReceived = false;
			break;
		}
	}

	return (bCommandReceived);

}	// End of wcm_usart_get_gps_data

/***************************************************************************
Function to wait for a data from Iridium Module
****************************************************************************/
bool wcm_usart_get_com_data(char *command, int command_length)
{
	enum status_code retval;
	int i;
	uint16_t received_data;
	bool bCommandReceived;

	i = 0;
	received_data = 0;
	bCommandReceived = false;
	while (1)
	{
		retval = usart_read_wait(&com_usart_module_struct, &received_data);
		if (retval == STATUS_OK)
		{
			if (received_data == '\n')
			{
				if (bCommandReceived)
				{
					break;
				}
				else
				{
					continue;
				}
			}

			command[i++] = (char)received_data;
			if (received_data == '\r')
			{
				command[i-1] = '\0';
				bCommandReceived = true;
				continue;
			}
			if (i == command_length)
			{
				// Null terminate the string
				command[i-1] = '\0';

				bCommandReceived = false;
				break;
			}
		}
		else if (retval == STATUS_BUSY)
		{
			continue;
		}
		else
		{
			if (retval == STATUS_ERR_BAD_FORMAT)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_BAD_FORMAT!\r\n");
			}
			else if (retval == STATUS_ERR_OVERFLOW)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_OVERFLOW!\r\n");
			}
			else if (retval == STATUS_ERR_BAD_DATA)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_BAD_DATA!\r\n");
			}
			else if (retval == STATUS_ERR_DENIED)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_DENIED!\r\n");
			}
			else if (retval == STATUS_ERR_PROTOCOL)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_PROTOCOL!\r\n");
			}
			else if (retval == STATUS_ERR_PACKET_COLLISION)
			{
				sprintf(command, "usart_read_wait STATUS_ERR_PACKET_COLLISION!\r\n");
			}
			else
			{
				sprintf(command, "usart_read_wait UNKNOWN!\r\n");
			}

			bCommandReceived = false;
			break;
		}
	}

	return (bCommandReceived);

}	// End of wcm_usart_get_com_command