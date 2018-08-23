/*
 * screenDisplayer.c
 *
 *  Created on: 7 Aug 2018
 *      Author: Colin Cina
 */

#include "screenDisplayer.h"

uint8_t rgb[60];



void screenDisplayerThread(void const *argument)
{
	osEvent rectColorEvent;
	colorPacketTypedef* colors;
	for(;;)
	{
		rectColorEvent = osMailGet(displayQueue, osWaitForever);
		if(rectColorEvent.status == osEventMail)
		{
//			BSP_LCD_Clear(0xFF808080);
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
