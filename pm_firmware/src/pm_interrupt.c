/****************************************************************************************
pm_interrupt.c:   power module (PM) interrupt functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021

Note(s):
- Main PM board is configured to be an SPI slave

-----------------------------------------------------------------------------------------
SAML21J18B
Pin		I/O		PM board pin	Function			Notes:
-----------------------------------------------------------------------------------------
37		PA18	/ACCEL_INT		EXTINT[2]
62		PB01	/WCM_FAULT		EXTINT[1]
20		PA11	/EXT_GPIO2		EXTINT[11]        In power.c
44		PA23	/VBS_RX			EXTINT[7]		  In power.c
*****************************************************************************************/


#include <extint.h>
#include <extint_callback.h>
#include "pm_interrupt.h"


/****************************************************************************************
Local variable(s)
*****************************************************************************************/

static volatile bool e_prs_eoc_interrupt_occurred = false;
static const int e_prs_eoc_interrupt_line = 5;

static volatile bool pwr_fault_interrupt_occurred = false;
static const int pwr_fault_interrupt_line = 2;

static volatile bool three_d_interrupt_occurred = false;
static const int three_d_interrupt_line = 2;
	

/****************************************************************************************
Local function(s)
*****************************************************************************************/

static void e_prs_eoc_interrupt_callback(void);
static void e_prs_eoc_interrupt_callback_configure(void);
static void e_prs_eoc_interrupt_configure(void);

static void pwr_fault_interrupt_callback(void);
static void pwr_fault_interrupt_callback_configure(void);
static void pwr_fault_interrupt_configure(void);

static void three_d_interrupt_callback(void);
static void three_d_interrupt_callback_configure(void);
static void three_d_interrupt_configure(void);


/****************************************************************************************
E_PRS_EOC interrupt callback function
*****************************************************************************************/
static void e_prs_eoc_interrupt_callback(void)
{
	e_prs_eoc_interrupt_occurred = true;
	
}	// End of e_prs_eoc_interrupt_callback


/****************************************************************************************
Local function to configure the E_PRS_EOC interrupt callback
*****************************************************************************************/
static void e_prs_eoc_interrupt_callback_configure(void)
{
	extint_register_callback(e_prs_eoc_interrupt_callback, e_prs_eoc_interrupt_line, EXTINT_CALLBACK_TYPE_DETECT);

	extint_chan_enable_callback(e_prs_eoc_interrupt_line, EXTINT_CALLBACK_TYPE_DETECT);

}	// End of e_prs_eoc_interrupt_callback_configure


/****************************************************************************************
Local function to configure the external E_PRS_EOC interrupt
*****************************************************************************************/
static void e_prs_eoc_interrupt_configure(void)
{
	struct extint_chan_conf extint_chan_conf_struct;

	extint_chan_get_config_defaults(&extint_chan_conf_struct);

	extint_chan_conf_struct.gpio_pin            = PIN_PA21A_EIC_EXTINT5;
	extint_chan_conf_struct.gpio_pin_mux        = MUX_PA21A_EIC_EXTINT5;
	extint_chan_conf_struct.gpio_pin_pull       = EXTINT_PULL_DOWN;
	extint_chan_conf_struct.detection_criteria  = EXTINT_DETECT_RISING;
	extint_chan_conf_struct.filter_input_signal = true;

	extint_chan_set_config(e_prs_eoc_interrupt_line, &extint_chan_conf_struct);

}	// End of e_prs_eoc_interrupt_configure


/****************************************************************************************
Function to configure the Main PM external interrupts
*****************************************************************************************/
void pm_interrupt_configure(void)
{
	e_prs_eoc_interrupt_configure();
	e_prs_eoc_interrupt_callback_configure();
	
	//pwr_fault_interrupt_configure();
	//pwr_fault_interrupt_callback_configure();
	
	three_d_interrupt_configure();
	three_d_interrupt_callback_configure();

}	// End of pm_interrupt_configure


/****************************************************************************************
Function to return the e_prs_eoc_interrupt_occurred variable
*****************************************************************************************/
bool pm_interrupt_e_prs_eoc_occurred(void)
{
	return e_prs_eoc_interrupt_occurred;
	
}	// End of pm_interrupt_e_prs_eoc_occurred


/****************************************************************************************
Function to clear the e_prs_eoc_interrupt_occurred variable
*****************************************************************************************/
void pm_interrupt_e_prs_eoc_clear(void)
{
	e_prs_eoc_interrupt_occurred = false;
	
}	// End of pm_interrupt_e_prs_eoc_clear


