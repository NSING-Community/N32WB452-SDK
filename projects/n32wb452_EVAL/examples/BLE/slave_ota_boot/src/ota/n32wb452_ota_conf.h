
/**
 * @file n32wb452_ota_conf.h * @author Nations
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#ifndef __BOOT_CONF_H__
#define __BOOT_CONF_H__

//#define IMAGE_HEADER_SIZE               sizeof(image_header_t)
#define FLASH_PAGE_SIZE                 2048

/* OF - on flash ------------------------------------------------------------*/
/* bootloader start address */
#define OF_BOOTLOADER_ADDR              (0x08000000)

/* bootloader size */
#define OF_BOOTLOADER_SIZE              (0x8000)

/* n32wb452 flash size */
#define OF_INNER_FLSASH_SIZE            (0x80000)

/* firmware start address */
#define OF_FIRMWARE_ADDR                (OF_BOOTLOADER_ADDR+OF_BOOTLOADER_SIZE)
#define OF_FIRMWARE_SIZE                (OF_INNER_FLSASH_SIZE-OF_BOOTLOADER_SIZE)

/* firmware entry address */
#define OF_FIRMWARE_ENTRY_ADDR          (OF_FIRMWARE_ADDR)

//#define OF_SYSINFO_SIZE                 128
//#define OF_FIRMWARE_HEADER_ADDR         (SYSINFO_ADDR + OF_SYSINFO_SIZE)

//#define OF_UPDATE_EN_ADDR               (OF_FIRMWARE_HEADER_ADDR + IMAGE_HEADER_SIZE)
//#define OF_UPDATE_EN_CLOSE              0xAA
//#define OF_UPDATE_EN_OPEN               0xFF

#define OF_QUIET_FLAG_ADDR              (OF_UPDATE_EN_ADDR + 1)
#define OF_QUIET_FLAG_SET               0xAA
#define OF_QUIET_FLAG_RESET             0xFF

/* SPI FLASH base address */
#define SF_FLASH_BASE_ADDR              (0x000000)

/* backup image */
#define SF_BACKUP_IMAGE_ADDR            (SF_FLASH_BASE_ADDR)
#define SF_BACKUP_IMAGE_SIZE            (OF_FIRMWARE_SIZE)
#define SF_BACKUP_IMAGE_INFO_ADDR       (SF_BACKUP_IMAGE_ADDR+SF_BACKUP_IMAGE_SIZE)
#define SF_BACKUP_IMAGE_INFO_SIZE       (FLASH_PAGE_SIZE)

/* new image */
#define SF_NEW_IMAGE_ADDR               (SF_BACKUP_IMAGE_INFO_ADDR+SF_BACKUP_IMAGE_INFO_SIZE)
#define SF_NEW_IMAGE_SIZE               (OF_FIRMWARE_SIZE)
#define SF_NEW_IMAGE_INFO_ADDR          (SF_NEW_IMAGE_ADDR+SF_NEW_IMAGE_SIZE)
#define SF_NEW_IMAGE_INFO_SIZE          (FLASH_PAGE_SIZE)

#define OF_FIRMWARE_HEADER_ADDR         (SF_NEW_IMAGE_INFO_ADDR + SF_NEW_IMAGE_INFO_SIZE) 
#define OF_FIRMWARE_HEADER_SIZE         (FLASH_PAGE_SIZE)

/*other memory define -----------------------------------------------------*/

#define SF_WRITE_PACK_512               512
#define SF_WRITE_PACK_256               256
#define SF_WRITE_PACK_64                64
#define SF_WRITE_PACK_SIZE              SF_WRITE_PACK_512   

/* types ---------------------------------------------------------------------*/
/* variable ------------------------------------------------------------------*/


#endif /*__BOOT_CONF_H__*/

