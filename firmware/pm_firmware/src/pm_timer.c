 /****************************************************************************************
pm_mc3416.c:   power management (PM) Timer functions

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	October 2022

Note(s):
- Timer usesd for pm_run wakeup length
*****************************************************************************************/

#include "pm_timer.h"
#include <tc.h>
#include <tc_interrupt.h>


#define CONF_TC_MODULE TC0
struct tc_module tc_instance;
volatile static bool timer_0_elapsed = false;

/***************************************************************************
Local functions to configure TC for real time MC3416 measurements
****************************************************************************/

void pm_timer_tc_callback_to_sleep(
struct tc_module *const module_inst)
{
	tc_set_count_value(&tc_instance, 0xFFFAA22C);
	timer_0_elapsed = true;
}


void pm_timer_config(void){
	
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
	
	tc_register_callback(&tc_instance, tc_callback_to_read_mc3416,
	TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
	
}