/****************************************************************************************
Function to return the pwr_fault_interrupt_occurred variable
*****************************************************************************************/
bool pm_interrupt_pwr_fault_occurred(void)
{
	return pwr_fault_interrupt_occurred;
	
}	// End of pm_interrupt_pwr_fault_occurred


/****************************************************************************************
Function to clear the pwr_fault_interrupt_occurred variable
*****************************************************************************************/
void pm_interrupt_pwr_fault_clear(void)
{
	pwr_fault_interrupt_occurred = false;
	
}	// End of pm_interrupt_pwr_fault_clear


/****************************************************************************************
Function to return the three_d_interrupt_occurred variable
*****************************************************************************************/
bool pm_interrupt_three_d_occurred(void)
{
	return three_d_interrupt_occurred;
	
}	// End of pm_interrupt_three_d_occurred


/****************************************************************************************
Function to clear the three_d_interrupt_occurred variable
*****************************************************************************************/
void pm_interrupt_three_d_clear(void)
{
	three_d_interrupt_occurred = false;
	
}	// End of pm_interrupt_three_d_clear


/****************************************************************************************
/PWR_FAULT interrupt callback function
*****************************************************************************************/
static void pwr_fault_interrupt_callback(void)
{
	pwr_fault_interrupt_occurred = true;
	
}	// End of pwr_fault_interrupt_callback


/****************************************************************************************
Local function to configure the /PWR_FAULT interrupt callback
*****************************************************************************************/
static void pwr_fault_interrupt_callback_configure(void)
{
	extint_register_callback(pwr_fault_interrupt_callback, pwr_fault_interrupt_line, EXTINT_CALLBACK_TYPE_DETECT);

	extint_chan_enable_callback(pwr_fault_interrupt_line, EXTINT_CALLBACK_TYPE_DETECT);

}	// End of pwr_fault_interrupt_callback_configure


/****************************************************************************************
Local function to configure the external /PWR_FAULT interrupt
*****************************************************************************************/
static void pwr_fault_interrupt_configure(void)
{
	struct extint_chan_conf extint_chan_conf_struct;

	extint_chan_get_config_defaults(&extint_chan_conf_struct);

	extint_chan_conf_struct.gpio_pin            = PIN_PB02A_EIC_EXTINT2;
	extint_chan_conf_struct.gpio_pin_mux        = MUX_PB02A_EIC_EXTINT2;
	extint_chan_conf_struct.gpio_pin_pull       = EXTINT_PULL_UP;
	extint_chan_conf_struct.detection_criteria  = EXTINT_DETECT_FALLING;
	extint_chan_conf_struct.filter_input_signal = true;

	extint_chan_set_config(pwr_fault_interrupt_line, &extint_chan_conf_struct);

}	// End of pwr_fault_interrupt_configure


/****************************************************************************************
/3D-IRQ interrupt callback function
*****************************************************************************************/
static void three_d_interrupt_callback(void)
{
	three_d_interrupt_occurred = true;
	
}	// End of three_d_interrupt_callback


/****************************************************************************************
Local function to configure the /3D-IRQ interrupt callback
*****************************************************************************************/
static void three_d_interrupt_callback_configure(void)
{
	extint_register_callback(three_d_interrupt_callback, three_d_interrupt_line, EXTINT_CALLBACK_TYPE_DETECT);

	extint_chan_enable_callback(three_d_interrupt_line, EXTINT_CALLBACK_TYPE_DETECT);

}	// End of three_d_interrupt_callback_configure


/****************************************************************************************
Local function to configure the external /3D-IRQ interrupt
*****************************************************************************************/
static void three_d_interrupt_configure(void)
{
	struct extint_chan_conf extint_chan_conf_struct;

	extint_chan_get_config_defaults(&extint_chan_conf_struct);

	extint_chan_conf_struct.gpio_pin            = PIN_PA18A_EIC_EXTINT2;
	extint_chan_conf_struct.gpio_pin_mux        = MUX_PA18A_EIC_EXTINT2;
	extint_chan_conf_struct.gpio_pin_pull       = EXTINT_PULL_UP;
	extint_chan_conf_struct.detection_criteria  = EXTINT_DETECT_FALLING;
	extint_chan_conf_struct.filter_input_signal = true;

	extint_chan_set_config(three_d_interrupt_line, &extint_chan_conf_struct);

}	// End of three_d_interrupt_configure


