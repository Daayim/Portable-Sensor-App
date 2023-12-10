/****************************************************************************************
pm_gpio.c:   power module (PM) general purpose I/O pin
	functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021

Note(s):
- The CTD sensor is not being used
- For now, just read /ACCEL_INT and /WCM_FAULT instead of using interrupts

CTD = Conductivity, Temperature, Depth
Main = 
VBS = Variable Buoyancy System
WCM = Wireless Control Module

----------------------------------------------
SAML21J18B
Pin		I/O		PM board pin	Function
----------------------------------------------
3		PA02	LEAK			AIN[0]
19		PA10	EXT_GPIO1		GPIO - Input
20		PA11	EXT_GPIO2		GPIO - Input
23		PB10	Main_PWR_EN		GPIO - Output
24		PB11	DRIVER_EN		GPIO - Output
29		PA12	BATT_SEL		GPIO - Output
30		PA13	/ALCC			GPIO - Input
37		PA18	/ACCEL_INT		EXTINT[2]
38		PA19	+3V3VA_EN		GPIO - Output
42		PA21	BAT_SER_PWR_EN	GPIO - Output
45		PA24	LT8618_PG		GPIO - Input
46		PA25	CTD_PWR_EN		GPIO - Output
59		PB30	VBS_SER_PWR_EN	GPIO - Output
60		PB31	WCM_RLY			GPIO - Output
61		PB00	WCM_PWR_EN		GPIO - Output
62		PB01	/WCM_FAULT		EXTINT[1]
63		PB02	WCM_PDIA_EN		GPIO - Output
64		PB03	VBS_PWR_EN		GPIO - Output
*****************************************************************************************/

#include <delay.h>
#include <port.h>
#include "pm_gpio.h"


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
static const uint8_t leak = PIN_PA02;

/***************************
GPIO pin(s) - Input for now
****************************/
//Clock test
//static const uint8_t ext_gpio3 = PIN_PB22;

// Main board
static const uint8_t ext_gpio1 = PIN_PA10;
static const uint8_t ext_gpio2 = PIN_PA11;

/***************************
Interrupt/input pin(s)
****************************/

// Accelerometer
static const uint8_t n_accel_int = PIN_PA18;

// LT8618 24 V to 3.3 V DC-DC regulator
static const uint8_t lt8618_pg = PIN_PA24;

// LTC2944 battery gas gauge
static const uint8_t n_ltc2944_alcc = PIN_PA13;

// Main board
static const uint8_t spi1_ss0 = PIN_PB13;

// WCM board
static const uint8_t n_wcm_fault = PIN_PB01;

/***************************
Output pin(s)
****************************/

// 3.3 V analog (sensor) power
static const uint8_t en_3v3va = PIN_PA19;

// Battery
static const uint8_t batt_sel = PIN_PA12;
static const uint8_t batt_ser_pwr_en = PIN_PA21;

//LTC2944
static const uint8_t ltc2944_i2c_en = PIN_PB17;

// CTD sensor
static const uint8_t ctd_pwr_en = PIN_PA25;

// Main / WCM relay driver
static const uint8_t driver_en = PIN_PB11;

// Main board
static const uint8_t Main_pwr_en = PIN_PB10;

// VBS board
static const uint8_t vbs_pwr_en = PIN_PB03;
static const uint8_t vbs_ser_pwr_en = PIN_PB30;
static const uint8_t usb_rx = PIN_PA23; //switch to vbs_rx for testing

// WCM board
static const uint8_t wcm_diag_en = PIN_PB02;
static const uint8_t wcm_pwr_en = PIN_PB00;
static const uint8_t wcm_rly = PIN_PB31;



