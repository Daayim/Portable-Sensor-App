/****************************************************************************************
pm_gpio.h: Include file for pm_gpio.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021
*****************************************************************************************/


#ifndef PM_GPIO_H
#define PM_GPIO_H


void pm_gpio_configure(void);
void pm_gpio_configure_lowpower(void);

bool pm_gpio_3v3va_get(void);
void pm_gpio_3v3va_off(void);
void pm_gpio_3v3va_on(void);

bool pm_gpio_accelerometer_interrupt_get(void);

bool pm_gpio_battery_select_get(void);
void pm_gpio_battery_select_off(void);
void pm_gpio_battery_select_on(void);

bool pm_gpio_battery_serial_power_get(void);
void pm_gpio_battery_serial_power_off(void);
void pm_gpio_battery_serial_power_on(void);

bool pm_gpio_ltc2944_i2c_en_get(void);
void pm_gpio_ltc2944_i2c_en_off(void);
void pm_gpio_ltc2944_i2c_en_on(void);


bool pm_gpio_ctd_power_get(void);
void pm_gpio_ctd_power_off(void);
void pm_gpio_ctd_power_on(void);

void pm_gpio_driver_disable(void);
void pm_gpio_driver_enable(void);
bool pm_gpio_driver_get(void);

bool pm_gpio_ext_gpio1_get(void);
void pm_gpio_ext_gpio1_off(void);
void pm_gpio_ext_gpio1_on(void);

bool pm_gpio_ext_gpio2_get(void);
void pm_gpio_ext_gpio2_off(void);
void pm_gpio_ext_gpio2_on(void);

bool pm_gpio_lt8618_pg_get(void);

bool pm_gpio_n_ltc2944_alcc_get(void);

bool pm_gpio_Main_power_get(void);
void pm_gpio_Main_power_off(void);
void pm_gpio_Main_power_on(void);

bool pm_gpio_spi_slave_select_get(void);

bool pm_gpio_vbs_power_get(void);
void pm_gpio_vbs_power_off(void);
void pm_gpio_vbs_power_on(void);

bool pm_gpio_vbs_serial_power_get(void);
void pm_gpio_vbs_serial_power_off(void);
void pm_gpio_vbs_serial_power_on(void);

bool pm_gpio_wcm_diagnostics_enable_get(void);
void pm_gpio_wcm_diagnostics_enable_off(void);
void pm_gpio_wcm_diagnostics_enable_on(void);

bool pm_gpio_wcm_fault_get(void);

bool pm_gpio_wcm_power_get(void);
void pm_gpio_wcm_power_off(void);
void pm_gpio_wcm_power_on(void);

bool pm_gpio_wcm_relay_get(void);
void pm_gpio_wcm_relay_off(void);
void pm_gpio_wcm_relay_on(void);


#endif	// PM_GPIO_H


