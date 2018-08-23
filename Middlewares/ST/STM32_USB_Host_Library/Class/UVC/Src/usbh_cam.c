/**
  ******************************************************************************
  * @file    usbh_cam.c
  * @author  Colin Cina
  * @version V1.0.0
  * @date    01-June-2018
  * @brief   This file is the CAM Layer Handlers for USB Host CAM class. This is a made up class corresponding to a logitech c920 webcam device. 
  *          
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                CAM Class  Description
  *          =================================================================== 
  *           This module manages custom the CAM class V1.0 based on the "Device Class Definition
  *           for Video Devices (UVC) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - Configuring the device to output a MJPEG video stream
  *             - Receive the incoming video stream (isochronous transfers) 
  *         
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_cam.h"
#include "usbh_cam_def.h"
#include "main.h"

//maybe include other files of the driver


/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_CAM_CLASS
* @{
*/

/** @defgroup USBH_CAM_CORE 
* @brief    This file includes CAM Layer Handlers for USB Host CAM class.
* @{
*/ 

/** @defgroup USBH_CAM_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_CAM_CORE_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_CAM_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_CAM_CORE_Private_Variables
 *
* @{
*/

/**
* @}
*/ 


/** @defgroup USBH_CAM_CORE_Private_FunctionPrototypes
* @{
*/ 

/*@brief declared as static because they must only be called from the host core, no
 *further visibility is needed
 */
static USBH_StatusTypeDef USBH_CAM_InterfaceInit  	(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_CAM_InterfaceDeInit  (USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_CAM_ClassRequest		(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_CAM_Process			(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_CAM_SOFProcess		(USBH_HandleTypeDef *phost);

/*@brief Declared as static because only called by functions that are located within this file
 * no further visibility is needed
 */

USBH_ClassTypeDef  CAM_Class = 
{
  "Webcam",
  USB_CAM_CLASS,
  USBH_CAM_InterfaceInit,
  USBH_CAM_InterfaceDeInit,
  USBH_CAM_ClassRequest,
  USBH_CAM_Process,
  USBH_CAM_SOFProcess,
  NULL,
};

/**
* @}
*/ 


/** @defgroup USBH_CAM_CORE_Private_Functions
* @{
*/ 


/**
  * @brief  USBH_CAM_InterfaceInit 
  *         The function init the CAM class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_InterfaceInit (USBH_HandleTypeDef *phost)
{	
  uint8_t max_ep;
  uint8_t num = 0;
  uint8_t streamInterface;

  USBH_StatusTypeDef status = USBH_FAIL ;
  CAM_HandleTypeDef *CAM_Handle;

  //The first interface corresponding to a video streaming interface does not have any endpoint.
  //Temporarly use the first alternative setting which has an endpoint.
  //The definitive video streaming interface alternate setting will be chosen after streaming negotiation
  //Find the first best video streaming interface
  streamInterface = USBH_FindInterfaceIndex(phost, 0x01, 0x03);

  if(streamInterface == 0xFF) /* No Valid Interface */
  {
    status = USBH_FAIL;
    USBH_DbgLog ("Cannot Find the interface for %s class.", phost->pActiveClass->Name);
  }
  else
  {
    USBH_SelectInterface (phost, streamInterface);
#if (USBH_USE_OS == 1)
    phost->pActiveClass->pData = (CAM_HandleTypeDef *) pvPortMalloc(sizeof(CAM_HandleTypeDef));
#else
    phost->pActiveClass->pData = (CAM_HandleTypeDef *) malloc(sizeof(CAM_HandleTypeDef));
#endif //USBH_USE_OS

    CAM_Handle =  (CAM_HandleTypeDef *) phost->pActiveClass->pData;

    //TODO init these values in a separate function
    CAM_Handle->state = CAM_ERROR;
    CAM_Handle->state = CAM_INIT;
    CAM_Handle->ctl_state = CAM_REQ_INIT;
    CAM_Handle->cfg_state = CAM_REQ_CFG_INIT;
    CAM_Handle->getParam_state = CAM_REQ_GET_INIT;
    CAM_Handle->setParam_state = CAM_REQ_SET_INIT;
    CAM_Handle->streamNeg_state = CAM_REQ_STREAM_INIT;
    CAM_Handle->timer = 0;
    CAM_Handle->headerOffset = 0xC;
    CAM_Handle->transferCnt = 0;
    CAM_Handle->offset = 0;

    /* Register a User Event channel name */
    //FIXME just for tracing
    //CAM_Handle->myChannel = xTraceRegisterString("ADC 1");

    //Dummy initialisation of the stream pipe some values might be adjusted later during negotiation
    CAM_Handle->streamPoll = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bInterval;
    CAM_Handle->streamEp_addr = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress;
    CAM_Handle->streamLength = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
	/* Check for available number of endpoints */
	/* Find the number of EPs in the Interface Descriptor */
	/* Choose the lower number in order not to overrun the buffer allocated */
	max_ep = ( (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS) ?
				  phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bNumEndpoints :
					  USBH_MAX_NUM_ENDPOINTS);


	/* Decode endpoint IN and OUT address from interface descriptor */
	for ( ;num < max_ep; num++)
	{
		if(phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bEndpointAddress & 0x80)
		{
			CAM_Handle->streamEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bEndpointAddress);
			CAM_Handle->streamPipe = USBH_AllocPipe(phost, CAM_Handle->streamEp);

			/* Open pipe for IN endpoint */
			USBH_UsrLog("[usb_cam] <USBH_CAM_InterfaceInit> Opening the pipe, the speed of the device is : %d", phost->device.speed);
			USBH_OpenPipe  (phost,
							CAM_Handle->streamPipe,
							CAM_Handle->streamEp,
							phost->device.address,
							phost->device.speed,
							USB_EP_TYPE_ISOC,
							CAM_Handle->streamLength);

			USBH_LL_SetToggle (phost, CAM_Handle->streamPipe, 0);
		}
	}
  }

  return USBH_OK;;
}

/**
  * @brief  USBH_CAM_InterfaceDeInit 
  *         The function DeInit the Pipes used for the CAM class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_InterfaceDeInit (USBH_HandleTypeDef *phost )
{	
  CAM_HandleTypeDef *CAM_Handle =  (CAM_HandleTypeDef *) phost->pActiveClass->pData; 
  
  if(CAM_Handle->streamPipe != 0x00)
  {   
    USBH_ClosePipe  (phost, CAM_Handle->streamPipe);
    USBH_FreePipe  (phost, CAM_Handle->streamPipe);
    CAM_Handle->streamPipe = 0;     /* Reset the pipe as Free */
  }

  return USBH_OK;
}

