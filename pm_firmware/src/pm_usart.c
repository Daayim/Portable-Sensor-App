/****************************************************************************************
pm_usart.c:   power module (PM) USART functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student

Date:
	November 2021

Note(s):

----------------------------------------------
SAML21J18B
Pin		I/O		PM board pin	Function
----------------------------------------------
17		PA08	VBS_TX			SERCOM0/PAD[0]
18		PA09	VBS_RX			SERCOM0/PAD[1]
39		PB16	BATT_TX			SERCOM5/PAD[0]
41		PA20	BATT_RX			SERCOM5/PAD[2]
43		PA22	USB_TX			SERCOM3/PAD[0]
44		PA23	USB_RX			SERCOM3/PAD[1]
*****************************************************************************************/


#include <string.h>
#include <usart.h>
#include "pm_usart.h"
#include "pm_config_codes.h"


/****************************************************************************************
// Local variable(s)
*****************************************************************************************/

static struct usart_module pc_usart_module_struct;
static struct usart_module vbs_usart_module_struct;

static SercomUsart *pc_usart_hw;

/****************************************************************************************
// Local function(s)
*****************************************************************************************/

static void pc_usart_configure(uint8_t mode);
static void vbs_usart_configure(uint8_t mode);


/****************************************************************************************
Local function to configure the SERCOM3 USART for communication with the control computer
*****************************************************************************************/
static void pc_usart_configure(uint8_t mode)
{
	static bool bFirst = true;

	struct usart_config usart_config_struct;

	usart_get_config_defaults(&usart_config_struct);

	usart_config_struct.baudrate                               = 38400;
	usart_config_struct.mux_setting                            = USART_RX_1_TX_0_XCK_1;
	usart_config_struct.pinmux_pad0                            = PINMUX_PA22C_SERCOM3_PAD0;
	usart_config_struct.pinmux_pad1                            = PINMUX_PA23C_SERCOM3_PAD1;
	usart_config_struct.pinmux_pad2                            = PINMUX_UNUSED;
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
	
	
	usart_init(&pc_usart_module_struct, SERCOM3, &usart_config_struct);
	usart_enable(&pc_usart_module_struct);
	
	if (mode == MODE_DISABLED){
		pm_usart_send_pc_message("pc usart disabled!\r\n");
		usart_disable(&pc_usart_module_struct);
	}

	// Get a pointer to the hardware module instance
	pc_usart_hw = &((&pc_usart_module_struct)->hw->USART);

}	// End of pc_usart_configure


/***************************************************************************
Function to check for a command from the control computer
****************************************************************************/
enum status_code pm_usart_check_for_pc_command(void)
{
	// Check if USART has new data
	if (pc_usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC)
	{
		return STATUS_OK;
	}

	return STATUS_BUSY;

}	// End of pm_usart_check_for_pc_command


/****************************************************************************************
Function to configure the serial ports for communications with the control
computer and VBS
*****************************************************************************************/
void pm_usart_configure(void)
{
	pc_usart_configure(MODE_ENABLED);
	vbs_usart_configure(MODE_ENABLED);	

}	// End of pm_usart_configure

void pm_usart_disable(void)
{	
	vbs_usart_configure(MODE_DISABLED);
	pc_usart_configure(MODE_DISABLED);
}


/***************************************************************************
Function to wait for a command from the control computer
****************************************************************************/
bool pm_usart_get_pc_command(char *command, int command_length)
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

}	// End of pm_usart_get_pc_command


/***************************************************************************
Function to send a message to the control computer
****************************************************************************/
void pm_usart_send_pc_message(const char *message)
{
	usart_write_buffer_wait(&pc_usart_module_struct, (uint8_t *)message, strlen(message));

}	// End of pm_usart_send_pc_message


/***************************************************************************
Function to send a command to the VBS
****************************************************************************/
void pm_usart_send_vbs_command(const char *command)
{
	usart_write_buffer_wait(&vbs_usart_module_struct, (uint8_t *)command, strlen(command));

}	// End of pm_usart_send_vbs_command


/****************************************************************************************
Local function to configure the SERCOM5 USART for communication with the VBS
*****************************************************************************************/
static void vbs_usart_configure(uint8_t mode)
{
	static bool bFirst = true;

	struct usart_config usart_config_struct;

	usart_get_config_defaults(&usart_config_struct);

	usart_config_struct.baudrate                               = 38400;
	usart_config_struct.mux_setting                            = USART_RX_2_TX_0_XCK_1;
	usart_config_struct.pinmux_pad0                            = PINMUX_PB16C_SERCOM5_PAD0;
	usart_config_struct.pinmux_pad1                            = PINMUX_UNUSED;
	usart_config_struct.pinmux_pad2                            = PINMUX_PA20C_SERCOM5_PAD2;
	usart_config_struct.pinmux_pad3                            = PINMUX_UNUSED;
	usart_config_struct.transfer_mode                          = USART_TRANSFER_ASYNCHRONOUSLY;

	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		usart_disable(&vbs_usart_module_struct);
	}
	
	usart_init(&vbs_usart_module_struct, SERCOM5, &usart_config_struct);
	
	
	if (mode == MODE_DISABLED){
		pm_usart_send_pc_message("vbs usart disabled!\r\n");
		usart_disable(&vbs_usart_module_struct);		
	}
	
	else{
		usart_enable(&vbs_usart_module_struct);		
	}


}	// End of vbs_usart_configure


