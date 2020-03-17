/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   This file Provides Device Core configuration Functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#include "usb_bsp.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void CRS_Config(void);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Initialize BSP configurations
  * @param  None
  * @retval None
  */

void USB_BSP_Init(USB_CORE_HANDLE *pdev)
{

  /* Enable USB clock */
  RCC->APB1ENR |= RCC_APB1ENR_USBEN;

  
  /* Configure USBCLK from PLL clock */
  RCC->CFGR3 |= RCC_CFGR3_USBSW;
  
}

/**
  * @brief  Enable USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_BSP_EnableInterrupt(USB_CORE_HANDLE *pdev)
{
	/* Enable the USB interrupt */
  NVIC_SetPriority(USB_IRQn, 0);
  NVIC_EnableIRQ(USB_IRQn);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
