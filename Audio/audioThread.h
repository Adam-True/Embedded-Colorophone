/*
 * audioThread.h
 *
 *  Created on: 22 Aug 2018
 *      Author: Colin Cina
 */

#ifndef AUDIOTHREAD_H_
#define AUDIOTHREAD_H_

#include "cmsis_os.h"
#include "audio_types.h"
#include "stm32f769i_discovery_audio.h"
#include "../Components/wm8994/wm8994.h"
#include "audioselector.h"
#include "color.h"

#include "main.h"

/**
 *	@file audioThread.h
 */

// Play buffer definition for the audio output
#define PLAY_BUFF_SIZE       2048
uint16_t                     PlayBuff[PLAY_BUFF_SIZE];

// Audio sample and SD card necessities
char SD_Path[4];
FATFS fs;

AudioSelector redSel;
AudioSelector greenSel;
AudioSelector blueSel;
AudioSelector whiteSel;

/*! \fn void Playback_Init(void)
 * 	\brief
 *
*/
void Playback_Init(void);


/*! \fn void audioThread(void const * argument)
 * 	\brief
 *
*/
void audioThread(void const * argument);


/*! \fn void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
 * 	\brief
 *
*/
void BSP_AUDIO_OUT_TransferComplete_CallBack(void);


/*! \fn void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
 * 	\brief
 *
*/
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void);


#endif /* AUDIOTHREAD_H_ */
