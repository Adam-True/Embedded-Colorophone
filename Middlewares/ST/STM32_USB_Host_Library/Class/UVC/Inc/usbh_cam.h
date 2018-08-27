/*
 * usbh_cam.h
 *
 *  Created on: 13 juin 2018
 *      Author: Colin Cina
 */

/** @file usbh_cam.h */

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
#include "cmsis_os.h"
 

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
 *	\enum CAM_StateTypeDef
 *	\brief States for CAM State Machine
 */
typedef enum
{
  CAM_INIT= 0,			/**< Text here */
  CAM_CONFIG,			/**< Text here */
  CAM_IDLE,				/**< Text here */
  CAM_POLL,				/**< Text here */
  CAM_HANDLE_INT,		/**< Text here */
  CAM_GET_STREAM,		/**< Text here */
  CAM_GET_STREAM_START,	/**< Text here */
  CAM_SUSPEND_STREAM,	/**< Text here */
  CAM_TRANSMIT_IMG,		/**< Text here */
  CAM_ERROR				/**< Text here */
}
CAM_StateTypeDef;


/**
 *	\enum CAM_CtlStateTypeDef
 *	\brief State machine to do the configuration of the device
 */
typedef enum
{
  CAM_REQ_IDLE = 0,		/**< Text here */
  CAM_REQ_INIT			/**< Text here */
}
CAM_CtlStateTypeDef;


/**
 *	\enum CAM_CfgStateTypeDef
 *	\brief State machine to do the configuration of the device
 */
typedef enum
{
	CAM_REQ_CFG_INIT = 0,	/**< Text here */
	CAM_REQ_CFG_IDLE,		/**< Text here */
	CAM_REQ_SET_PARAMS,		/**< Text here */
	CAM_REQ_GET_PARAMS,		/**< Text here */
	CAM_REQ_NEGOCIATE		/**< Text here */
}
CAM_CfgStateTypeDef;


/**
 *	\enum CAM_GetParamStateTypeDef
 *	\brief States to get values from the device
 */
typedef enum{
	CAM_REQ_GET_INIT,			/**< Text here */
	CAM_REQ_BRIGHTNESS_DEF,		/**< Text here */
	CAM_REQ_BRIGHTNESS_MIN,		/**< Text here */
	CAM_REQ_BRIGHTNESS_MAX,		/**< Text here */
	CAM_REQ_BRIGHTNESS_SET_CUR,	/**< Text here */
	CAM_REQ_BRIGHTNESS_GET_CUR,	/**< Text here */
	CAM_REQ_CONTRAST_DEF,		/**< Text here */
	CAM_REQ_CONTRAST_MIN,		/**< Text here */
	CAM_REQ_CONTRAST_MAX,		/**< Text here */
	CAM_REQ_CONTRAST_CUR,		/**< Text here */
	CAM_REQ_SATURATION_DEF,		/**< Text here */
	CAM_REQ_SATURATION_MIN,		/**< Text here */
	CAM_REQ_SATURATION_MAX,		/**< Text here */
	CAM_REQ_SATURATION_CUR,		/**< Text here */
	CAM_REQ_SHARPNESS_DEF,		/**< Text here */
	CAM_REQ_SHARPNESS_MIN,		/**< Text here */
	CAM_REQ_SHARPNESS_MAX,		/**< Text here */
	CAM_REQ_SHARPNESS_CUR,		/**< Text here */
	CAM_REQ_BACK_COMP,			/**< Text here */
	CAM_REQ_GAIN,				/**< Text here */
	CAM_REQ_PWRFREQ,			/**< Text here */
	CAM_REQ_TEST_ERROR			/**< Text here */
}CAM_GetParamStateTypeDef;


/**
 *	\enum CAM_SetParamStateTypeDef
 *	\brief States to set the parameters of the device
 */
typedef enum{
	CAM_REQ_SET_INIT,		/**< Text here */
	CAM_REQ_AE_MODE,		/**< Text here */
	CAM_REQ_AE_PRIO,		/**< Text here */
	CAM_REQ_FOCUS,			/**< Text here */
	CAM_REQ_WHITE_BAL,		/**< Text here */
	CAM_REQ_PWR,			/**< Text here */
	CAM_REQ_ERROR			/**< Text here */
}CAM_SetParamStateTypeDef;


/**
 *	\enum CAM_StreamNegStateTypeDef
 *	\brief States to negotiate the streaming parameters
 */
typedef enum{
	CAM_REQ_STREAM_INIT,		/**< Text here */
	CAM_REQ_SET_PROBE,			/**< Text here */
	CAM_REQ_SET_PROBE_WAIT,		/**< Text here */
	CAM_REQ_GET_PROBE,			/**< Text here */
	CAM_REQ_GET_PROBE_WAIT,		/**< Text here */
	CAM_REQ_SET_COMMIT,			/**< Text here */
	CAM_REQ_SET_COMMIT_WAIT,	/**< Text here */
	CAM_REQ_SET_INTERFACE,		/**< Text here */
	CAM_REQ_SET_INTERFACE_WAIT	/**< Text here */
}CAM_StreamNegStateTypeDef;


