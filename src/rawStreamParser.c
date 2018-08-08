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
	uint32_t* dispColor;
	float32_t intRed, intGreen, intBlue;

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
						tempY0 	= (float32_t)( (imageBuffer[i] > 255) ? 255 : ((imageBuffer[i] < 0) ? 0 : imageBuffer[i]) );
						tempCb 	= (float32_t)( (imageBuffer[i+1] > 255) ? 255 : ((imageBuffer[i+1] < 0) ? 0 : imageBuffer[i+1]) );
						tempY1  = (float32_t)( (imageBuffer[i+2] > 255) ? 255 : ((imageBuffer[i+2] < 0) ? 0 : imageBuffer[i+2]) );
						tempCr 	= (float32_t)( (imageBuffer[i+3] > 255) ? 255 : ((imageBuffer[i+3] < 0) ? 0 : imageBuffer[i+3]) );

						//Converting from YCbCr to RGB of the first pixel
						convRed = (1.164 * (tempY0 - 16)) + (1.596 * (tempCr - 128));
						convGreen = (1.164 * (tempY0 - 16)) - (0.813 * (tempCr - 128)) - (0.391 * (tempCb - 128));
						convBlue = (1.164 * (tempY0 - 16)) + (2.018 * (tempCb - 128));

						//Saturating the RGB values
						MEAN_Values[0] += (convRed > 255) ? 255 : ((convRed < 0) ? 0 : convRed);
						MEAN_Values[1] += (convGreen > 255) ? 255 : ((convGreen < 0) ? 0 : convGreen);
						MEAN_Values[2] += (convBlue > 255) ? 255 : ((convBlue < 0) ? 0 : convBlue);

						//Converting from YCbCr to RGB of the first pixel
						convRed = (1.164 * (tempY1 - 16)) + (1.596 * (tempCr - 128));
						convGreen = (1.164 * (tempY1 - 16)) - (0.813 * (tempCr - 128)) - (0.391 * (tempCb - 128));
						convBlue = (1.164 * (tempY1 - 16)) + (2.018 * (tempCb - 128));

						//Saturating the RGB values
						MEAN_Values[0] += (convRed > 255) ? 255 : ((convRed < 0) ? 0 : convRed);
						MEAN_Values[1] += (convGreen > 255) ? 255 : ((convGreen < 0) ? 0 : convGreen);
						MEAN_Values[2] += (convBlue > 255) ? 255 : ((convBlue < 0) ? 0 : convBlue);

						//YCbCr -> RGB equations set
						//MEAN_Values[0] += 1.164 * ((float32_t)(imageBuffer[i] - 16)) + 2.018 * ((float32_t)(imageBuffer[i + 1] - 128));
						//MEAN_Values[1] += 1.164 * ((float32_t)(imageBuffer[i] - 16)) - 0.813 * ((float32_t)(imageBuffer[i + 3] - 128)) - (0.391 * (float32_t)(imageBuffer[i + 1] - 128));
						//MEAN_Values[2] += 1.164 * ((float32_t)(imageBuffer[i] - 16)) + 1.596 * ((float32_t)(imageBuffer[i + 3] - 128));

						//YUV -> RGB equations set
//						intRed = (float32_t)imageBuffer[i] + (1.140 * (float32_t)imageBuffer[i + 3]);
//						intGreen = (float32_t)imageBuffer[i] - (0.395 * (float32_t)imageBuffer[i + 1]) - (0.581 * (float32_t)imageBuffer[i + 3]);
//						intBlue = (float32_t)imageBuffer[i] + (2.032 * (float32_t)imageBuffer[i + 1]);
//						MEAN_Values[0] += intRed > 255 ? 255 : (intRed < 0 ? 0 : intRed);
//						MEAN_Values[1] += intGreen> 255 ? 255 : (intGreen < 0 ? 0 : intGreen);
//						MEAN_Values[2] += intBlue > 255 ? 255 : (intBlue < 0 ? 0 : intBlue);

						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
						convertedPixels+=2;
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

						dispColor = osMailAlloc(displayQueue, 0);
						if(dispColor != 0)
						{
							*dispColor = (255 << 24) + (meanRed << 16) + (meanGreen << 8) + meanBlue;
							if(osMailPut(displayQueue, dispColor) != osOK)
							{
								osMailFree(displayQueue, dispColor);
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