/****************************************************************************************
Function to configure the Main PM GPIO pins
*****************************************************************************************/
void pm_gpio_configure(void)
{
	// Initialize the configuration structures
	port_get_config_defaults(&port_config_struct);
	system_pinmux_get_config_defaults(&system_pinmux_config_struct);

	// Configure output pin(s)
	port_config_struct.direction = PORT_PIN_DIR_OUTPUT;
	
	port_pin_set_config(en_3v3va, &port_config_struct);
	port_pin_set_config(batt_sel, &port_config_struct);
	port_pin_set_config(batt_ser_pwr_en, &port_config_struct);
	port_pin_set_config(ctd_pwr_en, &port_config_struct);
	port_pin_set_config(driver_en, &port_config_struct);
	port_pin_set_config(Main_pwr_en, &port_config_struct);
	port_pin_set_config(vbs_pwr_en, &port_config_struct);
	port_pin_set_config(vbs_ser_pwr_en, &port_config_struct);
	port_pin_set_config(wcm_diag_en, &port_config_struct);
	port_pin_set_config(wcm_pwr_en, &port_config_struct);
	port_pin_set_config(wcm_rly, &port_config_struct);
	
	// Configure input pin(s)
	port_config_struct.direction = PORT_PIN_DIR_INPUT;
	port_config_struct.input_pull = PORT_PIN_PULL_NONE;

	port_pin_set_config(n_accel_int, &port_config_struct);
	port_pin_set_config(ext_gpio1, &port_config_struct);
	port_pin_set_config(ext_gpio2, &port_config_struct);
	port_pin_set_config(lt8618_pg, &port_config_struct);
	port_pin_set_config(n_ltc2944_alcc, &port_config_struct);
	port_pin_set_config(n_wcm_fault, &port_config_struct);

	port_pin_set_config(spi1_ss0, &port_config_struct);

	// Configure special input pin(s)
	system_pinmux_config_struct.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	system_pinmux_config_struct.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;

	system_pinmux_config_struct.mux_position = MUX_PA02B_ADC_AIN0;
	system_pinmux_pin_set_config(leak, &system_pinmux_config_struct);

}	// End of pm_gpio_configure

void pm_gpio_configure_lowpower(void)
{
	// Initialize the configuration structures
	port_get_config_defaults(&port_config_struct);
	system_pinmux_get_config_defaults(&system_pinmux_config_struct);

	//Configure pin(s) in kept on during sleep mode
// 	port_pin_set_config(vbs_pwr_en, &port_config_struct);
// 	port_pin_set_config(ctd_pwr_en, &port_config_struct);
	
	//Configure pin(s) in powersave mode during sleep mode
	port_config_struct.powersave = true;

	//Output Pins
	port_pin_set_config(en_3v3va, &port_config_struct);
	port_pin_set_config(batt_sel, &port_config_struct);
	port_pin_set_config(batt_ser_pwr_en, &port_config_struct);
	port_pin_set_config(ltc2944_i2c_en, &port_config_struct);
	port_pin_set_config(driver_en, &port_config_struct);
	port_pin_set_config(Main_pwr_en, &port_config_struct);
	port_pin_set_config(vbs_ser_pwr_en, &port_config_struct);
	port_pin_set_config(wcm_diag_en, &port_config_struct);
	port_pin_set_config(wcm_pwr_en, &port_config_struct);
	port_pin_set_config(wcm_rly, &port_config_struct);
	
	//Input Pins
	port_pin_set_config(usb_rx, &port_config_struct);
	port_pin_set_config(n_accel_int, &port_config_struct);
	port_pin_set_config(ext_gpio1, &port_config_struct);
	port_pin_set_config(ext_gpio2, &port_config_struct);
	port_pin_set_config(lt8618_pg, &port_config_struct);
	port_pin_set_config(n_ltc2944_alcc, &port_config_struct);
	port_pin_set_config(n_wcm_fault, &port_config_struct);

	port_pin_set_config(spi1_ss0, &port_config_struct);


	// Configure special input pin(s)
	system_pinmux_config_struct.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	system_pinmux_config_struct.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
	
	system_pinmux_config_struct.powersave = true;
		
	system_pinmux_config_struct.mux_position = MUX_PA02B_ADC_AIN0;
	system_pinmux_pin_set_config(leak, &system_pinmux_config_struct);

}	// End of pm_gpio_configure
	

/****************************************************************************************
Function to return the +3.3 V analog (sensor) power bit
*****************************************************************************************/
bool pm_gpio_3v3va_get(void)
{
	return (port_pin_get_output_level(en_3v3va));

}	// End of pm_gpio_3v3va_get


