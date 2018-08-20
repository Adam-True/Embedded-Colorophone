/**
  ******************************************************************************
  * @file    USB_Host/MSC_Standalone/Src/main.c
  * @author  MCD Application Team
  * @brief   USB host Mass storage demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ----------------------------------------------------------------- */
#include "main.h"
#include "color.h"
#include "stevenslaw.h"
#include "audiosamples.h"
#include "audioselector.h"


// Play buffer definition for the audio output
#define PLAY_BUFF_SIZE       2048
uint16_t                     PlayBuff[PLAY_BUFF_SIZE];

// Mail boxes
osMailQId sampleDMAQueue;

// Thread & timer definitions
osThreadId audioHandle;
static void audioThread(void const * argument);

// Functions
static void SystemClock_Config(void);
static void Playback_Init(void);
static void CPU_CACHE_Enable(void);

// Audio sample and SD card necessities
char SD_Path[4];
FATFS fs;
AudioSelector redSel;
AudioSelector greenSel;
AudioSelector blueSel;
AudioSelector whiteSel;


/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
CAM_ApplicationTypeDef Appli_state = APPLICATION_IDLE;
#if (USBH_USE_OS == 1)
osMessageQId AppliEvent;
#endif //USBH_USE_OS == 1


uint8_t  lcd_status = LCD_OK;

/* Private function prototypes ---------------------------------------------- */
static void SystemClock_Config(void);
static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id);
static void CAM_InitApplication(void);
static void StartThread(void const *argument);
static void CPU_CACHE_Enable(void);
static void MX_GPIO_Init(void);

/* Private functions -------------------------------------------------------- */

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/* This project calls firstly two functions in order to configure MPU feature
	and to enable the CPU Cache, respectively MPU_Config() and CPU_CACHE_Enable()*/

	/* Enable the CPU Cache */
	CPU_CACHE_Enable();

	/* STM32F7xx HAL library initialization: - Configure the Flash ART
	* accelerator on ITCM interface - Configure the Systick to generate an
	* interrupt each 1 msec - Set NVIC Group Priority to 4 - Low Level
	* Initialization */
	HAL_Init();

	/* Configure the System clock to have a frequency of 200 Mhz */
	SystemClock_Config();

	/* Configure LED1 and LED2 */
	BSP_LED_Init(LED1);
	BSP_LED_Init(LED2);

	/* Initialize playback */
	Playback_Init();

	if(FATFS_LinkDriver(&SD_Driver, SD_Path) != 0)
	{
		// Failure to link SD card driver
		Error_Handler(ERROR_SD_DRIVER);
	}

	/*
	 * Start the different tasks
	 */

	//Call to start the tracing
	vTraceEnable(TRC_START);

	/* Main Application Task */
	osThreadDef(USER_Thread, StartThread, osPriorityAboveNormal, 0, 8 * configMINIMAL_STACK_SIZE);
	osThreadCreate(osThread(USER_Thread), NULL);

	/* Stream RAW Parser Task */
	osThreadDef(Raw_Stream_Parser, RawParserThread, osPriorityAboveNormal, 0, 8 * configMINIMAL_STACK_SIZE);
	osThreadCreate(osThread(Raw_Stream_Parser), NULL);

	/* Audio generation Task */
	osThreadDef(Audio, audioThread, osPriorityAboveNormal, 0, 2 * configMINIMAL_STACK_SIZE);
	audioHandle = osThreadCreate(osThread(Audio), NULL);

	/* Display Task */
//	osThreadDef(Display, screenDisplayerThread, osPriorityAboveNormal, 0, 1 * configMINIMAL_STACK_SIZE);
//	osThreadCreate(osThread(Display), NULL);

	/* Create Application Queue */
	osMessageQDef(osqueue, 1, uint16_t);
	AppliEvent = osMessageCreate(osMessageQ(osqueue), NULL);

	/* Create Image Data Mail Box */
	osMailQDef(mail_box, 10, imgPacketTypedef);
	imgMailBox = osMailCreate(osMailQ(mail_box), NULL);

	// Define mail box for updating sound
 	osMailQDef(SamplePositionQueue, (uint32_t) 1, uint16_t);
	sampleDMAQueue = osMailCreate(osMailQ(SamplePositionQueue), NULL);

	//Define mail box for upadting the color
	osMailQDef(ColorQueue, (uint32_t) 1, Color);
	colorQueue = osMailCreate(osMailQ(ColorQueue), NULL);

	//Define mail box for upadting the screen
	osMailQDef(DisplayQueue, 10, colorPacketTypedef);
	displayQueue = osMailCreate(osMailQ(DisplayQueue), NULL);

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	for (;;);
}

