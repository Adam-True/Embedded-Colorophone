/*
 * colourConverter.c
 *
 *  Created on: 22 Aug 2018
 *      Author: Colin Cina
 */

#include "colourConverter.h"

void colourConverter_Init()
{
	MEAN_Values[0] = 0;
	MEAN_Values[1] = 0;
	MEAN_Values[2] = 0;
	convRed = 0,
	convBlue = 0;
	convGreen = 0;
	meanRed = 0;
	meanGreen = 0;
	meanBlue = 0;
	lowestMeanVal = 0;
}

//YCbCr to RGB : Computer system Equations SDTV + Accumulation in the mean values array
void ycbcr2rgb_SDTV(float32_t y, float32_t cb, float32_t cr)
{
	convRed 	= (1.164 * (y - 16)) + (1.596 * (cr - 128));
	convGreen 	= (1.164 * (y - 16)) - (0.813 * (cr - 128)) - (0.391 * (cb - 128));
	convBlue 	= (1.164 * (y - 16)) + (2.018 * (cb - 128));

	convRed = (convRed > 255) ? 255 : ((convRed < 0) ? 0 : convRed);
	convGreen = (convGreen > 255) ? 255 : ((convGreen < 0) ? 0 : convGreen);
	convBlue = (convBlue > 255) ? 255 : ((convBlue < 0) ? 0 : convBlue);

	MEAN_Values[0] += convRed;
	MEAN_Values[1] += convGreen;
	MEAN_Values[2] += convBlue;
}

//YCbCr to RGB : Computer system Equations HDTV + Accumulation in the mean values array
void ycbcr2rgb_HDTV(float32_t y, float32_t cb, float32_t cr)
{
	convRed 	= (1.164 * (y - 16)) + (1.793 * (cr - 128));
	convGreen 	= (1.164 * (y - 16)) - (0.534 * (cr - 128)) - (0.213 * (cb - 128));
	convBlue 	= (1.164 * (y - 16)) + (2.115 * (cb - 128));

	saturate(&convRed, &convGreen, &convBlue);

	MEAN_Values[0] += convRed;
	MEAN_Values[1] += convGreen;
	MEAN_Values[2] += convBlue;
}

//Saturating the RGB values
void saturate(float32_t* red, float32_t* green, float32_t* blue)
{
	*red = (*red > 255) ? 255 : ((*red < 0) ? 0 : *red);
	*green = (*green > 255) ? 255 : ((*green < 0) ? 0 : *green);
	*blue = (*blue > 255) ? 255 : ((*blue < 0) ? 0 : *blue);
}

uint32_t getMeanValue(uint32_t divisor)
{
	meanRed = (uint8_t)(MEAN_Values[0]/divisor);
	meanGreen = (uint8_t)(MEAN_Values[1]/divisor);
	meanBlue = (uint8_t)(MEAN_Values[2]/divisor);

	//Whiteness calculation : take the lower RGB mean value as whiteness
	lowestMeanVal = meanBlue<(meanRed<meanGreen ? meanRed:meanGreen) ? meanBlue:(meanRed<meanGreen ? meanRed:meanGreen);

	//Subtracting whiteness from the mean RGB values, one will thu be set to zero
	return (uint32_t)((lowestMeanVal << 24) + ((meanBlue - lowestMeanVal) << 16) + ((meanGreen - lowestMeanVal) <<8) + (meanRed - lowestMeanVal));
}

uint32_t getDisplayValues()
{
	return (uint32_t)((255 << 24) + ((meanRed) << 16) + ((meanGreen) <<8) + meanBlue);
}

void resetMeanVals()
{
	MEAN_Values[0] = 0;
	MEAN_Values[1] = 0;
	MEAN_Values[2] = 0;
}
