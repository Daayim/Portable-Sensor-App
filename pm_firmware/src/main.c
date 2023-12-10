/****************************************************************************************
main.c: Power module (PM) firmware

Written by:
	Daayim Asim, B.Eng.
	Computer Engineer Student

Date:
	November 2021

Note(s):

March 2022:
- New board (POWER_MANAGER1A)
*****************************************************************************************/


#include <system.h>
#include "pm.h"


/****************************************************************************************
Application main function
*****************************************************************************************/
int main (void)
{
	system_init();
	
	pm_init();
	pm_run();
	
}	// End of main


