#ifndef WAVEHANDLER_H
#define WAVEHANDLER_H

#include <audiohandler.h>
#include "sd_diskio.h"

/**
 *	@file wavehandler.h Contains all the necessary information for the wave handler class.
 *
 */

#define WAV_NOT_OK 1
#define WAV_OK 0

#define SECTOR_SIZE 4096
#define NUMBER_OF_TRIES 6


/**
 * 	\struct
 * 	\brief Handler for reading audio data from the SD card
 */
typedef struct WaveHandler_s
{
	char fileName[15]; 		/**< File name (for example "sample.wav") */
	uint32_t fileSize; 		/**< Size of the file in bytes, to be parsed */
	uint32_t bytesRead;		/**< Current number of bytes read */

	uint8_t* currentSector;	/**< Pointer to the current section read from the file */
	uint32_t index;			/**< Current index within this section */

	FIL file;				/**< FatFs File */

	uint8_t* selector;		/**< Pointer to the audio selector, to change the selection if something goes wrong */

	uint32_t sector_size;	/**< Size of the section to be read by FatFs */
} WaveHandler;

/*! \fn void WaveHandler_create(WaveHandler* me, const char* name, uint8_t* s, uint32_t size)
 * 	\brief Create function for the class WaveHandler
 * 	\param me WaveHandler to be created
 * 	\param name Name of the file on the SD card
 * 	\param s Pointer to the audio selector
 * 	\param size Size of the sector
 *
*/
void WaveHandler_create(WaveHandler* me, const char* name, uint8_t* s, uint32_t size);

/*! \fn uint8_t WaveHandler_initialise(WaveHandler* me)
 * 	\brief Initialises reading from the file. Detects if something goes wrong, for example : the file doesn't exist, or
 * 	the format is incorrect. Returns if it is successful or not.
 * 	\param me WaveHandler to be initialised
 *
*/
uint8_t WaveHandler_initialise(WaveHandler* me);

/*! \fn audio_data WaveHandler_getNext(WaveHandler* me)
 * 	\brief Returns next value and reads the next sector when the index arrives at the end
 *
 * 	\param me WaveHandler to read the data from
 *
*/
audio_data WaveHandler_getNext(WaveHandler* me);

#endif // WAVEHANDLER_H
