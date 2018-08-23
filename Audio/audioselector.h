/*
 * audioselector.h
 *
 *  Created on: 30 Jul 2018
 *      Author: adamt
 */

#ifndef AUDIOSELECTOR_H_
#define AUDIOSELECTOR_H_

#include "wavehandler.h"

#define INTERNAL 0
#define EXTERNAL 1

typedef struct AudioSelector_s
{
	uint8_t selection;
	AudioSample internal;
	WaveHandler external;
} AudioSelector;

void AudioSelector_initialise(AudioSelector* me, const char* name, audio_length l, audio_data* data, uint32_t size);
audio_data AudioSelector_getNext(AudioSelector* me);

#endif /* AUDIOSELECTOR_H_ */
