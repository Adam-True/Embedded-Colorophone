/*
 * errorhandle.h
 *
 *  Created on: 26 Jun 2018
 *      Author: adamt
 */

#ifndef ERRORHANDLE_H_
#define ERRORHANDLE_H_

/*
 *	ErrorCode Class
 *
 */
typedef enum ErrorCode_e
{
	NO_ERROR,
//	ERROR_AUDIO_NOT_LOADED,
	ERROR_AUDIO_INIT,
	ERROR_AUDIO_PLAY,
	ERROR_HAL_DMA_INIT,
	ERROR_HAL_DMA_SAI,
	ERROR_HAL_CLOCK_CONFIG,
	ERROR_HAL_SAI_INIT,
	ERROR_WM8994_DRIVER,
	ERROR_WM8994_DRIVER_INIT,
	ERROR_BUFFER_TIME,
	ERROR_SD_DRIVER,
	ERROR_STACK_OVERFLOW
} ErrorCode;


#endif /* ERRORHANDLE_H_ */
