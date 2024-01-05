/********************************************************************************************************
 * @file	tlkmdi_le_command.c
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
#if (TLK_MDI_LEACL_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"
#include "tlkmdi/le/tlkmdi_le_device_manage.h"
#include "tlkmdi/le/tlkmdi_le_simple_sdp.h"
#include "tlkmdi/le/tlkmdi_le_custom_pair.h"
#include "tlkmdi/le/tlkmdi_lemgrAtt.h"
#include "tlkmdi/le/tlkmdi_leAcl.h"
#include "tlkmdi/le/tlkmdi_le_command.h"

static uint08 sTlkMdiLemgrAdvDataLen = 24;
static uint08 sTlkMdiLemgrAdvData[31] = {
	 10, DT_COMPLETE_LOCAL_NAME,   'T','E','L','I','N','K','-','L','E',
	 2,	 DT_FLAGS, 								0x05, 					// BLE limited discoverable mode and BR/EDR not supported
	 3,  DT_APPEARANCE, 						0xC1, 0x03, 			// Generic Keyboard Generic category
	 5,  DT_INCOMPLT_LIST_16BIT_SERVICE_UUID,	0x12, 0x18, 0x0F, 0x18,	// incomplete list of service class UUIDs (0x1812, 0x180F)
};
static uint08 sTlkMdiLemgrScanRspLen = 11;
static uint08 sTlkMdiLemgrScanRsp[] = {
	 10, DT_COMPLETE_LOCAL_NAME,   'T','E','L','I','N','K','-','L','E',
};

int tlkmdi_le_setAddr1(uint08 *pPubAddr, uint08 *pRndAddr)
{
	blc_ll_writeBDAddr(pPubAddr); //set ble addr
	return TLK_ENONE;
}


int tlkmdi_le_startAdv(void)
{
#if ACL_PERIPHR_MAX_NUM
	if(blc_ll_getCurrentSlaveRoleNumber() >= ACL_PERIPHR_MAX_NUM){
		return -TLK_ESTATUS;
	}
	blc_ll_setAdvEnable(BLC_ADV_ENABLE);
	return TLK_ENONE;
#else
	return TLK_ENOSUPPORT;
#endif
}
int tlkmdi_le_closeAdv(void)
{
#if ACL_PERIPHR_MAX_NUM
	blc_ll_setAdvEnable(BLC_ADV_DISABLE);
	return TLK_ENONE;
#else
	return TLK_ENOSUPPORT;
#endif
}

int tlkmdi_le_startScan(void)
{
	tlkapi_trace(TLKMDI_LE_COMMAND_FLAG, TLKMDI_LE_COMMAND_SIGN, "tlkmdi_le_startScan");

#if ACL_CENTRAL_MAX_NUM
	blc_ll_setScanEnable (BLC_SCAN_ENABLE, DUP_FILTER_DISABLE);
	return TLK_ENONE;
#else
	return TLK_ENOSUPPORT;
#endif
}
int tlkmdi_le_closeScan(void)
{
#if ACL_CENTRAL_MAX_NUM
	tlkapi_trace(TLKMDI_LE_COMMAND_FLAG, TLKMDI_LE_COMMAND_SIGN, "tlkmdi_le_closeScan");
	blc_ll_setScanEnable (BLC_SCAN_DISABLE, DUP_FILTER_DISABLE);
	return TLK_ENONE;
#else
	return TLK_ENOSUPPORT;
#endif
}
int tlkmdi_le_disconn(uint16 handle,u8 reason)
{
	if(handle == 0) return -TLK_ESTATUS;
	if(blc_ll_disconnect(handle, 0x13) == BLE_SUCCESS){
		return TLK_ENONE;
	}
	return -TLK_EFAIL;
}

bool tlkmdi_le_aclVolumeInc(uint16 handle)
{
	uint16 consumer_key;
	if(handle == 0) return false;
		
	consumer_key = 0x00e9; //MKEY_VOL_UP;
	blc_gatt_pushHandleValueNotify(handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	consumer_key = 0;
	blc_gatt_pushHandleValueNotify(handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	return true;
}

bool tlkmdi_le_aclVolumeDec(uint16 handle)
{
	uint16 consumer_key;

	if(handle == 0) return false;
		
	consumer_key = 0x00ea; //MKEY_VOL_UP;
	blc_gatt_pushHandleValueNotify(handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	consumer_key = 0;
	blc_gatt_pushHandleValueNotify(handle, HID_CONSUME_REPORT_INPUT_DP_H, (uint08*)&consumer_key, 2);
	
	return true;
}


int tlkmdi_le_setAclName(uint08 *pName, uint08 nameLen)
{
	unsigned char offset;
	
	if(pName == nullptr || nameLen == 0) return TLK_ENONE;
	if(nameLen > 16) nameLen = 16;
	offset = 0;
	sTlkMdiLemgrAdvData[offset++] = nameLen+1;
	sTlkMdiLemgrAdvData[offset++] = DT_COMPLETE_LOCAL_NAME;
	tmemcpy(sTlkMdiLemgrAdvData+offset, pName, nameLen);
	offset += nameLen;
	sTlkMdiLemgrAdvData[offset++] = 0x02; //BLE limited discoverable mode and BR/EDR not supported
	sTlkMdiLemgrAdvData[offset++] = DT_FLAGS;
	sTlkMdiLemgrAdvData[offset++] = 0x05;
	sTlkMdiLemgrAdvData[offset++] = 0x03; // 384,  Keyboard Control, Generic category
	sTlkMdiLemgrAdvData[offset++] = DT_APPEARANCE;
	sTlkMdiLemgrAdvData[offset++] = 0xC1;
	sTlkMdiLemgrAdvData[offset++] = 0x03;
	sTlkMdiLemgrAdvData[offset++] = 0x05; // incomplete list of service class UUIDs (0x1812, 0x180F)
	sTlkMdiLemgrAdvData[offset++] = DT_INCOMPLT_LIST_16BIT_SERVICE_UUID;
	sTlkMdiLemgrAdvData[offset++] = 0x12;
	sTlkMdiLemgrAdvData[offset++] = 0x18;
	sTlkMdiLemgrAdvData[offset++] = 0x0F;
	sTlkMdiLemgrAdvData[offset++] = 0x18;
	sTlkMdiLemgrAdvDataLen = offset;
	blc_ll_setAdvData((uint08 *)sTlkMdiLemgrAdvData, sTlkMdiLemgrAdvDataLen);

	offset = 0;
	sTlkMdiLemgrScanRsp[offset++] = nameLen+1;
	sTlkMdiLemgrScanRsp[offset++] = DT_COMPLETE_LOCAL_NAME;
	tmemcpy(sTlkMdiLemgrScanRsp+offset, pName, nameLen);
	offset += nameLen;
	sTlkMdiLemgrScanRspLen = offset;
	blc_ll_setScanRspData((uint08 *)sTlkMdiLemgrScanRsp, sTlkMdiLemgrScanRspLen);
	return TLK_ENONE;
}


int tlkmdi_le_setAclAddr(uint08 *pAddr, uint08 addrLen)
{
	return TLK_ENONE;
}

int tlkmdi_le_createConnection(uint08 pAddr_type,uint08 *pAddr)
{
	if(blc_ll_getCurrentMasterRoleNumber() >=  ACL_CENTRAL_MAX_NUM)
	{
		return TLK_EOPERATE;
	}
	u8 status = blc_ll_createConnection( SCAN_INTERVAL_100MS, SCAN_WINDOW_100MS, INITIATE_FP_ADV_SPECIFY,  \
			pAddr_type,pAddr, OWN_ADDRESS_PUBLIC, \
							 CONN_INTERVAL_31P25MS, CONN_INTERVAL_48P75MS, 0, CONN_TIMEOUT_4S, \
							 0, 0xFFFF);
	if(status == BLE_SUCCESS){ //create connection success
		tlkapi_trace(TLKMDI_LE_COMMAND_FLAG, TLKMDI_LE_COMMAND_SIGN, "blc_ll_createConnection mac:%02x %02x %02x %02x %02x %02x",\
				pAddr[0],pAddr[1],pAddr[2],pAddr[3],pAddr[4],pAddr[5]);
		return TLK_ENONE;
	}
	return TLK_ESTATUS;
}



#endif

