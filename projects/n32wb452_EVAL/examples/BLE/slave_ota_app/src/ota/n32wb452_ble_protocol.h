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
 * @file n32wb452_ble_protocol.h
 * @author Nations
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#ifndef __N32WB452_BLE_PROTOCOL_H__
#define __N32WB452_BLE_PROTOCOL_H__
#include "app.h"

#define BLE_PKT_MAX_LEN 20
#define BLE_BUF_LEN    1024
#define BLE_TX_BUF_LEN (BLE_BUF_LEN>>1)

//typedef struct app_env_tag ble_packet_tag;

typedef __packed struct 
{
    uint16_t RxTotalLen;
    uint8_t RxCurrentLen;
    uint8_t RxFinishFlag;
    uint8_t RxBuf[BLE_BUF_LEN];
}ble_packet_tag;

typedef struct
{
    uint8_t packet_len : 5; // bit[4:0] BLE communication frame length
    uint8_t frame_id : 2;   // bit[6:5] BLE communication frame number,00->11->00
    uint8_t frame_end : 1;  // bit[7] BLE communication frame end bit
} BLE_FRAME_HEAD;

typedef struct
{
    BLE_FRAME_HEAD cfhead; // BLE communication frame head
    uint8_t cfid;          // BLE communication frame number
    uint8_t cfpayload[18]; // BLE communication frame data,max=18
} BLE_FRAME_DATA;

typedef enum
{
    FRAME_END     = 0,
    FRAME_NOT_END = 1,
} BLE_FRAME_END;
    

typedef struct
{
    uint8_t lock_model[8];
    uint8_t hw_ver[3];
    uint8_t firmware_ver[3];
    uint8_t se_ver[3];
    uint8_t motor_driver_ver[3];
} BLE_VER_FRAME_T;

#define APP_CMD_AUTH          0x00
#define APP_CMD_ADD_USER      0x01
#define APP_CMD_OPENDOOR      0x02
#define APP_CMD_GET_BAT       0x03
#define APP_CMD_GET_VERSION   0x04
#define APP_CMD_MODIFY_PIN    0x05
#define APP_CMD_ISSUE_USER    0x06
#define APP_CMD_DELETE_USER   0x07
#define APP_CMD_SET_PARA      0x08
#define APP_CMD_GET_PARA      0x09
#define APP_CMD_GET_USER_INFO 0x0A
#define APP_CMD_SET_TIME      0x0B
#define APP_CMD_RESTORE_SYS   0x0C
#define APP_CMD_NETWORK       0x0D
#define APP_CMD_GET_RECORD    0x0E
                                  
#define APP_CMD_SEND_NOTICE 0x40
                                  
#define APP_EVT_AUTH          0x80
#define APP_EVT_ADD_USER      0x81
#define APP_EVT_OPENDOOR      0x82
#define APP_EVT_GET_BAT       0x83
#define APP_EVT_GET_VERSION   0x84
#define APP_EVT_MODIFY_PIN    0x85
#define APP_EVT_ISSUE_USER    0x86
#define APP_EVT_DELETE_USER   0x87
#define APP_EVT_SET_PARA      0x88
#define APP_EVT_GET_PARA      0x89
#define APP_EVT_GET_USER_INFO 0x8A
#define APP_EVT_SET_TIME      0x8B
#define APP_EVT_RESTORE_SYS   0x8C
#define APP_EVT_NETWORK       0x8D
#define APP_EVT_GET_RECORD    0x8E
                                  
                                  
#define APP_EVT_SEND_NOTICE 0xC0


//OTA command
#define APP_CMD_NEW_FW_RDY              0x10
#define APP_CMD_NEW_FW_RDY_ACK          0x90
                                            
#define APP_EVT_UPGRADE_FILE_REQ        0x41
#define APP_EVT_UPGRADE_FILE_RESP       0xC1
                                            
#define APP_EVT_UPGRADE_FILE_DONE       0x42
#define APP_EVT_UPGRADE_FILE_DONE_RESP  0xC2




typedef enum
{
    HEAD_ENCRYP = 0xff00,  
    HEAD_NOENCRYP = 0xff01,
    HEAD_AUTH = 0xff02     
}BLE_COMM_HEAD;

typedef enum
{
    BLE_PRIVILEGE_ADMIN = 0,
    BLE_PRIVILEGE_TEMP  = 1,

    BLE_PRIVILEGE_NONE = 0xFF,
} BLE_PRIVILEGE_T;

typedef enum
{
    AUTH_SUCCESS = 0,
    AUTH_FAIL,
    AUTH_SUCCESS2,
}BLE_AUTHENTICATE_RESULT;

typedef enum _BLE_CMD_ADDUSER_STATUS
{
    STA_ADDUSER_SUCCESS = 0,
    STA_ADDUSER_PERMISSION_DENIED,
    STA_ADDUSER_DATE_MISSMATCHING,
}BLE_CMD_ADDUSER_STATUS;


