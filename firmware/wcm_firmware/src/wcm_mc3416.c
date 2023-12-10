 /****************************************************************************************
wcm_mc3416.c: Marine Mammal Detection (MMD) Wireless Communication Module (WCM) MC3416
	pressure / temperature sensor functions

Written by:
	Daayim Asim, Computer Engineer Student.
	Work Term Student
	eSonar Inc.

Date:
	September 2022

Note(s):
- The Memsic Inc. Accelerometer part number is MC3416.
- It's slave address is 1001100.
*****************************************************************************************/
 
 //#include <driver_init.h>
 #include <stdint.h>
 #include <math.h>
 #include "wcm_i2c.h"
 #include "wcm_mc3416.h"
 #include "wcm_usart.h"
 #include "wcm_eeprom.h"
 #include "status_codes.h"
 #include "delay.h"
 
 
/****************************************************************************************
Local variable(s)
*****************************************************************************************/

//static const uint16_t mc3416_address = MC3416_ADDRESS;
static const uint16_t mc3416_address = MC3416_ADDRESS;

//static const uint32_t address_length = I2C_M_SEVEN;


static int16_t	xout;
static int16_t	yout;
static int16_t	zout;

//Values used for calibration initialize to zero
static uint16_t x_offset = 0;
static uint16_t y_offset = 0;
static uint16_t z_offset = 0;

static double xg;
static double yg;
static double zg;

static double x2;
static double y2;
static double z2;
// convert radians to degrees
static const double radian_to_degrees = MC3416_RAD_TO_DEG;

// resolution g/bit, 2g/2^15
static const double resolution = MC3416_RANGE_RES_2G;
// Y axis reference orientation is vertical 1G 
static const uint16_t y_ref_value = 16384;

static const uint16_t mc3416_wakeup_delay_ms = 1000;

/****************************************************************************************
 Local function(s)
*****************************************************************************************/
enum status_code mc3416_set_mode(uint8_t);
enum status_code mc3416_check_mode(void);
enum status_code mc3416_set_sampling_rate(void);
enum status_code mc3416_set_range_resolution(void);
//static int mc3416_reset(uint8_t);
enum status_code mc3416_validate_chip(void);
enum status_code mc3416_read_axis(void);
static void mc3416_convert_to_g(void);
enum status_code mc3416_flash_read_offset(void);



/****************************************************************************************
Local function to set the Mode (STANDBY or WAKE) of the MC3416 device
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code mc3416_set_mode(uint8_t mode)
{
	enum status_code status;
	
	uint8_t b_mode = mode;
	uint16_t wr_length = 0x02;
	
	status = wcm_i2c_command_write_reg(mc3416_address, MC3416_REG_MODE, &b_mode, wr_length);
	
	if((b_mode & MC3416_MODE_WAKE) ==  MC3416_MODE_WAKE)
	{
		delay_ms(mc3416_wakeup_delay_ms);
		wcm_usart_send_pc_message("wakeup delay finished!\r\n");
	}
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_set_mode: write_failed!\r\n");
	}
	return (status);
}	//	End of mc3416_set_mode

/****************************************************************************************
Local function to check the Mode (STANDBY or WAKE) of the MC3416 device
Returns AWAKE(0) if in WAKE state, SLEEP(-1) if in STANDBY and ERROR (1) if read failed 
*****************************************************************************************/
enum status_code mc3416_check_mode(void)
{
	enum status_code status;
	
	uint8_t b_mode;
	uint8_t rd_length = 0x01;
	uint8_t wr_length = 0x01;
	
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length , MC3416_REG_DEVICE_STATUS, &b_mode, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_check_mode: read_failed!\r\n");
		return (status);
	}
	b_mode = (b_mode & MC3416_STATE_MASK);
	
	if(b_mode == MC3416_MODE_STANDBY)
	{
		status = STATUS_BUSY;
		wcm_usart_send_pc_message("mc3416_check_mode: STANDBY_MODE!\r\n");
	}
	return (status);
}	//	End of mc3416_check_mode