void audioThread(void const * argument)
{
  uint16_t* position;
  Color c;
  Color* pC;

  osEvent event;

  // Initialize the data buffer
    for(int i=0; i < PLAY_BUFF_SIZE; i++)
    {
      PlayBuff[i] = 0;
    }

    c.rgbwValues.r = 0;
    c.rgbwValues.g = 0;
    c.rgbwValues.b = 0;
    c.rgbwValues.w = 0;

    // If no link to SD Card Driver, the AudioSelector_initialiser will detect it
    f_mount(&fs, (TCHAR const*)"",1);

    AudioSelector_initialise(&redSel, "red.wav", red_length, red);
    AudioSelector_initialise(&greenSel, "green.wav", green_length, green);
    AudioSelector_initialise(&blueSel, "blue.wav", blue_length, blue);
    AudioSelector_initialise(&whiteSel, "white.wav", white_length, white);

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
        // TODO change this addition from a simple addition to a saturating addition
        PlayBuff[i+(*position)] = (AudioSelector_getNext(&redSel)*stevensLawConversionTable[c.rgbwValues.r])/256;
        PlayBuff[i+(*position)] += (AudioSelector_getNext(&greenSel)*stevensLawConversionTable[c.rgbwValues.g])/256;
        PlayBuff[i+(*position)] += (AudioSelector_getNext(&blueSel)*stevensLawConversionTable[c.rgbwValues.b])/256;
        PlayBuff[i+(*position)] += (AudioSelector_getNext(&whiteSel)*stevensLawConversionTable[c.rgbwValues.w])/256;
      }

      osMailFree(sampleDMAQueue, position);
      BSP_LED_Toggle(LED2);
    }
  }
}

/**
  * @brief  Start task
  * @param  pvParameters not used
  * @retval None
  */
static void StartThread(void const *argument)
{
  osEvent event;

  /* Init CAM Application */
  CAM_InitApplication();

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
			USBH_ReEnumerate(&hUSBHost);//TODO check if this makes sense. This should be only called once thanks to the mailbox.
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
static void CAM_InitApplication(void)
{
	/* Initialize all configured peripherals */
	MX_GPIO_Init();

	/* Initialize the LCD   */
	lcd_status = BSP_LCD_Init();
	if(lcd_status != LCD_OK)
	{
		OnError_Handler();
	}

	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(0);

	/* Clear LCD */
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	/* Enable the display */
	BSP_LCD_DisplayOn();

	/* Initialize the LCD Log module */
	LCD_LOG_Init();

	#ifdef USE_USB_HS
	LCD_LOG_SetHeader((uint8_t *) " USB OTG HS CAM Host");
	#else
	LCD_LOG_SetHeader((uint8_t *) " USB OTG FS CAM Host");
	#endif

	LCD_UsrLog("USB Host library started.\n");
}

/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
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
		  break;

		case HOST_USER_CONNECTION:
		  osMessagePut(AppliEvent, APPLICATION_START, 0);
		  break;

		default:
		  break;
	}
}

/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on SysTick counter flag.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */

/*void HAL_Delay(__IO uint32_t Delay)
{
  while (Delay)
  {
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    {
      Delay--;
    }
  }
}*/

static void MX_GPIO_Init(void)
{
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PC7 */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  Toggles LEDs to show user input state.
  * @param  None
  * @retval None
  */
void Toggle_Leds(void)
{
  static uint32_t ticks;
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);

  if (ticks++ == 100)
  {
    BSP_LED_Toggle(LED1);
    BSP_LED_Toggle(LED2);
    ticks = 0;
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLLSAI_N                       = 384
  *            PLLSAI_P                       = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is
	   clocked below the maximum system frequency, to update the voltage scaling value
	   regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;
	RCC_OscInitStruct.PLL.PLLR = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler(ERROR_HAL_CLOCK_CONFIG);
	}

	/* Activate the OverDrive to reach the 200 Mhz Frequency */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		Error_Handler(ERROR_HAL_CLOCK_CONFIG);
	}

	/* Select PLLSAI output as USB clock source */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIQ = 4;
	PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler(ERROR_HAL_CLOCK_CONFIG);
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	* clocks dividers */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
								RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
	{
		Error_Handler(ERROR_HAL_CLOCK_CONFIG);
	}
}

/**
  * @brief  Playback initialization
  * @param  None
  * @retval None
  */
static void Playback_Init(void)
{
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 20, AUDIO_FREQUENCY_11K) == AUDIO_ERROR) // TODO understand why 11kHz works here
  {
    Error_Handler(ERROR_AUDIO_INIT);
  }
  if(BSP_AUDIO_OUT_Play((uint16_t*)&PlayBuff[0], 2*PLAY_BUFF_SIZE) == AUDIO_ERROR)  // Size is in bytes
  {
    Error_Handler(ERROR_AUDIO_PLAY);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(ErrorCode e)
{
  /* User may add here some code to deal with this error */
  while (1)
  {
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
      // TODO change error handling for it to not be in an interrupt
      // Error_Handler(ERROR_AUDIO_PLAY);
      osMailFree(sampleDMAQueue, val);
    }
  }
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
  Error_Handler(ERROR_STACK_OVERFLOW);
}


/**
  * @brief  On Error Handler.
  * @param  None
  * @retval None
  */
void OnError_Handler(void)
{
  //BSP_LED_On(LED1);
  while(1) { ; } /* Blocking on error */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
   * number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
   * line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
