/*
 * screenDisplayer.c
 *
 *  Created on: 7 Aug 2018
 *      Author: Colin Cina
 */

#include "screenDisplayer.h"

colorPacketTypedef* colors;

void screenDisplayerThread(void const *argument)
{
	for(;;)
	{
		rectColorEvent = osMailGet(displayQueue, osWaitForever);
		if(rectColorEvent.status == osEventMail)
		{
			colors = (colorPacketTypedef*)rectColorEvent.value.p;
			BSP_LCD_SetTextColor(0);
			sprintf((char*)rgb, "r:%d g:%d b:%d", (uint8_t)(colors->meanRGB >> 16), (uint8_t)(colors->meanRGB >> 8), (uint8_t)colors->meanRGB);
			BSP_LCD_DisplayStringAt(330, 100, rgb, LEFT_MODE);
			BSP_LCD_SetTextColor(colors->meanRGB);
			BSP_LCD_FillRect(300, 140, 200, 200);
			osMailFree(displayQueue, colors);
		}
	}
}
