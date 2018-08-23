#ifndef WAVEHANDLER_H
#define WAVEHANDLER_H

#include "audiohandle.h"
#include "sd_diskio.h"

#define WAV_NOT_OK 1
#define WAV_OK 0

#define SECTOR_SIZE 4096
#define NUMBER_OF_TRIES 6

typedef struct WaveHandler_s
{
	char fileName[15];
	uint32_t fileSize;
	uint32_t bytesRead;

	uint8_t* currentSector;
	uint32_t index;

	FIL file;

	uint8_t* selector;

	uint32_t sector_size;
} WaveHandler;

void WaveHandler_create(WaveHandler* me, const char* name, uint8_t* s, uint32_t size);
uint8_t WaveHandler_initialise(WaveHandler* me);
audio_data WaveHandler_getNext(WaveHandler* me);

#endif // WAVEHANDLER_H
