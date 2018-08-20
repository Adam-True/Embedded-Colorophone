/*
_ * usbh_cam_def.h
 *
 *  Created on: 13 juin 2018
 *      Author: Colin Cina
 */


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CLASS
  * @{
  */

/** @addtogroup USBH_LIB_CLASS_CAM
  * @{
  */

/** @defgroup USBH_CAM_DEF
  * @brief This file is includes USB descriptors
  * @{
  */

#ifndef ST_STM32_USB_HOST_LIBRARY_CLASS_CAM_USBH_CAM_DEF_H_
#define ST_STM32_USB_HOST_LIBRARY_CLASS_CAM_USBH_CAM_DEF_H_

/** @defgroup USBH_CLASS_CAM_Exported_Macros
  * @{
  */

#define  CS(val)				  (uint16_t)((val << 8) & 0xFF00)

/** @defgroup USBH_CLASS_CAM_Exported_Defines
  * @{
  */

/* Class Specific Request Codes */
/* bRequest , Value */
#define  USB_REQ_SET_CUR								0x01

#define  USB_REQ_GET_CUR								0x81
#define  USB_REQ_GET_MIN								0x82
#define  USB_REQ_GET_MAX								0x83
#define  USB_REQ_GET_RES								0x84
#define  USB_REQ_GET_LEN								0x85
#define  USB_REQ_GET_INFO								0x86
#define  USB_REQ_GET_DEF								0x87

/*Video Class Specific Control Selectors */

/* Control Selectors Indexes */
#define VIDEO_CONTROL									0x00
#define TERMINAL										0x01
#define SELECTOR_UNIT									0x02
#define CAMERA_TERMINAL									0x03
#define PROCESSING_UNIT									0x04
#define ENCODING_UNIT									0x05
#define EXTENTION_UNIT									0x06
#define VS_INTERFACE									0x07

//VIDEO CONTROL INTERFACE CS
#define VC_CONTROL_UNDEFINED							0x00
#define VC_VIDEO_POWER_MODE_CONTROL						0x01
#define VC_REQUEST_ERROR_CODE_CONTROL					0x02

//TERMINAL CS
#define TE_CONTROL_UNDEFINED							0x00

//SELECTOR UNIT CS
#define SU_CONTROL_UNDEFINED							0x00
#define SU_INPUT_SELECT_CONTROL							0x01

//CAMERA TERMINAL CS
#define CT_CONTROL_UNDEFINED 							0x00
#define CT_SCANNING_MODE_CONTROL 						0x01
#define CT_AE_MODE_CONTROL								0x02
#define CT_AE_PRIORITY_CONTROL							0x03
#define CT_EXPOSURE_TIME_ABSOLUTE_CONTROL				0x04
#define CT_EXPOSURE_TIME_RELATIVE_CONTROL				0x05
#define CT_FOCUS_ABSOLUTE_CONTROL						0x06
#define CT_FOCUS_RELATIVE_CONTROL						0x07
#define CT_FOCUS_AUTO_CONTROL							0x08
#define CT_IRIS_ABSOLUTE_CONTROL						0x09
#define CT_IRIS_RELATIVE_CONTROL						0x0A
#define CT_ZOOM_ABSOLUTE_CONTROL						0x0B
#define CT_ZOOM_RELATIVE_CONTROL						0x0C
#define CT_PANTILT_ABSOLUTE_CONTROL						0x0D
#define CT_PANTILT_RELATIVE_CONTROL						0x0E
#define CT_ROLL_ABSOLUTE_CONTROL						0x0F
#define CT_ROLL_RELATIVE_CONTROL						0x10
#define CT_PRIVACY_CONTROL								0x11
#define CT_FOCUS_SIMPLE_CONTROL							0x12
#define CT_WINDOW_CONTROL								0x13
#define CT_REGION_OF_INTEREST_CONTROL					0x14

//PROCESSING UNIT CS
#define PU_CONTROL_UNDEFINED							0x00
#define PU_BACKLIGHT_COMPENSATION_CONTROL				0x01
#define PU_BRIGHTNESS_CONTROL							0x02
#define PU_CONTRAST_CONTROL 							0x03
#define PU_GAIN_CONTROL 								0x04
#define PU_POWER_LINE_FREQUENCY_CONTROL 				0x05
#define PU_HUE_CONTROL 									0x06
#define PU_SATURATION_CONTROL 							0x07
#define PU_SHARPNESS_CONTROL 							0x08
#define PU_GAMMA_CONTROL 								0x09
#define PU_WHITE_BALANCE_TEMPERATURE_CONTROL 			0x0A
#define PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL 		0x0B
#define PU_WHITE_BALANCE_COMPONENT_CONTROL 				0x0C
#define PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL 		0x0D
#define PU_DIGITAL_MULTIPLIER_CONTROL 					0x0E
#define PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL 			0x0F
#define PU_HUE_AUTO_CONTROL 							0x10
#define PU_ANALOG_VIDEO_STANDARD_CONTROL 				0x11
#define PU_ANALOG_LOCK_STATUS_CONTROL 					0x12
#define PU_CONTRAST_AUTO_CONTROL 						0x13


//ENCODING UNIT CS
#define EU_CONTROL_UNDEFINED 							0x00
#define EU_SELECT_LAYER_CONTROL 						0x01
#define EU_PROFILE_TOOLSET_CONTROL 						0x02
#define EU_VIDEO_RESOLUTION_CONTROL 					0x03
#define EU_MIN_FRAME_INTERVAL_CONTROL 					0x04
#define EU_SLICE_MODE_CONTROL 							0x05
#define EU_RATE_CONTROL_MODE_CONTROL 					0x06
#define EU_AVERAGE_BITRATE_CONTROL 						0x07
#define EU_CPB_SIZE_CONTROL 							0x08
#define EU_PEAK_BIT_RATE_CONTROL 						0x09
#define EU_QUANTIZATION_PARAMS_CONTROL 					0x0A
#define EU_SYNC_REF_FRAME_CONTROL 						0x0B
#define EU_LTR_BUFFER_CONTROL 							0x0C
#define EU_LTR_PICTURE_CONTROL 							0x0D
#define EU_LTR_VALIDATION_CONTROL 						0x0E
#define EU_LEVEL_IDC_LIMIT_CONTROL 						0x0F
#define EU_SEI_PAYLOADTYPE_CONTROL 						0x10
#define EU_QP_RANGE_CONTROL 							0x11
#define EU_PRIORITY_CONTROL 							0x12
#define EU_START_OR_STOP_LAYER_CONTROL 					0x13
#define EU_ERROR_RESILIENCY_CONTROL 					0x14

//EXTENSION UNIT CS
#define XU_CONTROL_UNDEFINED							0x00

//VIDEO STREAMING INTERFACE CS
#define VS_CONTROL_UNDEFINED 							0x00
#define VS_PROBE_CONTROL 								0x01
#define VS_COMMIT_CONTROL 								0x02
#define VS_STILL_PROBE_CONTROL 							0x03
#define VS_STILL_COMMIT_CONTROL 						0x04
#define VS_STILL_IMAGE_TRIGGER_CONTROL 					0x05
#define VS_STREAM_ERROR_CODE_CONTROL 					0x06
#define VS_GENERATE_KEY_FRAME_CONTROL 					0x07
#define VS_UPDATE_FRAME_SEGMENT_CONTROL 				0x08
#define VS_SYNCH_DELAY_CONTROL 							0x09

/* Video Class-Specific Descriptor Types */
#define  USB_DESC_TYPE_CS_UNDEFINED							0x20
#define  USB_DESC_TYPE_CS_DEVICE							0x21
#define  USB_DESC_TYPE_CS_CONFIGURATION						0x22
#define  USB_DESC_TYPE_CS_STRING							0x23
#define  USB_DESC_TYPE_CS_INTERFACE							0x24
#define  USB_DESC_TYPE_CS_ENDPOINT							0x25

/* Video device Class Codes */

/* Interface Class Codes */
#define CC_VIDEO											0x0E

/* Interface SubClass Codes */
#define SC_UNDEFINED										0x00
#define SC_VIDEOCONTROL										0x01
#define SC_VIDEOSTREAMING									0x02
#define SC_VIDEO_INTERFACE_COLLECTION						0x03

/* Interface Protocol Codes */
#define PC_PROTOCOL_UNDEFINED								0x00

/* Video Class-Specific VC Interface Descriptor Subtypes */
#define USB_DESC_SUBTYPE_VC_UNDEFINED						0x00
#define USB_DESC_SUBTYPE_VC_HEADER							0x01
#define USB_DESC_SUBTYPE_VC_INPUT_TERMINAL					0x02
#define USB_DESC_SUBTYPE_VC_OUTPUT_TERMINAL					0x03
#define USB_DESC_SUBTYPE_VC_SELECTOR_UNIT					0x04
#define USB_DESC_SUBTYPE_VC_PROCESSING_UNIT					0x05
#define USB_DESC_SUBTYPE_VC_EXTENSION_UNIT					0x06

/* Video Class-Specific VS Interface Descriptor Subtypes */
#define USB_DESC_SUBTYPE_VS_UNDEFINED						0x00
#define USB_DESC_SUBTYPE_VS_INPUT_HEADER					0x01
#define USB_DESC_SUBTYPE_VS_OUTPUT_HEADER					0x02
#define USB_DESC_SUBTYPE_VS_STILL_IMAGE_FRAME				0x03
#define USB_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED				0x04
#define USB_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED				0x05
#define USB_DESC_SUBTYPE_VS_FORMAT_MJPEG					0x06
#define USB_DESC_SUBTYPE_VS_FRAME_MJPEG						0x07
#define USB_DESC_SUBTYPE_VS_FORMAT_MPEG2TS					0x0A
#define USB_DESC_SUBTYPE_VS_FORMAT_DV						0x0C
#define USB_DESC_SUBTYPE_VS_COLORFORMAT						0x0D
#define USB_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED				0x10
#define USB_DESC_SUBTYPE_VS_FRAME_FRAME_BASED				0x11
#define USB_DESC_SUBTYPE_VS_FORMAT_STREAM_BASED				0x12

#define USB_DEVICE_DESC_SIZE                       			18
#define USB_CONFIGURATION_DESC_SIZE                			9
#define USB_HID_DESC_SIZE                                  	9
#define USB_INTERFACE_DESC_SIZE                            	9
#define USB_ENDPOINT_DESC_SIZE                             	7
#define USB_CAM_DESC_SIZE								   	36

/* Video Class-Specific VC Interface Descriptor Subtypes Sizes */
#define USB_DESC_VC_UNDEFINED_SIZE								0x00 //length unknown
#define USB_DESC_VC_HEADER_SIZE									0x0D
#define USB_DESC_VC_INPUT_TERMINAL_SIZE							0x12
#define USB_DESC_VC_OUTPUT_TERMINAL_SIZE						0x09
#define USB_DESC_VC_PROCESSING_UNIT_SIZE						0x0B
#define USB_DESC_VC_EXTENSION_UNIT_SIZE							0x1B
#define USB_DESC_VC_SELECTOR_UNIT_SIZE						0x00 //length unknown

/* Video Class-Specific VS Interface Descriptor Subtypes Sizes */
#define USB_DESC_VS_UNDEFINED_SIZE								0x00 //length unknown
#define USB_DESC_VS_INPUT_HEADER_SIZE							0x10
#define USB_DESC_VS_FORMAT_UNCOMPRESSED_SIZE					0x1B
#define USB_DESC_VS_FRAME_UNCOMPRESSED_SIZE						0x36
#define USB_DESC_VS_FORMAT_MJPEG_SIZE							0x0B
#define USB_DESC_VS_FRAME_MJPEG_SIZE							0x36
#define USB_DESC_VS_STILL_IMAGE_FRAME_SIZE						0x00 //length unknown
#define USB_DESC_VS_OUTPUT_HEADER_SIZE							0x00 //length unknown
#define USB_DESC_VS_FORMAT_MPEG2TS_SIZE							0x00 //length unknown
#define USB_DESC_VS_FORMAT_DV_SIZE								0x00 //length unknown
#define USB_DESC_VS_COLORFORMAT_SIZE							0x00 //length unknown
#define USB_DESC_VS_FORMAT_FRAME_BASED_SIZE						0x00 //length unknown
#define USB_DESC_VS_FRAME_FRAME_BASED_SIZE						0x00 //length unknown
#define USB_DESC_VS_FORMAT_STREAM_BASED_SIZE					0x00 //length unknown

/* Video Class-Specific Descriptor Types */
#define  USB_DESC_CS_UNDEFINED								((USB_DESC_TYPE_CS_UNDEFINED << 8) & 0xFF00)
#define  USB_DESC_CS_DEVICE									((USB_DESC_TYPE_CS_DEVICE << 8) & 0xFF00)
#define  USB_DESC_CS_CONFIGURATION							((USB_DESC_TYPE_CS_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_CS_STRING									((USB_DESC_TYPE_CS_STRING << 8) & 0xFF00)
#define  USB_DESC_CS_INTERFACE								((USB_DESC_TYPE_CS_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_CS_ENDPOINT								((USB_DESC_TYPE_CS_ENDPOINT << 8) & 0xFF00)

/* Video Class-Specific VC Interface Descriptor Subtypes */
#define USB_DESC_VC_UNDEFINED								((USB_DESC_SUBTYPE_VC_UNDEFINED << 8) & 0xFF00)
#define USB_DESC_VC_HEADER									((USB_DESC_SUBTYPE_VC_HEADER << 8) & 0xFF00)
#define USB_DESC_VC_INPUT_TERMINAL							((USB_DESC_SUBTYPE_VC_INPUT_TERMINAL << 8) & 0xFF00)
#define USB_DESC_VC_OUTPUT_TERMINAL							((USB_DESC_SUBTYPE_VC_OUTPUT_TERMINAL << 8) & 0xFF00)
#define USB_DESC_VC_SELECTOR_UNIT							((USB_DESC_SUBTYPE_VC_SELECTOR_UNIT << 8) & 0xFF00)
#define USB_DESC_VC_PROCESSING_UNIT							((USB_DESC_SUBTYPE_VC_PROCESSING_UNIT << 8) & 0xFF00)
#define USB_DESC_VC_EXTENSION_UNIT							((USB_DESC_SUBTYPE_VC_EXTENSION_UNIT << 8) & 0xFF00)

/* Video Class-Specific VS Interface Descriptor Subtypes */
#define USB_DESC_VS_UNDEFINED								((USB_DESC_SUBTYPE_VS_UNDEFINED << 8) & 0xFF00)
#define USB_DESC_VS_INPUT_HEADER							((USB_DESC_SUBTYPE_VS_INPUT_HEADER << 8) & 0xFF00)
#define USB_DESC_VS_OUTPUT_HEADER							((USB_DESC_SUBTYPE_VS_OUTPUT_HEADER << 8) & 0xFF00)
#define USB_DESC_VS_STILL_IMAGE_FRAME						((USB_DESC_SUBTYPE_VS_STILL_IMAGE_FRAME << 8) & 0xFF00)
#define USB_DESC_VS_FORMAT_UNCOMPRESSED						((USB_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED << 8) & 0xFF00)
#define USB_DESC_VS_FRAME_UNCOMPRESSED						((USB_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED << 8) & 0xFF00)
#define USB_DESC_VS_FORMAT_MJPEG							((USB_DESC_SUBTYPE_VS_FORMAT_MJPEG << 8) & 0xFF00)
#define USB_DESC_VS_FRAME_MJPEG								((USB_DESC_SUBTYPE_VS_FRAME_MJPEG << 8) & 0xFF00)
#define USB_DESC_VS_FORMAT_MPEG2TS							((USB_DESC_SUBTYPE_VS_FORMAT_MPEG2TS << 8) & 0xFF00)
#define USB_DESC_VS_FORMAT_DV								((USB_DESC_SUBTYPE_VS_FORMAT_DV << 8) & 0xFF00)
#define USB_DESC_VS_COLORFORMAT								((USB_DESC_SUBTYPE_VS_COLORFORMAT << 8) & 0xFF00)
#define USB_DESC_VS_FORMAT_FRAME_BASED						((USB_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED << 8) & 0xFF00)
#define USB_DESC_VS_FRAME_FRAME_BASED						((USB_DESC_SUBTYPE_VS_FRAME_FRAME_BASED << 8) & 0xFF00)
#define USB_DESC_VS_FORMAT_STREAM_BASED						((USB_DESC_SUBTYPE_VS_FORMAT_STREAM_BASED << 8) & 0xFF00)

/**
  * @}
  */

/** @defgroup USBH_CLASS_CAM_Exported_Types
  * @{
  */
//typedefs

#endif /* ST_STM32_USB_HOST_LIBRARY_CLASS_CAM_USBH_CAM_DEF_H_ */

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



