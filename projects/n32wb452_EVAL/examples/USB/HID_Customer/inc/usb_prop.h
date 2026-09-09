/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file usb_prop.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#ifndef __USBFSD_PROP_H__
#define __USBFSD_PROP_H__
#include "stdint.h"
#include "usb_core.h"

typedef enum _HID_REQUESTS
{
    GET_REPORT = 1,
    GET_IDLE,
    GET_PROTOCOL,

    SET_REPORT = 9,
    SET_IDLE,
    SET_PROTOCOL
} HID_REQUESTS;

void CustomHID_init(void);
void CustomHID_Reset(void);
void CustomHID_SetConfiguration(void);
void CustomHID_SetDeviceAddress(void);
void CustomHID_Status_In(void);
void CustomHID_Status_Out(void);
USB_Result CustomHID_Data_Setup(uint8_t);
USB_Result CustomHID_NoData_Setup(uint8_t);
USB_Result CustomHID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t* CustomHID_GetDeviceDescriptor(uint16_t);
uint8_t* CustomHID_GetConfigDescriptor(uint16_t);
uint8_t* CustomHID_GetStringDescriptor(uint16_t);
USB_Result CustomHID_SetProtocol(void);
uint8_t* CustomHID_GetProtocolValue(uint16_t Length);
USB_Result CustomHID_SetProtocol(void);
uint8_t* CustomHID_GetReportDescriptor(uint16_t Length);
uint8_t* CustomHID_GetHIDDescriptor(uint16_t Length);

/* Exported define -----------------------------------------------------------*/
#define CustomHID_GetConfiguration USB_ProcessNop
//#define CustomHID_SetConfiguration          USB_ProcessNop
#define CustomHID_GetInterface       USB_ProcessNop
#define CustomHID_SetInterface       USB_ProcessNop
#define CustomHID_GetStatus          USB_ProcessNop
#define CustomHID_ClearFeature       USB_ProcessNop
#define CustomHID_SetEndPointFeature USB_ProcessNop
#define CustomHID_SetDeviceFeature   USB_ProcessNop
//#define CustomHID_SetDeviceAddress          USB_ProcessNop

#define REPORT_DESCRIPTOR 0x22

#endif /* __USBFSD_PROP_H__ */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
