/*
 * rawStreamParser.c
 *
 *  Created on: 19 juil. 2018
 *      Author: Colin Cina
 */

#include "rawStreamParser.h"

const float32_t YUV_TO_RGB_COEFFS[9] = { 1, 0, 1.13983, 1, -0.39465, -0.58060, 1, 2.03211, 0 };


/**
  * @brief  Start task
  * @param  pvParameters not used
  * @retval None
  */
void RawParserThread(void const *argument)
{
	//Init local stuff here
	parserState = PARSER_RAW_IDLE;
	parserMailState = PARSER_RAW_GET_MAIL;
	EOF_mask = 0x2;
	rem_length = 0;
	headerSize = 0;
	rowCount = 0;
	roiIndex = ROI_START_INDEX;
	dataSize = 0;
	dataCount = 0;
	bufferOffset = 0;
	packetOffset = 0;
	convertedPixels = 0;
	meanRed = 0;
	meanGreen = 0;
	meanBlue = 0;
	meanVals = 0;
	float32_t convRed = 0;
	float32_t convBlue = 0;
	float32_t convGreen = 0;
	float32_t tempY0 = 0;
	float32_t tempY1 = 0;
	float32_t tempCb = 0;
	float32_t tempCr = 0;
	MEAN_Values[0] = 0;
	MEAN_Values[1] = 0;
	MEAN_Values[2] = 0;
	imgPacketTypedef* packet = NULL;
	Color* c;
	uint32_t meanRGB;
	uint32_t instYCbCr;
	uint32_t instRGB;
	colorPacketTypedef* colorInfo;
	traceString mean_RGB = xTraceRegisterString("Mean RGB values");
	traceString inst_RGB = xTraceRegisterString("Instant RGB values");
	traceString inst_YCbCr = xTraceRegisterString("Instant YCbCr values");


	uint8_t* imageBuffer = pvPortMalloc(0x320);
	if(imageBuffer == 0x0)
	{
		//trap the program
		while(1){}
	}

	for(;;)
	{
		//Retrieve from mailbox.
		evt = osMailGet(imgMailBox, osWaitForever);
		if(evt.status == osEventMail)
		{
			packet = (imgPacketTypedef*) evt.value.p;
		}

		headerSize = packet->data[0];
		dataSize = packet->size - headerSize;
		dataPtr = packet->data + headerSize;

		switch(parserState)
		{
			case PARSER_RAW_IDLE:

				//Resync with the start of an image
				if((packet->data[1] & EOF_mask) == EOF_mask)
				{
					parserState = PARSER_RAW_GET_IMG;
				}
				break;

			case PARSER_RAW_GET_IMG:

				//ROI Isolation Algorithm
				//Wait until the ROI data is transmitted
				if(dataCount + dataSize >= roiIndex)
				{
					//Remaining row to bufferize before going to the next roiIndex
					if(rem_length != 0)
					{
						memcpy(imageBuffer + bufferOffset, dataPtr + packetOffset, rem_length);
						bufferOffset += rem_length;
						rem_length = 0;
					}
					packetOffset = (roiIndex - dataCount);

					do{
						//Does the packet contain at least an entire row ?
						if((dataSize - packetOffset) >= ROI_WIDTH)
						{
							memcpy(imageBuffer + bufferOffset, dataPtr + packetOffset, ROI_WIDTH);
							bufferOffset += ROI_WIDTH;
							packetOffset += ROI_WIDTH;
							rowCount++;

							//Is another row available
							if((dataSize - packetOffset) > OFFSET_TO_NEXT_ROW)
							{
								packetOffset += OFFSET_TO_NEXT_ROW;
							}

							//No more rows available in this packet, exit loop
							else
							{
								packetOffset = 0;
							}
						}

						//Else : the row is splitted between two packets
						//The next packet is gonna start in the middle of a ROI row. Store the remaining length
						else
						{
							memcpy(imageBuffer + bufferOffset, dataPtr + packetOffset, dataSize - packetOffset);
							bufferOffset += (dataSize - packetOffset);
							rem_length = ROI_WIDTH - (dataSize - packetOffset);
							packetOffset = 0;
							rowCount++;
						}
					}while(packetOffset != 0);
				}
				else
				{
					if(rem_length != 0)
					{
						memcpy(imageBuffer + bufferOffset, dataPtr + packetOffset, rem_length);
						bufferOffset += rem_length;
						rem_length = 0;
					}
				}

				if(bufferOffset % 4 == 0 && bufferOffset != 0)
				{
					for(int i = 0; i < bufferOffset; i+=4)
					{
						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);

						//Saturating the YCbCr values
						tempY0 	= (float32_t)imageBuffer[i];
						tempCb 	= (float32_t)imageBuffer[i+1];
						tempY1  = (float32_t)imageBuffer[i+2];
						tempCr 	= (float32_t)imageBuffer[i+3];

						//YCbCr to RGB : Digital Equations
//						convRed = tempY0  + (1.371 * (tempCr - 128));
//						convGreen = tempY0 - (0.698 * (tempCr - 128)) - (0.336 * (tempCb - 128));
//						convBlue = tempY0 + (1.732 * (tempCb - 128));

						//YCbCr to RGB : Wikipedia JPEG Conversion
//						convRed = tempY0  + (1.402 * (tempCr - 128));
//						convGreen = tempY0 - (0.714136 * (tempCr - 128)) - (0.344136 * (tempCb - 128));
//						convBlue = tempY0 + (1.772 * (tempCb - 128));

						//YCbCr to RGB : Computer system Equations HDTV
//						convRed 	= (1.164 * (tempY0 - 16)) + (1.793 * (tempCr - 128));
//						convGreen 	= (1.164 * (tempY0 - 16)) - (0.534 * (tempCr - 128)) - (0.213 * (tempCb - 128));
//						convBlue 	= (1.164 * (tempY0 - 16)) + (2.115 * (tempCb - 128));

						convRed 	= (1.164 * (tempY0 - 16)) + (1.596 * (tempCr - 128));
						convGreen 	= (1.164 * (tempY0 - 16)) - (0.813 * (tempCr - 128)) - (0.391 * (tempCb - 128));
						convBlue 	= (1.164 * (tempY0 - 16)) + (2.018 * (tempCb - 128));

						//YCbCr to RGB : Introduction to video and image processing
//						convRed = (tempY0) + (1.403 * (tempCr - 128));
//						convGreen = (tempY0) - (0.344 * (tempCb - 128)) - (0.714 * (tempCr - 128));
//						convBlue = (tempY0) + (1.773 * (tempCb - 128));


//						//Saturating the RGB values
						MEAN_Values[0] += (convRed > 255) ? 255 : ((convRed < 0) ? 0 : convRed);
						MEAN_Values[1] += (convGreen > 255) ? 255 : ((convGreen < 0) ? 0 : convGreen);
						MEAN_Values[2] += (convBlue > 255) ? 255 : ((convBlue < 0) ? 0 : convBlue);

						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
						convertedPixels++;
					}
					if(convertedPixels == NB_PIXELS_IN_ROI)
					{
						uint8_t meanRed = (uint8_t)(MEAN_Values[0]/NB_PIXELS_IN_ROI);
						uint8_t meanGreen = (uint8_t)(MEAN_Values[1]/NB_PIXELS_IN_ROI);
						uint8_t meanBlue = (uint8_t)(MEAN_Values[2]/NB_PIXELS_IN_ROI);

						//Whiteness calculation : take the lower RGB mean value as whiteness
						lowestMeanVal = meanBlue<(meanRed<meanGreen ? meanRed:meanGreen) ? meanBlue:(meanRed<meanGreen ? meanRed:meanGreen);

						//Subtracting whiteness from the mean RGB values, one will thu be set to zero
						meanVals = (lowestMeanVal << 24) + ((meanBlue - lowestMeanVal) << 16) + ((meanGreen - lowestMeanVal) <<8) + (meanRed - lowestMeanVal);
						c = osMailAlloc(colorQueue, 0);
						if(c != NULL)
						{
							c->concatValues = meanVals;
							if(osMailPut(colorQueue, c) != osOK)
							{
								osMailFree(colorQueue, c);
							}
						}

						colorInfo = osMailAlloc(displayQueue, 0);
						if(colorInfo != 0)
						{
//							vTracePrintF(mean_RGB, "red:%d green:%d blue:%d", meanRed, meanGreen, meanBlue);
//							vTracePrintF(inst_RGB, "red:%d green:%d blue:%d", convRed, convGreen, convBlue);
//							vTracePrintF(inst_YCbCr, "y0:%d cb:%d y1:%d cr:%d", imageBuffer[0], imageBuffer[1], imageBuffer[2], imageBuffer[3]);
							colorInfo->meanRGB = (255 << 24) + ((uint32_t)meanRed << 16) + ((uint32_t)meanGreen << 8) + (uint32_t)meanBlue;
//							colorInfo->instantRGB = ((uint32_t)convRed << 16) + ((uint32_t) convGreen << 8) + (uint32_t) convBlue;
//							colorInfo->instantYCbCr = ((uint32_t)imageBuffer[0] << 16) + ((uint32_t)imageBuffer[1] << 8) + (uint32_t)imageBuffer[3];
							if(osMailPut(displayQueue, colorInfo) != osOK)
							{
								osMailFree(displayQueue, colorInfo);
							}
						}

						convertedPixels = 0;
						MEAN_Values[0] = 0;
						MEAN_Values[1] = 0;
						MEAN_Values[2] = 0;
					}
					bufferOffset = 0;
				}
				dataCount += dataSize;
				roiIndex += (rowCount * IMAGE_WIDTH * 2); //Jump from the start of a row to the next. Calculations in bytes, not in pixels
				rowCount = 0;

				if(dataCount >= ROI_END_INDEX)
				{
					//reset the index variable for the next frame
					parserState = PARSER_RAW_WAIT_FOR_FRAME_END;
					dataCount = 0;
					roiIndex = ROI_START_INDEX;
					rem_length = 0;
					packetOffset = 0;
					rowCount = 0;
				}
				break;

			case PARSER_RAW_WAIT_FOR_FRAME_END:
				//if this packet is the last one
				if((packet->data[1] & EOF_mask) == EOF_mask)
				{
					//The next packet represents the start of another frame.
					parserState = PARSER_RAW_GET_IMG;
					dataCount = 0;
				}
				break;

			default:
				break;
		}
		vPortFree(packet->data);
		osMailFree(imgMailBox, packet);
	}
}
