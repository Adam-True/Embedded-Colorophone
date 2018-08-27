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
 *	@file audioselector.h Contains elements for the class "AudioSelector" which acts as a multiplexer
 *	to select between the signals in the flash memory or on the SD card.
 */

#define INTERNAL 0
#define EXTERNAL 1

/**
 * 	\struct
 * 	\brief Class that acts as a selection device between internal (flash) signals and
 * 	external (SD card) signals. It is also used as the interface from the main application
 * 	to these signals. That means it acts as an abstraction layer. The application calls getNext
 * 	and the audio selector will decide from which signal it sends a value
 */
typedef struct AudioSelector_s
{
	uint8_t selection; 		/**< Selection internal or external wave file */
	AudioHandler internal; 	/**< Handler for internal data */
	WaveHandler external; 	/**< Handler for SD card data */
} AudioSelector;

// Method for the creation of an audio sample
/*! \fn void AudioSelector_initialise(AudioSelector* me, const char* name, audio_length l, audio_data* data, uint32_t size)
 * 	\brief Function that initilaises the AudioSelector in question and initialises both of its handlers
 * 	\param me Audio selector to initialise
 * 	\param name Name of the wave file on the SD card. For example "sample.wav"
 * 	\param audio_length Length of the internal data array
 * 	\param data Pointer to the internal audio data
 * 	\param size Size of a sector to be read by FatFs for the WaveHandler
 *
*/
void AudioSelector_initialise(AudioSelector* me, const char* name, audio_length l, audio_data* data, uint32_t size);

// Method for the creation of an audio sample
/*! \fn audio_data AudioSelector_getNext(AudioSelector* me)
 * 	\brief Returns next single audio sample, reads from wave handler if
 * 	it can parse the file. If something goes wrong it takes it from the audio handler
 *
*/
audio_data AudioSelector_getNext(AudioSelector* me);

#endif /* AUDIOSELECTOR_H_ */
