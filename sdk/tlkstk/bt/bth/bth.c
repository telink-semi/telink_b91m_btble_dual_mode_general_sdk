/********************************************************************************************************
 * @file	bth.c
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
#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "bth_stdio.h"
#include "bth_adapt.h"
#include "bth_l2cap.h"
#include "bth_handle.h"
#include "bth_hcievt.h"
#include "bth.h"
#include "bth_acl.h"
#include "bth_sco.h"


/******************************************************************************
 * Function: BTH Init interface
 * Descript: This interface be used to initial bth layer.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int bth_init(void)
{
	int ret;

	ret = TLK_ENONE;
    if(ret == TLK_ENONE) ret = bth_adapt_init();
    if(ret == TLK_ENONE) ret = bth_hcievt_init();
    if(ret == TLK_ENONE) ret = bth_event_init();
    if(ret == TLK_ENONE) ret = bth_handle_init(TLK_STK_BTACl_NUMB, TLK_STK_BTSCO_NUMB);
    if(ret == TLK_ENONE) ret = bth_l2cap_init(TLK_STK_BTPSM_NUMB, TLK_STK_BTCHN_NUMB, TLK_STK_BTACl_NUMB);
    if(ret == TLK_ENONE) ret = bth_device_init();
    if(ret == TLK_ENONE) ret = bth_sco_setTxRxBandwidth(0xFFFFFFFF, 0xFFFFFFFF);
	if(ret != TLK_ENONE) bth_deinit();
    return TLK_ENONE;
}
void bth_deinit(void)
{
	bth_handle_deinit();
	bth_l2cap_deinit();
}

int bth_needMemLen(void)
{
	int needLen = 0;
    needLen += bth_handle_getMemLen(TLK_STK_BTACl_NUMB, TLK_STK_BTSCO_NUMB);
    needLen += bth_l2cap_getMemLen(TLK_STK_BTPSM_NUMB, TLK_STK_BTCHN_NUMB, TLK_STK_BTACl_NUMB);
    return needLen;
}


bool bth_isBusy(void)
{
    return false;
}

bool bth_pmIsBusy(void)
{
    uint08 count;
    bth_acl_handle_t *pHandle;

    count = bth_handle_getUsedAclCount();
    if (count == 0) {
        return false;
    }
    if (count != 1) {
        return true;
    }

    pHandle = bth_handle_getFirstConnAcl();
    if (pHandle != nullptr && pHandle->curRole == BTH_ROLE_SLAVE) {
        return false;
    }
    return true;
}

uint bth_getAclCount(void)
{
    return bth_handle_getUsedAclCount();
}

/******************************************************************************
 * Function: BTH Process interface
 * Descript: This interface be used to process the event and timer evt, seems
 *           like a schdluer of System.
 * Params: None.
 * Return: None.
 *******************************************************************************/
void bth_handler(void)
{
    bth_adapt_handler();
}

/******************************************************************************
 * Function: BTH Destroy interface
 * Descript: This interface be used to release the resource which allocate.
 * Params: @aclhandle[IN]--The acl link handle.
 * Return: None.
 *******************************************************************************/
void bth_destroy(uint16 aclHandle)
{
    bth_acl_destroy(aclHandle);
    bth_sco_destroyByAclHandle(aclHandle);
    bth_l2cap_aclDisconn(aclHandle);
}

/******************************************************************************
 * Function: bth_isHeadset
 * Descript: This interface be used to verify peer device is headset or
 *           others device.
 * Params: @aclhandle[IN]--The acl link handle.
 * Return: Return true means peer device type headset, false means others .
 *******************************************************************************/
bool bth_isHeadset(uint16 aclHandle)
{
    uint08 devType;
    bth_acl_handle_t *pHandle;

    pHandle = bth_handle_getUsedAcl(aclHandle);
    if (pHandle == nullptr) {
        return false;
    }

    devType = bth_devClassToDevType(pHandle->devClass);
    if (devType == BTH_REMOTE_DTYPE_HEADSET) {
        return true;
    }
    return false;
}

/******************************************************************************
 * Function: bth_devClassToDevType
 * Descript: This interface be used to reflect from devClass to device type.
 * Params: @devClass[IN]--The device class.
 * Return: The Device type.
 *******************************************************************************/
uint08 bth_devClassToDevType(uint32 devClass)
{
    uint08 dtype;
    uint32 majorDType;

    if (devClass == 0) {
        return BTH_REMOTE_DTYPE_OTHER;
    }

    majorDType = (devClass & BTH_COD_MAJOR_CLASS_MASK);
    if (majorDType == BTH_COD_MAJOR_CLASS_COMPUTER) {
        dtype = BTH_REMOTE_DTYPE_COMPUTER; // TLKMDI_BTINQ_DTYPE_PC;
    } else if (majorDType == BTH_COD_MAJOR_CLASS_PHONE) {
        dtype = BTH_REMOTE_DTYPE_PHONE;    // TLKMDI_BTINQ_DTYPE_PHONE;
    } else if (majorDType == BTH_COD_MAJOR_CLASS_AUDIO) {
        dtype = BTH_REMOTE_DTYPE_HEADSET;  // TLKMDI_BTINQ_DTYPE_HEADSET;
    } else if (majorDType != BTH_COD_MAJOR_CLASS_MISC || (devClass & BTH_COD_SERVICE_CLASS_AUDIO) == 0 ||
               (devClass & BTH_COD_SERVICE_CLASS_RENDER) == 0) {
        dtype = BTH_REMOTE_DTYPE_OTHER; // TLKMDI_BTINQ_DTYPE_UNKNOWN;
    } else {
        dtype = BTH_REMOTE_DTYPE_OTHER;
    }
    return dtype;
}
