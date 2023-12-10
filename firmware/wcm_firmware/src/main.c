/****************************************************************************************
main.c: Marine Mammal Detection (MMD) power module (WCM) firmware

Written by:
	Daayim Asim, B.Eng. 
	Computer Engineer Student
	eSonar Inc.

Date:
	November 2022

Note(s):

November 2022:
- New board (WIRELESS_COMMUNICATION_MODULE1A)
- Based of Power Manager Rev 1
*****************************************************************************************/



#include <system.h>
#include "wcm.h"

/****************************************************************************************
Application main function
*****************************************************************************************/

int main (void)
{
	system_init();
	
	wcm_init();
	wcm_run();

}	// End of main
