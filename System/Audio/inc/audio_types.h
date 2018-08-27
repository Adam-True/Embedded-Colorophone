/*
 * audio_types.h
 *
 *  Created on: 1 Aug 2018
 *      Author: adamt
 */

#ifndef AUDIO_TYPES_H_
#define AUDIO_TYPES_H_

#include "stdio.h"

/**
 *	@file audio_types.h This file contains type definitions for the audio signals.
 *	audio_data defines single audio samples and audio_length the type for audio file sizes.
 *	It also contains AUDIO_MIN and AUDIO_MAX to indicate the lowest and highest possible
 *	values for an aiudio sample.
 */

// Type defintions
typedef uint32_t audio_length;
typedef int16_t audio_data;

#define AUDIO_MAX 32767
#define AUDIO_MIN -32768

#endif /* AUDIO_TYPES_H_ */
