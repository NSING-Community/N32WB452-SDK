/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file main.c
 * @author Nations 
 * @version v1.1.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "log.h"
#include "n32wb452.h"
#include "n32wb452_rtc.h"
#include "n32wb452_pwr.h"
#include "User_LED_Config.h"
#include "User_RTC_Config.h"


/** @addtogroup RTC_Calendar
 * @{
 */

/**
 * @brief  Delay function.
 */
void delay(vu32 nCount)
{
    vu32 index = 0;
    for (index = (34000 * nCount); index != 0; index--)
    {
    }
}


/**
 * @brief  Reconfig the system clock.
 */
void SYSCLKConfig_STOP(uint32_t RCC_PLLMULL)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
	uint32_t Flash_Latency_Temp=0;
    uint32_t PLL_Temp=0;
    uint32_t System_Temp=0;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CTRL |= ((uint32_t)RCC_CTRL_HSEEN);
    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
       HSEStatus = RCC->CTRL & RCC_CTRL_HSERDF;
       StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));
    if ((RCC->CTRL & RCC_CTRL_HSERDF) != RESET)
    {
       HSEStatus = (uint32_t)0x01;
    }
    else
    {
       HSEStatus = (uint32_t)0x00;
    }
    if (HSEStatus == (uint32_t)0x01)
    {
       /* Enable Prefetch Buffer */
       FLASH->AC |= FLASH_AC_PRFTBFEN;
       /* Flash 2 wait state */
       Flash_Latency_Temp = FLASH->AC;
       Flash_Latency_Temp &= (uint32_t)((uint32_t)~FLASH_AC_LATENCY);
       Flash_Latency_Temp |= (uint32_t)FLASH_AC_LATENCY_4;
       FLASH->AC = Flash_Latency_Temp;
       /* HCLK = SYSCLK */
       RCC->CFG |= (uint32_t)RCC_CFG_AHBPRES_DIV1;
       /* PCLK2 = HCLK */
       RCC->CFG |= (uint32_t)RCC_CFG_APB2PRES_DIV2; 
       /* PCLK1 = HCLK */
       RCC->CFG |= (uint32_t)RCC_CFG_APB1PRES_DIV4; 
       /*  PLL configuration: PLLCLK = HSE * 18 = 144 MHz */
       PLL_Temp = RCC->CFG;
       PLL_Temp &= (uint32_t)((uint32_t) ~(RCC_CFG_PLLSRC | RCC_CFG_PLLHSEPRES | RCC_CFG_PLLMULFCT));
       PLL_Temp |= (uint32_t)(RCC_CFG_PLLSRC_HSE | RCC_PLLMULL);
       RCC->CFG = PLL_Temp;
       /* Enable PLL */
       RCC->CTRL |= RCC_CTRL_PLLEN;
       /* Wait till PLL is ready */
       while ((RCC->CTRL & RCC_CTRL_PLLRDF) == 0)
       {
           /* If this flag is always not set, it means PLL is failed, user can add here some code to deal with this error */
       }
       /* Select PLL as system clock source */
       System_Temp = RCC->CFG;
       System_Temp &= (uint32_t)((uint32_t) ~(RCC_CFG_SCLKSW));
       System_Temp |= (uint32_t)RCC_CFG_SCLKSW_PLL;
       RCC->CFG = System_Temp;
       /* Wait till PLL is used as system clock source */
       while ((RCC->CFG & (uint32_t)RCC_CFG_SCLKSTS) != (uint32_t)0x08)
       {
           /* If this bits are always not set, it means system clock select PLL failed, user can add here some code to deal with this error */
       }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
    }
}


/**
 * @brief  Selects HSI as System clock source and configure HCLK, PCLK2
 *         and PCLK1 prescalers.
 */
void SetSysClockToHSI(void)
{
    RCC_DeInit();

    RCC_EnableHsi(ENABLE);
    /* Wait HSI ready*/ 	
    /*  if HSI config fail, User can add here some code to deal with this error */	
    while((RCC->CTRL & RCC_CTRL_HSIRDF) == 0)
    {
    }

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufSet(FLASH_PrefetchBuf_EN);

    /* Flash 0 wait state */
    FLASH_SetLatency(FLASH_LATENCY_0);

    /* HCLK = SYSCLK */
    RCC_ConfigHclk(RCC_SYSCLK_DIV1);

    /* PCLK2 = HCLK */
    RCC_ConfigPclk2(RCC_HCLK_DIV1);

    /* PCLK1 = HCLK */
    RCC_ConfigPclk1(RCC_HCLK_DIV1);

    /* Select HSE as system clock source */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSI);

    /* Wait till PLL is used as system clock source */
    /* if system clock switch fail, User can add here some code to deal with this error */	
    while (RCC_GetSysclkSrc() != 0x00)
    {
    }
    RCC_ConfigHse(RCC_HSE_DISABLE);
    /* if HSE close fail, User can add here some code to deal with this error */		
    while((RCC->CTRL & RCC_CTRL_HSERDF) != 0)
    {
    }    
}


/**
 * @brief  Main program.
 */
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (startup_n32wb452_xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_n32wb452.c file
    */
    /* Initialize LEDs and USART on n32wb452-EVAL board */
    LEDInit(LED1_PORT, LED1_PIN);
    LEDOff(LED1_PORT,LED1_PIN);
    log_init();
    log_info("\r\n Alarm Wakeup Config \r\n ");
    /* RTC time and Date default Value */
    RTC_DateAndTimeDefaultVale();
    /* Enable PWR Clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR | RCC_APB1_PERIPH_BKP, ENABLE);
    /* Allow access to RTC */
    PWR_BackupAccessEnable(ENABLE);

    /* RTC clock source select */
    if(SUCCESS==RTC_CLKSourceConfig(RTC_CLK_SRC_TYPE_LSE, true, false))
    {
        if(USER_WRITE_BKP_DAT1_DATA != BKP_ReadBkpData(BKP_DAT1))
       {
          RTC_PrescalerConfig();
          /* RTC Date time and Alarm value set */
          RTC_DateRegulate();
          RTC_TimeRegulate();
          RTC_AlarmRegulate(RTC_A_ALARM);
          BKP_WriteBkpData(BKP_DAT1, USER_WRITE_BKP_DAT1_DATA);
          log_info("\r\n RTC Init Success\r\n");
       }
       else
       {
          log_info("\r\n RTC Init Faile\r\n");
       }
    }
    /* Enable RTC Alarm Interrupt */
    EXTI17_RTCAlarm_Configuration(ENABLE);
    /* Enable PWR clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    while (1)
    {
       /* Turn on LED and prin some flag imformation */
       LEDBlink(LED1_PORT,LED1_PIN);
       delay(400);
       log_info("\r\n start low power! \r\n");

       /* Before entering STOP mode, the system clock source must be switched to HSI */
       SetSysClockToHSI();

       /* Request to enter STOP mode with regulator in low power mode*/
       PWR_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
       /* Exit the low power ,need to reconfig the system clock */
       SYSCLKConfig_STOP(RCC_CFG_PLLMULFCT18);
       log_init();
       log_info("\r\n Exit low power! \r\n");
       delay(400);
    }
}

