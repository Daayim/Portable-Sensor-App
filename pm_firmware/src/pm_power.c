/***************************************************************************
pm_power.c: Contains functions to control the power manager power modes

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	September 2022

Note(s):
- Based on net_sounder_power.cz
-----------------------------------------------------------------------------------------
SAML21J18B
Pin		I/O		PM board pin	Function			Notes:
-----------------------------------------------------------------------------------------
37		PA18	/ACCEL_INT		EXTINT[2]		  In interrupt.c
62		PB01	/WCM_FAULT		EXTINT[1]         In interrupt.c
20		PA11	/EXT_GPIO2		EXTINT[11]
44		PA23	/VBS_RX			EXTINT[7]
*****************************************************************************************/


#include <extint.h>
#include <system.h>
#include <system_interrupt.h>
#include <delay.h>
#include "pm_power.h"
#include "pm_clocks.h"
#include "pm_gpio.h"
#include "pm_usart.h"
#include "pm_spi.h"
#include "pm_config_codes.h"

/***************************************************************************
// Local variable(s)
****************************************************************************/

//	Interrupt variable
static const uint8_t wakeup_en_interrupt_channel = 11;
static const uint8_t wakeup_en_pin = PIN_PA11;

static const uint8_t vbs_wakeup_en_interrupt_channel = 7;
static const uint8_t vbs_wakeup_en_pin = PIN_PA23;


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
	extint_chan_conf_struct.gpio_pin = PIN_PA11A_EIC_EXTINT11; 
	extint_chan_conf_struct.gpio_pin_mux = PINMUX_PA11A_EIC_EXTINT11;
	extint_chan_conf_struct.gpio_pin_pull = EXTINT_PULL_DOWN;
	extint_chan_set_config(wakeup_en_interrupt_channel, &extint_chan_conf_struct);

	extint_chan_enable_callback(wakeup_en_interrupt_channel, EXTINT_CALLBACK_TYPE_DETECT);

	extint_chan_get_config_defaults(&extint_chan_conf_struct);
	extint_chan_conf_struct.detection_criteria = EXTINT_DETECT_RISING;
	extint_chan_conf_struct.enable_async_edge_detection = false;
	extint_chan_conf_struct.filter_input_signal = false;
	extint_chan_conf_struct.gpio_pin = PIN_PA23A_EIC_EXTINT7;
	extint_chan_conf_struct.gpio_pin_mux = PINMUX_PA23A_EIC_EXTINT7;
	extint_chan_conf_struct.gpio_pin_pull = EXTINT_PULL_DOWN;
	extint_chan_set_config(vbs_wakeup_en_interrupt_channel, &extint_chan_conf_struct);

	extint_chan_enable_callback(vbs_wakeup_en_interrupt_channel, EXTINT_CALLBACK_TYPE_DETECT);

	// Disable I/O retention
	system_io_retension_disable();

}	// End of power_interrupt_configure


/***************************************************************************
Local function to configure the WAKEUP/EN input pin
****************************************************************************/
void pm_power_configure_wakeup_en(void)
{
	struct system_pinmux_config system_pinmux_config_struct;

	// Initialize the configuration structures
	system_pinmux_get_config_defaults(&system_pinmux_config_struct);

	system_pinmux_config_struct.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	system_pinmux_config_struct.input_pull = SYSTEM_PINMUX_PIN_PULL_UP;
	system_pinmux_config_struct.powersave = false;
	system_pinmux_config_struct.mux_position = MUX_PA11A_EIC_EXTINT11;
	system_pinmux_pin_set_config(wakeup_en_pin, &system_pinmux_config_struct);
	
 	system_pinmux_get_config_defaults(&system_pinmux_config_struct);
 
 	system_pinmux_config_struct.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
 	system_pinmux_config_struct.input_pull = SYSTEM_PINMUX_PIN_PULL_UP;
 	system_pinmux_config_struct.powersave = false;
 	system_pinmux_config_struct.mux_position = MUX_PA23A_EIC_EXTINT7;
 	system_pinmux_pin_set_config(vbs_wakeup_en_pin, &system_pinmux_config_struct);

}	// End of configure_wakeup_en

/***************************************************************************
Function to disable the external interrupt
****************************************************************************/
void power_interrupt_disable(void)
{
	extint_chan_disable_callback(wakeup_en_interrupt_channel, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_clear_detected(wakeup_en_interrupt_channel);
  	extint_chan_disable_callback(vbs_wakeup_en_interrupt_channel, EXTINT_CALLBACK_TYPE_DETECT);
  	extint_chan_clear_detected(vbs_wakeup_en_interrupt_channel);

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
void pm_power_low_power_mode(void)
{	
	pm_spi_configure(MODE_DISABLED);
	pm_usart_disable();
	pm_clocks_configure(MODE_LOWPOWER); 
	pm_gpio_configure_lowpower();
 	power_interrupt_configure();
	power_standby();
	power_sleep();
	power_interrupt_disable();
	

}	// End of low_power_mode


/***************************************************************************
Local function to enter normal power mode
****************************************************************************/
void pm_power_normal_power_mode(void)
{
	
	pm_gpio_configure();
	pm_clocks_configure(MODE_NORMALPOWER);
	pm_usart_configure();
	pm_spi_configure(MODE_NORMALPOWER);
	delay_init();

	
}	// End of normal_power_mode
