/********************************************************************************************************
 * @file	bth_device.h
 *
 * @brief	This is the header file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef BTH_DEVICE_H_
#define BTH_DEVICE_H_


#define BTH_DEVICE_NAME_ENABLE 		1 //
#if (BTH_DEVICE_NAME_ENABLE)
#define BTH_DEVICE_NAME_LENGTH 		16
#else
#define BTH_DEVICE_NAME_LENGTH 		0
#endif

// PDL -- Peer Device List
#define BTH_DEVICE_SAVE_SIGN       0x3C
#define BTH_DEVICE_SAVE_VERS       0x11 // 1.1
#define BTH_DEVICE_SAVE_ADDR0      TLK_CFG_FLASH_BT_PAIR_ADDR0
#define BTH_DEVICE_SAVE_ADDR1      TLK_CFG_FLASH_BT_PAIR_ADDR1
#define BTH_DEVICE_ITEM_NUMB       5
#define BTH_DEVICE_ITEM_LENS       (28 + BTH_DEVICE_NAME_LENGTH)
#define BTH_DEVICE_ITEM_SIZE       ((BTH_DEVICE_ITEM_LENS + 3) & 0xFFFC) // For Align

#define BTH_DEVICE_DEVCLASS_OFFSET 0
#define BTH_DEVICE_DEVCLASS_LENGTH 4
#define BTH_DEVICE_LINKKEY_OFFSET  4
#define BTH_DEVICE_LINKKEY_LENGTH  16
#define BTH_DEVICE_DEVADDR_OFFSET  20
#define BTH_DEVICE_DEVADDR_LENGTH  6
#define BTH_DEVICE_DEVNAME_OFFSET  28
#define BTH_DEVICE_DEVNAME_LENGTH  BTH_DEVICE_NAME_LENGTH


typedef enum {                     // Major Class
    BTH_DEVICE_DTYPE_NONE     = 0,
    BTH_REMOTE_DTYPE_COMPUTER = 1, // desktop, notebook, PDA, organizer,
    BTH_REMOTE_DTYPE_PHONE    = 2, // cellular, cordless, pay phone, modem,
    BTH_REMOTE_DTYPE_HEADSET  = 4, // Audio/Video: headset, speaker, stereo, video display, VCR,
    BTH_REMOTE_DTYPE_OTHER    = 0xFF,
} BTH_DEVICE_DTYPE_ENUM;


typedef struct {
    uint32 devClass; // Device Type, refer BTH_DEVICE_DTYPE_ENUM
    uint08 linkKey[16];
    uint08 devAddr[6];
    uint08 reserve[2]; // You need to modify the init interface at the same time.
#if (BTH_DEVICE_NAME_ENABLE)
    uint08 devName[BTH_DEVICE_NAME_LENGTH];
#endif
} __attribute__((__packed__)) bth_device_item_t;

typedef struct {
    uint08 sortNum;
    uint08 itemCnt;
    uint16 resv001;
    tlkapi_save_ctrl_t save; // Peer Device List Saved
    tlkapi_save_item_t item[BTH_DEVICE_ITEM_NUMB];
    uint08 buff[BTH_DEVICE_ITEM_NUMB][BTH_DEVICE_ITEM_SIZE];
} bth_device_ctrl_t;


/******************************************************************************
 * Function: bth_device_init
 * Descript:
 * Params: None.
 * Return: 0 on success, negative error code on error.
 *******************************************************************************/
int bth_device_init(void);

/******************************************************************************
 * Function:bth_device_count
 * Descript: Get the number of device.
 * Params: .
 * Return: The number of devices.
 *******************************************************************************/
int bth_device_count(void);
void bth_device_print(void);

void bth_device_update(uint08 *pDevAddr);


/******************************************************************************
 * Function: bth_device_addItem
 * Descript: store the information of a bonded device.
 * Params:
 *        @devClass[IN]--The class type of device.
 *        @pDevAddr[IN]--The peer address of device.
 *        @pLinkKey[IN]--The link Key.
 *        @pDevName[IN]-- The device name
 * Return: TLK_ENONE is success, other value failure.
 *******************************************************************************/
int bth_device_addItem(uint32 devClass, uint08 *pDevAddr, uint08 *pLinkKey, uint08 *pDevName);

/******************************************************************************
 * Function: bth_device_delItem
 * Descript: delete the information of a bonded device.
 * Params:
 *        @pDevAddr[IN]--The peer address of device.
 * Return: None.
 *******************************************************************************/
void bth_device_delItem(uint08 *pDevAddr);

/******************************************************************************
 * Function: bth_device_clsItem
 * Descript: clean all stored section.
 * Params:
 *        @pDevAddr[IN]--The peer address of device.
 * Return: None.
 *******************************************************************************/
void bth_device_clsItem(void);

/******************************************************************************
 * Function: bth_device_saveName
 * Descript: Save device name.
 * Params:
 *     @pDevAddr[IN]--The peer address of device.
 *     @pName--Name of the device to be saved.
 *     @nameLen--The length of the device's name.
 * return: "TLK_ENONE" means success, others mean failure.
 * Notice:
 *     1.The device name can be saved only once. If you need to re-record, you
 *       need to re-pair.
 *     2.In the "devName" field of item, the first byte is the name length.
 *******************************************************************************/
int bth_device_saveName(uint08 *pDevAddr, uint08 *pName, uint16 nameLen);

/******************************************************************************
 * Function: bth_device_getName
 * Descript: Retrieve the device name from the device list.
 * Params:
 *     @pDevAddr[IN]--The peer address of device.
 *     @pBuffer--The buffer of the name to be saved.
 *     @buffLen--The length of the buffer.
 *     @pNameLen--The length of the device's name to be saved.
 * return: "TLK_ENONE" means success, others mean failure.
 * Notice:
 *     1.If "pNameLen" is null, the field will not be operated.
 *******************************************************************************/
int bth_device_getName(uint08 *pDevAddr, uint08 *pBuffer, uint16 buffLen, uint16 *pNameLen);

/******************************************************************************
 * Function: bth_device_getInfo
 * Descript: Get the stored device info.
 * Params:
 *        @index[IN]--The index of a stored device.
 * Return: Bt device item.
 *******************************************************************************/
bth_device_item_t *bth_device_getInfo(uint08 index);

/******************************************************************************
 * Function: bth_device_getItem
 * Descript: Get specified device info.
 * Params:
 *        @pDevAddr[IN]--The device bt address.
 *        @index[IN]--The index of a stored device.
 * Return: Bt device item.
 *******************************************************************************/
bth_device_item_t *bth_device_getItem(uint08 *pDevAddr, uint08 *pIndex);

/******************************************************************************
 * Function: bth_device_getItem
 * Descript: Get specified device info.
 * Params:
 *        @pDevAddr[IN]--The device bt address.
 * Return: Bt device item.
 *******************************************************************************/
uint08 *bth_device_getLinkKey(uint08 *pDevAddr);

/******************************************************************************
 * Function: bth_device_getLast
 * Descript: Get The last device info.
 * Params: None.
 * Return: Bt device item.
 *******************************************************************************/
bth_device_item_t *bth_device_getLast(void);

/******************************************************************************
 * Function: bth_device_getLastByType
 * Descript: Get device info.
 * Params:
 *        @type[IN]--The device class type.
 * Return: Bt device item.
 *******************************************************************************/
bth_device_item_t *bth_device_getLastByType(uint08 type);


#endif /* BTH_DEVICE_H_ */
