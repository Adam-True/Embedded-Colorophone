#include "wavehandler.h"
#include "string.h"

uint8_t WaveHandler_parseHeader(WaveHandler* me);
uint8_t WaveHandler_readSector(WaveHandler* me);

/*
 * User Manual:
 *
 * Before using any of these functions FatFs_LinkDriver must be called in the main
 * and then f_mount must be called in the thread
 *
 * 1. Call setFileName
 * 2. Call initialise
 * 3. Call getNext regularly
 */

void WaveHandler_create(WaveHandler* me, const char* name, uint8_t* s, uint32_t size)
{
	strcpy(me->fileName, name);
	me->selector = s;
	me->sector_size = size;
}

uint8_t WaveHandler_initialise(WaveHandler* me)
{
	me->index = 0;
	me->currentSector = pvPortMalloc(me->sector_size);
	me->bytesRead = 0;

	if(WaveHandler_parseHeader(me) == WAV_OK)
	{
		if(me->currentSector != 0)
		{
			if(WaveHandler_readSector(me) == WAV_OK)
			{
				return WAV_OK;
			}
		}
	}

	return WAV_NOT_OK;
}

audio_data WaveHandler_getNext(WaveHandler* me)
{
	// Number of tries when a file fails to be read
	uint32_t i = 0;

	// Data to be returned
	audio_data ret = me->currentSector[me->index];
	ret |= me->currentSector[me->index+1] << 8;			///\todo do this for cases where audio_data is a different type
	me->index += 2;
	if(me->index >= me->sector_size)
	{
		BSP_LED_Toggle(LED1);
		me->index = 0;
		while(WaveHandler_readSector(me) == WAV_NOT_OK && i < NUMBER_OF_TRIES)
		{
			i++;
			me->bytesRead = 0;

			if(i == NUMBER_OF_TRIES)
			{
				*(me->selector) = 0;	// internal
				return 0;
			}

			// Restart reading and set back to set place in the file
			WaveHandler_parseHeader(me);
		}
		BSP_LED_Toggle(LED1);
	}
	return ret;
}

uint8_t WaveHandler_parseHeader(WaveHandler* me)
{
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint16_t bitsPerSample;

	uint8_t ret = WAV_NOT_OK;
	FRESULT res;

	// If the file opens succesfully
	res = f_open(&me->file, (TCHAR const*) me->fileName, FA_READ);
	if(res == FR_OK)
	{
		res = f_read(&me->file, me->currentSector, 44, (UINT *) &(me->bytesRead));
		if(res == FR_OK)
		{
			// Get values
			me->fileSize = *(uint16_t *) (me->currentSector + 4);
			me->fileSize |= (*(uint16_t *) (me->currentSector + 6)) << 16;
			me->fileSize += 8;	// Extra 4 bytes for chunk ID and chunk size

			audioFormat = *(uint16_t *) (me->currentSector + 20);

			numChannels = *(uint16_t *) (me->currentSector + 22);

			sampleRate = *(uint16_t *) (me->currentSector + 24);
			sampleRate |= (*(uint16_t *) (me->currentSector + 26)) << 16;

			bitsPerSample = *(uint16_t *) (me->currentSector + 34);

			// Check that our system supports this kind of wave file
			if(audioFormat==1 && numChannels==2 && sampleRate==22050 && bitsPerSample==16)
			{
				///\todo if it's not supported then down-sample / interpolate
				ret = WAV_OK;
			}
		}
	}
	return ret;
}

uint8_t WaveHandler_readSector(WaveHandler* me)
{
	FRESULT res;
	uint32_t bytesToRead;
	uint32_t bytesRead;
	uint8_t headerToDispose[44];
	uint8_t ret = WAV_NOT_OK;

	// Read end of file
	bytesToRead = me->fileSize - me->bytesRead;

	// System to loop the sample

	// Read beginning of file if at start
	if(me->fileSize == me->bytesRead)
	{
		// Close the file to reopen it
		f_close(&me->file);
		res = f_open(&me->file, (TCHAR const*) me->fileName, FA_READ);
		if(res == FR_OK)
		{
			// Read header then jump to rest
			if(f_read(&me->file, headerToDispose, 44, (UINT *) &(me->bytesRead)) == FR_OK)
			{
				// Read the beginning of the file
				if(f_read(&me->file, me->currentSector, me->sector_size, (UINT *) &(bytesRead)) == FR_OK)
				{
					me->bytesRead += bytesRead;
					ret = WAV_OK;
				}
			}
		}
	}

	// If near the end of the file
	else if(bytesToRead < me->sector_size)
	{
		// Read the end of the file
		res = f_read(&me->file, me->currentSector, bytesToRead, (UINT *) &(bytesRead));
		if(res == FR_OK)
		{
			// Close the file to reopen it
			f_close(&me->file);
			res = f_open(&me->file, (TCHAR const*) me->fileName, FA_READ);
			if(res == FR_OK)
			{
				// Read header then jump to rest
				if(f_read(&me->file, headerToDispose, 44, (UINT *) &(me->bytesRead)) == FR_OK)
				{
					// Read the beginning of the file
					if(f_read(&me->file, &me->currentSector[bytesRead], me->sector_size-bytesToRead, (UINT *) &(bytesRead)) == FR_OK)
					{
						me->bytesRead += bytesRead;
						ret = WAV_OK;
					}
				}
			}
		}
	}
	// Just read an uninterrupted sector
	else
	{
		res = f_read(&me->file, me->currentSector, me->sector_size, (UINT *) &(bytesRead));
		if(res == FR_OK)
		{
			me->bytesRead += bytesRead;
			ret = WAV_OK;
		}
	}

	// If something went wrong with the file close it
	if(ret == WAV_NOT_OK)
	{
		f_close(&me->file);
	}
	return ret;
}
