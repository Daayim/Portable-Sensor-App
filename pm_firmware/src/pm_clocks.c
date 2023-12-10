/****************************************************************************************
pm_clocks.c:   power module (PM) clock functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	February 2022
*****************************************************************************************/


#include <clock.h>
#include "pm_clocks.h"
#include "pm_config_codes.h"

/****************************************************************************************
Local variable(s)
*****************************************************************************************/

// System Sleep Mode Config
static const uint8_t lowpower_mode = 1;

// External crystal frequency
static const uint32_t crystal_frequency = 12000000ul;


/****************************************************************************************
Function to configure the clocks

See https://asf.microchip.com/docs/latest/saml21/html/asfdoc_sam0_system_clock_basic_use_case.html
*****************************************************************************************/
void pm_clocks_configure(uint8_t mode)
{
	struct system_clock_source_xosc_config xosc_config_struct;
	struct system_gclk_gen_config gclk_gen_config_struct;

	system_flash_set_waitstates(2);

	// XOSC
	system_clock_source_xosc_get_config_defaults(&xosc_config_struct);

	xosc_config_struct.auto_gain_control = false;
	xosc_config_struct.external_clock    = SYSTEM_CLOCK_EXTERNAL_CRYSTAL;
	xosc_config_struct.frequency         = crystal_frequency;
	xosc_config_struct.on_demand         = true;
	xosc_config_struct.run_in_standby    = false;
	xosc_config_struct.startup_time      = SYSTEM_XOSC_STARTUP_16384;

	system_clock_source_xosc_set_config(&xosc_config_struct);
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC);

	// GCLK generator 0 (Main Clock)
	system_gclk_gen_get_config_defaults(&gclk_gen_config_struct);

	gclk_gen_config_struct.division_factor    = 1;
	gclk_gen_config_struct.high_when_disabled = false;
	gclk_gen_config_struct.output_enable      = false;
	gclk_gen_config_struct.run_in_standby     = false;
	if (mode == MODE_LOWPOWER){
		gclk_gen_config_struct.source_clock   = SYSTEM_CLOCK_SOURCE_ULP32K;}			
	else{
		gclk_gen_config_struct.source_clock   = SYSTEM_CLOCK_SOURCE_XOSC;}

	system_gclk_gen_set_config(GCLK_GENERATOR_0, &gclk_gen_config_struct);
	system_gclk_gen_enable(GCLK_GENERATOR_0);

}	// End of pm_clocks_configure