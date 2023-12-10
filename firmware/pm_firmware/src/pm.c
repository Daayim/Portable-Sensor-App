/****************************************************************************************
pm.c:   power module (PM) board functions 

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021
*****************************************************************************************/

#include <delay.h>
#include <string.h>
#include "pm.h"
#include "pm_adc.h"
#include "pm_clocks.h"
#include "pm_gpio.h"
#include "pm_i2c.h"
/*
#include "pm_interrupt.h"
*/
#include "pm_ltc2944.h"
#include "pm_ms5637.h"
#include "pm_spi.h"
#include "pm_usart.h"

#include "pm_config_codes.h"
#include "pm_mc3416.h"
#include "pm_power.h"

#define COMMAND_LENGTH 64
#define SPI_BUFFER_LENGTH 16



#include <tc.h>
#include <tc_interrupt.h>


#define CONF_TC_MODULE TC0



/****************************************************************************************
Local variable(s)
*****************************************************************************************/

static bool bSPIInitialized;
static const uint8_t spi_command_length = 8;

// Timer Variables
struct tc_module tc_instance;
volatile static bool timer_0_elapsed = false;

/****************************************************************************************
Local function(s)
*****************************************************************************************/

static bool handle_command(char *);
static void handle_spi_command(char *, char *);
static void initInternalHW(void);

enum status_code read_mc3416(void);
void tc_callback_to_sleep_mode(struct tc_module *const module_inst);


/****************************************************************************************
Local function to read and send the MC3416 Angle data
*****************************************************************************************/
enum status_code read_mc3416(void)
{
	enum status_code status;
	double angle;
	char response[128];
	
	status = pm_mc3416_read_tilt(&angle);
	if (status == STATUS_OK)
	{
		sprintf(response, "ACCEL TILT ANGLE %.2f\r\n", angle);
		pm_usart_send_pc_message(response);
	}
	else if (status != STATUS_OK){
		sprintf(response, "ACCEL TILT ANGLE %.2f\r\n", angle);
		pm_usart_send_pc_message(response);		
	}
	return (status);	
}

/***************************************************************************
Local functions to configure TC for real time MC3416 measurements
****************************************************************************/

void tc_callback_to_sleep_mode(
struct tc_module *const module_inst)
{
	tc_set_count_value(&tc_instance, 0xFFFAA22C);
	timer_0_elapsed = true;
}




