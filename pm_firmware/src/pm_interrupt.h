/****************************************************************************************
pm_interrupt.h: Include file for pm_interrupt.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021
*****************************************************************************************/


#ifndef PM_INTERRUPT_H
#define PM_INTERRUPT_H


void pm_interrupt_configure(void);

bool pm_interrupt_e_prs_eoc_occurred(void);
void pm_interrupt_e_prs_eoc_clear(void);

bool pm_interrupt_pwr_fault_occurred(void);
void pm_interrupt_pwr_fault_clear(void);

bool pm_interrupt_three_d_occurred(void);
void pm_interrupt_three_d_clear(void);


#endif	// PM_INTERRUPT_H


