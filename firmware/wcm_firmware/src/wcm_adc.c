/****************************************************************************************
wcm_adc.c: Marine Mammal Detection (MMD) Wireless Communication Module (WCM) ADC functions

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022
*****************************************************************************************/


#include <adc.h>
#include "wcm_adc.h"


/****************************************************************************************
Local variable(s)
*****************************************************************************************/

static struct adc_module adc_module_struct;
static struct adc_module battery_adc_module_struct;


/****************************************************************************************
Function to configure the MMD WCM ADC module

See https://asf.microchip.com/docs/latest/saml21/html/asfdoc_sam0_adc_basic_use_case.html
*****************************************************************************************/
void wcm_adc_configure(void)
{
	static bool bFirst = true;
	
	struct adc_config adc_config_struct;
	
	adc_get_config_defaults(&adc_config_struct);
	
	adc_config_struct.positive_input = ADC_POSITIVE_INPUT_PIN7;
	adc_config_struct.reference = ADC_REFERENCE_INTVCC2;
	
	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		adc_disable(&adc_module_struct);
	}
	
	adc_init(&adc_module_struct, ADC, &adc_config_struct);
	
	adc_enable(&adc_module_struct);
	
}	// End of wcm_adc_configure

/****************************************************************************************
Function to configure adc for bat
*****************************************************************************************/

void wcm_bat_adc_configure(void)
{
	static bool bFirst = true;
	
	struct adc_config bat_adc_config_struct;
	
	adc_get_config_defaults(&bat_adc_config_struct);
	bat_adc_config_struct.sample_length = 64;
	bat_adc_config_struct.clock_prescaler = ADC_CLOCK_PRESCALER_DIV32;
	
	bat_adc_config_struct.positive_input = ADC_POSITIVE_INPUT_PIN19;
	bat_adc_config_struct.reference = ADC_REFERENCE_INTVCC2;
	
	if (bFirst)
	{
		bFirst = false;
	}
	else
	{
		adc_disable(&battery_adc_module_struct);
	}
	
	adc_init(&battery_adc_module_struct, ADC, &bat_adc_config_struct);
	
	adc_enable(&battery_adc_module_struct);
}

/****************************************************************************************
Function to read an ADC value
*****************************************************************************************/
enum status_code wcm_adc_read(float *v)
{
	uint16_t result;
	enum status_code status;
	
	adc_start_conversion(&adc_module_struct);
	
	while (1)
	{
		status = adc_read(&adc_module_struct, &result);
		if (status == STATUS_BUSY)
		{
			continue;
		}
		else
		{
			if (status == STATUS_OK)
			{
				*v = 3.3 * (float)result / 4095.0;
			}
			break;
		}
	}
	
	return (status);
	
}	// End of wcm_adc_read

/****************************************************************************************
Function to read an ADC value for battery detector
*****************************************************************************************/
enum status_code wcm_bat_adc_read(float *v)
{
	uint16_t result;
	enum status_code status;
	
	adc_start_conversion(&battery_adc_module_struct);
	
	while (1)
	{
		status = adc_read(&battery_adc_module_struct, &result);
		if (status == STATUS_BUSY)
		{
			continue;
		}
		else
		{
			if (status == STATUS_OK)
			{
				*v = 3.3 * (float)result / 4095.0;
			}
			break;
		}
	}
	
	return (status);
	
}	// End of wcm_bat_adc_read