/****************************************************************************************
Function to turn off the +3.3 V analog (sensor) power
*****************************************************************************************/
void pm_gpio_3v3va_off(void)
{
	port_pin_set_output_level(en_3v3va, false);

}	// End of pm_gpio_3v3va_off


/****************************************************************************************
Function to turn on the +3.3 V analog (sensor) power
*****************************************************************************************/
void pm_gpio_3v3va_on(void)
{
	port_pin_set_output_level(en_3v3va, true);
	delay_us(200);
}	// End of pm_gpio_3v3va_on


/****************************************************************************************
Function to return the accelerometer interrupt bit
*****************************************************************************************/
bool pm_gpio_accelerometer_interrupt_get(void)
{
	return (port_pin_get_input_level(n_accel_int));

}	// End of pm_gpio_accelerometer_interrupt_get


/****************************************************************************************
Function to return the battery select bit
*****************************************************************************************/
bool pm_gpio_battery_select_get(void)
{
	return (port_pin_get_output_level(batt_sel));

}	// End of pm_gpio_battery_select_get


/****************************************************************************************
Function to turn off the battery select bit
*****************************************************************************************/
void pm_gpio_battery_select_off(void)
{
	port_pin_set_output_level(batt_sel, false);

}	// End of pm_gpio_battery_select_off


/****************************************************************************************
Function to turn on the battery select bit
*****************************************************************************************/
void pm_gpio_battery_select_on(void)
{
	port_pin_set_output_level(batt_sel, true);

}	// End of pm_gpio_battery_select_on


/****************************************************************************************
Function to return the battery serial power bit
*****************************************************************************************/
bool pm_gpio_battery_serial_power_get(void)
{
	return (port_pin_get_output_level(batt_ser_pwr_en));

}	// End of pm_gpio_battery_serial_power_get


/****************************************************************************************
Function to turn off the battery serial power
*****************************************************************************************/
void pm_gpio_battery_serial_power_off(void)
{
	port_pin_set_output_level(batt_ser_pwr_en, false);

}	// End of pm_gpio_battery_serial_power_off


/****************************************************************************************
Function to turn on the battery serial power
*****************************************************************************************/
void pm_gpio_battery_serial_power_on(void)
{
	port_pin_set_output_level(batt_ser_pwr_en, true);

}	// End of pm_gpio_battery_serial_power_on

/****************************************************************************************
Function to return the LTC2944 I2C enable bit
*****************************************************************************************/
bool pm_gpio_ltc2944_i2c_en_get(void)
{
	return (port_pin_get_output_level(ltc2944_i2c_en));

}	// End of pm_gpio_ltc2944_i2c_en_get


/****************************************************************************************
Function to turn off the LTC2944 I2C enable
*****************************************************************************************/
void pm_gpio_ltc2944_i2c_en_off(void)
{
	port_pin_set_output_level(ltc2944_i2c_en, false);

}	// End of pm_gpio_ltc2944_i2c_en_off


/****************************************************************************************
Function to turn on the LTC2944 I2C enable
*****************************************************************************************/
void pm_gpio_ltc2944_i2c_en_on(void)
{
	port_pin_set_output_level(ltc2944_i2c_en, true);

}	// End of pm_gpio_ltc2944_i2c_en_on

/****************************************************************************************
Function to return the CTD power bit
*****************************************************************************************/
bool pm_gpio_ctd_power_get(void)
{
	return (port_pin_get_output_level(ctd_pwr_en));

}	// End of pm_gpio_ctd_power_get

/****************************************************************************************
Function to turn off the CTD power
*****************************************************************************************/
void pm_gpio_ctd_power_off(void)
{
	port_pin_set_output_level(ctd_pwr_en, false);

}	// End of pm_gpio_ctd_power_off


/****************************************************************************************
Function to turn on the CTD power
*****************************************************************************************/
void pm_gpio_ctd_power_on(void)
{
	port_pin_set_output_level(ctd_pwr_en, true);

}	// End of pm_gpio_ctd_power_on


