/*
 * audioselector.h
 *
 *  Created on: 30 Jul 2018
 *      Author: adamt
 */

#ifndef AUDIOSELECTOR_H_
#define AUDIOSELECTOR_H_

#include "wavehandler.h"

/**
 *	@file audioselector.h
 */

#define INTERNAL 0
#define EXTERNAL 1

/**
 * 	\struct
 * 	\brief Text here
 */
typedef struct AudioSelector_s
{
	uint8_t selection; 		/**< Text here */
	AudioHandler internal; 	/**< Text here */
	WaveHandler external; 	/**< Text here */
} AudioSelector;

// Method for the creation of an audio sample
/*! \fn void AudioSelector_initialise(AudioSelector* me, const char* name, audio_length l, audio_data* data, uint32_t size)
 * 	\brief
 *
*/
void AudioSelector_initialise(AudioSelector* me, const char* name, audio_length l, audio_data* data, uint32_t size);

// Method for the creation of an audio sample
/*! \fn audio_data AudioSelector_getNext(AudioSelector* me)
 * 	\brief
 *
*/
audio_data AudioSelector_getNext(AudioSelector* me);

#endif /* AUDIOSELECTOR_H_ */
