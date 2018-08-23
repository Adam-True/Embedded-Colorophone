/*
 * colourConverter.h
 *
 *  Created on: 22 Aug 2018
 *      Author: Colin Cina
 */

#ifndef INC_COLOURCONVERTER_H_
#define INC_COLOURCONVERTER_H_

#include "stdint.h"
#include "arm_math.h"

//red 	@ 	[0]
//gren 	@ 	[1]
//blue 	@	[2]
float32_t MEAN_Values[3];
float32_t convRed, convBlue, convGreen;
uint8_t meanRed, meanGreen, meanBlue, lowestMeanVal;

void colourConverter_Init();

void ycbcr2rgb_SDTV(float32_t y, float32_t cb, float32_t cr);
void ycbcr2rgb_HDTV(float32_t y, float32_t cb, float32_t cr);
void saturate(float32_t* red, float32_t* green, float32_t* blue);

uint32_t getMeanValue(uint32_t divisor);
uint32_t getDisplayValues();
void resetMeanVals();


#endif /* INC_COLOURCONVERTER_H_ */
