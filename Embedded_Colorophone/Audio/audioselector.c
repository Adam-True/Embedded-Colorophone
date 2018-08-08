/*
 * audioselector.c
 *
 *  Created on: 30 Jul 2018
 *      Author: adamt
 */

#include "audioselector.h"

void AudioSelector_initialise(AudioSelector* me, const char* name, audio_length l, audio_data* data)
{
	// Setup internal audio wave
	me->selection = INTERNAL;
	AudioSample_create(&me->internal, l, data);

	// Setup external audio wave
	WaveHandler_create(&me->external, name, me);
	if(WaveHandler_initialise(&me->external) == WAV_OK)
	{
		me->selection = EXTERNAL;
	}
}

audio_data AudioSelector_getNext(AudioSelector* me)
{
	if(me->selection == INTERNAL)
	{
		return AudioSample_getNext(&me->internal);
	}
	return WaveHandler_getNext(&me->external);
}
