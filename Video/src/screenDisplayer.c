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
			BSP_LCD_Clear(0xFF808080);
			colors = (colorPacketTypedef*)rectColorEvent.value.p;
			sprintf(rgb, "r:%d g:%d b:%d", (uint8_t)(colors->meanRGB >> 16), (uint8_t)(colors->meanRGB >> 8), (uint8_t)colors->meanRGB);
			BSP_LCD_DisplayStringAt(330, 100, rgb, LEFT_MODE);
			//LL_FillBuffer(LTDC_ACTIVE_LAYER_BACKGROUND, (uint32_t *)(hltdc_discovery.LayerCfg[LTDC_ACTIVE_LAYER_BACKGROUND].FBStartAdress), 200, 200, 0, colors->meanRGB);
			osMailFree(displayQueue, colors);
		}
	}
}