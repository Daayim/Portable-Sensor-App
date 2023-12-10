/****************************************************************************************
pm_mc3416.h: Include file for pm_mc3416.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	September 2022
*****************************************************************************************/


#ifndef PM_MC3614_H_
#define PM_MC3614_H_

/*******************************************
 Sample Rates for MC3416
*******************************************/
#define MC3416_ODR_128				(0x00)
#define MC3416_ODR_256				(0x01)
#define MC3416_ODR_512				(0x02)
#define MC3416_ODR_1024				(0x05)
/*******************************************
 G Range Register values for MC3416
*******************************************/
#define MC3416_REG_RANGE_2G				(0x09)
#define MC3416_REG_RANGE_4G				(0x19)
#define MC3416_REG_RANGE_8G				(0x29)
#define MC3416_REG_RANGE_16G			(0x39)
#define MC3416_REG_RANGE_12G			(0x49)

/***********************************************
 REGISTER MAP
***********************************************/

#define MC3416_REG_STATUS_1 						0x03
#define MC3416_REG_INTR_STAT_1						0x04
#define MC3416_REG_DEVICE_STATUS					0x05
#define MC3416_REG_INTERRUPT_ENABLE    				0x06
#define MC3416_REG_MODE			           			0x07
#define MC3416_REG_SAMPLE_RATE             			0x08
#define MC3416_REG_MOTION_CTRL		   				0x09

#define MC3416_REG_XOUT_EX_L               			0x0D
#define MC3416_REG_XOUT_EX_H		            	0x0E
#define MC3416_REG_YOUT_EX_L        		    	0x0F
#define MC3416_REG_YOUT_EX_H               			0x10
#define MC3416_REG_ZOUT_EX_L		            	0x11
#define MC3416_REG_ZOUT_EX_H        		    	0x12
#define MC3416_REG_STATUS_2 						0x13
#define MC3416_REG_INTR_STAT_2						0x14
#define MC3416_REG_SDM_X							0x15

#define MC3416_REG_CHIPID							0x18
#define MC3416_REG_RANGE		          			0x20
#define MC3416_REG_PRODUCT_CODE_H      				0x34
#define MC3416_REG_PRODUCT_CODE_L      				0x3B
    
                                          		
#define MC3416_REG_TF_THRESHOLD_LSB 				0x40
#define MC3416_REG_TF_THRESHOLD_MSB 				0x41
#define MC3416_REG_TF_DB							0x42
#define MC3416_REG_AM_THRESHOLD_LSB 				0x43
#define MC3416_REG_AM_THRESHOLD_MSB 				0x44
#define MC3416_REG_AM_DB							0x45
#define MC3416_REG_SHK_THRESHOLD_LSB				0x46
#define MC3416_REG_SHK_THRESHOLD_MSB				0x47
#define MC3416_REG_PK_P2P_DUR_THRESHOLD_LSB 		0x48
#define MC3416_REG_PK_P2P_DUR_THRESHOLD_MSB 		0x49
#define MC3416_REG_TIMER_CTRL						0x4A

/***********************************************
 MODE
***********************************************/
#define MC3416_MODE_STANDBY    (0x00)
#define MC3416_MODE_WAKE       (0x01)
/**********************************************
 Configuration
***********************************************/
#define MC3416_ODR					MC3416_ODR_128
#define MC3416_RANGE_REG_VAL		MC3416_REG_RANGE_2G

#define MC3416_RANGE_RES_2G				0.00006103515625	// 2g / 2^15
#define MC3416_RANGE_RES_4G				0.0001220703125		// 4g / 2^15
#define MC3416_RANGE_RES_8G				0.000244140625		// 8g / 2^15
#define MC3416_RANGE_RES_16G			0.00048828125		// 16g / 2^15
#define MC3416_RANGE_RES_12G			0.0003662109376		// 12g / 2^15

#define MC3416_RAD_TO_DEG				57.29577951		// 180 / pi

#define MC3416_CHIPID				0xA0
#define MC3416_PCODE				0x20
#define MC3416_ADDRESS				0x4C
#define MC3416_WRITE_ADDRESS		0x98
#define MC3416_READ_ADDRESS			0x99

#define	MC3416_STATE_MASK			0x03
#define MC3416_ODR_MASK				0xF8
#define MC3416_RANGE_MASK			0x80

#define MC3416_ERROR				1

enum status_code pm_mc3416_init(void);
enum status_code pm_mc3416_read_tilt(double *);
enum status_code pm_mc3416_standby(void);
void pm_mc3416_get_counts(int16_t*, int16_t*, int16_t*);
void pm_mc3416_get_g_values( double *, double *, double *);
enum status_code pm_mc3416_calibrate(void);
enum status_code pm_mc3416_zero_offsets(void);
void pm_mc3416_get_offsets(int16_t*, int16_t*, int16_t*);





#endif /* VBS_MC3614_H_ */

