/*
 * Application.h
 *
 *  Created on: 22 Aug 2018
 *      Author: Colin Cina
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "cmsis_os.h"
#include "usbh_core.h"
#include "usbh_cam.h"

USBH_HandleTypeDef hUSBHost;


typedef enum
{
  APPLICATION_IDLE = 0,
  APPLICATION_READY,
  APPLICATION_DISCONNECT,
  APPLICATION_START
} CAM_ApplicationTypeDef;



#if (USBH_USE_OS == 1)
osMessageQId AppliEvent;
#endif //USBH_USE_OS == 1

void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id);
void initApplication(void);
void AppThread(void const *argument);

#endif /* INC_APPLICATION_H_ */
