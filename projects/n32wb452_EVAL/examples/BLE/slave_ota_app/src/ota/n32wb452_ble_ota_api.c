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
 * @file n32wb452_ble_ota_api.c
 * @author Nations
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include <string.h>
#include "n32wb452_ota_conf.h"
#include "n32wb452_ble_ota_api.h"
#include "n32wb452_ota_iap.h"
#include "n32wb452_ota_upgrade.h"
#include "n32wb452_ble_protocol.h"
#include "n32wb452_w25qxx.h"
#include "log.h"
#include "crc16.h"
#include "bsp_spi.h"

ble_ota_param_t ble_ota_param;

 
extern uint16_t ble_tx_len;
extern uint8_t ble_tx_buff[BLE_TX_BUF_LEN];

extern void check_ble_tx_buff_is_empty(void);
extern void TIM6_deinit(void);

//ota initialize
void ble_ota_init(void)
{
    image_upgrade_info_t image_info;
    image_upgrade_info_t bak_image_info;
    uint8_t *pid = PRODUCE_ID;
    uint8_t *hw_ver = HW_VERSION;
    uint8_t *fw_ver = FW_VERSION;
    
    W25QXX_Init();

    //Initialize the current firmware information
    memcpy(ble_ota_param.product_id, pid, sizeof(ble_ota_param.product_id));
    memcpy(ble_ota_param.hw_version, hw_ver, sizeof(ble_ota_param.hw_version));
    memcpy(ble_ota_param.fw_version, fw_ver, sizeof(ble_ota_param.fw_version));
    ble_ota_param.customer_code = CUSTOMER_CODE;
//    ble_ota_param.file_size = 0;
//    ble_ota_param.image_crc = 0;
//    ble_ota_param.current_data_addr = 0;
//    ble_ota_param.current_data_size = 0;
    ble_ota_param.upgrade_status = OTA_STA_IDLE;
    ble_ota_param.upgrade_flag = FW_DOWNLOAD_NONE;
//    ble_ota_param.err_code = 0;

    ota_app_system_info_get(&image_info);

    //update current verision information to flash
    if (ble_ota_system_info_check(&image_info) != STA_FW_VERSION_ERROR)
    {
        memcpy(image_info.product_id, ble_ota_param.product_id, sizeof(image_info.product_id));
        memcpy(image_info.hw_version, ble_ota_param.hw_version, sizeof(image_info.hw_version));
        memcpy(image_info.fw_version, ble_ota_param.fw_version, sizeof(image_info.fw_version));
        image_info.customer_code = ble_ota_param.customer_code;
        ota_app_system_info_update(&image_info);
    }

    ota_read_backup_image_info(&bak_image_info);
    if (image_info.fw_version[0] != 0xff)
    {
        if (memcmp(image_info.fw_version, image_info.fw_version, sizeof(image_info.fw_version)) != 0)
        {
            //todo:
            //ota_param.upgrade_flag = FW_DOWNLOAD_ING;
        }
    }

    //todo:
}

//check system information
uint8_t ble_ota_system_info_check(image_upgrade_info_t *system_info)
{
    BLE_CMD_NEW_FW_RDY_STA err_code = STA_NEW_FW_SUCCESS;
    if (memcmp(system_info->product_id, ble_ota_param.product_id, sizeof(ble_ota_param.product_id)) != 0)
    {
        err_code = STA_PID_ERROR;
    }
    else if (memcmp(system_info->hw_version, ble_ota_param.hw_version, sizeof(ble_ota_param.hw_version)) != 0)
    {
        err_code = STA_HW_VERSION_ERROR;
    }
    else if (memcmp(system_info->fw_version, ble_ota_param.fw_version, sizeof(ble_ota_param.fw_version)) == 0)
    {
        err_code = STA_FW_VERSION_ERROR;
    }
    else if (system_info->customer_code != ble_ota_param.customer_code)
    {
        err_code = STA_CUSTOMER_CODE_ERROR;
    }

    return (uint8_t)err_code;
}

