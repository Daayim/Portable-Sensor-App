/****************************************************************************************
wcm_usart.h: Include file for wcm_usart.c

Written by:
	Daayim Asim, B.Eng. Student
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022
*****************************************************************************************/


#ifndef WCM_USART_H
#define WCM_USART_H


#include <stdbool.h>


enum status_code wcm_usart_check_for_pc_command(void);
enum status_code wcm_usart_check_for_gps_data(void);
enum status_code wcm_usart_check_for_com_data(void);


void wcm_usart_configure(void);
void wcm_usart_disable(void);

bool wcm_usart_get_pc_command(char *, int);
bool wcm_usart_get_gps_data(char *, int);
bool wcm_usart_get_com_data(char *, int);

void wcm_usart_send_pc_message(const char *);
void wcm_usart_send_gps_command(const char *);
void wcm_usart_send_com_command(const char *);


#endif	// WCM_USART_H


