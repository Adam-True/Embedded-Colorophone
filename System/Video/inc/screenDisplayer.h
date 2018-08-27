/*
 * screenDisplayer.h
 *
 *  Created on: 7 Aug 2018
 *      Author: Colin Cina
 */

#ifndef SCREENDISPLAYER_H_
#define SCREENDISPLAYER_H_

#include "stdint.h"
#include "main.h"
#include "cmsis_os.h"

/**
 *  @file screenDisplayer.h This file is used to diplay the colour on the screen. It communicates with the stream parser through a mailbox.
 *  In the future C++ implementation the screenDisplayer should be represented by a class.
 */
uint8_t rgb[60];
osEvent rectColorEvent;

/**
 *	@brief screenDisplayerThread This function is the screen Displayer thread. It updates the colour of the square in the middle of the screen as well as
 *	the displayed RGB values.
 */
void screenDisplayerThread(void const *argument);

#endif /* SCREENDISPLAYER_H_ */
