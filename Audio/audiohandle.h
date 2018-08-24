/*
 * audiohandle.h
 *
 *  Created on: 13 Jun 2018
 *      Author: adamt
 */

#ifndef AUDIOHANDLE_H_
#define AUDIOHANDLE_H_

#include "audio_types.h"

/**
 *	@file audiohandle.h
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
 * 	@struct AudioSample
 */
typedef struct AudioSample_s
{
	audio_length length;
	audio_length index;
    audio_data* data;
} AudioSample;

// Method for the creation of an audio sample
/*! \fn void AudioSample_create(AudioSample* me, audio_length l, audio_data* d)
 * 	\brief
 *
*/
void AudioSample_create(AudioSample* me, audio_length l, audio_data* d);

// Method that advances through the audio sample
// and restarts when it gets to the end
/*! \fn void audio_data AudioSample_getNext(AudioSample* me)
 * 	\brief
 *
*/
audio_data AudioSample_getNext(AudioSample* me);


#endif /* AUDIOHANDLE_H_ */
