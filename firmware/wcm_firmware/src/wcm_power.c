/***************************************************************************
wcm_power.c: Contains functions to control the Wireless Communication Module power modes

Written by:
	Daayim Asim, Computer Engineer Student.
	Work Term Student
	eSonar Inc.

Date:
	September 2022

Note(s):
- USB_RX Wakeup used for testing purposes. reconfigure for GPS when needed.
-----------------------------------------------------------------------------------------
SAML21J18B
Pin		I/O		WCM board pin	Function			Notes:
-----------------------------------------------------------------------------------------
 2		PA01	GPS_RX			EXTINT[1]			VBS usart wakeup
23		PA24	SPI_CS			EXTINT[12]			SPI CS wakeup  
12		PA09	USB_RX			EXTINT[9]			USB usart wakeup
*****************************************************************************************/


#include <extint.h>
#include <system.h>
#include <system_interrupt.h>
#include <delay.h>
#include "wcm_power.h"
#include "wcm_clocks.h"
#include "wcm_gpio.h"
#include "wcm_spi.h"
#include "wcm_usart.h"
#include "wcm_config_codes.h"


/***************************************************************************
// Local variable(s)
****************************************************************************/

//	Interrupt variable
static const uint8_t usb_wakeup_en_interrupt_channel = 9;
static const uint8_t usb_wakeup_en_pin = PIN_PA09;

static const uint8_t spi_wakeup_en_interrupt_channel = 12;
static const uint8_t spi_wakeup_en_pin = PIN_PA24;

/***************************************************************************
// Local function(s)
****************************************************************************/

void power_interrupt_configure(void);
void power_interrupt_disable(void);
void power_normal(void);
void power_sleep(void);
void power_standby(void);


/***************************************************************************
Function to configure the WAKEUP/EN pin external interrupt
****************************************************************************/
void power_interrupt_configure(void)
{

	struct extint_chan_conf extint_chan_conf_struct;

	extint_chan_get_config_defaults(&extint_chan_conf_struct);
	extint_chan_conf_struct.detection_criteria = EXTINT_DETECT_RISING;
	extint_chan_conf_struct.enable_async_edge_detection = false;
	extint_chan_conf_struct.filter_input_signal = false;
	extint_chan_conf_struct.gpio_pin = PIN_PA09A_EIC_EXTINT9; 
	extint_chan_conf_struct.gpio_pin_mux = PINMUX_PA09A_EIC_EXTINT9;
	extint_chan_conf_struct.gpio_pin_pull = EXTINT_PULL_DOWN;
	extint_chan_set_config(usb_wakeup_en_interrupt_channel, &extint_chan_conf_struct);

	extint_chan_enable_callback(usb_wakeup_en_interrupt_channel, EXTINT_CALLBACK_TYPE_DETECT);

	extint_chan_get_config_defaults(&extint_chan_conf_struct);
	extint_chan_conf_struct.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_conf_struct.enable_async_edge_detection = false;
	extint_chan_conf_struct.filter_input_signal = false;
	extint_chan_conf_struct.gpio_pin = PIN_PA24A_EIC_EXTINT12;
	extint_chan_conf_struct.gpio_pin_mux = PINMUX_PA24A_EIC_EXTINT12;
	extint_chan_conf_struct.gpio_pin_pull = EXTINT_PULL_UP;
	extint_chan_set_config(spi_wakeup_en_interrupt_channel, &extint_chan_conf_struct);

	extint_chan_enable_callback(spi_wakeup_en_interrupt_channel, EXTINT_CALLBACK_TYPE_DETECT);

	// Disable I/O retention
	system_io_retension_disable();

}	// End of power_interrupt_configure


/***************************************************************************
Local function to configure the WAKEUP/EN input pin
****************************************************************************/
void wcm_power_configure_wakeup_en(void)
{
	struct system_pinmux_config system_pinmux_config_struct;

	// Initialize the configuration structures
	system_pinmux_get_config_defaults(&system_pinmux_config_struct);

	system_pinmux_config_struct.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	system_pinmux_config_struct.input_pull = SYSTEM_PINMUX_PIN_PULL_UP;
	system_pinmux_config_struct.powersave = false;
	system_pinmux_config_struct.mux_position = MUX_PA09A_EIC_EXTINT9;
	system_pinmux_pin_set_config(usb_wakeup_en_pin, &system_pinmux_config_struct);
	
 	system_pinmux_get_config_defaults(&system_pinmux_config_struct);
 	
 	system_pinmux_config_struct.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
 	system_pinmux_config_struct.input_pull = SYSTEM_PINMUX_PIN_PULL_UP;
 	system_pinmux_config_struct.powersave = false;
 	system_pinmux_config_struct.mux_position = MUX_PA24A_EIC_EXTINT12;
 	system_pinmux_pin_set_config(spi_wakeup_en_pin, &system_pinmux_config_struct);

}	// End of configure_wakeup_en

/***************************************************************************
Function to disable the external interrupt
****************************************************************************/
void power_interrupt_disable(void)
{
	extint_chan_disable_callback(usb_wakeup_en_interrupt_channel, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_clear_detected(usb_wakeup_en_interrupt_channel);
	extint_chan_disable_callback(spi_wakeup_en_interrupt_channel, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_clear_detected(spi_wakeup_en_interrupt_channel);
	
}	// End of power_interrupt_disable


/***************************************************************************
Function to switch to performance level 2
****************************************************************************/
void power_normal(void)
{
	system_switch_performance_level(SYSTEM_PERFORMANCE_LEVEL_2);

}	// End of power_normal


/***************************************************************************
Function to put the microprocessor to sleep and wait for an interrupt
****************************************************************************/
void power_sleep(void)
{
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	system_sleep();

}	// End of power_sleep


/***************************************************************************
Function to switch to performance level 0
****************************************************************************/
void power_standby(void)
{
	system_switch_performance_level(SYSTEM_PERFORMANCE_LEVEL_0);

}	// End of power_standby


/***************************************************************************
Local function to enter low power mode
****************************************************************************/
void wcm_power_low_power_mode(void)
{	
	wcm_spi_configure(MODE_DISABLED);
	wcm_usart_disable();
	wcm_clocks_configure(MODE_LOWPOWER); 
	wcm_gpio_configure_lowpower();
 	power_interrupt_configure();
	power_standby();
	power_sleep();
	power_interrupt_disable();

	
}	// End of low_power_mode


/***************************************************************************
Local function to enter normal power mode
****************************************************************************/
void wcm_power_normal_power_mode(void)
{
	
	wcm_gpio_configure();
	wcm_clocks_configure(MODE_NORMALPOWER);
	wcm_usart_configure();
	wcm_spi_configure(MODE_NORMALPOWER);
	delay_init();

}	// End of normal_power_mode

