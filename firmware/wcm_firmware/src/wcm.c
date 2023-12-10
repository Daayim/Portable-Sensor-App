/****************************************************************************************
wcm.c: Marine Mammal Detection (MMD) Wireless Control Module (WCM) board functions 

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022
*****************************************************************************************/

#include <delay.h>
#include <string.h>
#include "wcm.h"
#include "wcm_adc.h"
#include "wcm_clocks.h"
#include "wcm_gpio.h"
#include "wcm_i2c.h"

#include "wcm_ms5637.h"
#include "wcm_spi.h"
#include "wcm_usart.h"

#include "wcm_mc3416.h"
#include "wcm_power.h"
#include "wcm_config_codes.h"

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
void tc_callback_to_read_mc3416(struct tc_module *const module_inst);


/****************************************************************************************
Local function to read and send the MC3416 Angle data
*****************************************************************************************/
enum status_code read_mc3416(void)
{
	enum status_code status;
	double angle;
	char response[128];
	
	status = wcm_mc3416_read_tilt(&angle);
	if (status == STATUS_OK)
	{
		sprintf(response, "TILT ANGLE %.2f\r\n", angle);
		wcm_usart_send_pc_message(response);
	}
	else if (status != STATUS_OK){
		sprintf(response, "TILT ANGLE %.2f\r\n", angle);
		wcm_usart_send_pc_message(response);		
	}
	return (status);	
}

/***************************************************************************
Local functions to configure TC for real time MC3416 measurements
****************************************************************************/

