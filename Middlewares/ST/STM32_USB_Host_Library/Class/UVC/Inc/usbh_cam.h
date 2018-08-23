// DISCLAIMER : User version copied from usbh_hid.h

/**
  ******************************************************************************
  * @file    usbh_CAM.h
  * @author  MCD Application Team
  * @version V3.2.2
  * @date    07-July-2015
  * @brief   This file contains all the prototypes for the usbh_CAM.c
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

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_CAM_H
#define __USBH_CAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "lcd_log.h"
#include "usbh_cam_def.h"
#include "../CMSIS_RTOS/cmsis_os.h" //fixme this include should not be here as it is already specified in the project config
 
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
  * @brief This file is the Header file for usbh_CAM.c
  * @{
  */ 

/** @defgroup USBH_CAM_CORE_Exported_Defines
* @{
*/
#define USB_CAM_GET_IDLE         	    0x02
#define USB_CAM_GET_PROTOCOL      	  	0x03
#define USB_CAM_SET_IDLE             	0x0A


/* CAM Class Codes */
#define USB_CAM_CLASS                   0x0E

/* Interface Descriptor field values for CAM video streaming */
#define CAM_ROOT_CODE					0x00 //Base interface
#define CAM_STREAMING_CODE              0x00 //Video streaming interface

/* Class-Specific Request bitmaps */
//auto exposure mode control
#define AUTO_EXPOSURE_MODE_MAN			0x01
#define AUTO_EXPOSURE_MODE_AUTO			0x02	//<- use this
#define AUTO_EXPOSURE_MODE_SHUTTER		0x04
#define AUTO_EXPOSURE_MODE_APERTURE 	0x08

//auto exposure priority control
#define AUTO_EXPOSURE_PRIO_CONST		0x00 	//<- use this. Means that the frame rate must remain constant
#define AUTO_EXPOSURE_PRIO_DYN			0x01

//exposure time absolute
//do not use this control if the AUTO_EXPOSURE_MODE_AUTO is set. Default case
#define EXPOSURE_TIME_ABS				0x0000 	//some value between 1(0.0001 sec) and 100 000 (10 sec) aperture

//focus auto control
#define AUTO_FOCUS_OFF					0x00
#define AUTO_FOCUS_ON					0x01 	//<- use this

//white balance temperature auto control
#define WHITE_BAL_TEMP_AUTO				0x01 	//<- use this

/* Probe control structure size */
#define PROBE_CONTROL11_SIZE			0x22
#define PROBE_CONTROL15_SIZE			0x30
#define PROBE_COMMIT_CUSTOM_SIZE		0x1A

/* Maximum size of returned arguments for a get_cur request during stream negotiation */
#define MAX_GET_SIZE					0x04
#define MAX_VS_INTERFACE_CNT			0x10
/**
  * @}
  */

/** @defgroup USBH_CAM_CORE_Exported_Types
  * @{
  */ 

/* States for CAM State Machine */
typedef enum
{
  CAM_INIT= 0,
  CAM_CONFIG,
  CAM_IDLE,
  CAM_POLL,
  CAM_HANDLE_INT,
  CAM_GET_STREAM,
  CAM_GET_STREAM_START,
  CAM_SUSPEND_STREAM,
  CAM_TRANSMIT_IMG,
  CAM_ERROR
}
CAM_StateTypeDef;

//State machine to do the configuration of the device
typedef enum
{
  CAM_REQ_IDLE = 0,
  CAM_REQ_INIT
}
CAM_CtlStateTypeDef;

typedef enum
{
	CAM_REQ_CFG_INIT = 0,
	CAM_REQ_CFG_IDLE,
	CAM_REQ_SET_PARAMS,
	CAM_REQ_GET_PARAMS,
	CAM_REQ_NEGOCIATE
}
CAM_CfgStateTypeDef;

typedef enum{
	CAM_REQ_GET_INIT,
	CAM_REQ_BRIGHTNESS_DEF,
	CAM_REQ_BRIGHTNESS_MIN,
	CAM_REQ_BRIGHTNESS_MAX,
	CAM_REQ_BRIGHTNESS_SET_CUR,
	CAM_REQ_BRIGHTNESS_GET_CUR,
	CAM_REQ_CONTRAST_DEF,
	CAM_REQ_CONTRAST_MIN,
	CAM_REQ_CONTRAST_MAX,
	CAM_REQ_CONTRAST_CUR,
	CAM_REQ_SATURATION_DEF,
	CAM_REQ_SATURATION_MIN,
	CAM_REQ_SATURATION_MAX,
	CAM_REQ_SATURATION_CUR,
	CAM_REQ_SHARPNESS_DEF,
	CAM_REQ_SHARPNESS_MIN,
	CAM_REQ_SHARPNESS_MAX,
	CAM_REQ_SHARPNESS_CUR,
	CAM_REQ_BACK_COMP,
	CAM_REQ_GAIN,
	CAM_REQ_PWRFREQ,
	CAM_REQ_TEST_ERROR
}CAM_GetParamStateTypeDef;

typedef enum{
	CAM_REQ_SET_INIT,
	CAM_REQ_AE_MODE,
	CAM_REQ_AE_PRIO,
	CAM_REQ_FOCUS,
	CAM_REQ_WHITE_BAL,
	CAM_REQ_ERROR
}CAM_SetParamStateTypeDef;

