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
 ******************************************************************************
 * @file n32wb452_w25qxx.c
 * @author Nations
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-12-20
 *
 ******************************************************************************
 */
#include "n32wb452.h"
#include "n32wb452_w25qxx.h"
#include "main.h"
#include "log.h"
#include "bsp_spi.h"
#include "string.h"

#define SPI_ReadWrite(dat)    SPI3_ReadWriteByte(dat)

uint16_t W25QXX_TYPE = W25Q32;
//0XEF13: W25Q80
//0XEF14: W25Q16
//0XEF15: W25Q32
//0XEF16: W25Q64
//0XEF17: W25Q128


typedef struct w25qxx_type_t
{
    uint16_t val;
    uint8_t *pstr;
} w25qxx_type;


const w25qxx_type spi_flash_string[] = {
    {0xEF13, "W25Q80"},
    {0xEF14, "W25Q16"},
    {0xEF15, "W25Q32"},
    {0xEF16, "W25Q64"},
    {0xEF17, "W25Q128"}
};

void W25QXX_Init(void)
{
    uint32_t i;
    GPIO_InitType GPIO_InitStructure;

    //CLOSE JTAG/OPEN SWD, PA15
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO | RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB, ENABLE);
    AFIO->RMP_CFG &= 0xF8FFFFFF;
    AFIO->RMP_CFG |= 0x02000000;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    RCC_EnableAPB2PeriphClk(SPI_FLASH_RCCCLK , ENABLE);
    
    GPIO_InitStructure.Pin   = SPI_FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(SPI_FLASH_CS_PORT, &GPIO_InitStructure);
    
    __SPI_FLASH_CS_SET();
    
    SPI_Configuration();

    W25QXX_TYPE = W25QXX_ReadID();
    W25QXX_TYPE = W25QXX_ReadID();

    /* FLASHID CHECK */
    for (i = 0; i < (sizeof(spi_flash_string) / sizeof(spi_flash_string[0])); i++) {
        if (W25QXX_TYPE == spi_flash_string[i].val) {
            log_debug("(W25Qxx)spi flash id = %s.\r\n", spi_flash_string[i].pstr);
            break;
        }
    }
    if (i >= (sizeof(spi_flash_string) / sizeof(spi_flash_string[0]))) {
        log_debug("(W25Qxx)spi flash id = %s.\r\n", "none");
    }

}

uint8_t W25QXX_ReadSR(void)
{
    uint8_t byte = 0;
    uint8_t snd  = 0;

    __SPI_FLASH_CS_CLR();

    snd = W25X_ReadStatusReg;
    SPI_ReadWrite(snd);

    snd = 0Xff;
    byte = SPI_ReadWrite(snd);

    __SPI_FLASH_CS_SET();
    return byte;
}

void W25QXX_Write_SR(uint8_t sr)
{
    uint8_t snd = 0;

    __SPI_FLASH_CS_CLR();
    snd = W25X_WriteStatusReg;
    SPI_ReadWrite(snd);
    
    snd = sr;
    SPI_ReadWrite(snd);
    __SPI_FLASH_CS_SET();
}

void W25QXX_Write_Enable(void)
{
    uint8_t snd = 0;

    __SPI_FLASH_CS_CLR();
    snd = W25X_WriteEnable;
    SPI_ReadWrite(snd);
    __SPI_FLASH_CS_SET();
}

void W25QXX_Write_Disable(void)
{
    uint8_t snd = 0;

    __SPI_FLASH_CS_CLR();
    snd = W25X_WriteDisable;
    SPI_ReadWrite(snd);
    __SPI_FLASH_CS_SET();
}

//0XEF13: W25Q80
//0XEF14: W25Q16
//0XEF15: W25Q32
//0XEF16: W25Q64
//0XEF17: W25Q128
uint16_t W25QXX_ReadID(void)
{
    uint8_t snd;
    uint8_t rcv;
    uint16_t Temp = 0;

    __SPI_FLASH_CS_CLR();

    snd = 0x90;
    rcv = SPI_ReadWrite(snd);


    snd = 0x00;
    rcv = SPI_ReadWrite(snd);


    snd = 0x00;
    rcv = SPI_ReadWrite(snd);


    snd = 0x00;
    rcv = SPI_ReadWrite(snd);


    snd = 0xFF;
    rcv = SPI_ReadWrite(snd);

    Temp |= rcv << 8;

    snd = 0xFF;
    rcv = SPI_ReadWrite(snd);
    Temp |= rcv;

    __SPI_FLASH_CS_SET();

    return Temp;
}
//read SPI FLASH
//pBuffer: data buffer point
//ReadAddr:read address(24bit)
//NumByteToRead:read byte number
void W25QXX_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
    uint8_t snd = 0;
    uint8_t rcv = 0;
    uint16_t i;

    __SPI_FLASH_CS_CLR();
    snd = W25X_ReadData;
    rcv = SPI_ReadWrite(snd);


    snd = ((uint8_t)((ReadAddr) >> 16));
    rcv = SPI_ReadWrite(snd);


    snd = ((uint8_t)((ReadAddr) >> 8));
    rcv = SPI_ReadWrite(snd);


    snd = ((uint8_t)ReadAddr);
    rcv = SPI_ReadWrite(snd);

    for (i = 0; i < NumByteToRead; i++)
    {
        snd = 0XFF;
        rcv = SPI_ReadWrite(snd);

        pBuffer[i] = rcv;
        //log_debuginfo("<%02x>,", pBuffer[i]);
    }

    __SPI_FLASH_CS_SET();
}

