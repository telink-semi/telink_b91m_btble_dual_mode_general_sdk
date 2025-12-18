/********************************************************************************************************
 * @file	bth_device.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#include "bth_stdio.h"
#include "bth_device.h"


#define BTH_DEV_DBG_FLAG ((TLK_MAJOR_DBGID_BTH << 24) | (TLK_MINOR_DBGID_BTH_DEV << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTH_DEV_DBG_SIGN nullptr


static bth_device_ctrl_t sBthDeviceCtrl;


/******************************************************************************
 * Function: bth_device_init
 * Descript:
 * Params: None.
 * return: "TLK_ENONE" means success, others mean failure.
 *******************************************************************************/
int bth_device_init(void)
{
    int ret;
    uint08 index;
	unsigned int addr0, addr1;

    tmemset(&sBthDeviceCtrl, 0, sizeof(bth_device_ctrl_t));
	addr0 = tlkcfg_getFlashAddr(BTH_DEVICE_SAVE_ADDR0);
	addr1 = tlkcfg_getFlashAddr(BTH_DEVICE_SAVE_ADDR1);
	if(addr0 == 0 || addr1 == 0) {
		return -TLK_EFAIL;
	}

    // Init
    for (index = 0; index < BTH_DEVICE_ITEM_NUMB; index++) {
        sBthDeviceCtrl.item[index].address = 0;
        sBthDeviceCtrl.item[index].dataLen = BTH_DEVICE_ITEM_LENS;
        sBthDeviceCtrl.item[index].buffLen = BTH_DEVICE_ITEM_SIZE;
        sBthDeviceCtrl.item[index].pBuffer = sBthDeviceCtrl.buff[index];
    }
    ret = tlkapi_save4_init(&sBthDeviceCtrl.save,
                            BTH_DEVICE_SAVE_SIGN,
                            BTH_DEVICE_SAVE_VERS,
                            BTH_DEVICE_ITEM_LENS,
                            addr0,
                            addr1);
    if (ret != TLK_ENONE) {
        tlkapi_error(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_init: init failure");
    }

    // Load
    ret = tlkapi_save4_load(&sBthDeviceCtrl.save, sBthDeviceCtrl.item, BTH_DEVICE_ITEM_NUMB, 0);
    if (ret < 0) {
        tlkapi_error(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_init: load failure", 0, 0);
    } else {
        sBthDeviceCtrl.itemCnt = ret;
        tlkapi_trace(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_init: load success");
    }

    return TLK_ENONE;
}


/******************************************************************************
 * Function:bth_device_count
 * Descript: Get the number of device.
 * Params: .
 * return: The number of devices.
 *******************************************************************************/
int bth_device_count(void)
{
    return sBthDeviceCtrl.itemCnt;
}

void bth_device_update(uint08 *pDevAddr)
{
    uint32 devClass;
    uint08 linkKey[16];
    bth_device_item_t *pItem;

    pItem = bth_device_getItem(pDevAddr, nullptr);
    if (pItem == nullptr) {
        return;
    }

    devClass = pItem->devClass;
    tmemcpy(linkKey, pItem->linkKey, 16);
#if BTH_DEVICE_NAME_ENABLE
    bth_device_addItem(devClass, pDevAddr, linkKey, pItem->devName);
#else
    bth_device_addItem(devClass, pDevAddr, linkKey, nullptr);
#endif
}

/******************************************************************************
 * Function: bth_device_addItem
 * Descript: store the information of a bonded device.
 * Params:
 *        @devClass[IN]--The class type of device.
 *        @pDevAddr[IN]--The peer address of device.
 *        @pLinkKey[IN]--The link Key.
 *        @pDevName[IN]-- The device name
 * return: TLK_ENONE is success, other value failure.
 *******************************************************************************/
int bth_device_addItem(uint32 devClass, uint08 *pDevAddr, uint08 *pLinkKey, uint08 *pDevName)
{
    int ret;
#if (BTH_DEVICE_NAME_ENABLE)
    uint08 devName[BTH_DEVICE_NAME_LENGTH];
#endif
    tlkapi_save_item_t *pIItem;
    bth_device_item_t *pDItem;

    if (pDevAddr == nullptr || pLinkKey == nullptr) {
        tlkapi_trace(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_addItem: failure - error param ");
        return -TLK_EPARAM;
    }

#if (BTH_DEVICE_NAME_ENABLE)
	if (pDevName == nullptr) return -TLK_EPARAM;
    pDItem = bth_device_getItem(pDevAddr, nullptr);
    if (pDItem == nullptr || pDItem->devName[0] == 0xFF) {
        if (pDevName[0] == 0) {
            devName[0] = 0xFF;
        } else {
            if (pDevName[0] > BTH_DEVICE_NAME_LENGTH - 1) {
                devName[0] = BTH_DEVICE_NAME_LENGTH - 1;
            } else {
                devName[0] = pDevName[0];
            }
            tmemcpy(&devName[1], &pDevName[1], devName[0]);
        }
    } else {
        if (pDevName[0] != 0) {
            if (pDevName[0] > BTH_DEVICE_NAME_LENGTH - 1) {
                devName[0] = BTH_DEVICE_NAME_LENGTH - 1;
            } else {
                devName[0] = pDevName[0];
            }
            tmemcpy(&devName[1], &pDevName[1], devName[0]);
        } else {
            tmemcpy(devName, pDItem->devName, BTH_DEVICE_NAME_LENGTH);
        }
    }
#endif	// (BTH_DEVICE_NAME_ENABLE)

    bth_device_delItem(pDevAddr);
    if (sBthDeviceCtrl.itemCnt == BTH_DEVICE_ITEM_NUMB) {
        pDItem = bth_device_getInfo(0);
        bth_device_delItem(pDItem->devAddr);
    }
    if (sBthDeviceCtrl.itemCnt >= BTH_DEVICE_ITEM_NUMB) {
        tlkapi_fatal(BTH_DEV_DBG_FLAG,
                     BTH_DEV_DBG_SIGN,
                     "bth_device_addItem: fault !!! cnt-%d",
                     sBthDeviceCtrl.itemCnt);
        return -TLK_EFAIL;
    }

    pIItem           = &sBthDeviceCtrl.item[sBthDeviceCtrl.itemCnt];
    pDItem           = (bth_device_item_t *)pIItem->pBuffer;
    pDItem->devClass = devClass;
    tmemcpy(pDItem->linkKey, pLinkKey, BTH_DEVICE_LINKKEY_LENGTH);
    tmemcpy(pDItem->devAddr, pDevAddr, BTH_DEVICE_DEVADDR_LENGTH);
#if (BTH_DEVICE_NAME_ENABLE)
    if (devName[0] == 0xFF) {
        tmemset(pDItem->devName, 0xFF, BTH_DEVICE_NAME_LENGTH);
    } else {
        tmemcpy(pDItem->devName, devName, BTH_DEVICE_NAME_LENGTH);
    }
#endif

    ret = tlkapi_save4_save(&sBthDeviceCtrl.save, pIItem, 1);
    if (ret == -TLK_EOVERFLOW) {
        ret = tlkapi_save4_migrate(&sBthDeviceCtrl.save, sBthDeviceCtrl.item, sBthDeviceCtrl.itemCnt + 1);
    }
    if (ret != TLK_ENONE) {
        tlkapi_array(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_addItem: failure ", pDevAddr, 6);
    } else {
        sBthDeviceCtrl.itemCnt++;
        tlkapi_array(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_addItem: success ", pDevAddr, 6);
    }
    return ret;
}

/******************************************************************************
 * Function: bth_device_delItem
 * Descript: delete the information of a bonded device.
 * Params:
 *        @pDevAddr[IN]--The peer address of device.
 * return: None.
 *******************************************************************************/
void bth_device_delItem(uint08 *pDevAddr)
{
    uint08 index = 0;
    if (sBthDeviceCtrl.itemCnt == 0) {
        return;
    }
    if (bth_device_getItem(pDevAddr, &index) == nullptr) {
        tlkapi_array(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_delItem: failure - no item", pDevAddr, 6);
        return;
    }
    tlkapi_save4_remove(&sBthDeviceCtrl.save, &sBthDeviceCtrl.item[index], 1);
    tlkapi_trace(BTH_DEV_DBG_FLAG,
                 BTH_DEV_DBG_SIGN,
                 "bth_device_delItem: {index-%d,addr-0x%x}",
                 index,
                 *(uint32 *)pDevAddr);
    if (index + 1 != sBthDeviceCtrl.itemCnt) {
        uint08 *pBuffer = sBthDeviceCtrl.item[index].pBuffer;
        tmemcpy(&sBthDeviceCtrl.item[index],
                &sBthDeviceCtrl.item[index + 1],
                (sBthDeviceCtrl.itemCnt - index - 1) * sizeof(tlkapi_save_item_t));
        sBthDeviceCtrl.item[sBthDeviceCtrl.itemCnt - 1].pBuffer = pBuffer;
    }
    sBthDeviceCtrl.itemCnt--;
}

/******************************************************************************
 * Function: bth_device_clsItem
 * Descript: clean all stored section.
 * Params:
 *        @pDevAddr[IN]--The peer address of device.
 * return: None.
 *******************************************************************************/
void bth_device_clsItem(void)
{
    if (sBthDeviceCtrl.itemCnt == 0) {
        return;
    }
    sBthDeviceCtrl.itemCnt = 0;
    tlkapi_save4_clean(&sBthDeviceCtrl.save);
}

void bth_device_print(void)
{
    uint08 index;
    bth_device_item_t *pDItem;
    tlkapi_trace(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_print: %d", sBthDeviceCtrl.itemCnt);
    for (index = 0; index < sBthDeviceCtrl.itemCnt; index++) {
        pDItem = bth_device_getInfo(index);
        if (pDItem == nullptr) {
            break;
        }
        tlkapi_trace(BTH_DEV_DBG_FLAG,
                     BTH_DEV_DBG_SIGN,
                     "   (index-%d,class-0x%x,addr-0x%x,linkey-0x%x): ",
                     index,
                     pDItem->devClass,
                     *(uint32 *)(pDItem->devAddr),
                     *(uint32 *)(pDItem->linkKey));
    }
}

/******************************************************************************
 * Function: bth_device_saveName
 * Descript: Store the device name in the flash.
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
int bth_device_saveName(uint08 *pDevAddr, uint08 *pName, uint16 nameLen)
{
#if (BTH_DEVICE_NAME_ENABLE)
    int ret;
    uint08 index;
    bth_device_item_t *pItem;

    if (pDevAddr == nullptr || pName == nullptr || nameLen == 0) {
        tlkapi_error(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_saveName: failure - error param");
        return -TLK_EPARAM;
    }

    index = 0;
    pItem = bth_device_getItem(pDevAddr, &index);
    if (pItem == nullptr) {
        tlkapi_array(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_saveName: failure - no item", pDevAddr, 6);
        return -TLK_ENOOBJECT;
    }
    if (pItem->devName[0] != 0xFF) {
        tlkapi_error(BTH_DEV_DBG_FLAG,
                     BTH_DEV_DBG_SIGN,
                     "bth_device_saveName: failure - name exist nameLen[%d]",
                     pItem->devName[0]);
        return -TLK_EEXIST;
    }
    if (nameLen > BTH_DEVICE_NAME_LENGTH - 1) {
        pItem->devName[0] = BTH_DEVICE_NAME_LENGTH - 1;
    } else {
        pItem->devName[0] = nameLen;
    }
    tmemcpy(pItem->devName + 1, pName, pItem->devName[0]);
    ret = tlkapi_save4_modify(&sBthDeviceCtrl.save,
                              &sBthDeviceCtrl.item[index],
                              BTH_DEVICE_DEVNAME_OFFSET,
                              BTH_DEVICE_DEVNAME_LENGTH);
    if (ret == TLK_ENONE) {
        tlkapi_trace(BTH_DEV_DBG_FLAG,
                     BTH_DEV_DBG_SIGN,
                     "bth_device_saveName: success - nameLen[%d],saveLen[%d]",
                     nameLen,
                     pItem->devName[0]);
    } else {
        tlkapi_error(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_saveName: failure - save fail[%d]", ret);
    }
    return ret;
#else
    return -TLK_ENOSUPPORT;
#endif
}

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
int bth_device_getName(uint08 *pDevAddr, uint08 *pBuffer, uint16 buffLen, uint16 *pNameLen)
{
#if (BTH_DEVICE_NAME_ENABLE)
    bth_device_item_t *pItem;

    if (pDevAddr == nullptr || pBuffer == nullptr || buffLen == 0) {
        tlkapi_error(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getName: failure - error param");
        return -TLK_EPARAM;
    }

    pItem = bth_device_getItem(pDevAddr, nullptr);
    if (pItem == nullptr) {
        tlkapi_array(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getName: failure - no item", pDevAddr, 6);
        return -TLK_ENOOBJECT;
    }

    if (pItem->devName[0] == 0xFF) {
        tlkapi_error(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getName: failure - no name");
        return -TLK_EFAIL;
    }
    if (buffLen > pItem->devName[0]) {
        buffLen = pItem->devName[0];
    }
    if (buffLen != 0) {
        tmemcpy(pBuffer, pItem->devName + 1, buffLen);
    }
    if (pNameLen != nullptr) {
        *pNameLen = buffLen;
    }
    return TLK_ENONE;
#else
    return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: bth_device_getInfo
 * Descript: Get the stored device info.
 * Params:
 *        @index[IN]--The index of a stored device.
 * return: Bt device item.
 *******************************************************************************/
bth_device_item_t *bth_device_getInfo(uint08 index)
{
    if (index >= sBthDeviceCtrl.itemCnt) {
        return nullptr;
    } else {
        return ((bth_device_item_t *)(sBthDeviceCtrl.item[index].pBuffer));
    }
}

/******************************************************************************
 * Function: bth_device_getItem
 * Descript: Get specified device info.
 * Params:
 *        @pDevAddr[IN]--The device bt address.
 *        @index[IN]--The index of a stored device.
 * return: Bt device item.
 *******************************************************************************/
bth_device_item_t *bth_device_getItem(uint08 *pDevAddr, uint08 *pIndex)
{
    uint08 index;
    bth_device_item_t *pDItem;

    if (pDevAddr == nullptr) {
        tlkapi_error(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getItem: failure - error param");
        return nullptr;
    }

    //	tlkapi_trace(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getItem: ", sBthDeviceCtrl.itemCnt);
    for (index = 0; index < sBthDeviceCtrl.itemCnt; index++) {
        pDItem = (bth_device_item_t *)(sBthDeviceCtrl.item[index].pBuffer);
        //		tlkapi_trace(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getItem:
        //{index-%d,class-0x%x,addr-0x%x,linkey-0x%x}", 			index, pDItem->devClass, *(uint32*)pDItem->devAddr,
        //*(uint32*)pDItem->linkKey);
        if (tmemcmp(pDItem->devAddr, pDevAddr, 6) == 0) {
            break;
        }
    }
    if (index == sBthDeviceCtrl.itemCnt) {
        return nullptr;
    }

    //	tlkapi_array(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getItem: success - ", pDevAddr, 6);
    if (pIndex != nullptr) {
        *pIndex = index;
    }
    return ((bth_device_item_t *)(sBthDeviceCtrl.item[index].pBuffer));
}

uint08 *bth_device_getLinkKey(uint08 *pDevAddr)
{
    uint08 index;
    bth_device_item_t *pDItem;

    if (pDevAddr == nullptr) {
        tlkapi_error(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getLinkKey: failure - error param");
        return nullptr;
    }

    pDItem = nullptr;
    for (index = 0; index < sBthDeviceCtrl.itemCnt; index++) {
        pDItem = (bth_device_item_t *)(sBthDeviceCtrl.item[index].pBuffer);
        if (tmemcmp(pDItem->devAddr, pDevAddr, 6) == 0) {
            break;
        }
    }
    if (index == sBthDeviceCtrl.itemCnt) {
        return nullptr;
    }

    tlkapi_array(BTH_DEV_DBG_FLAG, BTH_DEV_DBG_SIGN, "bth_device_getLinkKey: success - ", pDevAddr, 6);
    return pDItem->linkKey;
}

/******************************************************************************
 * Function: bth_device_getLast
 * Descript: Get The last device info.
 * Params: None.
 * return: Bt device item.
 *******************************************************************************/
bth_device_item_t *bth_device_getLast(void)
{
    if (sBthDeviceCtrl.itemCnt == 0) {
        return nullptr;
    }
    return bth_device_getInfo(sBthDeviceCtrl.itemCnt - 1);
}

/******************************************************************************
 * Function: bth_device_getLastByType
 * Descript: Get device info.
 * Params:
 *        @type[IN]--The device class type.
 * return: Bt device item.
 *******************************************************************************/
bth_device_item_t *bth_device_getLastByType(uint08 type)
{
    //
    uint08 index;
    uint08 target;
    bth_device_item_t *pDItem;

    target = 0xFF;
    for (index = 0; index < sBthDeviceCtrl.itemCnt; index++) {
        pDItem = bth_device_getInfo(index);
        if (type == bth_devClassToDevType(pDItem->devClass)) {
            target = index;
        }
    }
    if (target == 0xFF) {
        return nullptr;
    }
    return bth_device_getInfo(index);
}
