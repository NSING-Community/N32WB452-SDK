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
 * @file gsensor.c
 * @author Nations
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "main.h"
#include "qma_i2c_drv.h"
#include "gsensor.h"

void qma7981_int(void)
{
    qma_i2c_write_data(0xc3, 0x11); //set device to active mode and sleep time to 0

    /*set interrupt, chip senses vibration INT1 pin outputs high level */
    qma_i2c_write_data(0x07, 0x09); // any_motion interrupt is triggered by X and Y axis
    qma_i2c_write_data(0x1F, 0x18); // enable X and Y axis any_motion interrupt
    qma_i2c_write_data(0x63, 0x1a); // any_motion interrupt remap to INT1 pin
    qma_i2c_write_data(0x32, 0x2e); // any_motion threshold value

    delay_ms(70);
}

void qma7981_read_raw_xyz(G_SENSOR_DATA* g_sensor_data) //read X,Y,Z axis data
{
//    uint16_t databuf[3];

//    databuf[0] = qma_i2c_read_data(0x01); // X axis low bits
//    databuf[1] = qma_i2c_read_data(0x02); // X axis high bits
//    databuf[2] = qma_i2c_read_data(0x03); // Y axis low bits
//    databuf[3] = qma_i2c_read_data(0x04); // Y axis high bits
//    databuf[4] = qma_i2c_read_data(0x05); // Z axis low bits
//    databuf[5] = qma_i2c_read_data(0x06); // Z axis high bits
    qma_i2c_read_ndata(0x01, 6, (uint8_t *)(&(g_sensor_data->x_acc)));
//    printf("x=%04x,y=%04x,z=%04x\r\n",g_sensor_data->x_acc,g_sensor_data->y_acc,g_sensor_data->z_acc);
    
    g_sensor_data->x_acc = g_sensor_data->x_acc >> 2; // X axis origin data
    g_sensor_data->y_acc = g_sensor_data->y_acc >> 2; // Y axis origin data
    g_sensor_data->z_acc = g_sensor_data->z_acc >> 2; // Z axis origin data
}

int16_t dabs(int16_t x) //turn a negative number into a positive number
{
    int16_t Acc_data1 = 0;
    int16_t Acc_data2 = 0;

    if (x > 0)
    {
        Acc_data1 = x - 0;
        return Acc_data1;
    }
    else
    {
        Acc_data2 = 65535 - x;
        return Acc_data2;
    }
}

#if 0

 QMA7981_int();
 
 
 qma7981_read_raw_xyz();//read X,Y axis data
 
 if((dabs(data[0]) > 1500)|(dabs(data[1]) > 1500))  
 {
                  //the tilt is greater than a certain angle
 }
 else
 {
                  //the tilt lower than a certain angle or in a horizontal position
 }
#endif
