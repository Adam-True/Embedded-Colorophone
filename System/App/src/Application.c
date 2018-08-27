/*
 * Application.c
 *
 *  Created on: 22 Aug 2018
 *      Author: Colin Cina
 */

#include "Application.h"

CAM_ApplicationTypeDef Appli_state = APPLICATION_IDLE;


/**
  * @brief  Start task
  * @param  pvParameters not used
  * @retval None
  */
void AppThread(void const *argument)
{
  osEvent event;

  /* Init CAM Application */
  initApplication();

  /* Start Host Library */
  USBH_Init(&hUSBHost, USBH_UserProcess, 0);

  /* Add Supported Class */
  USBH_RegisterClass(&hUSBHost, USBH_CAM_CLASS);

  /* Start Host Process */
  USBH_Start(&hUSBHost);

  for (;;)
  {
    event = osMessageGet(AppliEvent, osWaitForever);

    if (event.status == osEventMessage)
    {
      switch (event.value.v)
      {
		  case APPLICATION_DISCONNECT:
			Appli_state = APPLICATION_DISCONNECT;
			BSP_LCD_Clear(LCD_COLOR_WHITE);
			LCD_LOG_SetHeader((uint8_t *) " USB OTG HS UVC Host");
			break;

		  case APPLICATION_READY:
			Appli_state = APPLICATION_READY;
			break;

		  case APPLICATION_START:
			Appli_state = APPLICATION_START;
			break;

		  default:
			break;
      }
    }
  }
}

/**
  * @brief  MSC application Init.
  * @param  None
  * @retval None
  */
void initApplication(void)
{
	#ifdef USE_USB_HS
	LCD_LOG_SetHeader((uint8_t *) " USB OTG HS UVC Host");
	#else
	LCD_LOG_SetHeader((uint8_t *) " USB OTG FS UVC Host");
	#endif

	LCD_UsrLog("USB Host library started.\n");
}

/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
{
	switch (id)
	{
		case HOST_USER_SELECT_CONFIGURATION:
		  break;

		case HOST_USER_DISCONNECTION:
		  osMessagePut(AppliEvent, APPLICATION_DISCONNECT, 0);
		  break;

		case HOST_USER_CLASS_ACTIVE:
		  osMessagePut(AppliEvent, APPLICATION_READY, 0);
		  BSP_LCD_Clear(0xFF808080);
		  break;

		case HOST_USER_CONNECTION:
		  osMessagePut(AppliEvent, APPLICATION_START, 0);
		  break;

		default:
		  break;
	}
}
