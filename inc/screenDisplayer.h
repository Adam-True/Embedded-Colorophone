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
#include "FreeRTOS.h"

void screenDisplayerThread(void const *argument);

#endif /* SCREENDISPLAYER_H_ */