/****************************************************************************************
wcm_gpio.h: Include file for wcm_gpio.c

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022
*****************************************************************************************/


#ifndef WCM_GPIO_H
#define WCM_GPIO_H


void wcm_gpio_configure(void);
void wcm_gpio_configure_lowpower(void);

bool wcm_gpio_spi_slave_select_get(void);

bool wcm_gpio_3v3va_get(void);
void wcm_gpio_3v3va_off(void);
void wcm_gpio_3v3va_on(void);

bool wcm_gpio_gps_pwr_en_get(void);
void wcm_gpio_gps_pwr_en_off(void);
void wcm_gpio_gps_pwr_en_on(void);

bool wcm_gpio_battery_serial_power_get(void);
void wcm_gpio_battery_serial_power_off(void);
void wcm_gpio_battery_serial_power_on(void);

bool wcm_gpio_com_sw_a_get(void);
void wcm_gpio_com_sw_a_off(void);
void wcm_gpio_com_sw_a_on(void);

bool wcm_gpio_sat_pwr_en_get(void);
void wcm_gpio_sat_pwr_en_off(void);
void wcm_gpio_sat_pwr_en_on(void);

void wcm_gpio_cell_pwr_en_on(void);
void wcm_gpio_cell_pwr_en_off(void);
bool wcm_gpio_cell_pwr_en_get(void);

bool wcm_gpio_wf_pwr_en_get(void);
void wcm_gpio_wf_pwr_en_off(void);
void wcm_gpio_wf_pwr_en_on(void);

bool wcm_lgt_get(void);
void wcm_lgt_off(void);
void wcm_lgt_on(void);

#endif	// WCM_GPIO_H


