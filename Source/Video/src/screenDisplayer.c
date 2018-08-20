/*
 * screenDisplayer.c
 *
 *  Created on: 7 Aug 2018
 *      Author: Colin Cina
 */

#include "screenDisplayer.h"

char rgb[60];



void screenDisplayerThread(void const *argument)
{
	osEvent rectColorEvent;
	colorPacketTypedef* colors;
	for(;;)
	{
		rectColorEvent = osMailGet(displayQueue, osWaitForever);
		if(rectColorEvent.status == osEventMail)
		{
			colors = (colorPacketTypedef*)rectColorEvent.value.p;
			sprintf(rgb, "r:%d g:%d b:%d", (uint8_t)(colors->meanRGB >> 16), (uint8_t)(colors->meanRGB >> 8), (uint8_t)colors->meanRGB);
			BSP_LCD_Clear(colors->meanRGB);
			BSP_LCD_DisplayStringAt(100, 100, rgb, LEFT_MODE);
			osMailFree(displayQueue, colors);
		}
	}
}