typedef enum _BLE_CMD_OPENDOOR_STATUS
{
    STA_OPENDOOR_SUCCESS = 0,
    STA_OPENDOOR_FAIL,
    STA_OPENDOORING,
}BLE_CMD_OPENDOOR_STATUS;

typedef enum _BLE_CMD_GET_BATTERY_STATUS
{
    STA_GET_BATTERY_SUCCESS = 0,
    STA_GET_BATTERY_FAIL,
}BLE_CMD_GET_BATTERY_STATUS;

typedef enum _BLE_CMD_BATTERY_LEVEL
{
    BATTERY_LEVEL_0 = 0,
    BATTERY_LEVEL_25,
    BATTERY_LEVEL_50,
    BATTERY_LEVEL_75,
    BATTERY_LEVEL_100,
}BLE_CMD_BATTERY_LEVEL;

typedef enum _BLE_CMD_NEW_FW_RDY_ACK
{
    STA_NEW_FW_SUCCESS = 0, 
    STA_CODE_ERROR,         
    STA_CMD_ERROR,          
    STA_VERIFIED_ERROR,     
    STA_PID_ERROR,          
    STA_HW_VERSION_ERROR,   
    STA_FW_VERSION_ERROR,   
    STA_CUSTOMER_CODE_ERROR,
    STA_UPDATE_DONE         
}BLE_CMD_NEW_FW_RDY_STA;    

typedef enum _BLE_CMD_GET_FW_ERR_CODE
{
    GET_FW_DONE = 0,           
    GET_FW_DATA_ERROR,         
    GET_FW_CRC_ERROR,          
    GET_FW_NIMAGE_FLASH_ERROR, 
    GET_FW_NEWIMAGE_CRC_ERROR, 
    GET_FW_BAKIMAGE_ERROR      
}BLE_CMD_GET_FW_ERR_CODE;

typedef enum _BLE_CMD_GEI_FW_DONE_STA
{
    GEI_FW_DONE_SUCCESS = 0,        
    GEI_FW_DONE_CODE_ERROR,         
    GEI_FW_DONE_CMD_ERROR,          
    GEI_FW_DONE_CMD_FAIL,           
    GEI_FW_DONE_PID_ERROR,          
    GEI_FW_DONE_HW_VERSION_ERROR,   
    GEI_FW_DONE_FW_VERSION_ERROR,   
    GEI_FW_DONE_CUSTOMER_CODE_ERROR,
    GEI_FW_DONE_FILE_CRC_ERROR,     
    GEI_FW_DONE_FILE_SIZE_ERROR,    
}BLE_CMD_GEI_FW_DONE_STA;

typedef __packed struct  _ble_comm_authenticate_t 
{
    uint8_t user_privilege;
    uint8_t user_auth_data[20];
    uint8_t encryp_factor[8];
}ble_comm_authenticate_t;

typedef __packed struct  _ble_comm_authenticate_ack_t 
{
    uint8_t op_result;
    uint8_t encryp_factor[8];
}ble_comm_authenticate_ack_t;

typedef __packed struct  _ble_comm_add_user_t
{
    uint8_t user_type;
    uint8_t time_valid[15];
}ble_comm_add_user_t;

typedef __packed struct  _ble_comm_add_user_ack_t
{
    uint8_t op_result;
}ble_comm_add_user_ack_t;

typedef __packed struct  _ble_comm_opendoor_ack_t
{
    uint8_t op_result;
}ble_comm_opendoor_ack_t;

typedef __packed struct  _ble_comm_get_battery_ack_t
{
    uint8_t op_result;
    uint8_t batt_value_level;
    uint8_t batt_value_percent; 
}ble_comm_get_battery_ack_t;

typedef __packed struct  _ble_comm_get_version_ack_t 
{
    uint8_t op_result;
    uint8_t lock_model[8];
    uint8_t hw_version[3];
    uint8_t sw_version[3];
    uint8_t se_version[3];
    uint8_t reserved0[3]; 
    uint8_t sn[24];       
    uint8_t se_sn[16];    
    uint8_t fp_vendor;    
    uint8_t fp_model[8];  
    uint8_t nb_pid[4];    
    uint8_t imei[15];     
    uint8_t imsi[15];     
//    uint8_t hw_info[24];
}ble_comm_get_version_ack_t;

typedef __packed struct  _ble_comm_set_systime_t
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t week;
}ble_comm_set_systime_t;

typedef __packed struct  _ble_comm_set_systime_ack_t
{
    uint8_t op_result;
}ble_comm_set_systime_ack_t;

typedef __packed struct  _ble_comm_set_param_t 
{
    uint8_t volume;        
    uint8_t language;      
    uint8_t lockmode;      
    uint8_t alarm_en;      
    uint8_t dynamic_pwd_en;
    uint8_t open_dir;      
    uint8_t bluetooth_en;  
    uint8_t network_en;    
    uint8_t open_time;     

}ble_comm_set_param_t;