/****************************************************************************************
Local function to set the output data rate of the MC3416 device
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code mc3416_set_sampling_rate(void)
{
	enum status_code status;
	
	uint8_t b_odr = 0x00;
	uint8_t rd_length = 0x01;
	uint8_t wr1_length = 0x01;
	uint8_t wr2_length = 0x02;
	
	status = wcm_i2c_command_read_reg(mc3416_address, wr1_length, MC3416_REG_SAMPLE_RATE, &b_odr, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_set_sampling_rate: read_failed!\r\n");
		return (status);
	}
	b_odr = ((b_odr & MC3416_ODR_MASK) | MC3416_ODR);
	status = wcm_i2c_command_write_reg(mc3416_address, MC3416_REG_SAMPLE_RATE, &b_odr, wr2_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_set_sampling_rate: write_failed!\r\n");
	}
	
	return (status);
}	//	End of mc3416_set_sampling rate

/****************************************************************************************
Local function to set the G range of the MC3416 device
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code mc3416_set_range_resolution(void)
{
	enum status_code status;
	
	uint8_t b_range = 0x00;
	uint8_t wr2_length = 0x02;
	uint8_t wr1_length = 0x01;
	uint8_t rd_length = 0x01;
	
	status = wcm_i2c_command_read_reg(mc3416_address, wr1_length, MC3416_REG_RANGE, &b_range, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_set_range_resolution: read_failed!\r\n");
		return (status);
	}
	
	b_range = ((b_range & MC3416_RANGE_MASK) | MC3416_RANGE_REG_VAL);
	status = wcm_i2c_command_write_reg(mc3416_address, MC3416_REG_RANGE, &b_range, wr2_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_set_range_resolution: write_failed!\r\n");
	}
	
	return (status);
}	//	End of mc3416_set_range_resolution

/****************************************************************************************
Local function to check a valid Chip ID and Product Code for MC3416 device
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code mc3416_validate_chip(void)
{
	enum status_code status;
	uint8_t b_chipid = 0x00;
	uint8_t b_pcode = 0x00;
	uint8_t rd_length = 0x01;
	uint8_t wr_length = 0x01;
	
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length, MC3416_REG_CHIPID, &b_chipid, rd_length);
	
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_validate_chip: chipID_read_failed!\r\n");
		return (status);
	}
	
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length, MC3416_REG_PRODUCT_CODE_L, &b_pcode, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_validate_chip: Pcode_read_failed!\r\n");
		return (status);
	}
	
	b_chipid = b_chipid & 0xF0;
	b_pcode = b_pcode & 0xF1;
	if(b_chipid != MC3416_CHIPID)
	{
		wcm_usart_send_pc_message("mc3416_validate_chip: Chip_ID_incorrect!\r\n");
		return MC3416_ERROR;
	}

	if(b_pcode != MC3416_PCODE)
	{
		wcm_usart_send_pc_message("mc3416_validate_chip: Product_Code_incorrect!\r\n");
		return MC3416_ERROR;
	}
	
	return (status);
}	//	End of mc3416_validate_chip

/****************************************************************************************
Local function to sample the three axis of the MC3416 device
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code mc3416_read_axis(void)
{
	enum status_code status;
	uint8_t b_xl;
	uint8_t b_xh;
	uint8_t b_yl;
	uint8_t b_yh;
	uint8_t b_zl;
	uint8_t b_zh;
	
	uint8_t rd_length = 0x01;
	uint8_t wr_length = 0x01;

	
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length, MC3416_REG_XOUT_EX_L, &b_xl, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_sample_axis: axis_read_failed!\r\n");
	}
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length,MC3416_REG_XOUT_EX_H, &b_xh, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_sample_axis: axis_read_failed!\r\n");
	}
	xout = (int16_t)((uint16_t)b_xh << 8 | b_xl);
	
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length, MC3416_REG_YOUT_EX_L, &b_yl, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_sample_axis: axis_read_failed!\r\n");
	}
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length, MC3416_REG_YOUT_EX_H, &b_yh, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_sample_axis: axis_read_failed!\r\n");
	}
	yout = (int16_t)((uint16_t)b_yh << 8 | b_yl);
	
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length, MC3416_REG_ZOUT_EX_L, &b_zl, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_sample_axis: axis_read_failed!\r\n");
	}
	status = wcm_i2c_command_read_reg(mc3416_address, wr_length, MC3416_REG_ZOUT_EX_H, &b_zh, rd_length);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("mc3416_sample_axis: axis_read_failed!\r\n");
	}
	zout = (int16_t)((uint16_t)b_zh << 8 | b_zl);
	
	return (status); //return error code or status?
	
	
}

/****************************************************************************************
Local function to sample the three axis of the MC3416 device

*****************************************************************************************/
static void mc3416_convert_to_g(void)
{
	xout -= x_offset;
	yout -= y_offset;
	zout -= z_offset;
	xg = (double)xout * resolution;
	yg = (double)yout * resolution;
	zg = (double)zout * resolution;
	
	x2 = xg * xg;
	y2 = yg * yg;
	z2 = zg * zg;
}	//	End of mc3416_convert_to_g

