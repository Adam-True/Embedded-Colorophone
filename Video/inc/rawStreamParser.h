/*
 * rawStreamParser.h
 *
 *  Created on: 19 juil. 2018
 *      Author: Colin Cina
 */

#ifndef RAWSTREAMPARSER_H_
#define RAWSTREAMPARSER_H_

#include "colourConverter.h"
#include "stdint.h"
#include "main.h"
#include "cmsis_os.h"
#include "arm_math.h"
#include "color.h"


#define IMAGE_WIDTH			320
#define IMAGE_HEIGHT		240
#define EOF_mask			0x02
#define ROI_WIDTH			0x28 	//Width of the ROI in terms of bytes considering that one pixel is coded on two bytes, with the specified IMAGE_WIDTH
#define ROI_START_INDEX		70060 	//Index of the top left byte in the ROI assuming the bytes are received in a left right, top down manner.
#define ROI_END_INDEX		82260 	//Index of the bottom right byte of the ROI assuming the bytes are received in a left right, top down manner.
#define OFFSET_TO_NEXT_ROW  0x258	//Offset to the next start of a row in the ROI in terms of bytes, with the specified IMAGE_WIDTH
#define NB_PIXELS_IN_ROI	200

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


uint8_t rem_length;
uint8_t headerSize;

uint8_t* dataPtr;
uint8_t* imageBuffer;

uint32_t convertedPixels;
uint32_t packetSize;
uint32_t rowCount;
uint32_t roiIndex;
uint32_t dataSize;
uint32_t dataCount;
uint32_t bufferOffset;
uint32_t packetOffset;

osEvent evt;
rawParserStateTypedef parserState;
Color* c;


void RawParserThread(void const *argument);
void rawParserInit();

#endif /* RAWSTREAMPARSER_H_ */