typedef __packed struct  _ble_comm_set_param_ack_t 
{
    uint8_t op_result;      
    uint8_t volume;         
    uint8_t language;       
    uint8_t lockmode;       
    uint8_t alarm_en;       
    uint8_t dynamic_pwd_en; 
    uint8_t open_dir;       
    uint8_t bluetooth_en;   
    uint8_t network_en;     
    uint8_t open_time;      

}ble_comm_set_param_ack_t;


typedef __packed struct  _ble_comm_get_param_t
{
    uint8_t op_result;      
    uint8_t battery;       
    uint8_t volume;        
    uint8_t language;      
    uint8_t lockmode;      
    uint8_t alarm_en;      
    uint8_t dynamic_pwd_en;
    uint8_t open_dir;      
    uint8_t bluetooth_en;  
    uint8_t network_en;    
    uint8_t open_time;     
}ble_comm_get_param_t;

typedef __packed struct  _ble_comm_new_fw_rdy_t
{
    uint8_t product_id[8]; 
    uint8_t hw_version[4]; 
    uint8_t fw_version[4]; 
    uint32_t file_size;    
    uint16_t image_crc;    
    uint16_t customer_code;
}ble_comm_new_fw_rdy_t;

typedef __packed struct  _ble_comm_new_fw_rdy_ack_t
{
    uint8_t op_result;      
}ble_comm_new_fw_rdy_ack_t;

typedef __packed struct  _ble_comm_get_upgrade_file_t
{
    uint8_t product_id[8];
    uint8_t hw_version[4];
    uint8_t fw_version[4];
    uint32_t file_size;
    uint16_t image_crc;
    uint16_t customer_code;
    uint32_t data_addr;
    uint16_t data_size;
}ble_comm_get_upgrade_file_t;

typedef __packed struct  _ble_comm_upgrade_data_t
{
    uint32_t data_addr;  
    uint16_t data_size;   
    //uint8_t *data_image;   
    //uint16_t checksum_crc16; 
}ble_comm_upgrade_data_t;

typedef __packed struct  _ble_comm_get_upgrade_file_done_t 
{
    uint8_t product_id[8];  
    uint8_t hw_version[4];  
    uint8_t fw_version[4];  
    uint32_t file_size;     
    uint16_t image_crc;     
    uint16_t customer_code; 
    uint8_t err_code;       
}ble_comm_get_upgrade_file_done_t;

typedef __packed struct  _ble_comm_get_upgrade_file_done_ack_t
{
    uint8_t product_id[8]; 
    uint8_t hw_version[4]; 
    uint8_t fw_version[4]; 
    uint32_t file_size;    
    uint16_t image_crc;    
    uint16_t customer_code;
    uint8_t err_code;      
}ble_comm_get_upgrade_file_done_ack_t;

typedef __packed struct _ble_comm_header_t
{
    uint16_t frame_head;
    uint16_t frame_len;
    uint16_t frame_len_invert;
    uint8_t option;
}ble_comm_header_t;

typedef __packed union _ble_payload_t
{
    ble_comm_authenticate_t ble_comm_authenticate;
    ble_comm_authenticate_ack_t ble_comm_authenticate_ack;
    ble_comm_add_user_t ble_comm_add_user;
    ble_comm_add_user_ack_t ble_comm_add_user_ack;
    ble_comm_opendoor_ack_t ble_comm_opendoor_ack;
    ble_comm_get_battery_ack_t ble_comm_get_battery_ack;
    ble_comm_get_version_ack_t ble_comm_get_version_ack;    
    ble_comm_set_systime_t ble_comm_set_systime;
    ble_comm_set_systime_ack_t ble_comm_set_systime_ack;
    ble_comm_set_param_t ble_comm_set_param;
    ble_comm_set_param_ack_t ble_comm_set_param_ack;
    ble_comm_get_param_t ble_comm_get_param;
    ble_comm_new_fw_rdy_t ble_comm_new_fw_rdy;
    ble_comm_get_upgrade_file_t ble_comm_get_upgrade_file;
    ble_comm_upgrade_data_t ble_comm_upgrade_data;
    ble_comm_get_upgrade_file_done_t ble_comm_get_upgrade_file_done;
    ble_comm_get_upgrade_file_done_ack_t ble_comm_get_upgrade_file_done_ack;
}ble_payload_t;

typedef __packed struct  _ble_comm_frame_t
{
    ble_comm_header_t ble_header;
    ble_payload_t ble_payload;
    //uint16_t chksum_crc16;
}ble_comm_frame_t;


void ble_send_packet(uint16_t* tx_len, uint8_t* pdata);
uint8_t ble_communication_parse(uint8_t* in_data, uint16_t in_len, uint8_t* out_data, uint16_t* out_len);
void ble_packet_parse(uint8_t const* in_data, uint8_t const in_len, uint8_t* out_data);
void ble_nodified_host_process(uint8_t option, uint8_t status, uint8_t *out_data, uint16_t *out_len);
void data_to_smallmode(uint8_t *data, uint8_t len);

#endif //__N32WB452_BLE_PROTOCOL_H__

