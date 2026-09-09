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
 * @file n32wb452_ota_iap.c
 * @author Nations
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32wb452.h"
#include "n32wb452_ota_iap.h"
#include "n32wb452_ota_conf.h"


#define APP_FLASH_ADDR_OFFSET (OF_BOOTLOADER_SIZE)

void set_vector_table(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, APP_FLASH_ADDR_OFFSET);
}

/**
 * @brief  jump to app function.
 * @param appAddr: app address
 * @return 0
 */
uint8_t jump2app(uint32_t appAddr)
{
    uint32_t app_msp_addr;
    uint32_t app_jump_addr;
    void  (*pAppFun)(void);  

    //FLASH_Lock();

    app_msp_addr = (*(__IO uint32_t*)appAddr);    
    app_jump_addr = (*(__IO uint32_t*)(appAddr + 4)); 

    if ((app_msp_addr & 0x2FFDC000) != 0x20000000)  
        return 1;

    pAppFun = (void (*)(void))app_jump_addr;     

    NVIC_SetVectorTable(NVIC_VectTab_FLASH, APP_FLASH_ADDR_OFFSET);

    __set_MSP(app_msp_addr);   
    //__set_PSP(app_msp_addr);
    (*pAppFun)();       
    
    return 0;
}

/**
 * @brief  jump to iap function.
 * @param iapAddr: iap address
 * @return 0
 */
void jump2iap(uint32_t iapAddr)
{
#if 0
    uint32_t  IapSpInitVal;
    uint32_t  IapJumpAddr;     
    void (*pIapFun)(void);       

    //RCC_DeInit();
    //nvic_reset();           

    //__set_CONTROL(0);                

    IapSpInitVal = *(__IO uint32_t*)iapAddr;      
    IapJumpAddr = *(__IO uint32_t*)(iapAddr + 4); 

    __set_MSP(IapSpInitVal);                     
    //__set_PSP(IapSpInitVal);

    pIapFun = (void (*)(void))IapJumpAddr;      
    (*pIapFun)();                              

#else

    __set_FAULTMASK(1);      
    NVIC_SystemReset();   

#endif
}