//pBuffer: data buffer point
//WriteAddr:write address(24bit)
//NumByteToWrite:write byte number
void W25QXX_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;
    uint8_t snd = 0;

    W25QXX_Write_Enable(); //SET WEL

    __SPI_FLASH_CS_CLR();

    snd = W25X_PageProgram; //send write page command
    SPI_ReadWrite(snd);


    snd = ((uint8_t)((WriteAddr) >> 16)); //send address
    SPI_ReadWrite(snd);


    snd = ((uint8_t)((WriteAddr) >> 8));
    SPI_ReadWrite(snd);

    snd = ((uint8_t)WriteAddr);
    SPI_ReadWrite(snd);


    for (i = 0; i < NumByteToWrite; i++)
    {
        SPI_ReadWrite(pBuffer[i]);
    }
    __SPI_FLASH_CS_SET();
    W25QXX_Wait_Busy();
}

//Write SPI FLASH with no check
//pBuffer: data buffer point
//WriteAddr:write address(24bit)
//NumByteToWrite:write byte number
void W25QXX_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t pageremain;
    pageremain = 256 - WriteAddr % 256;
    if (NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite;
    while (1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        if (NumByteToWrite == pageremain)
            break;
        else       //NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;
            if (NumByteToWrite > 256)
                pageremain = 256;
            else
                pageremain = NumByteToWrite;
        }
    };
}

//Write SPI FLASH
//this function include erase 
//pBuffer: data buffer point
//WriteAddr:write address(24bit)
//NumByteToWrite:write byte number
static uint8_t W25QXX_BUFFER[4096];
void W25QXX_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t* W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos     = WriteAddr / 4096;
    secoff     = WriteAddr % 4096;
    secremain  = 4096 - secoff;
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);
    if (NumByteToWrite <= secremain)
        secremain = NumByteToWrite;
    while (1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);
        for (i = 0; i < secremain; i++)
        {
            if (W25QXX_BUF[secoff + i] != 0XFF)
                break;
        }
        if (i < secremain)
        {
            W25QXX_Erase_Sector(secpos);
            for (i = 0; i < secremain; i++)
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);
        }
        else
            W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);
        if (NumByteToWrite == secremain)
            break;
        else
        {
            secpos++;
            secoff = 0;

            pBuffer += secremain;
            WriteAddr += secremain;
            NumByteToWrite -= secremain;
            if (NumByteToWrite > 4096)
                secremain = 4096;
            else
                secremain = NumByteToWrite;
        }
    };
}

void W25QXX_Clear(u32 ReadAddr, uint32_t NumByteToWrite)
{
    memset(W25QXX_BUFFER, 0, sizeof(W25QXX_BUFFER));

    W25QXX_Write(W25QXX_BUFFER, ReadAddr, NumByteToWrite);
}


void W25QXX_Erase_Chip(void)
{
    uint8_t snd = 0;

    W25QXX_Write_Enable(); //SET WEL
    W25QXX_Wait_Busy();
    __SPI_FLASH_CS_CLR();    
    snd = ((uint8_t)W25X_ChipErase); 
    SPI_ReadWrite(snd);

    __SPI_FLASH_CS_SET();
    W25QXX_Wait_Busy();     
}
//Erase a sector
//Dst_Addr:sector address
void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{
    uint8_t snd = 0;
    //printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr *= 4096;
    W25QXX_Write_Enable(); //SET WEL
    W25QXX_Wait_Busy();

    __SPI_FLASH_CS_CLR();

    SPI_ReadWrite(W25X_SectorErase);
    snd = ((uint8_t)((Dst_Addr) >> 16));
    SPI_ReadWrite(snd);

    snd = ((uint8_t)((Dst_Addr) >> 8));
    SPI_ReadWrite(snd);

    snd = ((uint8_t)Dst_Addr);
    SPI_ReadWrite(snd);

    __SPI_FLASH_CS_SET();
    W25QXX_Wait_Busy();
}
void W25QXX_Wait_Busy(void)
{
    while ((W25QXX_ReadSR() & 0x01) == 0x01)
        ;
}

//W25Qxx enter power down mode
void W25QXX_PowerDown(void)
{
    __SPI_FLASH_CS_CLR();
    SPI_ReadWrite(W25X_PowerDown);
    __SPI_FLASH_CS_SET();

    delay_ms(1);        
}

//W25Qxx wakeup from power down mode
void W25QXX_WAKEUP(void)
{
    __SPI_FLASH_CS_CLR();
    SPI_ReadWrite(W25X_ReleasePowerDown);
    __SPI_FLASH_CS_SET();

    delay_ms(3);
}