/**
 *	\struct puControlTypedef
 *	\brief Contains values that are possible to measure for a device unit.
 */
typedef struct _processingUnitControlVals
{
	uint16_t defVal; 	/**< Text here */
	uint16_t minVal;	/**< Text here */
	uint16_t maxVal;	/**< Text here */
	uint16_t curVal;	/**< Text here */
}puControlTypedef;


/**
 *	\union ProbeData_Typedef
 *	\brief Union that contains the parameters for the stream negotiation according to the USB Revision 1.5.
 *	It can be viewed as a struct or an array of bytes
 */
typedef union _customProbeData
{
		uint8_t data[26];

		/**
		 * 	\struct
		 * 	\brief Parameters for the probe control unit
		 */
	struct __attribute__((packed, aligned(4))){
		uint16_t bmHint; 						/**< Dunno yet maybe keep the frame interval constant. Let's see for the others*/
		uint8_t bFormatIndex; 					/**< Video Streaming Format Type Descriptor */
		uint8_t bFrameIndex; 					/**< Video Streaming Frame Type Descriptor */
		uint32_t bFrameInterval; 				/**< Frame interval*/
		uint16_t wKeyFrameRate; 				/**< Not supported by the Logitech C920 webcam*/
		uint16_t wPFrameRate; 					/**< Not supported by the Logitech C920 webcam*/
		uint16_t wCompQuality; 					/**< Compression quality. Range is 1 to 1000*/
		uint16_t wCompWindowSize; 				/**< Not supported by the Logitech C920 Webcam*/
		uint16_t wDelay; 						/**< Read by the host*/
		uint32_t dwMaxVideoFrameSize;			/**< Read by the host*/
		uint32_t dwMaxPayloadTransferSize;		/**< Read by the host*/
	}params;

}ProbeData_Typedef;


/**
 *	\struct CAM_HandleTypeDef
 *	\brief Describing the UVC device driver. This struct hold the main variable necessary for stream pipe creation, stream negociation and so on.
 */

///\todo cleanup the unecessary elements of this struct
typedef struct _CAM_Process
{
  CAM_CtlStateTypeDef  			ctl_state;			/**< Text here */
  CAM_CfgStateTypeDef			cfg_state;			/**< Text here */
  CAM_GetParamStateTypeDef		getParam_state;		/**< Text here */
  CAM_SetParamStateTypeDef		setParam_state;		/**< Text here */
  CAM_StreamNegStateTypeDef		streamNeg_state;	/**< Text here */
  CAM_StateTypeDef    	 		state;				/**< Text here */
  ProbeData_Typedef 			probeSet;			/**< Text here */
  uint16_t            	 		streamLength;		/**< Text here */
  uint8_t						interruptLength;	/**< Text here */
  uint8_t __attribute__((aligned(4))) pImgData[0x320]; 	/**< Text here *///\Fixme Allocate this dynamically in the future
  uint32_t 						headerOffset;			/**< Text here */
  uint32_t						offset;					/**< Text here */
  uint32_t						transferCnt;			/**< Text here */
  uint8_t             	 		streamPipe;				/**< Text here */
  uint8_t             	 		streamEp;				/**< Text here */
  uint8_t              			streamEp_addr;			/**< Text here */
  uint32_t					 	param;					/**< Text here */
  uint32_t 						streamPoll;				/**< Text here */
  uint8_t						stream_interface;		/**< Text here */
  uint8_t						stream_altSetting;		/**< Text here */
}
CAM_HandleTypeDef;


/**
 *	\struct bGetValues
 *	\brief Struct holding the values that will be measured. Used for testing the boundary and default values of some units of the device.
 */
typedef struct _bGetValues
{
	puControlTypedef        brightness;		/**< Text here */
	puControlTypedef  		contrast;		/**< Text here */
	puControlTypedef  		saturation;		/**< Text here */
	puControlTypedef		sharpness;		/**< Text here */
	uint16_t				backlight_comp;	/**< Text here */
	uint16_t				gain;			/**< Text here */
	uint8_t					pwrline_freq;	/**< Text here */
}bGetValues;


extern USBH_ClassTypeDef  CAM_Class;
#define USBH_CAM_CLASS    &CAM_Class


/**
  * @brief  USBH_CAM_InterfaceInit
  *         The function init the CAM class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_InterfaceInit  	(USBH_HandleTypeDef *phost);

/**
  * @brief  USBH_CAM_InterfaceDeInit
  *         The function DeInit the Pipes used for the CAM class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_InterfaceDeInit  (USBH_HandleTypeDef *phost);

/**
  * @brief  USBH_CAM_ClassRequest
  *         The function is responsible for handling Standard requests
  *         for CAM class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_ClassRequest		(USBH_HandleTypeDef *phost);

/**
  * @brief  USBH_CAM_Process
  *         The function is for managing state machine for CAM data transfers
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_Process			(USBH_HandleTypeDef *phost);

/**
  * @brief  USBH_CAM_SOFProcess
  *         The function is for managing the SOF Process
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CAM_SOFProcess		(USBH_HandleTypeDef *phost);


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


#ifdef __cplusplus
}
#endif

#endif /* __USBH_CAM_H */