//check upgrade information
uint8_t ble_ota_upgrade_info_check(ble_ota_param_t *pble_ota_param)
{
    BLE_CMD_NEW_FW_RDY_STA err_code = STA_NEW_FW_SUCCESS;
    if (memcmp(pble_ota_param->product_id, ble_ota_param.product_id, sizeof(ble_ota_param.product_id)) != 0)
    {
        err_code = STA_PID_ERROR;
    }
    else if (memcmp(pble_ota_param->hw_version, ble_ota_param.hw_version, sizeof(ble_ota_param.hw_version)) != 0)
    {
        err_code = STA_HW_VERSION_ERROR;
    }
    else if (memcmp(pble_ota_param->fw_version, ble_ota_param.fw_version, sizeof(ble_ota_param.fw_version)) == 0)
    {
        err_code = STA_UPDATE_DONE;//STA_FW_VERSION_ERROR;
    }
    else if (pble_ota_param->customer_code != ble_ota_param.customer_code)
    {
        err_code = STA_CUSTOMER_CODE_ERROR;
    }

    ble_ota_param.err_code = err_code;
    if (err_code == STA_NEW_FW_SUCCESS)
        ble_ota_upgrade_info_update(pble_ota_param);
    

    return (uint8_t)err_code;
}

uint8_t ble_ota_get_err_code(void)
{
    return ble_ota_param.err_code;
}

uint8_t ble_ota_set_status_get_file(void)
{
    ble_ota_param.upgrade_status = OTA_STA_GET_FILE;
    return 0;
}

//OTA update, check data address and length, CRC value
void ble_ota_upgrade_fw_process(uint8_t *upgrade_data)
{
    ble_comm_upgrade_data_t *p_upgrade_data = (ble_comm_upgrade_data_t *)upgrade_data;
    uint8_t *p_data_image = (uint8_t*)(upgrade_data+6);
    uint8_t ret = 0;
    uint32_t remain_size = 0;
    uint16_t chksun_crc = 0;
    uint16_t chksun_crc_in = 0;
    
    if ((p_upgrade_data->data_addr != ble_ota_param.current_data_addr)
        || (p_upgrade_data->data_size != ble_ota_param.current_data_size))
    {
        log_debug("GET_FW_DATA_ERROR\r\n");
        ble_ota_param.err_code = GET_FW_DATA_ERROR;
        return;
    }

    chksun_crc = CRC16(0, p_data_image, p_upgrade_data->data_size);
    chksun_crc_in = (uint16_t)p_data_image[p_upgrade_data->data_size]<<8 | p_data_image[p_upgrade_data->data_size+1];
    if (chksun_crc_in != chksun_crc)
    {
        log_debug("GET_FW_CRC_ERROR 0x%04x 0x%04x\r\n", chksun_crc_in, chksun_crc);
        ble_ota_param.err_code = GET_FW_CRC_ERROR;
        return;
    }

    ret = ota_write_new_image_file(p_data_image, ble_ota_param.current_data_addr,  p_upgrade_data->data_size);
    if (ret == 0)
    {
        log_debug("GET_FW_CRC_ERROR\r\n");
        ble_ota_param.err_code = GET_FW_NIMAGE_FLASH_ERROR;
        return;
    }

    ble_ota_param.current_data_addr = ble_ota_param.current_data_addr + ble_ota_param.current_data_size;

    remain_size = ble_ota_param.file_size - ble_ota_param.current_data_addr;
    if (remain_size >= SF_WRITE_PACK_SIZE)
    {
        ble_ota_param.current_data_size = SF_WRITE_PACK_SIZE;
    }
    else 
    {
        ble_ota_param.current_data_size = remain_size;
    }
    
    log_debug("next add %08x, ds %04x\r\n", ble_ota_param.current_data_addr, ble_ota_param.current_data_size);

    ble_ota_param.upgrade_status = OTA_STA_GET_FILE;
}


