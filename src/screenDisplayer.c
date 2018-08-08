/*
 * screenDisplayer.c
 *
 *  Created on: 7 Aug 2018
 *      Author: Colin Cina
 */

#include "screenDisplayer.h"

void screenDisplayerThread(void const *argument)
{
	osEvent rectColorEvent;
	uint32_t* rectColor;
	for(;;)
	{
		rectColorEvent = osMailGet(displayQueue, osWaitForever);
		if(rectColorEvent.status == osEventMail)
		{
			rectColor = (uint32_t*)rectColorEvent.value.p;
			BSP_LCD_Clear(*rectColor);
			osMailFree(displayQueue, rectColor);
		}
	}
}
