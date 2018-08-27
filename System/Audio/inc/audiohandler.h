/*
 * audiohandle.h
 *
 *  Created on: 13 Jun 2018
 *      Author: adamt
 */

#ifndef AUDIOHANDLER_H_
#define AUDIOHANDLER_H_

#include "audio_types.h"

/**
 *	@file audiohandle.h This file conatains the class AudioSample
 */

/*
 *	AudioSample Class
 *
 *	length	: how many values in data
 *	index 	: current write position in the data
 *	data 	: array of audio PCM values
 *
 */

/**
 * 	\struct
 */
typedef struct AudioHandler_s
{
	audio_length length;  	/**<  Text here */
	audio_length index;  	/**<  Text here */
    audio_data* data; 		/**<  Text here */
} AudioHandler;

// Method for the creation of an audio sample
/*! \fn void AudioSample_create(AudioSample* me, audio_length l, audio_data* d)
 * 	\brief
 *
*/
void AudioHandler_create(AudioHandler* me, audio_length l, audio_data* d);

// Method that advances through the audio sample
// and restarts when it gets to the end
/*! \fn void audio_data AudioSample_getNext(AudioSample* me)
 * 	\brief
 *
*/
audio_data AudioHandler_getNext(AudioHandler* me);


#endif /* AUDIOHANDLE_H_ */
