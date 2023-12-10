/****************************************************************************************
wcm_gpio.c: Marine Mammal Detection (MMD) Wireless Communication Module (WCM) general purpose I/O pin
	functions

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022

Note(s):

MMD = Marine Mammal Detection
VBS = Variable Buoyancy System
WCM = Wireless Control Module

----------------------------------------------
SAML21E17B
Pin		I/O		WCM board pin	Function
----------------------------------------------
 3		PA02	GPS_PWER_EN		GPIO - Output 
 4		PA03	COM_SW_A		GPIO - Output
 7		PA06	SAT_PWR_EN		GPIO - Output
 8		PA07	LEAK			AIN[7]
11		PA08	+3V3VA_EN		GPIO - Output
14		PA11	BAT_DETECT		AIN[19]
19		PA18	CELL_PWR_EN		GPIO - Output
20		PA19	LGT_ON			GPIO - Output 
25		PA27	WF_PWR_EN		GPIO - Output

*****************************************************************************************/

#include <delay.h>
#include <port.h>
#include "wcm_gpio.h"


/****************************************************************************************
Local variable(s)
*****************************************************************************************/

// Configuration structures
static struct port_config port_config_struct;
static struct system_pinmux_config system_pinmux_config_struct;

/***************************
Analog input pin(s)
****************************/

// Leak detector analog input
static const uint8_t leak = PIN_PA07;
static const uint8_t bat_detect = PIN_PA11;

/***************************
GPIO pin(s) - Input for now
****************************/

//Clock test
//static const uint8_t ext_gpio3 = PIN_PB22;

/***************************
Interrupt/input pin(s)
****************************/

// VBS board
static const uint8_t spi_cs0 = PIN_PA24;

/***************************
Output pin(s)
****************************/

// 3.3 V analog (sensor) power
static const uint8_t en_3v3va = PIN_PA08;

// GPS PWR
static const uint8_t gps_pwr_en = PIN_PA02;

// COM SWITCH A PWR
static const uint8_t com_sw_a = PIN_PA03;

// SAT PWR
static const uint8_t sat_pwr_en = PIN_PA06;

// CELL PWR
static const uint8_t cell_pwr_en = PIN_PA18;

// LGT EN
static const uint8_t lgt_on = PIN_PA19;

// WIFI PWR
static const uint8_t wf_pwr_en = PIN_PA27;


/****************************************************************************************
Function to configure the MMD WCM GPIO pins
*****************************************************************************************/
void wcm_gpio_configure(void)
{
	// Initialize the configuration structures
	port_get_config_defaults(&port_config_struct);
	system_pinmux_get_config_defaults(&system_pinmux_config_struct);

	// Configure output pin(s)
	port_config_struct.direction = PORT_PIN_DIR_OUTPUT;
	
	port_pin_set_config(en_3v3va, &port_config_struct);
	port_pin_set_config(gps_pwr_en, &port_config_struct);
	port_pin_set_config(sat_pwr_en, &port_config_struct);
	port_pin_set_config(cell_pwr_en, &port_config_struct);
	port_pin_set_config(wf_pwr_en, &port_config_struct);
	port_pin_set_config(lgt_on, &port_config_struct);

	
	// Configure input pin(s)
	port_config_struct.direction = PORT_PIN_DIR_INPUT;
	port_config_struct.input_pull = PORT_PIN_PULL_NONE;

	port_pin_set_config(spi_cs0, &port_config_struct);


	// Configure special input pin(s)
	system_pinmux_config_struct.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	system_pinmux_config_struct.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;

	system_pinmux_config_struct.mux_position = MUX_PA07B_ADC_AIN7;
	system_pinmux_pin_set_config(leak, &system_pinmux_config_struct);
	system_pinmux_config_struct.mux_position = MUX_PA11B_ADC_AIN19;
	system_pinmux_pin_set_config(bat_detect, &system_pinmux_config_struct);	

}	// End of wcm_gpio_configure

void wcm_gpio_configure_lowpower(void)
{
	// Initialize the configuration structures
	port_get_config_defaults(&port_config_struct);
	system_pinmux_get_config_defaults(&system_pinmux_config_struct);
	
	//Configure pin(s) in powersave mode during sleep mode
	port_config_struct.powersave = true;

	//Output Pins
	port_pin_set_config(en_3v3va, &port_config_struct);
	port_pin_set_config(gps_pwr_en, &port_config_struct);
	port_pin_set_config(com_sw_a, &port_config_struct);
	port_pin_set_config(sat_pwr_en, &port_config_struct);
	port_pin_set_config(cell_pwr_en, &port_config_struct);
	port_pin_set_config(wf_pwr_en, &port_config_struct);
	port_pin_set_config(lgt_on, &port_config_struct);
	
	//Input Pins
	port_pin_set_config(spi_cs0, &port_config_struct);

	// Configure special input pin(s)
	system_pinmux_config_struct.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	system_pinmux_config_struct.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
	
	system_pinmux_config_struct.powersave = true;
		
	system_pinmux_config_struct.mux_position = MUX_PA02B_ADC_AIN0;
	system_pinmux_pin_set_config(leak, &system_pinmux_config_struct);
	system_pinmux_config_struct.mux_position = MUX_PA11B_ADC_AIN19;
	system_pinmux_pin_set_config(bat_detect, &system_pinmux_config_struct);

}	// End of wcm_gpio_configure
	
	
/****************************************************************************************
Function to return the slave select pin
*****************************************************************************************/
bool wcm_gpio_spi_slave_select_get(void)
{
	system_pinmux_config_struct.mux_position = SYSTEM_PINMUX_GPIO;

	system_pinmux_pin_set_config(spi_cs0, &system_pinmux_config_struct);

	return (port_pin_get_input_level(spi_cs0));

}	// End of wcm_gpio_spi_slave_select_get