void tc_callback_to_read_mc3416(
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
	bool data_received;
	char response[128];
	char *token;
	enum status_code status;
	float v;
	float batt;
	int i;
	
	bool satCommandReceived;
//	bool satValid;
	char satcommand[COMMAND_LENGTH];
	enum status_code sat_status;

	// Respond to valid commands
	bValid = true;

	if (strstr(command, "read_leak"))
	{
		status = wcm_adc_read(&v);
		if (status == STATUS_OK)
		{
			sprintf(response, "LEAK %.2lf\r\n", v);
			wcm_usart_send_pc_message(response);
		}
		else
		{
			wcm_usart_send_pc_message("handle_command: Could not read leak detector!\r\n");
		}
	}
	else if (strstr(command, "read_batt"))
	{
		status = wcm_bat_adc_read(&batt);
		if (status == STATUS_OK)
		{
			sprintf(response, "BATTERY DETECT %.2lf\r\n", batt);
			wcm_usart_send_pc_message(response);
		}
		else
		{
			wcm_usart_send_pc_message("handle_command: Could not read battery detector!\r\n");
		}
	}
	
	// Check for a sat data
	else if  (strstr(command, "read_coms"))
	{
		wcm_gpio_sat_pwr_en_on();
		sat_status = wcm_usart_check_for_com_data();
		if (sat_status == STATUS_OK)
		{
//			wcm_usart_send_com_command("AT&D0\r\n");
			satCommandReceived = wcm_usart_get_com_data(satcommand, COMMAND_LENGTH);
			if (satCommandReceived)
			{
				wcm_usart_send_pc_message(satcommand);
				wcm_usart_send_pc_message(" ");					
				wcm_usart_send_pc_message("VALID\r\n");
					
			}
			else
			{
				wcm_usart_send_pc_message("INVALID\r\n");
			}			
		}
		wcm_gpio_sat_pwr_en_off();
		
	}
	else if (strstr(command, "read_gps"))
	{
		if (wcm_gpio_gps_pwr_en_get())
		{
			status = wcm_usart_check_for_gps_data();
			
			if (status == STATUS_OK)
			{			
				data_received = wcm_usart_get_gps_data(command, COMMAND_LENGTH);
				if (data_received)
				{
					wcm_usart_send_pc_message(command);
					wcm_usart_send_pc_message(" ");
				}
			}
		}
	}
	else if (strstr(command, "wcm_ping"))
	{
		wcm_usart_send_pc_message("handle_command: Ping Received!\r\n");
		tc_set_count_value(&tc_instance, 0xFFFAA22C);
		timer_0_elapsed = true;
	}
	else if (strstr(command, "read_ms5637"))
	{
		uint32_t d1;
		double temperature;
		uint32_t d2;
		double pressure;

		status = wcm_ms5637_read(&d1, &pressure, &d2, &temperature);
		if (status == STATUS_OK)
		{
			sprintf(response, "D1 %lu\r\n", d1);
			wcm_usart_send_pc_message(response);

			sprintf(response, "D2 %lu\r\n", d2);
			wcm_usart_send_pc_message(response);

			sprintf(response, "MS5637 PRESSURE %.2f\r\n", pressure);
			wcm_usart_send_pc_message(response);

			sprintf(response, "MS5637 TEMPERATURE %.2f\r\n", temperature);
			wcm_usart_send_pc_message(response);
		}
		else
		{
			wcm_usart_send_pc_message("handle_command: Could not read MS5637!\r\n");
		}
	}	
	else if (strstr(command, "read_mc3416"))
	{

		status = read_mc3416();
		if (status != STATUS_OK)
		{
			wcm_usart_send_pc_message("handle_command: Could not read MC3416!\r\n");
		}
	}
	
	else if (strstr(command, "calibrate_mc3416"))
	{
		status = wcm_mc3416_calibrate();
		if (status != STATUS_OK)
		{
			wcm_usart_send_pc_message("handle_command: Could not calibrate MC3416!\r\n");
		}
	}
	
	else if (strstr(command, "zero_mc3416"))
	{
		status = wcm_mc3416_zero_offsets();
		if (status != STATUS_OK)
		{
			wcm_usart_send_pc_message("handle_command: Could not zero MC3416!\r\n");
		}
	}	
	else if (strstr(command, "read_power_bits"))
	{
		b = wcm_gpio_3v3va_get();
		sprintf(response, "+3V3VA_EN %d\r\n", (b) ? 1 : 0);
		wcm_usart_send_pc_message(response);

		b = wcm_gpio_gps_pwr_en_get();
		sprintf(response, "GPS_PWR_EN %d\r\n", (b) ? 1 : 0);
		wcm_usart_send_pc_message(response);
		
		b = wcm_gpio_com_sw_a_get();
		sprintf(response, "COM_SW_A %d\r\n", (b) ? 1 : 0);
		wcm_usart_send_pc_message(response);

		b = wcm_gpio_sat_pwr_en_get();
		sprintf(response, "SAT_PWR_EN %d\r\n", (b) ? 1 : 0);
		wcm_usart_send_pc_message(response);

		b = wcm_gpio_cell_pwr_en_get();
		sprintf(response, "CELL PWR_EN %d\r\n", (b) ? 1 : 0);
		wcm_usart_send_pc_message(response);

		b = wcm_gpio_wf_pwr_en_get();
		sprintf(response, "WF_PWR_EN %d\r\n", (b) ? 1 : 0);
		wcm_usart_send_pc_message(response);

		b = wcm_lgt_get();
		sprintf(response, "LGT_ON %d\r\n", (b) ? 1 : 0);
		wcm_usart_send_pc_message(response);

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
			wcm_gpio_3v3va_off();
		}
		else
		{
			wcm_gpio_3v3va_on();
		}
	}
	else if (strstr(command, "GPS_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "GPS_PWR_EN %d", i);
		if (i == 0)
		{
			wcm_gpio_gps_pwr_en_off();
		}
		else
		{
			wcm_gpio_gps_pwr_en_on();
		}
	}
	else if (strstr(command, "COM_SW_A"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "COM_SW_A %d", i);
		if (i == 0)
		{
			wcm_gpio_com_sw_a_off();
		}
		else
		{
			wcm_gpio_com_sw_a_on();
		}
	}
	else if (strstr(command, "SAT_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "SAT_PWR_EN %d", i);
		if (i == 0)
		{
			wcm_gpio_sat_pwr_en_off();
		}
		else
		{
			wcm_gpio_sat_pwr_en_on();
		}
	}
	else if (strstr(command, "CELL_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "CELL_PWR_EN %d", i);
		if (i == 1)
		{
			wcm_gpio_cell_pwr_en_on();
		}
		else
		{
			wcm_gpio_cell_pwr_en_off();
		}
	}
	else if (strstr(command, "LGT_ON"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "LGT_ON %d", i);
		if (i == 0)
		{
			wcm_lgt_off();
		}
		else
		{
			wcm_lgt_on();
		}
	}
	else if (strstr(command, "WF_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "WF_PWR_EN %d", i);
		if (i == 0)
		{
				wcm_gpio_wf_pwr_en_off();
		}
		else
		{
				wcm_gpio_wf_pwr_en_on();
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
//	bool b;
	char *token;
	enum status_code status;
	int i;
	static char last_command[SPI_BUFFER_LENGTH] = {0x00};
	static uint8_t num_sent = 0;
	
	// Accelerometer
	double mc3416_angle;
	
	// GPS
	static bool data_received;

	// Power bits
	bool en_3v3va;
	static bool gps_pwr_en;
	static bool cell_pwr_en;
	static bool lgt_on;
	static bool sat_pwr_en;
	static bool wf_pwr_en;
	static bool com_sw_a;

	// LEAK
	float v;

	// MS5637
	double pressure;
	static double ms5637_temperature = 0.0;
	uint32_t d1;
	uint32_t d2;

	// Respond to valid commands
	if (strstr(command, "LEAK"))
	{
		strcpy(last_command, command);

		status = wcm_adc_read(&v);
		if (status == STATUS_OK)
		{
			sprintf(response, "%*.2f", spi_command_length, v);
		}
		else
		{
			wcm_usart_send_pc_message("handle_spi_command: Could not read leak detector!\r\n");
		}
	}
	else if (strstr(command, "read_gps"))
	{
		strcpy(last_command, command);
		status = STATUS_BUSY;
		while (status == STATUS_BUSY){
			status = wcm_usart_check_for_gps_data();
		}
		if (status == STATUS_OK)
		{
			data_received = wcm_usart_get_gps_data(command, COMMAND_LENGTH);
			if (data_received)
			{
				sprintf(response, COMMAND_LENGTH, command);
				num_sent = 1;
			}
		}
	}
	else if (strstr(command, "wcm_ping"))
	{
		strcpy(last_command, command);
		wcm_usart_send_pc_message("handle_command: Ping!\r\n");
		tc_set_count_value(&tc_instance, 0xFFFAA22C);
		timer_0_elapsed = true;
	}
	else if (strstr(command, "MS5637"))
	{
		strcpy(last_command, command);

		status = wcm_ms5637_read(&d1, &pressure, &d2, &ms5637_temperature);
		if (status == STATUS_OK)
		{
			sprintf(response, "%*.2f", spi_command_length, pressure);
			num_sent = 1;
		}
		else
		{
			wcm_usart_send_pc_message("handle_spi_command: Could not read MS5637!\r\n");
		}
	}
	else if (strstr(command, "read_mc3416"))
	{
		strcpy(last_command, command);	
	
		status = wcm_mc3416_read_tilt(&mc3416_angle);
		if (status == STATUS_OK)
		{
			sprintf(response, "%*.2f", spi_command_length, mc3416_angle);
			num_sent = 1;
		}
		else
		{
			wcm_usart_send_pc_message("handle_command: Could not read MC3416!\r\n");
		}
	}
	else if (strstr(command, "calibrate_mc3416"))
	{
		strcpy(last_command, command);	
		
		status = wcm_mc3416_calibrate();
		if (status != STATUS_OK)
		{
			wcm_usart_send_pc_message("handle_command: Could not calibrate MC3416!\r\n");
		}
	}
	else if (strstr(command, "zero_mc3416"))
	{
		strcpy(last_command, command);	
		
		status = wcm_mc3416_zero_offsets();
		if (status != STATUS_OK)
		{
			wcm_usart_send_pc_message("handle_command: Could not zero MC3416!\r\n");
		}
	}	
	else if (strstr(command, "POWER"))
	{
		strcpy(last_command, command);

		en_3v3va = wcm_gpio_3v3va_get();
		gps_pwr_en = wcm_gpio_gps_pwr_en_get();
		cell_pwr_en = wcm_gpio_cell_pwr_en_get();
		lgt_on = wcm_lgt_get();
		sat_pwr_en = wcm_gpio_sat_pwr_en_get();
		wf_pwr_en = wcm_gpio_wf_pwr_en_get();
		com_sw_a = wcm_gpio_com_sw_a_get();

		sprintf(response, "%*d",spi_command_length, (en_3v3va) ? 1 : 0);
		num_sent = 1;
	}
	else if (strstr(command, "RESP"))
	{
		if (strstr(last_command, "LEAK"))
		{
			sprintf(response, "--------");
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
				sprintf(response, "%*d",spi_command_length, (gps_pwr_en) ? 1 : 0);
				num_sent = 2;
			}
			else if (num_sent == 2)
			{
				sprintf(response, "%*d",spi_command_length, (cell_pwr_en) ? 1 : 0);
				num_sent = 3;
			}
			else if (num_sent == 3)
			{
				sprintf(response, "%*d",spi_command_length, (lgt_on) ? 1 : 0);
				num_sent = 4;
			}
			else if (num_sent == 4)
			{
				sprintf(response, "%*d",spi_command_length, (sat_pwr_en) ? 1 : 0);
				num_sent = 5;
			}
			else if (num_sent == 5)
			{
				sprintf(response, "%*d",spi_command_length, (wf_pwr_en) ? 1 : 0);
				num_sent = 6;
			}
			else if (num_sent == 6)
			{
				sprintf(response, "%*d",spi_command_length, (com_sw_a) ? 1 : 0);
				num_sent = 7;
			}
			else
			{
				sprintf(response, "--------");
			}
		}
	else if (strstr(command, "+3V3VA_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "+3V3VA_EN %d", i);
		if (i == 0)
		{
			wcm_gpio_3v3va_off();
		}
		else
		{
			wcm_gpio_3v3va_on();
		}
	}
	else if (strstr(command, "GPS_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "GPS_PWR_EN %d", i);
		if (i == 0)
		{
			wcm_gpio_gps_pwr_en_off();
		}
		else
		{
			wcm_gpio_gps_pwr_en_on();
		}
	}
	else if (strstr(command, "COM_SW_A"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "COM_SW_A %d", i);
		if (i == 0)
		{
			wcm_gpio_com_sw_a_off();
		}
		else
		{
			wcm_gpio_com_sw_a_on();
		}
	}
	else if (strstr(command, "SAT_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "SAT_PWR_EN %d", i);
		if (i == 0)
		{
			wcm_gpio_sat_pwr_en_off();
		}
		else
		{
			wcm_gpio_sat_pwr_en_on();
		}
	}
	else if (strstr(command, "CELL_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "CELL_PWR_EN %d", i);
		if (i == 0)
		{
			wcm_gpio_cell_pwr_en_off();
		}
		else
		{
			wcm_gpio_cell_pwr_en_on();
		}
	}
	else if (strstr(command, "WF_PWR_EN"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "WF_PWR_EN %d", i);
		if (i == 0)
		{
			wcm_gpio_wf_pwr_en_off();
		}
		else
		{
			wcm_gpio_wf_pwr_en_on();
		}
	}
	else if (strstr(command, "LGT_ON"))
	{
		token = strtok(command, " ");
		token = strtok(NULL, " ");
		i = atoi(token);
		sprintf(command, "LGT_ON %d", i);
		if (i == 0)
		{
			wcm_lgt_off();
		}
		else
		{
			wcm_lgt_on();
		}
	}
	else
	{
		wcm_usart_send_pc_message("handle_spi_command: Unknown command!\r\n");
	}
	}
}	// End of handle_spi_command


/****************************************************************************************
Local function to initialize the internal hardware
*****************************************************************************************/
static void initInternalHW(void)
{
	enum status_code status;

 	status = wcm_ms5637_init();
 	if (status == STATUS_OK)
 	{
 		wcm_usart_send_pc_message("initInternalHW: wcm_ms5637_init done\r\n");
 		wcm_usart_send_gps_command("initInternalHW: wcm_ms5637_init done\r\n");
 	}
 	else
 	{
 		wcm_usart_send_pc_message("initInternalHW: Could not initialize MS5637!\r\n");
 		wcm_usart_send_gps_command("initInternalHW: Could not initialize MS5637!\r\n");
 	}
	
	status = wcm_mc3416_init();
	if (status == STATUS_OK)
	{
		wcm_usart_send_pc_message("initInternalHW: wcm_mc3416_init done\r\n");
		wcm_usart_send_gps_command("initInternalHW: wcm_mc3416_init done\r\n");
	}
	else
	{
		wcm_usart_send_pc_message("initInternalHW: Could not initialize MC3416!\r\n");
		wcm_usart_send_gps_command("initInternalHW: Could not initialize MC3416!\r\n");
	}
	
}	// End of initInternalHW


/****************************************************************************************
Function to initialize the MMD WCM board
*****************************************************************************************/
void wcm_init(void)
{
	// Initialize the clocks, drivers, interfaces and interrupts
	wcm_power_normal_power_mode();
	
	wcm_adc_configure();
	wcm_bat_adc_configure();		
	wcm_i2c_configure();
/*
	wcm_interrupt_configure();
*/

	// Turn on WCM board and sensors
	wcm_gpio_3v3va_on();
	wcm_gpio_gps_pwr_en_on();
	

	
	//GPIO2 SET OFF for Testing
//	wcm_gpio_ext_gpio2_off();
	

	// Turn off everything else
//	wcm_gpio_gps_pwr_en_off();
	wcm_gpio_cell_pwr_en_off();	
	wcm_gpio_wf_pwr_en_off();
	wcm_gpio_com_sw_a_off();
	wcm_gpio_sat_pwr_en_off();
	wcm_lgt_off();

	initInternalHW();

}	// End of wcm_init


/****************************************************************************************
Function to run the MMD WCM board operations
*****************************************************************************************/
void wcm_run(void)
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
	// 	Subtract from max value to get number of clk cycles to load in timer; 2^32 - 351700 = 0xFFFAA22C 
	config_tc.counter_32_bit.value = 0xFFFAA22C;
	tc_init(&tc_instance, CONF_TC_MODULE, &config_tc);
	tc_enable(&tc_instance);
		
	tc_register_callback(&tc_instance, tc_callback_to_read_mc3416,
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

	wcm_usart_send_pc_message("wcm_run: started\r\n");
	wcm_usart_send_gps_command("wcm_run: started\r\n");
	
	bSPIInitialized = false;
	i = 0;
	
	while (1)
	{	
		
		timer_0_elapsed = false;

		while (timer_0_elapsed == false)		
		{
			if (bSPIInitialized == false)
			{
				// Slave select is high when SPI master (MMD) is on
				b = wcm_gpio_spi_slave_select_get();
				if (b == true)
				{
					for (i = 0; i < 8; i++)
					{
						spi_tx_buffer[i] = '-';
					}

					wcm_spi_configure(MODE_ENABLED);

					retval = wcm_spi_start_read(spi_tx_buffer, spi_rx_buffer, spi_command_length);
					if (retval == STATUS_OK)
					{
						bSPIInitialized = true;

						wcm_usart_send_pc_message("wcm_run: SPI initialized\r\n");
					}
					else
					{
						wcm_usart_send_pc_message("wcm_run: wcm_spi_start_read failed (1)!\r\n");
					}
				}
			}
			// Check for an SPI command
			if (bSPIInitialized == true)
			{
				if (wcm_spi_transfer_complete())
				{
					delay_ms(10);

					// Handle the SPI buffer data
					spi_rx_buffer[spi_command_length] = '\0';
					handle_spi_command((char *)spi_rx_buffer, (char *)spi_tx_buffer);

					// Start another SPI read
					retval = wcm_spi_start_read(spi_tx_buffer, spi_rx_buffer, spi_command_length);
					if (retval != STATUS_OK)
					{
						bSPIInitialized = false;

						wcm_usart_send_pc_message("wcm_run: wcm_spi_start_read failed (2)!\r\n");
					}
				}
			}
		
			// Check for a serial command
			retval = wcm_usart_check_for_pc_command();
			if (retval == STATUS_OK)
			{
				bCommandReceived = wcm_usart_get_pc_command(command, COMMAND_LENGTH);
				if (bCommandReceived)
				{
					bValid = handle_command(command);
					wcm_usart_send_pc_message(command);
					wcm_usart_send_pc_message(" ");
					if (bValid)
					{
						wcm_usart_send_pc_message("VALID\r\n");
					}
					else
					{
						wcm_usart_send_pc_message("INVALID\r\n");
					}
				}
			}
		}

  
//   		wcm_usart_send_pc_message("wcm_run: entering sleep mode\r\n");	
//   		wcm_power_configure_wakeup_en();
//    	  	wcm_power_low_power_mode();
//    	  	wcm_power_normal_power_mode();
//    	  	wcm_usart_send_pc_message("wcm_run: exiting sleep mode\r\n");
	}
	
}	// End of wcm_run


