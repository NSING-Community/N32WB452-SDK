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
 * @file n32wb452_flash_interface.c
 * @author Nations
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
/* Scheduler includes. */
#include "n32wb452.h"
#include "n32wb452_flash_interface.h"
#include "log.h"
#include "string.h"

#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 2048
#endif

u16 N32WB452_FLASH_ReadWord(u32 faddr)
{
    return *(vu32*)faddr;
}

FLASH_STS FLASH_ProgramPage(uint32_t *SrcAddr, uint32_t DstAddr, uint16_t Len)
{
    uint32_t i;

    for (i = 0; i < Len; i += 4)
    {
        FLASH_ProgramWord(DstAddr + i, *(uint32_t *)((uint32_t)SrcAddr + i));
        if (*(uint32_t *)(DstAddr + i) != *(uint32_t *)((uint32_t)SrcAddr + i))
        {
            return FLASH_ERR_PG;
        }
    }
    return FLASH_COMPL;
}


// WriteAddr:write start address
// pBuffer: data pointer
// NumToWrite: write data number(4 bytes)
void N32WB452_FLASH_Write_NoCheck(u32 WriteAddr, u32* pBuffer, u32 NumToWrite)
{
    u32 i;

    NumToWrite = NumToWrite / 4;

    for (i = 0; i < NumToWrite; i++)
    {
        FLASH_ProgramWord(WriteAddr, pBuffer[i]);

        WriteAddr += 4;
    }

}
// WriteAddr:write data start address 
// pBuffer:write data buffer pointer
// NumToWrite:write data number
#define N32WB452_SECTOR_SIZE 2048
static u8 N32WB452_FLASH_BUF[FLASH_PAGE_SIZE];

// static uint8_t g_FlashBuf[FLASH_PAGE_SIZE];

int32_t N32WB452_FLASH_Write(u32 addr, const u8* src, u32 len)
{
    uint32_t i;
    uint32_t pageAddr; 
    uint32_t pageCount;
    uint32_t tmpAddr;  
    uint32_t tmpLen;   
    uint32_t startAddr;
    uint32_t endAddr;  
    uint32_t flag = 0; 

//    log_debug("FLH:Write.%0x, %0x, %0x.\r\n", addr, src, len);

    FLASH_Unlock();
    startAddr = addr % FLASH_PAGE_SIZE;
    endAddr   = (addr + len) % FLASH_PAGE_SIZE;
    if (startAddr == 0)
    {
        pageAddr = addr;
        pageCount = len / FLASH_PAGE_SIZE;
        for (i = 0; i < pageCount; i++)
        {
            if (FLASH_COMPL != FLASH_EraseOnePage(pageAddr) != 0)
            {
//                log_debug("***erase 1 page err.\r\n");
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
            if (FLASH_COMPL != FLASH_ProgramPage((uint32_t*)(src + (i * FLASH_PAGE_SIZE)), pageAddr, FLASH_PAGE_SIZE))
            {
                log_debug("***prog 1 page err.\r\n");
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
            pageAddr += FLASH_PAGE_SIZE;
        }
        if (endAddr != 0)
        {
            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                N32WB452_FLASH_BUF[i] = ((uint8_t*)pageAddr)[i];
            }
            tmpAddr = len % FLASH_PAGE_SIZE;

            for (i = 0; i < tmpAddr; i++)
            {
                N32WB452_FLASH_BUF[i] = ((uint8_t*)(src + (pageCount * FLASH_PAGE_SIZE)))[i];
            }

            if (FLASH_COMPL != FLASH_EraseOnePage(pageAddr))
            {
                log_debug("***erase 2 page err.\r\n");
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            if (FLASH_COMPL != FLASH_ProgramPage((uint32_t*)N32WB452_FLASH_BUF, pageAddr, FLASH_PAGE_SIZE))
            {
                log_debug("***prog 2 page err.\r\n");
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
        }
    }
    else
    {
        pageAddr = (addr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;

        tmpAddr = addr % FLASH_PAGE_SIZE;

        tmpLen = FLASH_PAGE_SIZE - tmpAddr;
        if (tmpLen > len)
        {
            tmpLen = len;
            flag   = 0;
        }
        else
        {
            flag = 1;
        }

        pageCount = (len - tmpLen) / FLASH_PAGE_SIZE;

        for (i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            N32WB452_FLASH_BUF[i] = ((uint8_t*)pageAddr)[i];
        }

        for (i = 0; i < tmpLen; i++)
        {
            N32WB452_FLASH_BUF[tmpAddr + i] = ((uint8_t*)src)[i];
        }

        if (FLASH_COMPL != FLASH_EraseOnePage(pageAddr))
        {
            log_debug("***erase 3 page err.\r\n");
            FLASH_Lock();
            return FLASH_ERR_WRP;
        }

        if (FLASH_COMPL != FLASH_ProgramPage((uint32_t*)N32WB452_FLASH_BUF, pageAddr, FLASH_PAGE_SIZE))
        {
            log_debug("***prog 3 page err.\r\n");
            FLASH_Lock();
            return FLASH_ERR_WRP;
        }
        pageAddr += FLASH_PAGE_SIZE;
        for (i = 0; i < pageCount; i++)
        {

            if (FLASH_COMPL != FLASH_EraseOnePage(pageAddr))
            {
                log_debug("***erase 4 page err.\r\n");
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            if (FLASH_COMPL != FLASH_ProgramPage((uint32_t*)(src + tmpLen + (i * FLASH_PAGE_SIZE)), pageAddr, FLASH_PAGE_SIZE))
            {
                log_debug("***prog 4 page err.\r\n");
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
            pageAddr += FLASH_PAGE_SIZE;
        }
        if ((endAddr != 0) && (flag == 1))
        {

            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                N32WB452_FLASH_BUF[i] = ((uint8_t*)pageAddr)[i];
            }

            tmpAddr = (len - tmpLen) % FLASH_PAGE_SIZE;

            for (i = 0; i < tmpAddr; i++)
            {
                N32WB452_FLASH_BUF[i] = ((uint8_t*)(src + tmpLen + (pageCount * FLASH_PAGE_SIZE)))[i];
            }

            if (FLASH_COMPL != FLASH_EraseOnePage(pageAddr))
            {
                log_debug("***erase 5 page err.\r\n");
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            if (FLASH_COMPL != FLASH_ProgramPage((uint32_t*)N32WB452_FLASH_BUF, pageAddr, FLASH_PAGE_SIZE))
            {
                log_debug("***prog 5 page err.\r\n");
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
        }
    }

    FLASH_Lock();

//    log_debug("FLH:Write ok.\r\n");

    return FLASH_COMPL;
}

// ReadAddr:read data start address 
// pBuffer:read data buffer pointer
// NumToWrite:read data number
void N32WB452_FLASH_Read(u32 ReadAddr, u32* pBuffer, u32 NumToRead)
{
    u32 i;
    for (i = 0; i < NumToRead; i++)
    {
        // pBuffer[i]=N32WB452_FLASH_ReadWord(ReadAddr);
        // ReadAddr+=4;
        *((u8*)pBuffer + i) = *((u8*)ReadAddr + i);
    }
}

// ReadAddr:erase page address
void N32WB452_Clear_Pages(u32 ReadAddr)
{
    memset(N32WB452_FLASH_BUF, 0, sizeof(N32WB452_FLASH_BUF));
    FLASH_Unlock();

    FLASH_EraseOnePage(ReadAddr);

    N32WB452_FLASH_Write_NoCheck(ReadAddr, (u32*)N32WB452_FLASH_BUF, N32WB452_SECTOR_SIZE);
    FLASH_Lock();
}