/****************************************************************************************
Local function to read the offset values from Flash memory
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code mc3416_flash_read_offset(void)
{
	enum status_code status;
	
	eeprom_read_settings(&x_offset, &y_offset, &z_offset); //Pass address
	
	status = STATUS_OK;
	
	return (status);
}
/****************************************************************************************
Function to initialize the MC3416 Accelerometer
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code wcm_mc3416_init(void)
{
	enum status_code status;
	
	eeprom_configure(&x_offset, &y_offset, &z_offset); //Correct configuration??
	
	status = mc3416_validate_chip();
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_init: no_chip_detected!\r\n");
		return (status);
	}
	status = mc3416_set_mode(MC3416_MODE_STANDBY);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_init: standby_mode_not_entered!\r\n");
		return (status);
	}
	status = mc3416_set_sampling_rate();
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_init: Sampling_rate_not_configured!\r\n");
		return (status);
	}
	status = mc3416_set_range_resolution();
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_init: Range_res_not_configured!\r\n");
		return (status);
	}
	
	status = mc3416_flash_read_offset();
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_init: Offset_reads_failed!\r\n");
		return (status);
	}
	status = mc3416_set_mode(MC3416_MODE_WAKE);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_init: wake_mode_not_entered!\r\n");
		return (status);
	}
	
	return (status);	
}	//	End of vbs_m3416_init

/****************************************************************************************
Function to read the MC3416 Accelerometer
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code wcm_mc3416_read_tilt(double *tilt_arg)
{
	
	enum status_code status;
	double tilt_angle;
	status = mc3416_check_mode();
	if(status != STATUS_BUSY && status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_read_tilt: mode_check_failed!\r\n");
		return (status);
	}
	else if(status == STATUS_BUSY)
	{
		status = mc3416_set_mode(MC3416_MODE_WAKE);
		if(status != STATUS_OK)
		{
			wcm_usart_send_pc_message("wcm_mc3416_read_tilt: Wakeup_failed!\r\n");
			return (status);
		}
	}
	status = mc3416_read_axis();
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_read_tilt: Sample_axis_failed!\r\n");
		return (status);
	}
	
	mc3416_convert_to_g();

	tilt_angle = radian_to_degrees * acos(yg / sqrt(x2 + y2 + z2));
	
	*tilt_arg = tilt_angle;
	
	return (status);
	
}	//	End of wcm_mc3416_read_tilt

/****************************************************************************************
Function to put the MC3416 into Standby mode
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code wcm_mc3416_standby(void)
{
	enum status_code status;
	
	status = mc3416_set_mode(MC3416_MODE_STANDBY);
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_standby: failed to enter standby mode!\r\n");
	}
	return (status);
}	//	End of vbs_mc3416_standby

/****************************************************************************************
Function to return the MC3416 axis count values
*****************************************************************************************/
void wcm_mc3416_get_counts(int16_t *x_arg, int16_t *y_arg, int16_t *z_arg)
{
	*x_arg = xout;
	*y_arg = yout;
	*z_arg = zout; 
}	// End of vbs_mc3416_get_counts

/****************************************************************************************
Function to return the MC3416 gravity values
*****************************************************************************************/
void wcm_mc3416_get_g_values( double *xg_arg, double *yg_arg, double *zg_arg)
{
	*xg_arg = xg;
	*yg_arg = yg;
	*zg_arg = zg; 
}	// End of vbs_mc3416_get_g_values

/****************************************************************************************
Function to read the MC3416 Accelerometer and save values for offset adjustment
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code wcm_mc3416_calibrate(void)
{
	enum status_code status;
	status = mc3416_check_mode();
	if(status != STATUS_BUSY && status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_calibrate: mode_check_failed!\r\n");
		return (status);
	}
	else if(status == STATUS_BUSY)
	{
		status = mc3416_set_mode(MC3416_MODE_WAKE);
		if(status != STATUS_OK)
		{
			wcm_usart_send_pc_message("wcm_mc3416_calibrate: Wakeup_failed!\r\n");
			return (status);
		}
	}
	status = mc3416_read_axis();
	if(status != STATUS_OK)
	{
		wcm_usart_send_pc_message("wcm_mc3416_calibrate: Read_axis_failed!\r\n");
		return (status);
	}

	yout = yout - y_ref_value; // y_offset is referenced to 1G due to orientation
	
	x_offset = xout;
	y_offset = yout;
	z_offset = zout;
	
	eeprom_write_settings(x_offset,  y_offset,  z_offset);
	
	return (status);
}	//	End of vbs_mc3416_calibrate

/****************************************************************************************
Function to set the Offset values to 0
Returns status code indicating success or failure 
*****************************************************************************************/
enum status_code wcm_mc3416_zero_offsets(void)
{
	enum status_code status;
	//write zeros to offsets in flash
	
	x_offset = 0;
	y_offset = 0;
	z_offset = 0;
	
	eeprom_write_settings( x_offset,  y_offset,  z_offset);
	
	status = STATUS_OK;
	
	return (status);
}	//	End of vbs_mc3416_zero_offsets

/****************************************************************************************
Function to return the calibrated offsets for the MC3416 Accelerometer
*****************************************************************************************/
void wcm_mc3416_get_offsets(int16_t * xo_arg, int16_t *yo_arg, int16_t *zo_arg)
{
	*xo_arg = x_offset;
	*yo_arg = y_offset;
	*zo_arg = z_offset;
	
}	//	End of vbs_mc3416_get_offsets