/****************************************************************************************
Local function to handle serial commands
*****************************************************************************************/
static bool handle_command(char *command)
{
	bool b;
	bool bValid;
	char response[128];
	char *token;
	enum status_code status;
	float v;
	int i;

	// Respond to valid commands
	bValid = true;

	if (strstr(command, "read_leak"))
	{
		status = pm_adc_read(&v);
		if (status == STATUS_OK)
		{
			sprintf(response, "LEAK %.2lf\r\n", v);
			pm_usart_send_pc_message(response);
		}
		else
		{
			pm_usart_send_pc_message("handle_command: Could not read leak detector!\r\n");
		}
	}
	else if (strstr(command, "pm_ping"))
	{
		pm_usart_send_pc_message("handle_command: Ping Received!\r\n");
		timer_0_elapsed = true;
	}
	else if (strstr(command, "read_ltc2944"))
	{
		double voltage;
		double current;
		double temperature;
		double charge;
		uint8_t status_value;
		pm_gpio_ltc2944_i2c_en_on();
		status = pm_ltc2944_read(&voltage, &current, &temperature, &charge, &status_value);
		pm_gpio_ltc2944_i2c_en_off();
		if (status == STATUS_OK)
		{
			sprintf(response, "VOLTAGE %.3lf\r\n", voltage);
			pm_usart_send_pc_message(response);

			sprintf(response, "CURRENT %.3lf\r\n", current);
			pm_usart_send_pc_message(response);

			sprintf(response, "LTC2944 TEMPERATURE %.2lf\r\n", temperature);
			pm_usart_send_pc_message(response);

			sprintf(response, "CHARGE %.2lf\r\n", charge);
			pm_usart_send_pc_message(response);

			sprintf(response, "STATUS 0x%02x\r\n", status_value);
			pm_usart_send_pc_message(response);
		}
		else
		{
			pm_usart_send_pc_message("handle_command: Could not read LTC2944!\r\n");
		}
	}
	else if (strstr(command, "read_ms5637"))
	{
		uint16_t c[7];
		uint32_t d1;
		double temperature;
		uint32_t d2;
		double pressure;
		
		pm_ms5637_get_calibration_coefficients(c);

		sprintf(response, "CRC %hu\r\n", c[0]);
		pm_usart_send_pc_message(response);

		sprintf(response, "C1 %hu\r\n", c[1]);
		pm_usart_send_pc_message(response);

		sprintf(response, "C2 %hu\r\n", c[2]);
		pm_usart_send_pc_message(response);

		sprintf(response, "C3 %hu\r\n", c[3]);
		pm_usart_send_pc_message(response);

		sprintf(response, "C4 %hu\r\n", c[4]);
		pm_usart_send_pc_message(response);

		sprintf(response, "C5 %hu\r\n", c[5]);
		pm_usart_send_pc_message(response);

		sprintf(response, "C6 %hu\r\n", c[6]);
		pm_usart_send_pc_message(response);

		status = pm_ms5637_read(&d1, &pressure, &d2, &temperature);
		if (status == STATUS_OK)
		{
			sprintf(response, "D1 %lu\r\n", d1);
			pm_usart_send_pc_message(response);

			sprintf(response, "D2 %lu\r\n", d2);
			pm_usart_send_pc_message(response);

			sprintf(response, "PRESSURE %.2f\r\n", pressure);
			pm_usart_send_pc_message(response);

			sprintf(response, "TEMPERATURE %.2f\r\n", temperature);
			pm_usart_send_pc_message(response);
		}
		else
		{
			pm_usart_send_pc_message("handle_command: Could not read MS5637!\r\n");
		}
	}	
	else if (strstr(command, "read_mc3416"))
	{

		status = read_mc3416();
		if (status != STATUS_OK)
		{
			pm_usart_send_pc_message("handle_command: Could not read MC3416!\r\n");
		}
	}
	
	else if (strstr(command, "calibrate_mc3416"))
	{
		status = pm_mc3416_calibrate();
		if (status != STATUS_OK)
		{
			pm_usart_send_pc_message("handle_command: Could not calibrate MC3416!\r\n");
		}
	}
	
	else if (strstr(command, "zero_mc3416"))
	{
		status = pm_mc3416_zero_offsets();
		if (status != STATUS_OK)
		{
			pm_usart_send_pc_message("handle_command: Could not zero MC3416!\r\n");
		}
	}	
	else if (strstr(command, "read_power_bits"))
	{
		b = pm_gpio_3v3va_get();
		sprintf(response, "+3V3VA_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_battery_select_get();
		sprintf(response, "BATT_SEL %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

/*
		b = pm_gpio_battery_serial_power_get();
		sprintf(response, "BATT_SER_PWR_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_ctd_power_get();
		sprintf(response, "CTD_PWR_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);
*/

		b = pm_gpio_driver_get();
		sprintf(response, "DRIVER_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_Main_power_get();
		sprintf(response, "Main_PWR_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_vbs_power_get();
		sprintf(response, "VBS_PWR_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_vbs_serial_power_get();
		sprintf(response, "VBS_SER_PWR_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_wcm_diagnostics_enable_get();
		sprintf(response, "WCM_DIAG_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_wcm_power_get();
		sprintf(response, "WCM_PWR_EN %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_wcm_relay_get();
		sprintf(response, "WCM_RLY %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);
	}
	else if (strstr(command, "read_status_bits"))
	{
		b = pm_gpio_accelerometer_interrupt_get();
		sprintf(response, "/ACCEL_INT %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_ext_gpio1_get();
		sprintf(response, "EXT_GPIO1 %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_ext_gpio2_get();
		sprintf(response, "EXT_GPIO2 %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_lt8618_pg_get();
		sprintf(response, "LT8618_PG %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_n_ltc2944_alcc_get();
		sprintf(response, "/LTC2944_ALCC %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);

		b = pm_gpio_wcm_fault_get();
		sprintf(response, "/WCM_FAULT %d\r\n", (b) ? 1 : 0);
		pm_usart_send_pc_message(response);
	}
	else if (strstr(command, "reinitialize"))
	{
		initInternalHW();
	}
	else if (strstr(command, "+3V3VA_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "+3V3VA_EN %d", i);
		if (i == 0)
		{
			pm_gpio_3v3va_off();
		}
		else
		{
			pm_gpio_3v3va_on();
		}
	}
	else if (strstr(command, "BATT_SEL"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "BATT_SEL %d", i);
		if (i == 0)
		{
			pm_gpio_battery_select_off();
		}
		else
		{
			pm_gpio_battery_select_on();
		}
	}
	else if (strstr(command, "BATT_SER_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "BATT_SER_PWR_EN %d", i);
		if (i == 0)
		{
			pm_gpio_battery_serial_power_off();
		}
		else
		{
			pm_gpio_battery_serial_power_on();
		}
	}
	else if (strstr(command, "CTD_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "CTD_PWR_EN %d", i);
		if (i == 0)
		{
			pm_gpio_ctd_power_off();
		}
		else
		{
			pm_gpio_ctd_power_on();
		}
	}
	else if (strstr(command, "DRIVER_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "DRIVER_EN %d", i);
		if (i == 1)
		{
			pm_gpio_driver_disable();
		}
		else
		{
			pm_gpio_driver_enable();
		}
	}
	else if (strstr(command, "Main_PWR_EN"))
	{
		b = pm_gpio_driver_get();
		if (b == 0)
		{
			token = strtok(command, " ");
			token = strtok(NULL, " ");
			i = atoi(token);

			sprintf(command, "Main_PWR_EN %d", i);
			if (i == 0)
			{
				pm_gpio_Main_power_off();
				bSPIInitialized = false;
	
				pm_usart_send_pc_message("handle_command: pm_gpio_Main_power_off\r\n");
			}
			else
			{
				pm_gpio_Main_power_on();
	
				pm_usart_send_pc_message("handle_command: pm_gpio_Main_power_on\r\n");
			}
		}
		else
		{
			b = pm_gpio_Main_power_get();
			sprintf(response, "Main_PWR_EN %d\r\n", (b) ? 1 : 0);
			pm_usart_send_pc_message(response);

			sprintf(command, "Main_PWR_EN unchanged");
		}
	}
	else if (strstr(command, "VBS_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "VBS_PWR_EN %d", i);
		if (i == 0)
		{
			pm_gpio_vbs_power_off();
		}
		else
		{
			pm_gpio_vbs_power_on();
		}
	}
	else if (strstr(command, "VBS_SER_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "VBS_SER_PWR_EN %d", i);
		if (i == 0)
		{
			pm_gpio_vbs_serial_power_off();
		}
		else
		{
			pm_gpio_vbs_serial_power_on();
		}
	}
	else if (strstr(command, "WCM_DIAG_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "WCM_DIAG_EN %d", i);
		if (i == 0)
		{
			pm_gpio_wcm_diagnostics_enable_off();
		}
		else
		{
			pm_gpio_wcm_diagnostics_enable_on();
		}
	}
	else if (strstr(command, "WCM_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "WCM_PWR_EN %d", i);
		if (i == 0)
		{
			pm_gpio_wcm_power_off();
		}
		else
		{
			pm_gpio_wcm_power_on();
		}
	}
	else if (strstr(command, "WCM_RLY"))
	{
		b = pm_gpio_driver_get();
		if (b == 0)
		{
			token = strtok(command, " ");
			token = strtok(NULL, " ");
			i = atoi(token);
			sprintf(command, "WCM_RLY %d", i);
			if (i == 0)
			{

				pm_gpio_wcm_power_off();
				pm_gpio_wcm_relay_off();
				b = pm_gpio_wcm_power_get();
				sprintf(response, "WCM_PWR_EN %d\r\n", (b) ? 1 : 0);
				pm_usart_send_pc_message(response);
			}
			else
			{
				pm_gpio_wcm_relay_on();
				pm_gpio_wcm_power_on();
				b = pm_gpio_wcm_power_get();
				sprintf(response, "WCM_PWR_EN %d\r\n", (b) ? 1 : 0);
				pm_usart_send_pc_message(response);				
			}
		}
		else
		{
			b = pm_gpio_wcm_relay_get();
			sprintf(response, "WCM_RLY %d\r\n", (b) ? 1 : 0);
			pm_usart_send_pc_message(response);

			sprintf(command, "WCM_RLY unchanged");
		}
	}
	else
	{
		bValid = false;
	}

	return (bValid);

}	// End of handle_command


/****************************************************************************************
Local function to handle SPI commands
*****************************************************************************************/
static void handle_spi_command(char *command, char *response)
{
	bool b;
	char *token;
	enum status_code status;
	int i;
	static char last_command[SPI_BUFFER_LENGTH] = {0x00};
	static uint8_t num_sent = 0;
	
	//Altimeter
	double mc3416_angle;

	// Power bits
	bool en_3v3va;
	static bool batt;
	static bool driver;
	static bool vbs;
	static bool vbs_serial;
	static bool wcm_d;
	static bool wcm_p;
	static bool wcm_r;

	// Status bits
	bool accel;
	static bool alcc;
	static bool fault;
	static bool gpio1;
	static bool gpio2;
	static bool pg;

	// LEAK
	float v;

	// LTC2944
	double voltage;
	static double charge = 0.0;
	static double current = 0.0;
	static double ltc2944_temperature = 0.0;
	static uint8_t status_value = 0;

	// MS5637
	double pressure;
	static double ms5637_temperature = 0.0;
	uint32_t d1;
	uint32_t d2;

	// Respond to valid commands
	if (strstr(command, "LEAK"))
	{
		strcpy(last_command, command);

		status = pm_adc_read(&v);
		if (status == STATUS_OK)
		{
			sprintf(response, "%*.2f", spi_command_length, v);
		}
		else
		{
			pm_usart_send_pc_message("handle_spi_command: Could not read leak detector!\r\n");
		}
	}
	if (strstr(command, "pm_ping"))
	{
		strcpy(last_command, command);
		pm_usart_send_pc_message("handle_command: Ping!\r\n");
		timer_0_elapsed = true;
	}
	else if (strstr(command, "LTC2944"))
	{
		strcpy(last_command, command);
		pm_gpio_ltc2944_i2c_en_on();
		status = pm_ltc2944_read(&voltage, &current, &ltc2944_temperature, &charge, &status_value);
		pm_gpio_ltc2944_i2c_en_off();
		if (status == STATUS_OK)
		{
			sprintf(response, "%*.3f", spi_command_length, voltage);
			num_sent = 1;

			//pm_usart_send_pc_message("handle_spi_command: LTC2944 voltage <");
			//pm_usart_send_pc_message(response);
			//pm_usart_send_pc_message(">\r\n");
		}
		else
		{
			pm_usart_send_pc_message("handle_spi_command: Could not read LTC2944!\r\n");
		}
	}
	else if (strstr(command, "MS5637"))
	{
		strcpy(last_command, command);

		status = pm_ms5637_read(&d1, &pressure, &d2, &ms5637_temperature);
		if (status == STATUS_OK)
		{
			sprintf(response, "%*.2f", spi_command_length, pressure);
			num_sent = 1;
		}
		else
		{
			pm_usart_send_pc_message("handle_spi_command: Could not read MS5637!\r\n");
		}
	}
	else if (strstr(command, "read_mc3416"))
	{
		strcpy(last_command, command);	
	
		status = pm_mc3416_read_tilt(&mc3416_angle);
		if (status == STATUS_OK)
		{
			sprintf(response, "%*.2f", spi_command_length, mc3416_angle);
			num_sent = 1;
		}
		else
		{
			pm_usart_send_pc_message("handle_command: Could not read MC3416!\r\n");
		}
	}
	else if (strstr(command, "calibrate_mc3416"))
	{
		strcpy(last_command, command);	
		
		status = pm_mc3416_calibrate();
		if (status != STATUS_OK)
		{
			pm_usart_send_pc_message("handle_command: Could not calibrate MC3416!\r\n");
		}
	}
	else if (strstr(command, "zero_mc3416"))
	{
		strcpy(last_command, command);	
		
		status = pm_mc3416_zero_offsets();
		if (status != STATUS_OK)
		{
			pm_usart_send_pc_message("handle_command: Could not zero MC3416!\r\n");
		}
	}	
	else if (strstr(command, "POWER"))
	{
		strcpy(last_command, command);

		en_3v3va = pm_gpio_3v3va_get();
		batt = pm_gpio_battery_select_get();
		driver = pm_gpio_driver_get();
		vbs = pm_gpio_vbs_power_get();
		vbs_serial = pm_gpio_vbs_serial_power_get();
		wcm_d = pm_gpio_wcm_diagnostics_enable_get();
		wcm_p = pm_gpio_wcm_power_get();
		wcm_r = pm_gpio_wcm_relay_get();

		sprintf(response, "%*d",spi_command_length, (en_3v3va) ? 1 : 0);
		num_sent = 1;
	}
	else if (strstr(command, "STATUS"))
	{
		strcpy(last_command, command);

		accel = pm_gpio_accelerometer_interrupt_get();
		gpio1 = pm_gpio_ext_gpio1_get();
		gpio2 = pm_gpio_ext_gpio2_get();
		pg = pm_gpio_lt8618_pg_get();
		alcc = pm_gpio_n_ltc2944_alcc_get();
		fault = pm_gpio_wcm_fault_get();

		sprintf(response, "%*d",spi_command_length, (accel) ? 1 : 0);
		num_sent = 1;
	}
	else if (strstr(command, "RESP"))
	{
		if (strstr(last_command, "LEAK"))
		{
			sprintf(response, "--------");
		}
		else if (strstr(last_command, "LTC2944"))
		{
			if (num_sent == 1)
			{
				sprintf(response, "%*.3f", spi_command_length, current);
				num_sent = 2;

				//pm_usart_send_pc_message("handle_spi_command: LTC2944 current\r\n");
			}
			else if (num_sent == 2)
			{
				sprintf(response, "%*.2f", spi_command_length, ltc2944_temperature);
				num_sent = 3;

				//pm_usart_send_pc_message("handle_spi_command: LTC2944 temperature\r\n");
			}
			else if (num_sent == 3)
			{
				sprintf(response, "%*.2f", spi_command_length, charge);
				num_sent = 4;

				//pm_usart_send_pc_message("handle_spi_command: LTC2944 charge\r\n");
			}
			else if (num_sent == 4)
			{
				sprintf(response, "%*d", spi_command_length, status_value);
				num_sent = 5;

				//pm_usart_send_pc_message("handle_spi_command: LTC2944 status\r\n");
			}
			else
			{
				sprintf(response, "--------");
			}
		}
		else if (strstr(last_command, "MS5637"))
		{
			if (num_sent == 1)
			{
				sprintf(response, "%*.2f", spi_command_length, ms5637_temperature);
				num_sent = 2;
			}
			else
			{
				sprintf(response, "--------");
			}
		}
		else if (strstr(last_command, "POWER"))
		{
			if (num_sent == 1)
			{
				sprintf(response, "%*d",spi_command_length, (batt) ? 1 : 0);
				num_sent = 2;
			}
			else if (num_sent == 2)
			{
				sprintf(response, "%*d",spi_command_length, (driver) ? 1 : 0);
				num_sent = 3;
			}
			else if (num_sent == 3)
			{
				sprintf(response, "%*d",spi_command_length, (vbs) ? 1 : 0);
				num_sent = 4;
			}
			else if (num_sent == 4)
			{
				sprintf(response, "%*d",spi_command_length, (vbs_serial) ? 1 : 0);
				num_sent = 5;
			}
			else if (num_sent == 5)
			{
				sprintf(response, "%*d",spi_command_length, (wcm_d) ? 1 : 0);
				num_sent = 6;
			}
			else if (num_sent == 6)
			{
				sprintf(response, "%*d",spi_command_length, (wcm_p) ? 1 : 0);
				num_sent = 7;
			}
			else if (num_sent == 7)
			{
				sprintf(response, "%*d",spi_command_length, (wcm_r) ? 1 : 0);
				num_sent = 8;
			}
			else
			{
				sprintf(response, "--------");
			}
		}
		else if (strstr(last_command, "STATUS"))
		{
			if (num_sent == 1)
			{
				sprintf(response, "%*d",spi_command_length, (gpio1) ? 1 : 0);
				num_sent = 2;
			}
			else if (num_sent == 2)
			{
				sprintf(response, "%*d",spi_command_length, (gpio2) ? 1 : 0);
				num_sent = 3;
			}
			else if (num_sent == 3)
			{
				sprintf(response, "%*d",spi_command_length, (pg) ? 1 : 0);
				num_sent = 4;
			}
			else if (num_sent == 4)
			{
				sprintf(response, "%*d",spi_command_length, (alcc) ? 1 : 0);
				num_sent = 5;
			}
			else if (num_sent == 5)
			{
				sprintf(response, "%*d",spi_command_length, (fault) ? 1 : 0);
				num_sent = 6;
			}
			else
			{
				sprintf(response, "--------");
			}
		}
	}
	else if (strstr(command, "+3V3VA"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "+3V3VA_EN %d", i);
		if (i == 0)
		{
			pm_gpio_3v3va_off();
		}
		else
		{
			pm_gpio_3v3va_on();
		}
	}
	else if (strstr(command, "BATT"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "BATT_SEL %d", i);
		if (i == 0)
		{
			pm_gpio_battery_select_off();
		}
		else
		{
			pm_gpio_battery_select_on();
		}
	}
	else if (strstr(command, "DRIVER"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "DRIVER_EN %d", i);
		if (i == 0)
		{
			pm_gpio_driver_disable();
		}
		else
		{
			pm_gpio_driver_enable();
		}
	}
	else if (strstr(command, "VBS_P"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "VBS_PWR_EN %d", i);
		if (i == 0)
		{
			pm_gpio_vbs_power_off();
		}
		else
		{
			pm_gpio_vbs_power_on();
		}
	}
	else if (strstr(command, "VBS_S"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "VBS_SER_PWR_EN %d", i);
		if (i == 0)
		{
			pm_gpio_vbs_serial_power_off();
		}
		else
		{
			pm_gpio_vbs_serial_power_on();
		}
	}
	else if (strstr(command, "WCM_D"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "WCM_DIAG_EN %d", i);
		if (i == 0)
		{
			pm_gpio_wcm_diagnostics_enable_off();
		}
		else
		{
			pm_gpio_wcm_diagnostics_enable_on();
		}
	}
	else if (strstr(command, "WCM_P"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "WCM_PWR_EN %d", i);
		if (i == 0)
		{
			pm_gpio_wcm_power_off();
		}
		else
		{
			pm_gpio_wcm_power_on();
		}
	}
	else if (strstr(command, "WCM_EN")) //WCM_RLY Changed to enable for Main SPI Com.
	{
		b = pm_gpio_driver_get();
		if (b == 1)
		{
			token = strtok(command, " ");
			token = strtok(NULL, " ");
			i = atoi(token);
			sprintf(command, "WCM_EN %d", i);
			if (i == 0)
			{
				pm_gpio_wcm_power_off();
				pm_gpio_wcm_relay_off();
			}
			else
			{
				pm_gpio_wcm_relay_on();
				pm_gpio_wcm_power_on();
			}
		}
	}
	else
	{
		pm_usart_send_pc_message("handle_spi_command: Unknown command!\r\n");
	}
}	// End of handle_spi_command


/****************************************************************************************
Local function to initialize the internal hardware
*****************************************************************************************/
static void initInternalHW(void)
{
	enum status_code status;

	pm_gpio_ltc2944_i2c_en_on();
	status = pm_ltc2944_init();
	pm_gpio_ltc2944_i2c_en_off();
	if (status == STATUS_OK)
	{
		pm_usart_send_pc_message("initInternalHW: pm_ltc2944_init done\r\n");
		pm_usart_send_vbs_command("initInternalHW: pm_ltc2944_init done\r\n");
	}
	else
	{
		pm_usart_send_pc_message("initInternalHW: Could not initialize LTC2944!\r\n");
		pm_usart_send_vbs_command("initInternalHW: Could not initialize LTC2944!\r\n");
	}

 	status = pm_ms5637_init();
 	if (status == STATUS_OK)
 	{
 		pm_usart_send_pc_message("initInternalHW: pm_ms5637_init done\r\n");
 		pm_usart_send_vbs_command("initInternalHW: pm_ms5637_init done\r\n");
 	}
 	else
 	{
 		pm_usart_send_pc_message("initInternalHW: Could not initialize MS5637!\r\n");
 		pm_usart_send_vbs_command("initInternalHW: Could not initialize MS5637!\r\n");
 	}
	
	status = pm_mc3416_init();
	if (status == STATUS_OK)
	{
		pm_usart_send_pc_message("initInternalHW: pm_mc3416_init done\r\n");
		pm_usart_send_vbs_command("initInternalHW: pm_mc3416_init done\r\n");
	}
	else
	{
		pm_usart_send_pc_message("initInternalHW: Could not initialize MC3416!\r\n");
		pm_usart_send_vbs_command("initInternalHW: Could not initialize MC3416!\r\n");
	}
	
}	// End of initInternalHW


/****************************************************************************************
Function to initialize the Main PM board
*****************************************************************************************/
void pm_init(void)
{
	// Initialize the clocks, drivers, interfaces and interrupts
	pm_power_normal_power_mode();
//	pm_clocks_configure(); // Replace by normal_power_mode functions
//	delay_init();
	pm_adc_configure();	
//	pm_gpio_configure();
	pm_i2c_configure();
/*
	pm_interrupt_configure();
*/
	pm_usart_configure();

	// Turn on Main board and sensors
	pm_gpio_3v3va_on();
	pm_gpio_driver_enable();	
	
	

	
	//GPIO2 SET OFF for Testing
//	pm_gpio_ext_gpio2_off();
	
//	pm_gpio_Main_power_on();

	// Turn off everything else
	pm_gpio_battery_select_off();
	pm_gpio_battery_serial_power_off();
	pm_gpio_ltc2944_i2c_en_off();
	pm_gpio_ctd_power_off();
	pm_gpio_vbs_power_off();
	pm_gpio_vbs_serial_power_off();
	pm_gpio_wcm_diagnostics_enable_off();
	pm_gpio_wcm_power_off();
	pm_gpio_wcm_relay_off();

	initInternalHW();

}	// End of pm_init


/****************************************************************************************
Function to run the Main PM board operations
*****************************************************************************************/
void pm_run(void)
{
	//Timer Configuration for MC3416
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_32BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
		
	//	Delay  = 30s
	// 	Find number of cycles required to get given time delay
	// 	Desired Time/(prescaler/system clk frequency) = #cycles.
	//	30s/(1024/12000000) = 351563 cycles.
	// 	Subtract from max value to get number of clk cycles to load in timer; 2^32 - 351700 ? 0xFFFAA22C
	config_tc.counter_32_bit.value = 0xFFFAA22C;
	tc_init(&tc_instance, CONF_TC_MODULE, &config_tc);
	tc_enable(&tc_instance);
		
	tc_register_callback(&tc_instance, tc_callback_to_sleep_mode,
	TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
		
	bool b;
	bool bCommandReceived;
	bool bValid;
	char command[COMMAND_LENGTH];
	enum status_code retval;
	int i;
	uint8_t spi_rx_buffer[SPI_BUFFER_LENGTH] = {0x00};
	uint8_t spi_tx_buffer[SPI_BUFFER_LENGTH] = {0x00};

	pm_usart_send_pc_message("pm_run: started\r\n");
	pm_usart_send_vbs_command("pm_run: started\r\n");
	

	bSPIInitialized = false;
	i = 0;
	
	while (1)
	{	

   		
		
		timer_0_elapsed = false;
		
		while (timer_0_elapsed == false)		
		{
					
			if (bSPIInitialized == false)
			{
				// Slave select is high when SPI master  is on
				b = pm_gpio_spi_slave_select_get();
				if (b == true)
				{
					for (i = 0; i < 8; i++)
					{
						spi_tx_buffer[i] = '-';
					}

					pm_spi_configure(MODE_ENABLED);

					retval = pm_spi_start_read(spi_tx_buffer, spi_rx_buffer, spi_command_length);
					if (retval == STATUS_OK)
					{
						bSPIInitialized = true;

						pm_usart_send_pc_message("pm_run: SPI initialized\r\n");
					}
					else
					{
						pm_usart_send_pc_message("pm_run: pm_spi_start_read failed (1)!\r\n");
					}
				}
			}

	/*
			// Check for a /PWR_FAULT interrupt
			if (pm_interrupt_pwr_fault_occurred())
			{
				pm_usart_send_pc_message("pm_run: /PWR_FAULT interrupt occurred!\r\n");

				pm_interrupt_pwr_fault_clear();
			}

			// Check for an E_PRS_EOC interrupt
			if (pm_interrupt_e_prs_eoc_occurred())
			{
				pm_usart_send_pc_message("pm_run: E_PRS_EOC interrupt occurred!\r\n");

				pm_interrupt_e_prs_eoc_clear();
			}

			// Check for a /3D-IRQ interrupt
			if (pm_interrupt_three_d_occurred())
			{
				pm_usart_send_pc_message("pm_run: /3D-IRQ interrupt occurred!\r\n");

				pm_interrupt_three_d_clear();
			}
	*/

			// Check for an SPI command
			if (bSPIInitialized == true)
			{
				if (pm_spi_transfer_complete())
				{
					delay_ms(10);

					// Handle the SPI buffer data
					spi_rx_buffer[spi_command_length] = '\0';
					handle_spi_command((char *)spi_rx_buffer, (char *)spi_tx_buffer);

					// Start another SPI read
					retval = pm_spi_start_read(spi_tx_buffer, spi_rx_buffer, spi_command_length);
					if (retval != STATUS_OK)
					{
						bSPIInitialized = false;

						pm_usart_send_pc_message("pm_run: pm_spi_start_read failed (2)!\r\n");
					}
				}
			}
		
			// Check for a serial command
			retval = pm_usart_check_for_pc_command();
			if (retval == STATUS_OK)
			{
				bCommandReceived = pm_usart_get_pc_command(command, COMMAND_LENGTH);
				if (bCommandReceived)
				{
					bValid = handle_command(command);
					pm_usart_send_pc_message(command);
					pm_usart_send_pc_message(" ");
					if (bValid)
					{
						pm_usart_send_pc_message("VALID\r\n");
					}
					else
					{
						pm_usart_send_pc_message("INVALID\r\n");
					}
				}
			}
// 			if (timer_0_elapsed == true)
// 			{
// 				timer_0_elapsed = false;
// 
// 				read_mc3416();
// 			}
			
		}
		pm_usart_send_pc_message("pm_run: entering sleep mode\r\n");
		pm_power_configure_wakeup_en();
		pm_power_low_power_mode();
		pm_power_normal_power_mode();
		pm_usart_send_pc_message("pm_run: exiting sleep mode\r\n");
	}
}	// End of pm_run


