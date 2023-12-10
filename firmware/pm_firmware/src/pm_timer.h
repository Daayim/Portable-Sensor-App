/****************************************************************************************
pm_timer.h: Include file for pm_timer.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student

Date:
	October 2022
*****************************************************************************************/


#ifndef PM_TIMER_H_
#define PM_TIMER_H_

void pm_timer_config(void);

void pm_timer_tc_callback_to_sleep(struct tc_module *const module_inst);

#endif /* PM_TIMER_H_ */