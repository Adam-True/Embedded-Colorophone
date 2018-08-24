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

/**
 *	@file colourConverter.h This module deals with colour space conversion. It supports YCbCr to RGB conversion. The converted values are
 *	accumulated and the mean value is calculated by calling the getMeanValue() function.
 *	Should be transferred to a C++ class in the future.
 */

//red 	@ 	[0]
//gren 	@ 	[1]
//blue 	@	[2]
float32_t MEAN_Values[3];
float32_t convRed, convBlue, convGreen;
uint8_t meanRed, meanGreen, meanBlue, lowestMeanVal;


/*! \fn colourConverter_Init()
    \brief Initialisation function. Here the variables related to colour conversion are set to initial values.
 *	This function could later be implemented by the class constructor.
*/
void colourConverter_Init();


/*! \fn ycbcr2rgb_SDTV(float32_t y, float32_t cb, float32_t cr)
    \brief ycbcr2rgb_SDTV This function converts YCbCr to RGB using the SDTV equations. These equations are taken from :
 *	Introduction to video and image processing, Thomas B. Moeslund, ISBN 978-1-4471-2502-0.
    \param y The y component to convert.
    \param cb The cb component to convert.
    \param cr The cr component to convert.
*/
void ycbcr2rgb_SDTV(float32_t y, float32_t cb, float32_t cr);


/*! \fn ycbcr2rgb_HDTV(float32_t y, float32_t cb, float32_t cr)
    \brief This function converts YCbCr to RGB using the HDTV equations. These equations are taken from :
 *	Introduction to video and image processing, Thomas B. Moeslund, ISBN 978-1-4471-2502-0.
    \param y The y component to convert.
    \param cb The cb component to convert.
    \param cr The cr component to convert.
*/
void ycbcr2rgb_HDTV(float32_t y, float32_t cb, float32_t cr);


/*! \fn saturate(float32_t* red, float32_t* green, float32_t* blue)
    \brief This function saturates the values between the 0 and 255 levels so that they can be stored in 8 bit variables.
    \param red The red value to saturate.
    \param green The green value to saturate.
    \param blue The blue value to saturate.
*/
void saturate(float32_t* red, float32_t* green, float32_t* blue);


/*! \fn uint32_t getMeanValue(uint32_t divisor)
    \brief This function calculates the mean value of the accumulated RGB values. The return value is formated for the
 * 	generation system in a uint32_t that contains whiteness (8 bit) - blue (8 bit) - green (8 bit) - red (8 bit).
    \param divisor the number of pixels that have been converted.
*/
uint32_t getMeanValue(uint32_t divisor);


/*! \fn uint32_t getDisplayValues()
    \brief This function returns the RGB values formatted to be displayed on the screen. The return value is a uint32_t
 * 	that contains alpha (8 bit), red (8 bit), green (8 bit).
*/
uint32_t getDisplayValues();


/*! \fn void resetMeanVals()
    \brief This function resets the accumulated values usually contolled by the stream parser. Eventually manage this internally.
*/
void resetMeanVals();


#endif /* INC_COLOURCONVERTER_H_ */