/****************************************************************************************
Function to disable the Main / WCM relay driver
*****************************************************************************************/
void pm_gpio_driver_disable(void)
{
	port_pin_set_output_level(driver_en, true);

}	// End of pm_gpio_driver_disable


/****************************************************************************************
Function to enable the Main / WCM relay driver
*****************************************************************************************/
void pm_gpio_driver_enable(void)
{
	port_pin_set_output_level(driver_en, false);

}	// End of pm_gpio_driver_enable

/****************************************************************************************
Function to return the Main / WCM relay driver enable bit
*****************************************************************************************/
bool pm_gpio_driver_get(void)
{
	return (port_pin_get_output_level(driver_en));

}	// End of pm_gpio_driver_get


/****************************************************************************************
Function to return external GPIO bit 1
*****************************************************************************************/
bool pm_gpio_ext_gpio1_get(void)
{
	return (port_pin_get_input_level(ext_gpio1));

}	// End of pm_gpio_ext_gpio1_get

/****************************************************************************************
Function to turn off the +3.3 V gpio1 output
*****************************************************************************************/
void pm_gpio_ext_gpio1_off(void)
{
	port_pin_set_output_level(ext_gpio1, false);

}	// End of pm_gpio2_off


/****************************************************************************************
Function to turn on the +3.3 V gpio1 output
*****************************************************************************************/
void pm_gpio_ext_gpio1_on(void)
{
	port_pin_set_output_level(ext_gpio1, true);

}	// End of pm_gpio2_on


/****************************************************************************************
Function to turn off the +3.3 V gpio2 output
*****************************************************************************************/
void pm_gpio_ext_gpio2_off(void)
{
	port_pin_set_output_level(ext_gpio2, false);

}	// End of pm_gpio2_off


/****************************************************************************************
Function to turn on the +3.3 V gpio2 output
*****************************************************************************************/
void pm_gpio_ext_gpio2_on(void)
{
	port_pin_set_output_level(ext_gpio2, true);

}	// End of pm_gpio2_on


/****************************************************************************************
Function to return external GPIO bit 2
*****************************************************************************************/
bool pm_gpio_ext_gpio2_get(void)
{
	return (port_pin_get_input_level(ext_gpio2));

}	// End of pm_gpio_ext_gpio2_get


/****************************************************************************************
Function to return the LT8618 power good (PG) bit
*****************************************************************************************/
bool pm_gpio_lt8618_pg_get(void)
{
	return (port_pin_get_input_level(lt8618_pg));

}	// End of pm_gpio_lt8618_pg_get


/****************************************************************************************
Function to return the LTC2944 alert output or charge complete input (/ALCC) bit
*****************************************************************************************/
bool pm_gpio_n_ltc2944_alcc_get(void)
{
	return (port_pin_get_input_level(n_ltc2944_alcc));

}	// End of pm_gpio_n_ltc2944_alcc_get


/****************************************************************************************
Function to return the Main power bit
*****************************************************************************************/
bool pm_gpio_Main_power_get(void)
{
	return (port_pin_get_output_level(Main_pwr_en));

}	// End of pm_gpio_Main_power_get


/****************************************************************************************
Function to turn off the Main power
*****************************************************************************************/
void pm_gpio_Main_power_off(void)
{
	port_pin_set_output_level(Main_pwr_en, false);

}	// End of pm_gpio_Main_power_off


/****************************************************************************************
Function to turn on the Main power
*****************************************************************************************/
void pm_gpio_Main_power_on(void)
{
	port_pin_set_output_level(Main_pwr_en, true);

}	// End of pm_gpio_Main_power_on


/****************************************************************************************
Function to return the slave select pin
*****************************************************************************************/
bool pm_gpio_spi_slave_select_get(void)
{
	system_pinmux_config_struct.mux_position = SYSTEM_PINMUX_GPIO;

	system_pinmux_pin_set_config(spi1_ss0, &system_pinmux_config_struct);

	return (port_pin_get_input_level(spi1_ss0));

}	// End of pm_gpio_spi_slave_select_get


/****************************************************************************************
Function to return the VBS power bit
*****************************************************************************************/
bool pm_gpio_vbs_power_get(void)
{
	return (port_pin_get_output_level(vbs_pwr_en));

}	// End of pm_gpio_vbs_power_get


