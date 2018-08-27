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
 *	@file audiohandle.h This file contains the class AudioHandler and the methods of this class.
 *	This class is used for handling internal audio signals.
 */

/*
 *	AudioHandler Class
 *
 *	length	: how many values in data
 *	index 	: current write position in the data
 *	data 	: array of audio PCM values
 *
 */

/**
 * 	\struct
 * 	\brief Class used for handling internal audio signals (stored in the flash memory)
 *
 */
typedef struct AudioHandler_s
{
	audio_length length;  	/**<  length of the data array */
	audio_length index;  	/**<  current index in the data array */
    audio_data* data; 		/**<  pointer to the array */
} AudioHandler;

// Method for the creation of an audio sample
/*! \fn void AudioHandler_create(AudioHandler* me, audio_length l, audio_data* d)
 * 	\brief Called when an instance of AudioHandler structure must be initiated
 * 	\param me The audio handler to create
 * 	\param l The length of the array of audio_data
 * 	\param d Pointer to the the audio signal
 *
*/
void AudioHandler_create(AudioHandler* me, audio_length l, audio_data* d);

// Method that advances through the audio sample
// and restarts when it gets to the end
/*! \fn void audio_data AudioHandler_getNext(AudioHandler* me)
 * 	\brief Returns next audio sample and loops when it gets to the end
 * 	\param me The audio handler from which we wish to get the next value
 *
*/
audio_data AudioHandler_getNext(AudioHandler* me);


#endif /* AUDIOHANDLE_H_ */
