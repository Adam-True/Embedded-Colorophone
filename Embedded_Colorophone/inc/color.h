/*
 * color.h
 *
 *  Created on: 13 Jun 2018
 *      Author: adamt
 */

#ifndef COLOR_H_
#define COLOR_H_

#include "stdio.h"

/*
 * RGBW Class
 *
 * Defines values of each colour component as
 * unsigned 8 bit integers
 *
 * red, green, blue and white
 */
typedef struct RGBW_s
{
	uint8_t r, g, b, w;
} RGBW;

/*
 * Color union
 *
 * The colours can be read as individual 8 bit
 * values or as one 32 bit number, used for message queues
 */
typedef union Color_u
{
	RGBW rgbwValues;
	uint32_t concatValues;
} Color;

#endif /* COLOR_H_ */
