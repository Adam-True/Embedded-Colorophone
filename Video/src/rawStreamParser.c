/*
 * rawStreamParser.c
 *
 *  Created on: 19 juil. 2018
 *      Author: Colin Cina
 */

#include "rawStreamParser.h"

imgPacketTypedef* packet;
colorPacketTypedef* colorInfo;

void rawParserInit()
{
	//Init local stuff here
	parserState = PARSER_RAW_IDLE;
	rem_length = 0;
	headerSize = 0;
	rowCount = 0;
	roiIndex = ROI_START_INDEX;
	dataSize = 0;
	dataCount = 0;
	bufferOffset = 0;
	packetOffset = 0;
	convertedPixels = 0;
	packet = NULL;
	imageBuffer = pvPortMalloc(0x320);

	if(imageBuffer == 0x0)
	{
		//trap the program
		while(1){}
	}
}



/**
  * @brief  Start task
  * @param  pvParameters not used
  * @retval None
  */
void RawParserThread(void const *argument)
{
	rawParserInit();
	colourConverter_Init();

	for(;;)
	{
		//Retrieve from mailbox.
		evt = osMailGet(imgMailBox, osWaitForever);
		if(evt.status == osEventMail)
		{
			packet = (imgPacketTypedef*) evt.value.p;

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
							ycbcr2rgb_SDTV((float32_t)imageBuffer[i], (float32_t)imageBuffer[i+1], (float32_t)imageBuffer[i+3]);
							convertedPixels++;
						}
						if(convertedPixels == NB_PIXELS_IN_ROI)
						{
							c = osMailAlloc(colorQueue, 0);
							if(c != NULL)
							{
								uint32_t mean = getMeanValue(convertedPixels);
								c->concatValues = mean;
								if(osMailPut(colorQueue, c) != osOK)
								{
									osMailFree(colorQueue, c);
								}
							}

							colorInfo = osMailAlloc(displayQueue, 0);
							if(colorInfo != 0)
							{
								colorInfo->meanRGB = getDisplayValues();

								if(osMailPut(displayQueue, colorInfo) != osOK)
								{
									osMailFree(displayQueue, colorInfo);
								}
							}

							convertedPixels = 0;
							resetMeanVals();
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
}