typedef enum{
	CAM_REQ_STREAM_INIT,
	CAM_REQ_SET_PROBE,
	CAM_REQ_SET_PROBE_WAIT,
	CAM_REQ_GET_PROBE,
	CAM_REQ_GET_PROBE_WAIT,
	CAM_REQ_SET_COMMIT,
	CAM_REQ_SET_COMMIT_WAIT,
	CAM_REQ_SET_INTERFACE,
	CAM_REQ_SET_INTERFACE_WAIT
}CAM_StreamNegStateTypeDef;


typedef struct _processingUnitControlVals
{
	uint16_t defVal;
	uint16_t minVal;
	uint16_t maxVal;
	uint16_t curVal;
}puControlTypedef;

/* Stream negotiation data structures for the USB revisions 1.1 and 1.5 */
//stream negotiation probe control USB Revision 1.5
//set_cur probe control

typedef union _customProbeData
{
		uint8_t data[26];

	struct __attribute__((packed, aligned(4))){
		uint16_t bmHint; 						//Dunno yet maybe keep the frame interval constant. Let's see for the others
		uint8_t bFormatIndex; 					//Video Streaming MJPEG Format Type Descriptor
		uint8_t bFrameIndex; 					//Video Streaming MJPEG Frame Type Descriptor 640x480
		uint32_t bFrameInterval; 				//33.333300 mSec (30.00 Hz)
		uint16_t wKeyFrameRate; 				//not supported by the webcam
		uint16_t wPFrameRate; 					//not supported by the webcam
		uint16_t wCompQuality; 					//set to 500 (middle value) provisionally -> let's see what the GET_DEF value is
		uint16_t wCompWindowSize; 				//not supported by the webcam
		uint16_t wDelay; 						//read by the host
		uint32_t dwMaxVideoFrameSize;			//read by the host should be 0x00096000 default
		uint32_t dwMaxPayloadTransferSize;		//read by the host
	}params;

}ProbeData_Typedef;

/* Structure for CAM process */
//TODO finish characterising this struct in order to give a good representation of the Webcam
typedef struct _CAM_Process
{
  CAM_CtlStateTypeDef  			ctl_state;
  CAM_CfgStateTypeDef			cfg_state;
  CAM_GetParamStateTypeDef		getParam_state;
  CAM_SetParamStateTypeDef		setParam_state;
  CAM_StreamNegStateTypeDef		streamNeg_state;
  CAM_StateTypeDef    	 		state;
  ProbeData_Typedef 			probeSet;
  uint16_t            	 		streamLength;
  uint8_t						interruptLength;
  uint8_t __attribute__((aligned(4))) pImgData[0x320]; //Allocate this dynamically in the future
  uint32_t 						headerOffset;
  uint32_t						offset;
  uint32_t						transferCnt;
  uint8_t             	 		streamPipe;
  uint8_t             	 		streamEp;
  uint8_t              			streamEp_addr;
  uint32_t					 	param;
  uint32_t 						streamPoll;
  uint32_t						timer;
  uint8_t						stream_interface;
  uint8_t						stream_altSetting;
}
CAM_HandleTypeDef;

typedef struct _bGetValues
{
	puControlTypedef        brightness;
	puControlTypedef  		contrast;
	puControlTypedef  		saturation;
	puControlTypedef		sharpness;
	uint16_t				backlight_comp;
	uint16_t				gain;
	uint8_t					pwrline_freq;
}bGetValues;

/**
  * @}
  */ 

/** @defgroup USBH_CAM_CORE_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_CAM_CORE_Exported_Variables
  * @{
  */ 
extern USBH_ClassTypeDef  CAM_Class;
#define USBH_CAM_CLASS    &CAM_Class
/**
  * @}
  */ 

/** @defgroup USBH_CAM_CORE_Exported_FunctionsPrototype
  * @{
  */ 
void USBH_CAM_EventCallback						(USBH_HandleTypeDef *phost);
void USBH_CAM_InitProbeSet						(ProbeData_Typedef* probeCtrl);
uint8_t USBH_CAM_SetStreamInterface				(USBH_HandleTypeDef* phost, uint32_t maxPayloadTranferSize);

USBH_StatusTypeDef USBH_CAM_ConfigDevice		(USBH_HandleTypeDef* phost, CAM_HandleTypeDef *CAM_Handle);
USBH_StatusTypeDef USBH_CAM_GetParamRequests	(USBH_HandleTypeDef* phost, CAM_HandleTypeDef* CAM_Handle);
USBH_StatusTypeDef USBH_CAM_SetParamRequests	(USBH_HandleTypeDef* phost, CAM_HandleTypeDef* CAM_Handle);
USBH_StatusTypeDef USBH_CAM_NegociateRequests	(USBH_HandleTypeDef* phost, CAM_HandleTypeDef* CAM_Handle);
USBH_StatusTypeDef USBH_CSRequest 				(USBH_HandleTypeDef *phost, uint8_t requ, uint8_t cont_sel, uint8_t entityType);
USBH_StatusTypeDef USBH_CAM_SetCur	(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType);
USBH_StatusTypeDef USBH_CAM_GetCur	(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType);
USBH_StatusTypeDef USBH_CAM_GetMax 	(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType);
USBH_StatusTypeDef USBH_CAM_GetMin 	(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType);
USBH_StatusTypeDef USBH_CAM_GetDef 	(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType);
USBH_StatusTypeDef USBH_CAM_GetInfo (USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType);
USBH_StatusTypeDef USBH_CAM_GetLen 	(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType);
USBH_StatusTypeDef USBH_CAM_GetRes 	(USBH_HandleTypeDef* phost, uint8_t cont_sel, uint8_t entityType);


/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __USBH_CAM_H */

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

