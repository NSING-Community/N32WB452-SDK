/*
 * ble_encrypt_interface.c
 *
 *  Created on: Apr 4, 2019
 *      Author: andy
 *  Description:

 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sha1.h"
#include "SecuritySM4.h"
#include "ble_encrypt_interface.h"
#include "sys_type.h"

static volatile char cBleSessionKey[16];

u8 ble_encrypt_authenticate_to_get_session_key(BLE_ENCRYPT_AUTH_DATA_T* auth_data,
                                               u8 admin_pwd[20],
                                               u8 admin_len,
                                               u8 out_rnd[8])
{
    u8 rtv = FALSE;
    u8 hash[20];
    u8 temp_buf[28];        // 20B HASH + 8 random
    int temp_len = 28;      
    u8 local_auth_data[20]; 

    if (get_sha1((const char*)admin_pwd, admin_len, hash) == TRUE)
    {   
        //0x31, 0xD3, 0x49, 0x2F, 0x3D, 0x96, 0x2D, 0xA7;

        memcpy(temp_buf, hash, 20);
        memcpy(&temp_buf[20], auth_data->ble_rnd, 8);

        if (get_sha1((const char*)temp_buf, temp_len, local_auth_data) == TRUE)
        {
            if (memcmp(auth_data->ble_auth, local_auth_data, 20) == 0)
            {
                rtv = TRUE;
            }
        }
    }

    if (rtv == TRUE) // caculate session key
    {
        // if (hub_get_random(8, out_rnd) == FALSE)
        {
            memset(out_rnd, 0xF1, 8);
        }

        memcpy(temp_buf, auth_data->ble_rnd, 4);
        memcpy(&temp_buf[4], out_rnd, 4);
        memcpy(&temp_buf[8], &(auth_data->ble_rnd[4]), 4);
        memcpy(&temp_buf[12], &out_rnd[4], 4);
        temp_len = 16;

        getByteEncryptMessageSM4((char*)temp_buf,
                                 (char*)cBleSessionKey,
                                 (int*)&temp_len,
                                 (const char*)hash);
        //        PRINTF("ble_encrypt_authenticate_to_get_session_key,cBleSessionKey:");
        //        SEGGER_RTT_print_data(cBleSessionKey, 16);
    }
    
    return rtv;
}

u8 ble_encrypt_data(u8* text, u8* out_encrypt, u32 len)
{
    if (getByteEncryptMessageSM4((char*)text, (char*)out_encrypt, (int*)&len, (const char*)cBleSessionKey) == TRUE)
    {
        return TRUE;
    }

    return FALSE;
}


u8 ble_decrypt_data(u8* encrypted_data,
                    u8* out_decrypt,
                    u32 len) 
{
    //    PRINTF("ble_decrypt_data,cBleSessionKey:");
    //    SEGGER_RTT_print_data(cBleSessionKey, 16);
    if (getByteDecryptMessageSM4((char*)encrypted_data, (char*)out_decrypt, (int)len, (const char*)cBleSessionKey) == TRUE)
    {
        return TRUE;
    }

    return FALSE;
}