/****************************************************************************************
Function to turn off the VBS power
*****************************************************************************************/
void pm_gpio_vbs_power_off(void)
{
	port_pin_set_output_level(vbs_pwr_en, false);

}	// End of pm_gpio_vbs_power_off


/****************************************************************************************
Function to turn on the VBS power
*****************************************************************************************/
void pm_gpio_vbs_power_on(void)
{
	port_pin_set_output_level(vbs_pwr_en, true);

}	// End of pm_gpio_vbs_power_on


/****************************************************************************************
Function to return the VBS serial power bit
*****************************************************************************************/
bool pm_gpio_vbs_serial_power_get(void)
{
	return (port_pin_get_output_level(vbs_ser_pwr_en));

}	// End of pm_gpio_vbs_serial_power_get


/****************************************************************************************
Function to turn off the VBS serial power
*****************************************************************************************/
void pm_gpio_vbs_serial_power_off(void)
{
	port_pin_set_output_level(vbs_ser_pwr_en, false);

}	// End of pm_gpio_vbs_serial_power_off


/****************************************************************************************
Function to turn on the VBS serial power
*****************************************************************************************/
void pm_gpio_vbs_serial_power_on(void)
{
	port_pin_set_output_level(vbs_ser_pwr_en, true);

}	// End of pm_gpio_vbs_serial_power_on


/****************************************************************************************
Function to return the WCM diagnostics enable bit
*****************************************************************************************/
bool pm_gpio_wcm_diagnostics_enable_get(void)
{
	return (port_pin_get_output_level(wcm_diag_en));

}	// End of pm_gpio_wcm_diagnostics_enable_get


/****************************************************************************************
Function to turn off the WCM diagnostics
*****************************************************************************************/
void pm_gpio_wcm_diagnostics_enable_off(void)
{
	port_pin_set_output_level(wcm_diag_en, false);

}	// End of pm_gpio_wcm_diagnostics_enable_off


/****************************************************************************************
Function to turn on the WCM diagnostics
*****************************************************************************************/
void pm_gpio_wcm_diagnostics_enable_on(void)
{
	port_pin_set_output_level(wcm_diag_en, true);

}	// End of pm_gpio_wcm_diagnostics_enable_on


/****************************************************************************************
Function to return the TPS27S100 (WCM power switch) /FLT bit
*****************************************************************************************/
bool pm_gpio_wcm_fault_get(void)
{
	return (port_pin_get_input_level(n_wcm_fault));

}	// End of pm_gpio_wcm_fault_get


/****************************************************************************************
Function to return the WCM power bit
*****************************************************************************************/
bool pm_gpio_wcm_power_get(void)
{
	return (port_pin_get_output_level(wcm_pwr_en));

}	// End of pm_gpio_wcm_power_get


/****************************************************************************************
Function to turn off the WCM power
*****************************************************************************************/
void pm_gpio_wcm_power_off(void)
{
	port_pin_set_output_level(wcm_pwr_en, false);

}	// End of pm_gpio_wcm_power_off


/****************************************************************************************
Function to turn on the WCM power
*****************************************************************************************/
void pm_gpio_wcm_power_on(void)
{
	port_pin_set_output_level(wcm_pwr_en, true);

}	// End of pm_gpio_wcm_power_on


/****************************************************************************************
Function to return the WCM relay bit
*****************************************************************************************/
bool pm_gpio_wcm_relay_get(void)
{
	return (port_pin_get_output_level(wcm_rly));

}	// End of pm_gpio_wcm_relay_get


/****************************************************************************************
Function to turn off the WCM relay
*****************************************************************************************/
void pm_gpio_wcm_relay_off(void)
{
	port_pin_set_output_level(wcm_rly, false);

}	// End of pm_gpio_wcm_relay_off


/****************************************************************************************
Function to turn on the WCM relay
*****************************************************************************************/
void pm_gpio_wcm_relay_on(void)
{
	port_pin_set_output_level(wcm_rly, true);

}	// End of pm_gpio_wcm_relay_on


