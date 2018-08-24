/*
 * audioThread.c
 *
 *  Created on: 22 Aug 2018
 *      Author: Colin Cina
 */

#include "audioThread.h"
#include "audiosamples.h"
#include "stevenslaw.h"
/**
  * @brief  Playback initialization
  * @param  None
  * @retval None
  */
void Playback_Init(void)
{
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 30, AUDIO_FREQUENCY_11K) == AUDIO_ERROR) // TODO understand why 11kHz works here
	{
		Error_Handler(ERROR_AUDIO_INIT);
	}
	if(BSP_AUDIO_OUT_Play((uint16_t*)&PlayBuff[0], 2*PLAY_BUFF_SIZE) == AUDIO_ERROR)  // Size is in bytes
	{
		Error_Handler(ERROR_AUDIO_PLAY);
	}
}



void audioThread(void const * argument)
{
	uint16_t* position;
	Color c;
	Color* pC;

	osEvent event;

	audio_data r;
	audio_data g;
	audio_data b;
	audio_data w;
	int32_t sat;

	// Initialize the data buffer
    for(int i=0; i < PLAY_BUFF_SIZE; i++)
    {
    	PlayBuff[i] = 0;
    }

    c.rgbwValues.r = 128;
    c.rgbwValues.g = 128;
    c.rgbwValues.b = 128;
    c.rgbwValues.w = 0;

    // If no link to SD Card Driver, the AudioSelector_initialiser will detect it
    osDelay(1000);
    f_mount(&fs, (TCHAR const*)"",1);

    AudioSelector_initialise(&redSel, "red.wav", red_length, red, 4096);
    AudioSelector_initialise(&greenSel, "green.wav", green_length, green, 4096);
    AudioSelector_initialise(&blueSel, "blue.wav", blue_length, blue, 4096);
    AudioSelector_initialise(&whiteSel, "white.wav", white_length, white, 4096);

	// Start loopback
	for(;;)
	{
		// Wait until DMA has finished a full or half transfer
		event = osMailGet(sampleDMAQueue, osWaitForever);
		if(event.status == osEventMail)
		{
			// If a full transfer has occurred, fill second half of buffer
			// If a half transfer has occurred, fill first half of buffer
			position = event.value.p;
			BSP_LED_Toggle(LED2);

			// If a colour has been updated then it can be saved
			event = osMailGet(colorQueue, 0);
			if(event.status == osEventMail)
			{
				pC = event.value.p;
				c = *pC;
				osMailFree(colorQueue, pC);
			}

			// Update the first or the second part of the buffer
			for(int i = 0; i < PLAY_BUFF_SIZE/2; i++)
			{
				r = (AudioSelector_getNext(&redSel)*stevensLawConversionTable[c.rgbwValues.r])/256;
				g = (AudioSelector_getNext(&greenSel)*stevensLawConversionTable[c.rgbwValues.g])/256;
				b = (AudioSelector_getNext(&blueSel)*stevensLawConversionTable[c.rgbwValues.b])/256;
				w = (AudioSelector_getNext(&whiteSel)*stevensLawConversionTable[c.rgbwValues.w])/256;

				sat = r+g+b+w;

				if(sat > AUDIO_MAX)
				{
					PlayBuff[*position + i] = AUDIO_MAX;
				}
				else if(sat < AUDIO_MIN)
				{
					PlayBuff[*position + i] = AUDIO_MIN;
				}
				else
				{
					PlayBuff[*position + i] = sat;
				}
			}

			osMailFree(sampleDMAQueue, position);
			BSP_LED_Toggle(LED2);
		}
	}
}

/**
  * @brief Tx Transfer completed callbacks.
  * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
  uint16_t* val;
  val = osMailAlloc(sampleDMAQueue, 0);

  if(val != NULL)
  {
    *val = PLAY_BUFF_SIZE/2;

    if(osMailPut(sampleDMAQueue, val) != osOK)
    {
      // TODO change error handling for it to not be in an interrupt
      // Error_Handler(ERROR_AUDIO_PLAY);
      osMailFree(sampleDMAQueue, val);
    }
  }
}

/**
  * @brief Tx Transfer Half completed callbacks
  * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	uint16_t* val;
	val = osMailAlloc(sampleDMAQueue, 0);
	if(val != NULL)
	{
		*val = 0;
		if(osMailPut(sampleDMAQueue, val) != osOK)
		{
			osMailFree(sampleDMAQueue, val);
		}
	}
}
