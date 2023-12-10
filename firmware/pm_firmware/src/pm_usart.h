/****************************************************************************************
pm_usart.h: Include file for pm_usart.c

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student
	 

Date:
	November 2021
*****************************************************************************************/


#ifndef PM_USART_H
#define PM_USART_H


#include <stdbool.h>


enum status_code pm_usart_check_for_pc_command(void);

void pm_usart_configure(void);
void pm_usart_disable(void);

bool pm_usart_get_pc_command(char *, int);

void pm_usart_send_pc_message(const char *);
void pm_usart_send_vbs_command(const char *);


#endif	// PM_USART_H