//update OTA upgrade information
uint8_t  ble_ota_upgrade_info_update(ble_ota_param_t *pble_ota_param)
{
    uint8_t err_code = 0;
    image_upgrade_info_t image_info;
    //todo:
    
//    err_code = ble_ota_upgrade_info_check(pble_ota_param);

    if (ble_ota_param.upgrade_flag == FW_DOWNLOAD_NONE)
    {
        memcpy(ble_ota_param.fw_version, pble_ota_param->fw_version, sizeof(ble_ota_param.fw_version));
        ble_ota_param.upgrade_status = OTA_STA_GET_FILE_INFO;
        ble_ota_param.file_size = pble_ota_param->file_size;
        ble_ota_param.image_crc = pble_ota_param->image_crc;
        ble_ota_param.current_data_addr = 0;
        ble_ota_param.current_data_size = SF_WRITE_PACK_SIZE;
        
        memcpy(image_info.product_id, pble_ota_param->product_id, sizeof(image_info.product_id));
        memcpy(image_info.hw_version, pble_ota_param->hw_version, sizeof(image_info.hw_version));
        memcpy(image_info.fw_version, pble_ota_param->fw_version, sizeof(image_info.fw_version));
        image_info.file_size  = pble_ota_param->file_size;
        image_info.image_crc  = pble_ota_param->image_crc;
        image_info.customer_code  = pble_ota_param->customer_code;
        image_info.upgrage_tag = 0;
        image_info.header_crc = CRC16_INITIAL;
        image_info.header_crc = CRC16(0, (uint8_t *)&image_info.header_crc, sizeof(image_upgrade_info_t));
        ota_write_new_image_info(&image_info);
    }
    else if(ble_ota_param.upgrade_flag == FW_DOWNLOAD_ING)
    {
        //todo:
     }
    log_debug("upgrade_info_update %d %d %d\r\n", ble_ota_param.upgrade_flag, ble_ota_param.current_data_addr, ble_ota_param.current_data_size);
    return err_code;

}

uint8_t ble_ota_upgrade_info(void)
{
    BLE_CMD_NEW_FW_RDY_STA err_code = STA_NEW_FW_SUCCESS;

    if (err_code == STA_NEW_FW_SUCCESS)
    {
        ble_ota_param.upgrade_status = OTA_STA_GET_FILE;
        log_debug("turn to OTA_STA_GET_FILE\r\n");
    }
    return (uint8_t)err_code;
}

void ble_ota_get_current_upfrade_file_info(uint8_t *upgrade_info)
{
    ble_comm_get_upgrade_file_t *get_upgrade_file = (ble_comm_get_upgrade_file_t *)upgrade_info;
    
    memcpy(get_upgrade_file->product_id, ble_ota_param.product_id, sizeof(get_upgrade_file->product_id));
    memcpy(get_upgrade_file->hw_version, ble_ota_param.hw_version, sizeof(get_upgrade_file->hw_version));
    memcpy(get_upgrade_file->fw_version, ble_ota_param.fw_version, sizeof(get_upgrade_file->fw_version));
    get_upgrade_file->file_size = ble_ota_param.file_size;
    get_upgrade_file->image_crc = ble_ota_param.image_crc;
    get_upgrade_file->customer_code = ble_ota_param.customer_code;
    get_upgrade_file->data_addr = ble_ota_param.current_data_addr;
    get_upgrade_file->data_size = ble_ota_param.current_data_size;

/*    log_debug("get fw pid %s, hw %s, fw %s si 0x%08x, crc 0x%04x, code %d ad 0x%08x d_s %04x\r\n", 
                            (char *)get_upgrade_file->product_id,
                            (char *)get_upgrade_file->hw_version,
                            (char *)get_upgrade_file->fw_version,
                            get_upgrade_file->file_size,
                            get_upgrade_file->image_crc, 
                            get_upgrade_file->customer_code,
                            get_upgrade_file->data_addr,
                            get_upgrade_file->data_size);
*/

    data_to_smallmode((uint8_t *)&get_upgrade_file->file_size, sizeof(get_upgrade_file->file_size));
    data_to_smallmode((uint8_t *)&get_upgrade_file->image_crc, sizeof(get_upgrade_file->image_crc));
    data_to_smallmode((uint8_t *)&get_upgrade_file->customer_code, sizeof(get_upgrade_file->customer_code));
    data_to_smallmode((uint8_t *)&get_upgrade_file->data_addr, sizeof(get_upgrade_file->data_addr));
    data_to_smallmode((uint8_t *)&get_upgrade_file->data_size, sizeof(get_upgrade_file->data_size));
}

