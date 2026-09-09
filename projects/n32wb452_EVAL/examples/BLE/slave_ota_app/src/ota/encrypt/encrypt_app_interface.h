#ifndef ENCRYPT_APP_INTERFACE_H
#define ENCRYPT_APP_INTERFACE_H

#include "n32wb452.h"

#define NOT_ENOUGH_HEAP_MEMORY (FALSE)
#define DECRYPTED_FALSE        (0xFF)

typedef struct
{
    u8* data;
    u16 len;
} MSG_T;

typedef struct
{
    MSG_T msg;
    u8 random_from_lock[8];
    u8 random_from_server[8];
} ENCRYPTED_DATA_T;

char encrypt_data(MSG_T* encrypted_msg_out, ENCRYPTED_DATA_T* unencrypted_data_in, u8 current_lock_rnd[8]);

char decrypt_data(MSG_T* decrypted_msg_out, ENCRYPTED_DATA_T* encrypted_data_in, u8 current_server_rnd[8]);

u8 encrypt_unlock_data(u8* in_data, u8* out_data, u32 in_len);
u8 decrypt_unlock_data(u8* in_data, u8* out_data, u32 in_len);

#endif
