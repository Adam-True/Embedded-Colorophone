/*
 * rawStreamParser.h
 *
 *  Created on: 19 juil. 2018
 *      Author: Colin Cina
 */

#ifndef RAWSTREAMPARSER_H_
#define RAWSTREAMPARSER_H_

#include "stdint.h"
#include "main.h"
#include "FreeRTOS.h"
#include "arm_math.h"
#include "color.h"


#define IMAGE_WIDTH			320
#define IMAGE_HEIGHT		240
#define ROI_WIDTH			0x28 	//Width of the ROI in terms of bytes considering that one pixel is coded on two bytes, with the specified IMAGE_WIDTH
#define ROI_START_INDEX		70060 	//Index of the top left byte in the ROI assuming the bytes are received in a left right, top down manner.
#define ROI_END_INDEX		82260 	//Index of the bottom right byte of the ROI assuming the bytes are received in a left right, top down manner.
#define OFFSET_TO_NEXT_ROW  0x258	//Offset to the next start of a row in the ROI in terms of bytes, with the specified IMAGE_WIDTH
#define NB_PIXELS_IN_ROI	400

typedef enum _RAWparserState
{
	PARSER_RAW_IDLE = 0,
	PARSER_RAW_CONVERT,
	PARSER_RAW_WAIT_FOR_FRAME_END,
	PARSER_RAW_GET_IMG,
}rawParserStateTypedef;

typedef enum _RAWparserMailState{
	PARSER_RAW_GET_MAIL,
	PARSER_RAW_PROCESS_MAIL
}rawParserMailStateTypedef;

arm_matrix_instance_f32 YUV_MATRIX;
arm_matrix_instance_f32 RGB_MATRIX;
arm_matrix_instance_f32 YUV_TO_RGB_MATRIX;
float32_t YUV_Values[3];
float32_t RGB_Values[3];

//red 	@ 	[0]
//gren 	@ 	[1]
//blue 	@	[2]
float32_t MEAN_Values[3];

rawParserStateTypedef parserState;
rawParserMailStateTypedef parserMailState;
osEvent evt;
uint8_t EOF_mask;
uint8_t rem_length;
uint8_t headerSize;
uint8_t convertedPixels;
uint8_t meanRed, meanGreen, meanBlue, lowestMeanVal;
uint8_t* packetData;
uint8_t* dataPtr;
uint32_t packetSize;
uint32_t rowCount;
uint32_t roiIndex;
uint32_t dataSize;
uint32_t dataCount;
uint32_t bufferOffset;
uint32_t packetOffset;
uint32_t meanVals;

void RawParserThread(void const *argument);

#endif /* RAWSTREAMPARSER_H_ */