/****************************************************************************************
Function to return the +3.3 V analog (sensor) power bit
*****************************************************************************************/
bool wcm_gpio_3v3va_get(void)
{
	return (port_pin_get_output_level(en_3v3va));

}	// End of wcm_gpio_3v3va_get


/****************************************************************************************
Function to turn off the +3.3 V analog (sensor) power
*****************************************************************************************/
void wcm_gpio_3v3va_off(void)
{
	port_pin_set_output_level(en_3v3va, false);

}	// End of wcm_gpio_3v3va_off


/****************************************************************************************
Function to turn on the +3.3 V analog (sensor) power
*****************************************************************************************/
void wcm_gpio_3v3va_on(void)
{
	port_pin_set_output_level(en_3v3va, true);
	delay_us(200);
}	// End of wcm_gpio_3v3va_on


/****************************************************************************************
Function to return the gps power bit
*****************************************************************************************/
bool wcm_gpio_gps_pwr_en_get(void)
{
	return (port_pin_get_output_level(gps_pwr_en));

}	// End of wcm_gpio_gps_pwr_en_get


/****************************************************************************************
Function to turn off the gps power
*****************************************************************************************/
void wcm_gpio_gps_pwr_en_off(void)
{
	port_pin_set_output_level(gps_pwr_en, false);

}	// End of wcm_gpio_gps_pwr_en_off


/****************************************************************************************
Function to turn on the gps power
*****************************************************************************************/
void wcm_gpio_gps_pwr_en_on(void)
{
	port_pin_set_output_level(gps_pwr_en, true);

}	// End of wcm_gpio_gps_pwr_en_on


/****************************************************************************************
Function to return the com switch A enable bit
*****************************************************************************************/
bool wcm_gpio_com_sw_a_get(void)
{
	return (port_pin_get_output_level(com_sw_a));

}	// End of wcm_gpio_com_sw_a_get


/****************************************************************************************
Function to turn off the com switch A
*****************************************************************************************/
void wcm_gpio_com_sw_a_off(void)
{
	port_pin_set_output_level(com_sw_a, false);

}	// End of wcm_gpio_com_sw_a_off


/****************************************************************************************
Function to turn on the com switch A
*****************************************************************************************/
void wcm_gpio_com_sw_a_on(void)
{
	port_pin_set_output_level(com_sw_a, true);

}	// End of wcm_gpio_com_sw_a_on


/****************************************************************************************
Function to return the SAT power bit
*****************************************************************************************/
bool wcm_gpio_sat_pwr_en_get(void)
{
	return (port_pin_get_output_level(sat_pwr_en));

}	// End of wcm_gpio_sat_pwr_en_get


/****************************************************************************************
Function to turn off the SAT power
*****************************************************************************************/
void wcm_gpio_sat_pwr_en_off(void)
{
	port_pin_set_output_level(sat_pwr_en, false);

}	// End of wcm_gpio_sat_pwr_en_off


/****************************************************************************************
Function to turn on the SAT power
*****************************************************************************************/
void wcm_gpio_sat_pwr_en_on(void)
{
	port_pin_set_output_level(sat_pwr_en, true);

}	// End of wcm_gpio_sat_pwr_en_on


/****************************************************************************************
Function to return the CELL power bit
*****************************************************************************************/
bool wcm_gpio_cell_pwr_en_get(void)
{
	return (port_pin_get_output_level(cell_pwr_en));

}	// End of wcm_gpio_cell_pwr_en_get


/****************************************************************************************
Function to turn off the CELL power
*****************************************************************************************/
void wcm_gpio_cell_pwr_en_off(void)
{
	port_pin_set_output_level(cell_pwr_en, false);

}	// End of wcm_gpio_cell_pwr_en_off


/****************************************************************************************
Function to turn on the CELL power
*****************************************************************************************/
void wcm_gpio_cell_pwr_en_on(void)
{
	port_pin_set_output_level(cell_pwr_en, true);

}	// End of wcm_gpio_cell_pwr_en_on


/****************************************************************************************
Function to return the WIFI power bit
*****************************************************************************************/
bool wcm_gpio_wf_pwr_en_get(void)
{
	return (port_pin_get_output_level(wf_pwr_en));

}	// End of wcm_gpio_wf_pwr_en_get


/****************************************************************************************
Function to turn off the WIFI power
*****************************************************************************************/
void wcm_gpio_wf_pwr_en_off(void)
{
	port_pin_set_output_level(wf_pwr_en, false);

}	// End of wcm_gpio_wf_pwr_en_off


/****************************************************************************************
Function to turn on the WIFI power
*****************************************************************************************/
void wcm_gpio_wf_pwr_en_on(void)
{
	port_pin_set_output_level(wf_pwr_en, true);

}	// End of wcm_gpio_wf_pwr_en_on


/****************************************************************************************
Function to return the LGT power bit
*****************************************************************************************/
bool wcm_lgt_get(void)
{
	return (port_pin_get_output_level(lgt_on));

}	// End of wcm_lgt_get


/****************************************************************************************
Function to turn off the LGT power
*****************************************************************************************/
void wcm_lgt_off(void)
{
	port_pin_set_output_level(lgt_on, false);

}	// End of wcm_lgt_off


/****************************************************************************************
Function to turn on the LGT power
*****************************************************************************************/
void wcm_lgt_on(void)
{
	port_pin_set_output_level(lgt_on, true);

}	// End of wcm_lgt_on