/**
  * @brief  USBH_CAM_ClassRequest 
  *         The function is responsible for handling Standard requests
  *         for CAM class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_ClassRequest(USBH_HandleTypeDef *phost)
{
	return USBH_OK;
}

/**
  * @brief  USBH_CAM_Process 
  *         The function is for managing state machine for CAM data transfers 
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_Process(USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef status = USBH_OK;
  USBH_URBStateTypeDef urbStatus = USBH_URB_IDLE;
  CAM_HandleTypeDef *CAM_Handle =  (CAM_HandleTypeDef *) phost->pActiveClass->pData;
  uint32_t rx_size = 0;
  
	switch (CAM_Handle->state)
	{
	  case CAM_INIT:
		CAM_Handle->state = CAM_CONFIG;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
		break;

	  case CAM_IDLE:
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
		break;

		case CAM_CONFIG:
			if(USBH_CAM_ConfigDevice(phost, CAM_Handle) == USBH_OK)
			{
			  CAM_Handle->state = CAM_GET_STREAM_START;
			}
		break;

		case CAM_GET_STREAM_START:
			USBH_IsocReceiveData(phost, CAM_Handle->pImgData, CAM_Handle->streamLength, CAM_Handle->streamPipe);
			CAM_Handle->state = CAM_GET_STREAM;
		break;

		case CAM_GET_STREAM:
			CAM_Handle->state = CAM_GET_STREAM_START;
			urbStatus = USBH_LL_GetURBState(phost, CAM_Handle->streamPipe);
			if(urbStatus == USBH_URB_DONE)
			{
				rx_size = USBH_LL_GetLastXferSize(phost, CAM_Handle->streamPipe);
				if(rx_size != 0)
				{

					//Put the data into the mail box
					imgPacketTypedef* packet = (imgPacketTypedef*)osMailAlloc(imgMailBox, 0);
					if(packet != NULL)
					{
						packet->data = pvPortMalloc(rx_size);
						if(packet->data != 0x00)
						{
							HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
							memcpy(packet->data, (CAM_Handle->pImgData), rx_size);
							packet->size = rx_size;
							osMailPut(imgMailBox, packet);
						}
						else
						{
							//If not able to allocate the packet buffer, free the mailbox element
							//vTracePrint(CAM_Handle->myChannel, "Heap full");
							osMailFree(imgMailBox, packet);
						}
					}
					else
					{
						//vTracePrint(CAM_Handle->myChannel, "Mailbox full");
					}
				}
			}
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
		break;


		case CAM_ERROR:
			  break;

		default:
		break;
	}
	return status;
}

/**
  * @brief  USBH_CAM_SOFProcess 
  *         The function is for managing the SOF Process 
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_SOFProcess(USBH_HandleTypeDef *phost)
{
	CAM_HandleTypeDef* CAM_Handle =  (CAM_HandleTypeDef *) phost->pActiveClass->pData;
	if(CAM_Handle->state == CAM_IDLE)
	{
		CAM_Handle->state = CAM_GET_STREAM_START;
	}
	return USBH_OK;
}

void USBH_CAM_InitProbeSet(ProbeData_Typedef* probeCtrl)
{
	//Init for 640x480 30fps MJPEG Video stream
#if(USE_JPEG == 1)
	probeCtrl->params.bmHint 					= 0x0001; 		//Keep the frame interval constant.
	probeCtrl->params.bFormatIndex 				= 0x03;			//Video Streaming Uncompressed Format Type Descriptor
	probeCtrl->params.bFrameIndex 				= 0x01; 		//Video Streaming Uncompressed Frame Type Descriptor 640x480
	probeCtrl->params.bFrameInterval 			= 0x00051615;	//33.333300 mSec (30.00 Hz)
	probeCtrl->params.wKeyFrameRate 			= 0x0000; 		//not supported by the webcam
	probeCtrl->params.wPFrameRate 				= 0x0000; 		//not supported by the webcam
	probeCtrl->params.wCompQuality 				= 0x3D; 		//set to 500 (middle value) provisionally -> let's see what the GET_DEF value is
	probeCtrl->params.wCompWindowSize 			= 0x0000; 		//not supported by the webcam
	probeCtrl->params.wDelay 					= 0x0000; 		//read by the host
	probeCtrl->params.dwMaxVideoFrameSize 		= 0x00000000;	//read by the host should be 0x00096000 default
	probeCtrl->params.dwMaxPayloadTransferSize 	= 0x00000000;	//read by the host

#else

	//Init for 320x240 30fps Uncompressed (YUY2) Video stream

	probeCtrl->params.bmHint 					= 0x0001; 		//Keep the frame interval constant.
	probeCtrl->params.bFormatIndex 				= 0x01;			//Video Streaming Uncompressed Format Type Descriptor
	probeCtrl->params.bFrameIndex 				= 0x06; 		//Video Streaming Uncompressed Frame Type Descriptor 320x240
	probeCtrl->params.bFrameInterval 			= 0x00051615;	//33.333300 mSec (30.00 Hz)
	probeCtrl->params.wKeyFrameRate 			= 0x0000; 		//not supported by the webcam
	probeCtrl->params.wPFrameRate 				= 0x0000; 		//not supported by the webcam
	probeCtrl->params.wCompQuality 				= 0x3D; 		//set to 500 (middle value) provisionally -> let's see what the GET_DEF value is
	probeCtrl->params.wCompWindowSize 			= 0x0000; 		//not supported by the webcam
	probeCtrl->params.wDelay 					= 0x0000; 		//read by the host
	probeCtrl->params.dwMaxVideoFrameSize 		= 0x00000000;	//read by the host should be 0x00096000 default
	probeCtrl->params.dwMaxPayloadTransferSize 	= 0x00000000;	//read by the host

#endif//USE_JPEG
}

uint8_t USBH_CAM_SetStreamInterface (USBH_HandleTypeDef* phost, uint32_t maxPayloadTranferSize)
{
	uint8_t alt_setting;
	int32_t delta = maxPayloadTranferSize;
	int32_t maxPacket;
	int32_t result;

	for(int i = 0; i < USBH_MAX_NUM_INTERFACES; i++)
	{
		if(phost->device.CfgDesc.Itf_Desc[i].bInterfaceClass == CC_VIDEO){
			if(phost->device.CfgDesc.Itf_Desc[i].bInterfaceSubClass == SC_VIDEOSTREAMING)
			{
				uint8_t numEndpoints = phost->device.CfgDesc.Itf_Desc[i].bNumEndpoints;
				for(int j = 0; j < numEndpoints; j++)
				{
					maxPacket = (int32_t)(phost->device.CfgDesc.Itf_Desc[i].Ep_Desc[j].wMaxPacketSize);
					result = maxPacket - (int32_t) maxPayloadTranferSize;
					if((result < delta) && (result >= 0))
					{
						delta = result;
						alt_setting = phost->device.CfgDesc.Itf_Desc[i].bAlternateSetting;
					}
				}
			}
		}
	}
	return alt_setting;
}

USBH_StatusTypeDef USBH_CSRequest (USBH_HandleTypeDef *phost, uint8_t requ, uint8_t cont_sel, uint8_t entityType)
{
	USBH_StatusTypeDef status;

	switch (requ) {
		case USB_REQ_SET_CUR:
			status = USBH_CAM_SetCur(phost,  cont_sel, entityType);
			break;
		case USB_REQ_GET_CUR:
			status = USBH_CAM_GetCur(phost,  cont_sel, entityType);
			break;

		case USB_REQ_GET_MAX:
			status = USBH_CAM_GetMax(phost,  cont_sel, entityType);
			break;

		case USB_REQ_GET_MIN:
			status = USBH_CAM_GetMin(phost,  cont_sel, entityType);
			break;

		case USB_REQ_GET_DEF:
			status = USBH_CAM_GetDef(phost,  cont_sel, entityType);
			break;

		case USB_REQ_GET_INFO:
			status = USBH_CAM_GetInfo(phost,  cont_sel, entityType);
			break;

		case USB_REQ_GET_LEN:
			status = USBH_CAM_GetLen(phost,  cont_sel, entityType);
			break;

		case USB_REQ_GET_RES:
			status = USBH_CAM_GetRes(phost,  cont_sel, entityType);
			break;
		default: break;
	}
	return status;
}

USBH_StatusTypeDef USBH_CAM_NegociateRequests(USBH_HandleTypeDef* phost, CAM_HandleTypeDef* CAM_Handle)
{
	USBH_StatusTypeDef testStatus = USBH_BUSY;
	USBH_StatusTypeDef status = USBH_BUSY;

	switch(CAM_Handle->streamNeg_state)
	{
		case CAM_REQ_STREAM_INIT:
			CAM_Handle->streamNeg_state = CAM_REQ_SET_PROBE;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
		break;

		case CAM_REQ_SET_PROBE:
			USBH_CAM_InitProbeSet(&(CAM_Handle->probeSet));
			CAM_Handle->streamNeg_state = CAM_REQ_SET_PROBE_WAIT;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
		break;

		case CAM_REQ_SET_PROBE_WAIT:
			//SET_CUR request [probe] with the probe control data structure
			if(USBH_CSRequest(phost, USB_REQ_SET_CUR, VS_PROBE_CONTROL, VS_INTERFACE) == USBH_OK)
			{
				CAM_Handle->streamNeg_state = CAM_REQ_GET_PROBE;
			}
		break;

		case CAM_REQ_GET_PROBE:
			//GET_CUR request [probe] -> store the probe control data received
			if(USBH_CSRequest(phost, USB_REQ_GET_CUR, VS_PROBE_CONTROL, VS_INTERFACE) == USBH_OK)
			{
				CAM_Handle->streamNeg_state = CAM_REQ_SET_COMMIT;
			}

		break;

		case CAM_REQ_SET_COMMIT:
				//SET_CUR request [commit]	with the data read back from the GET_CUR request
				//Compare the two videoProbe data structure and set the definitive version to the probe commit terminal
				memcpy(CAM_Handle->probeSet.data, phost->device.Data, 26);
				LCD_UsrLog("Max Video Frame Buffer Size : %d\n", CAM_Handle->probeSet.params.dwMaxVideoFrameSize);
				LCD_UsrLog("Max Payload Transfer Size : %d\n", CAM_Handle->probeSet.params.dwMaxPayloadTransferSize);
				CAM_Handle->streamNeg_state = CAM_REQ_SET_COMMIT_WAIT;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
		break;

		case CAM_REQ_SET_COMMIT_WAIT:
				testStatus = USBH_CSRequest(phost, USB_REQ_SET_CUR, VS_COMMIT_CONTROL, VS_INTERFACE);
				if(testStatus == USBH_OK)
				{
					CAM_Handle->streamNeg_state = CAM_REQ_SET_INTERFACE;
				}
		break;

		case CAM_REQ_SET_INTERFACE:
				//select an aternate setting for the interface based on the MaxPayloadTransferSize received in the GET_CUR
				CAM_Handle->stream_altSetting = USBH_CAM_SetStreamInterface(phost, CAM_Handle->probeSet.params.dwMaxPayloadTransferSize);
				CAM_Handle->stream_interface = USBH_FindInterfaceIndex(phost, 0x01, CAM_Handle->stream_altSetting); //1 stands for the video streaming interface number
				if(CAM_Handle->stream_interface != 0xFF)
				{
					if(USBH_SelectInterface(phost, CAM_Handle->stream_interface) == USBH_OK)
					{
						CAM_Handle->streamLength = phost->device.CfgDesc.Itf_Desc[CAM_Handle->stream_interface].Ep_Desc[0].wMaxPacketSize;
						USBH_OpenPipe  (phost,
										CAM_Handle->streamPipe,
										CAM_Handle->streamEp,
										phost->device.address,
										phost->device.speed,
										USB_EP_TYPE_ISOC,
										CAM_Handle->streamLength);
						CAM_Handle->streamNeg_state = CAM_REQ_SET_INTERFACE_WAIT;
					}
				}
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
		break;

		case CAM_REQ_SET_INTERFACE_WAIT:
				if(USBH_SetInterface(phost, 0x01, CAM_Handle->stream_altSetting) == USBH_OK)
				{
					//allow the host process state machine to move on the the HOST_CLASS state from where the bgnd function is gonna be called
					status = USBH_OK;
					CAM_Handle->ctl_state = CAM_REQ_IDLE;
				}
		break;

		default:
			break;
	}
	return status;
}

USBH_StatusTypeDef USBH_CAM_GetParamRequests (USBH_HandleTypeDef* phost, CAM_HandleTypeDef* CAM_Handle)
{
	USBH_StatusTypeDef testStatus = USBH_BUSY;
	USBH_StatusTypeDef status = USBH_BUSY;
	bGetValues readValues;

 	switch(CAM_Handle->getParam_state)
	{
			case CAM_REQ_GET_INIT:
				CAM_Handle->getParam_state = CAM_REQ_BRIGHTNESS_DEF;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
				break;

			//Brightness Def
			case CAM_REQ_BRIGHTNESS_DEF:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_DEF, PU_BRIGHTNESS_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.brightness.defVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_BRIGHTNESS_MIN;
				}
			break;

			//Brightness MIN
			case CAM_REQ_BRIGHTNESS_MIN:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_MIN, PU_BRIGHTNESS_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.brightness.minVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_BRIGHTNESS_MAX;
				}
			break;

			//Brightness MAX
			case CAM_REQ_BRIGHTNESS_MAX:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_MAX, PU_BRIGHTNESS_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.brightness.maxVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_BRIGHTNESS_GET_CUR;
				}
			break;

			//Brightness CUR
			case CAM_REQ_BRIGHTNESS_GET_CUR:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_CUR, PU_BRIGHTNESS_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.brightness.curVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_CONTRAST_DEF;
				}
			break;

			//Contrast
			case CAM_REQ_CONTRAST_DEF:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_DEF, PU_CONTRAST_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.contrast.defVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_CONTRAST_MIN;
				}
			break;

			//Contrast
			case CAM_REQ_CONTRAST_MIN:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_MIN, PU_CONTRAST_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.contrast.minVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_CONTRAST_MAX;
				}
			break;

			//Contrast
			case CAM_REQ_CONTRAST_MAX:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_MAX, PU_CONTRAST_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.contrast.maxVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_CONTRAST_CUR;
				}
			break;

			//Contrast
			case CAM_REQ_CONTRAST_CUR:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_CUR, PU_CONTRAST_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.contrast.curVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_SATURATION_DEF;
				}
			break;

			//Saturation
			case CAM_REQ_SATURATION_DEF:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_DEF, PU_SATURATION_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.saturation.defVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_SATURATION_MIN;
				}
			break;

			//Saturation
			case CAM_REQ_SATURATION_MIN:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_MIN, PU_SATURATION_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.saturation.minVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_SATURATION_MAX;
				}
			break;

			//Saturation
			case CAM_REQ_SATURATION_MAX:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_MAX, PU_SATURATION_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.saturation.maxVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_SATURATION_CUR;
				}
			break;

			//Saturation
			case CAM_REQ_SATURATION_CUR:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_CUR, PU_SATURATION_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.saturation.curVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_SHARPNESS_DEF;
				}
			break;

			//Sharpness
			case CAM_REQ_SHARPNESS_DEF:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_DEF, PU_SHARPNESS_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.sharpness.defVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_SHARPNESS_MIN;
				}
				break;

			//Sharpness
			case CAM_REQ_SHARPNESS_MIN:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_MIN, PU_SHARPNESS_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.sharpness.minVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_SHARPNESS_MAX;
				}
				break;

			//Sharpness
			case CAM_REQ_SHARPNESS_MAX:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_MAX, PU_SHARPNESS_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.sharpness.maxVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_SHARPNESS_CUR;
				}
				break;

			//Sharpness
			case CAM_REQ_SHARPNESS_CUR:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_CUR, PU_SHARPNESS_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.sharpness.curVal = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_BACK_COMP;
				}
				break;

			//Backlight compensation
			case CAM_REQ_BACK_COMP:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_CUR, PU_BACKLIGHT_COMPENSATION_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.backlight_comp = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_GAIN;
				}
			break;

			//Gain
			case CAM_REQ_GAIN:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_CUR, PU_GAIN_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.gain = LE16(phost->device.Data);
					CAM_Handle->getParam_state = CAM_REQ_PWRFREQ;
				}
			break;

			//Power Line Frequency
			case CAM_REQ_PWRFREQ:
				testStatus = USBH_CSRequest(phost, USB_REQ_GET_DEF, PU_POWER_LINE_FREQUENCY_CONTROL, PROCESSING_UNIT);
				if(testStatus == USBH_OK)
				{
					readValues.pwrline_freq = *(phost->device.Data);
					status = USBH_OK;
				}
			break;


			case CAM_REQ_TEST_ERROR:
				if(USBH_CSRequest(phost, USB_REQ_GET_CUR, VC_REQUEST_ERROR_CODE_CONTROL, VIDEO_CONTROL) == USBH_OK)
				{
					LCD_UsrLog("The request could not be performed. Error code : %d", phost->device.Data[0]);
					LCD_UsrLog("You are now trapped in this state, restart system");
					CAM_Handle->setParam_state = 0xFF;
				}
				break;

			default:
			break;
	}
	return status;
}

USBH_StatusTypeDef USBH_CAM_SetParamRequests (USBH_HandleTypeDef* phost, CAM_HandleTypeDef* CAM_Handle)
{
	USBH_StatusTypeDef testStatus = USBH_BUSY;
	USBH_StatusTypeDef status = USBH_BUSY;

	switch(CAM_Handle->setParam_state)
	{
		case CAM_REQ_SET_INIT:
			CAM_Handle->setParam_state = CAM_REQ_AE_MODE;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
		break;

		case CAM_REQ_AE_MODE:
			//Exposure Mode
			CAM_Handle->param = 0x01;
			testStatus = USBH_CSRequest(phost, USB_REQ_SET_CUR, CT_AE_MODE_CONTROL, CAMERA_TERMINAL);
			if(testStatus == USBH_OK)
			{
				CAM_Handle->setParam_state = CAM_REQ_AE_PRIO;
			}
			else if (testStatus == USBH_NOT_SUPPORTED)
			{
				CAM_Handle->setParam_state = CAM_REQ_ERROR;
			}
			break;

		case CAM_REQ_AE_PRIO:
			//Exposure Priority
			CAM_Handle->param = 0x00;
			testStatus = USBH_CSRequest(phost, USB_REQ_SET_CUR, CT_AE_PRIORITY_CONTROL, CAMERA_TERMINAL);
			if(testStatus == USBH_OK)
			{
				CAM_Handle->setParam_state = CAM_REQ_FOCUS;
			}
			else if (testStatus == USBH_NOT_SUPPORTED)
			{
				CAM_Handle->setParam_state = CAM_REQ_ERROR;
			}
			break;

		case CAM_REQ_FOCUS:
			//Auto Focus
			CAM_Handle->param = 0x01;
			testStatus = USBH_CSRequest(phost, USB_REQ_SET_CUR, CT_FOCUS_AUTO_CONTROL, CAMERA_TERMINAL);
			if(testStatus == USBH_OK)
			{
				CAM_Handle->setParam_state = CAM_REQ_WHITE_BAL;
			}
			else if (testStatus == USBH_NOT_SUPPORTED)
			{
				CAM_Handle->setParam_state = CAM_REQ_ERROR;
			}
			break;

		case CAM_REQ_WHITE_BAL:
			//White Balance Temperature
			CAM_Handle->param = 0x01;
			testStatus = USBH_CSRequest(phost, USB_REQ_SET_CUR, PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL, PROCESSING_UNIT);
			if(testStatus == USBH_OK)
			{
				status = USBH_OK;
			}
			else if (testStatus == USBH_NOT_SUPPORTED)
			{
				CAM_Handle->setParam_state = CAM_REQ_ERROR;
			}
			break;

		case CAM_REQ_ERROR:
			if(USBH_CSRequest(phost, USB_REQ_GET_CUR, VC_REQUEST_ERROR_CODE_CONTROL, VIDEO_CONTROL) == USBH_OK)
			{
				LCD_UsrLog("The request could not be performed. Error code : %d", phost->device.Data[0]);
				LCD_UsrLog("You are now trapped in this state, restart system");
				CAM_Handle->setParam_state = 0xFF;
			}
			break;

		default:
			break;
	}
	return status;
}

USBH_StatusTypeDef USBH_CAM_SetCur(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType)
{
	uint16_t wValue,wIndex,wLength;
	CAM_HandleTypeDef *CAM_Handle;
	CAM_Handle = (CAM_HandleTypeDef*) phost->pActiveClass->pData;
	wValue = (cont_sel << 8) & 0xFF00;
	uint8_t* data = (uint8_t*)(&(CAM_Handle->param));

	//LIST ALL CONTROL SELECTORS THAT SUPPORT GET_CUR COMMAND
	switch(entityType)
	{
		case CAMERA_TERMINAL:
			switch(cont_sel)
			{
				case CT_AE_MODE_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;

				case CT_AE_PRIORITY_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;

				case CT_FOCUS_AUTO_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;

				default: break;
			}
		break;

		case PROCESSING_UNIT:
			switch(cont_sel)
			{
				case PU_BACKLIGHT_COMPENSATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_BRIGHTNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_CONTRAST_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_GAIN_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_POWER_LINE_FREQUENCY_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_SATURATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SHARPNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;
			}
		break;

		case VS_INTERFACE:
			data = CAM_Handle->probeSet.data;
			switch(cont_sel)
			{
				case VS_PROBE_CONTROL:
					wIndex = 0X0001;
					wLength = PROBE_COMMIT_CUSTOM_SIZE;
					break;

				case VS_COMMIT_CONTROL:
					wIndex = 0X0001;
					wLength = PROBE_COMMIT_CUSTOM_SIZE;
				break;

				default:
				break;
			}
		break;

		default:  break;
	}

	phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | \
	USB_REQ_TYPE_CLASS;

	phost->Control.setup.b.bRequest = USB_REQ_SET_CUR;
	phost->Control.setup.b.wValue.w = wValue;
	phost->Control.setup.b.wIndex.w = wIndex;
	phost->Control.setup.b.wLength.w = wLength;
	return(USBH_CtlReq(phost, (uint8_t*) data , wLength ));
}

USBH_StatusTypeDef USBH_CAM_GetCur(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType)
{
	uint16_t wValue,wIndex,wLength;
	wValue = (cont_sel << 8) & 0xFF00;

	//LIST ALL CONTROL SELECTORS THAT SUPPORT GET_CUR COMMAND
	switch(entityType)
	{
		case VIDEO_CONTROL:
			switch(cont_sel)
			{
				case VC_REQUEST_ERROR_CODE_CONTROL:
					wIndex = 0x0000;
					wLength = 1;
					break;

				default: break;
			}
		break;

		case CAMERA_TERMINAL:
			switch(cont_sel)
			{
				case CT_AE_MODE_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;

				case CT_AE_PRIORITY_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;

				case CT_FOCUS_AUTO_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;

				default: break;
			}
		break;

		case PROCESSING_UNIT:
			switch(cont_sel)
			{
				case PU_BACKLIGHT_COMPENSATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_BRIGHTNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_CONTRAST_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_GAIN_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_POWER_LINE_FREQUENCY_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_SATURATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SHARPNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				default: break;
			}
		break;

		case VS_INTERFACE:
			switch(cont_sel)
			{
				case VS_PROBE_CONTROL:
					wIndex = 0X0001;
					wLength = PROBE_COMMIT_CUSTOM_SIZE;
				break;

				case VS_COMMIT_CONTROL:
					wIndex = 0X0001;
					wLength = PROBE_COMMIT_CUSTOM_SIZE;
				break;

				case VS_STREAM_ERROR_CODE_CONTROL:
					wIndex = 0x0001;
					wLength = 1;
				break;

				default: break;
			}
		break;
	}

	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
	USB_REQ_TYPE_CLASS;

	phost->Control.setup.b.bRequest = USB_REQ_GET_CUR;
	phost->Control.setup.b.wValue.w = wValue;
	phost->Control.setup.b.wIndex.w = wIndex;
	phost->Control.setup.b.wLength.w = wLength;

	return(USBH_CtlReq(phost, phost->device.Data , wLength ));
}

USBH_StatusTypeDef USBH_CAM_GetMax(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType)
{
	uint16_t wValue,wIndex,wLength;
	wValue = (cont_sel << 8) & 0xFF00;

	//LIST ALL CONTROL SELECTORS THAT SUPPORT GET_CUR COMMAND
	switch(entityType)
	{
		case PROCESSING_UNIT:
			switch(cont_sel)
			{
				case PU_BACKLIGHT_COMPENSATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_BRIGHTNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_CONTRAST_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_GAIN_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SATURATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SHARPNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;
			}
		break;

		case VS_INTERFACE:
			switch(cont_sel)
			{
				case VS_PROBE_CONTROL:
					wIndex = 0X0001;
					wLength = PROBE_COMMIT_CUSTOM_SIZE;
				break;

				default:
				break;
			}
		break;
	}

	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
	USB_REQ_TYPE_CLASS;

	phost->Control.setup.b.bRequest = USB_REQ_GET_MAX;
	phost->Control.setup.b.wValue.w = wValue;
	phost->Control.setup.b.wIndex.w = wIndex;
	phost->Control.setup.b.wLength.w = wLength;

	return(USBH_CtlReq(phost, phost->device.Data , wLength ));
}

USBH_StatusTypeDef USBH_CAM_GetMin(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType)
{
	uint16_t wValue,wIndex,wLength;
	wValue = (cont_sel << 8) & 0xFF00;

	//LIST ALL CONTROL SELECTORS THAT SUPPORT GET_CUR COMMAND
	switch(entityType)
	{
		case PROCESSING_UNIT:
			switch(cont_sel)
			{
				case PU_BACKLIGHT_COMPENSATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_BRIGHTNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_CONTRAST_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_GAIN_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SATURATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SHARPNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;
			}
		break;

		case VS_INTERFACE:
			switch(cont_sel)
			{
				case VS_PROBE_CONTROL:
					wIndex = 0X0001;
					wLength = PROBE_COMMIT_CUSTOM_SIZE;
				break;

				default:
				break;
			}
		break;
	}

	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
	USB_REQ_TYPE_CLASS;

	phost->Control.setup.b.bRequest = USB_REQ_GET_MIN;
	phost->Control.setup.b.wValue.w = wValue;
	phost->Control.setup.b.wIndex.w = wIndex;
	phost->Control.setup.b.wLength.w = wLength;

	return(USBH_CtlReq(phost, phost->device.Data , wLength ));
}

USBH_StatusTypeDef USBH_CAM_GetDef(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType)
{
	uint16_t wValue,wIndex,wLength;
	wValue = (cont_sel << 8) & 0xFF00;

	//LIST ALL CONTROL SELECTORS THAT SUPPORT GET_CUR COMMAND
	switch(entityType)
	{
		case CAMERA_TERMINAL:
			switch(cont_sel)
			{
				case CT_AE_MODE_CONTROL:
					wIndex = 0x0100;
					wValue = (cont_sel << 8) & 0xFF00;
				break;

				case CT_FOCUS_AUTO_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;
			}
		break;

		case PROCESSING_UNIT:
			switch(cont_sel)
			{
				case PU_BACKLIGHT_COMPENSATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_BRIGHTNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_CONTRAST_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_GAIN_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_POWER_LINE_FREQUENCY_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_SATURATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SHARPNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;
			}
		break;

		case VS_INTERFACE:
			switch(cont_sel)
			{
				case VS_PROBE_CONTROL:
					wIndex = 0X0001;
					wLength = PROBE_COMMIT_CUSTOM_SIZE;
				break;

				default:
				break;
			}
		break;
	}

	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
	USB_REQ_TYPE_CLASS;

	phost->Control.setup.b.bRequest = USB_REQ_GET_DEF;
	phost->Control.setup.b.wValue.w = wValue;
	phost->Control.setup.b.wIndex.w = wIndex;
	phost->Control.setup.b.wLength.w = wLength;

	return(USBH_CtlReq(phost, phost->device.Data , wLength ));
}

USBH_StatusTypeDef USBH_CAM_GetInfo(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType)
{
	uint16_t wValue,wIndex,wLength;
	wValue = (cont_sel << 8) & 0xFF00;

	//LIST ALL CONTROL SELECTORS THAT SUPPORT GET_CUR COMMAND
	switch(entityType)
	{
		case CAMERA_TERMINAL:
			switch(cont_sel)
			{
				case CT_AE_MODE_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;

				case CT_AE_PRIORITY_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;

				case CT_FOCUS_AUTO_CONTROL:
					wIndex = 0x0100;
					wLength = 1;
				break;
			}
		break;

		case PROCESSING_UNIT:
			switch(cont_sel)
			{
				case PU_BACKLIGHT_COMPENSATION_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_BRIGHTNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_CONTRAST_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_GAIN_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_POWER_LINE_FREQUENCY_CONTROL:
					wIndex = 0X0400;
					wLength = 1;
				break;

				case PU_SATURATION_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_SHARPNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;

				case PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;
			}
		break;

		case VS_INTERFACE:
			switch(cont_sel)
			{
				case VS_PROBE_CONTROL:
					wIndex = 0X0001;
					wLength = 1;
				break;

				case VS_COMMIT_CONTROL:
					wIndex = 0X0001;
					wLength = 1;
				break;

				case VS_STREAM_ERROR_CODE_CONTROL:
					wIndex = 0x0001;
					wLength = 1;
				break;

				default :
				break;
			}
		break;
	}

	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
	USB_REQ_TYPE_CLASS;

	phost->Control.setup.b.bRequest = USB_REQ_GET_INFO;
	phost->Control.setup.b.wValue.w = wValue;
	phost->Control.setup.b.wIndex.w = wIndex;
	phost->Control.setup.b.wLength.w = wLength;

	return(USBH_CtlReq(phost, phost->device.Data , wLength ));
}

USBH_StatusTypeDef USBH_CAM_GetLen(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType)
{
	uint16_t wValue,wIndex,wLength;
	wValue = (cont_sel << 8) & 0xFF00;

	//LIST ALL CONTROL SELECTORS THAT SUPPORT GET_CUR COMMAND
	switch(entityType)
	{
		case PROCESSING_UNIT:
			switch(cont_sel)
			{
				case PU_BACKLIGHT_COMPENSATION_CONTROL:
					wIndex = 0X0300;
					wLength = 1;
				break;
			}
		break;

		case VS_INTERFACE:
			switch(cont_sel)
			{
				case VS_PROBE_CONTROL:
					wIndex = 0X0001;
					wLength = 1;
				break;

				case VS_COMMIT_CONTROL:
					wIndex = 0X0001;
					wLength = 1;
				break;

				default :
				break;
			}
		break;
	}

	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
	USB_REQ_TYPE_CLASS;

	phost->Control.setup.b.bRequest = USB_REQ_GET_LEN;
	phost->Control.setup.b.wValue.w = wValue;
	phost->Control.setup.b.wIndex.w = wIndex;
	phost->Control.setup.b.wLength.w = wLength;

	return(USBH_CtlReq(phost, phost->device.Data , wLength ));
}

USBH_StatusTypeDef USBH_CAM_GetRes(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType)
{
	uint16_t wValue,wIndex,wLength;
	wValue = (cont_sel << 8) & 0xFF00;

	//LIST ALL CONTROL SELECTORS THAT SUPPORT GET_CUR COMMAND
	switch(entityType)
	{
		case CAMERA_TERMINAL:
			switch(cont_sel)
			{
				case CT_AE_MODE_CONTROL:
					wIndex = 0x0100;
					wValue = (cont_sel << 8) & 0xFF00;
				break;
			}
		break;

		case PROCESSING_UNIT:
			switch(cont_sel)
			{
				case PU_BACKLIGHT_COMPENSATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_BRIGHTNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_CONTRAST_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_GAIN_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SATURATION_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;

				case PU_SHARPNESS_CONTROL:
					wIndex = 0X0300;
					wLength = 2;
				break;
			}
		break;

		case VS_INTERFACE:
			switch(cont_sel)
			{
				case VS_PROBE_CONTROL:
					wIndex = 0X0001;
					wLength = PROBE_COMMIT_CUSTOM_SIZE;
				break;

				default:
				break;
			}
		break;
	}

	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
	USB_REQ_TYPE_CLASS;

	phost->Control.setup.b.bRequest = USB_REQ_GET_RES;
	phost->Control.setup.b.wValue.w = wValue;
	phost->Control.setup.b.wIndex.w = wIndex;
	phost->Control.setup.b.wLength.w = wLength;

	return(USBH_CtlReq(phost, phost->device.Data , wLength ));
}

USBH_StatusTypeDef USBH_CAM_ConfigDevice(USBH_HandleTypeDef* phost, CAM_HandleTypeDef* CAM_Handle)
{
	USBH_StatusTypeDef status = USBH_BUSY;
	USBH_StatusTypeDef testStatus = USBH_BUSY;

	switch(CAM_Handle->cfg_state)
	{
		case CAM_REQ_CFG_INIT:
			CAM_Handle->cfg_state = CAM_REQ_SET_PARAMS;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
    		LCD_UsrLog("cfg INIt");
			break;

		case CAM_REQ_CFG_IDLE:
			break;

		case CAM_REQ_GET_PARAMS:
			if(USBH_CAM_GetParamRequests(phost, CAM_Handle) == USBH_OK)
			{
				CAM_Handle->cfg_state = CAM_REQ_SET_PARAMS;
			}
			break;

		case CAM_REQ_SET_PARAMS:
			if(USBH_CAM_SetParamRequests(phost, CAM_Handle) == USBH_OK)
			{
				CAM_Handle->cfg_state = CAM_REQ_NEGOCIATE;
				LCD_UsrLog("cfg set params");
			}
		break;

		case CAM_REQ_NEGOCIATE:
			if(USBH_CAM_NegociateRequests(phost, CAM_Handle) == USBH_OK)
			{
				status = USBH_OK;
				LCD_UsrLog("cfg negotiate");
			}
			break;

		default: break;
	}
	return status;
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/


/**
* @}
*/


/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
