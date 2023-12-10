/****************************************************************************************
pm_adc.c:   power module (PM) ADC functions

Written by:
	Daayim Asim, B.Eng. 
	Computer Engineer Student
	 

Date:
	February 2022
*****************************************************************************************/


#include <adc.h>
#include "pm_adc.h"


/****************************************************************************************
Local variable(s)
*****************************************************************************************/

static struct adc_module adc_module_struct;


/****************************************************************************************
Function to configure the Main PM ADC module

See https://asf.microchip.com/docs/latest/saml21/html/asfdoc_sam0_adc_basic_use_case.html
*****************************************************************************************/
void pm_adc_configure(void)
{
	static bool bFirst = true;
	
	struct adc_config adc_config_struct;
	
	adc_get_config_defaults(&adc_config_struct);
	
	adc_config_struct.positive_input = ADC_POSITIVE_INPUT_PIN0;
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
	
}	// End of pm_adc_configure


/****************************************************************************************
Function to read an ADC value
*****************************************************************************************/
enum status_code pm_adc_read(float *v)
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
	
}	// End of pm_adc_read


