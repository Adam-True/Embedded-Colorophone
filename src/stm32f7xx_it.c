/**
  ******************************************************************************
  * @file    USB_Host/MSC_Standalone/Src/stm32f7xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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

/* Includes ----------------------------------------------------------------- */
#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include <cmsis_os.h>
#include "stm32f7xx_it.h"
#include "main.h"

/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
extern HCD_HandleTypeDef                hhcd;
extern SD_HandleTypeDef                 uSdHandle;
extern TIM_HandleTypeDef                htim6;
extern SAI_HandleTypeDef                haudio_out_sai;
/* Private function prototypes ---------------------------------------------- */
/* Private functions -------------------------------------------------------- */

/******************************************************************************/
/* Cortex-M7 Processor Exceptions Handlers */
/******************************************************************************/


					/**@&......&...(
			  @.....................@
			...@........................
		   &.............................@
		  ................................@
		 &..............................@...
		#.............................@....,@
		#...%,,,,,,,,,@..,....,....,,..,,,,(
		@...........,,%,,,,((@@((@@ #,,,,,/
		@,,,...,,,@@(((((((@(((((((
			*@@@(((/(((((((@%(((((@
				/&*//**&@@(((%
				((./#./**#(/@/(&
  @,,....,,,..,%./(#(,/,,(,.......,,,,&@
.,.....@           *%............             (...@
@,,...,&         ................&    #@@%,,...,.
	  &(,............................,,@#
				  ..,....%.......
				 ,..*..%.%..&....
		   ,,,,,,,#..../.........,,,,,.
		  ,,,,,,,,,,/,,@,,,@,,*,,,,,,,,,%
		 (/,,,,,,,,,,,,,,,,,,,,,,,,,,,'/@
			/////,,,,,,,,,,,,,,,,/////
				,%(@ @///%///@*/

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	  //if not good, big problem
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	osSystickHandler();
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim6);
	HAL_IncTick();
}

/******************************************************************************/
/* STM32F7xx Peripherals Interrupt Handlers */
/* Add here the Interrupt Handler for the used peripheral(s) (PPP), for the */
/* available peripheral interrupt handler's name please refer to the startup */
/* file (startup_stm32f7xx.s).  */
/******************************************************************************/

/**
  * @brief  This function handles DMA2 Stream 1 interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_OUT_SAIx_DMAx_IRQHandler(void)
{
  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

/**
 * @brief Handles SDMMC1 DMA Rx transfer interrupt request.
 * @retval None
 */
void BSP_SDMMC_DMA_Rx_IRQHandler(void)
{
  HAL_DMA_IRQHandler(uSdHandle.hdmarx);
}

/**
 * @brief Handles SD1 card interrupt request.
 * @retval None
 */
void BSP_SDMMC_IRQHandler(void)
{
  HAL_SD_IRQHandler(&uSdHandle);
}

void NMI_Handler(void)
{
}

/**
  * @brief  This function handles USB-On-The-Go FS/HS global interrupt request.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  HAL_HCD_IRQHandler(&hhcd);
}





/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