uint8_t ble_ota_download_upgrade_file_block(ble_ota_param_t *pble_ota_param)
{
    ble_nodified_host_process(APP_EVT_UPGRADE_FILE_REQ, 0, ble_tx_buff, &ble_tx_len);
    return 0;
}


uint8_t ble_ota_download_upgrade_file_block_done(ble_ota_param_t *pble_ota_param)
{
    ble_nodified_host_process(APP_EVT_UPGRADE_FILE_REQ, 0, ble_tx_buff, &ble_tx_len);
    return 0;
}


uint8_t ble_ota_download_upgrade_file(void)
{
//    BLE_CMD_GET_FW_ERR_CODE err_code;
    image_upgrade_info_t uprade_info;
    uint32_t ret = 0;

    if (ble_ota_param.current_data_addr == ble_ota_param.file_size)
    {
        log_debug("OTA_STA_GET_FILE_DONE %d \r\n", ble_ota_param.current_data_addr);
        uprade_info.file_size = ble_ota_param.file_size;
        uprade_info.image_crc = ble_ota_param.image_crc;
        
        if (ota_image_file_check_crc(&uprade_info, TYPE_IMAGE_NEW))
        {
            if (ota_copy_new_image_to_backup_image())
            {
                ret = ota_image_file_check_crc(&uprade_info, TYPE_IMAGE_BACKUP);
                log_debug("bak fw crc %d\r\n", ret);

                ota_upgrade_tag_update();
                ble_ota_param.err_code = GEI_FW_DONE_SUCCESS;
                ble_ota_param.upgrade_status = OTA_STA_ENTER_UPGRADE;
                log_debug("upgrade_tag_update\r\n");
            }
            else 
                log_debug("copy to backup_image error\r\n");
        }
        else 
        {
            ota_erase_image_file(TYPE_IMAGE_NEW);
            ble_ota_param.err_code = GEI_FW_DONE_FILE_CRC_ERROR;
            ble_ota_param.upgrade_status = OTA_STA_IDLE;
            log_debug("check_crc error\r\n");
        }
        ble_ota_download_upgrade_file_done();
    }
    else if (ble_ota_param.current_data_addr > ble_ota_param.file_size)
    {
    }
    else 
    {
//        log_debug("current addr %08x\r\n", ble_ota_param.current_data_addr);
        ble_ota_download_upgrade_file_block(&ble_ota_param);
        ble_ota_param.upgrade_status = OTA_STA_IDLE;
    }
    
    return 0;
}

uint8_t ble_ota_download_upgrade_file_done(void)
{
    ble_nodified_host_process(APP_EVT_UPGRADE_FILE_DONE, 0, ble_tx_buff, &ble_tx_len);
    return 0;
}

uint8_t ble_ota_enter_upgrade(void)
{
    image_upgrade_info_t upgrade_info;
    
    if (ota_check_upgrade_tag(&upgrade_info))
    {
        check_ble_tx_buff_is_empty();
        log_debug("@@@@jump to boot!!!\r\n");
        SPI_DisConfiguration();
        TIM6_deinit();
        //jump to boot
        jump2iap(OF_BOOTLOADER_ADDR);
    }
    ble_ota_param.upgrade_status = OTA_STA_IDLE;
    return 0;
}


//BLE OTA process
void ble_ota_process(void)
{
    switch (ble_ota_param.upgrade_status)
    {
        case OTA_STA_IDLE:
            break;
        case OTA_STA_GET_FILE_INFO:
            log_debug("OTA_STA_GET_FILE_INFO\r\n");
            ble_ota_upgrade_info();
            break;
        case OTA_STA_GET_FILE:
            ble_ota_download_upgrade_file();
            break;
        case OTA_STA_ENTER_UPGRADE:
            ble_ota_enter_upgrade();
            break;
        default:
            break;
    }
}

