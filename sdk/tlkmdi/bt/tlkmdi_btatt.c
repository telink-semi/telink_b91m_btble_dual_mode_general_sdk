/********************************************************************************************************
 * @file	tlkmdi_btatt.c
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
#if (TLK_MDI_BTATT_ENABLE)
#include "tlkmdi/bt/tlkmdi_btatt.h"

#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/att/btp_att.h"


static const btp_gattItem_t scTlkMdiBtAttItems[];
static TlkMdiBtAttOtaWriteCB sTlkMdiBtAttOtaWriteCB = nullptr;


int tlkmdi_btatt_init(void)
{
	btp_attsrv_setTable(scTlkMdiBtAttItems, TLKMDI_BTATT_HANDLE_MAX);
	
	return TLK_ENONE;
}

void tlkmdi_btatt_regOtaWriteCB(TlkMdiBtAttOtaWriteCB cb)
{
	sTlkMdiBtAttOtaWriteCB = cb;
}

int tlkmdi_btatt_otaSendData(uint16 handle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen)
{
	uint16 chnID = btp_attsrv_getChnID(handle);
	return btp_att_sendHandleValueNtf1(handle, chnID, TLKMDI_BTATT_OTA_CMD_OUT_DP_H, pHead, headLen, pData, dataLen);
}
static uint08 tlkmdi_btatt_otaWriteCB(uint16 handle, uint16 chnID, uint08 *pData, uint16 dataLen)
{
	if(sTlkMdiBtAttOtaWriteCB != nullptr){
		sTlkMdiBtAttOtaWriteCB(handle, chnID, pData, dataLen);
	}
	return BTP_ATT_ECODE_NONE;
}


#if 1
#define BTP_GATT_CONN_INTERVAL_10MS      8
#define BTP_GATT_CONN_TIMEOUT_5S         500
static uint08 sLepGapsDevNameValue[] = {'G','A','T','T',' ','B','R','/','E','D','R'};
static uint08 sLepGapsAppearanceValue[2] = {0x00, 0x09}; //384
static uint08 sLepGapsConnParamValue[8] = {
	(BTP_GATT_CONN_INTERVAL_10MS)&0x00FF, ((BTP_GATT_CONN_INTERVAL_10MS)&0xFF00)>>8, //Min Interval
	(BTP_GATT_CONN_INTERVAL_10MS)&0x00FF, ((BTP_GATT_CONN_INTERVAL_10MS)&0xFF00)>>8, //Max Interval
	99, 00, //Latency
	(BTP_GATT_CONN_TIMEOUT_5S)&0x00FF, ((BTP_GATT_CONN_TIMEOUT_5S)&0xFF00)>>8, //Timeout
};
static const uint08 scLepGapsDevNameChar[5] = {
	BTP_GATT_PROPERTY_READ,
	SPLIT_WORD(TLKMDI_BTATT_GAP_DEVICE_NAME_DP_H),
	SPLIT_WORD(BTP_CHARACT_UUID_DEVICE_NAME),
};
static const uint08 scLepGapsAppearanceChar[5] = {
	BTP_GATT_PROPERTY_READ,
	SPLIT_WORD(TLKMDI_BTATT_GAP_APPEARANCE_DP_H),
	SPLIT_WORD(BTP_CHARACT_UUID_APPEARANCE),
};
static const uint08 scLepGapsConnParamChar[5] = {
	BTP_GATT_PROPERTY_READ,
	SPLIT_WORD(TLKMDI_BTATT_GAP_CONN_PARAM_DP_H),
	SPLIT_WORD(BTP_CHARACT_UUID_PERI_CONN_PARAM),
};
#endif
#if 1
static const uint08 scTlkMdiBtAttGattServiceChangedChar[5] = {
	BTP_GATT_PROPERTY_INDICATE,
	SPLIT_WORD(TLKMDI_BTATT_GATT_SERVICE_CHANGED_DP_H),
	SPLIT_WORD(BTP_CHARACT_UUID_SERVICE_CHANGE),
};
static const uint08 scTlkMdiBtAttGattServiceChangedValue[2] = {0x00, 0x00}; 
static uint16 scTlkMdiBtAttGattServiceChangedCCC = 0;
#endif
#if 1
static const uint08 scTlkMdiBtAttDevPnpIDChar[5] = {
	BTP_GATT_PROPERTY_READ,
	SPLIT_WORD(TLKMDI_BTATT_DEV_PNPID_CD_H),
	SPLIT_WORD(BTP_CHARACT_UUID_PNP_ID),
};
static const u8 scTlkMdiBtAttDevPnpIDValue[] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};	
#endif
#if 1
static uint08 sTlkMdiBtAttOtaData = 0x00;
static uint08 sTlkMdiBtAttOtaDataCCC[2] = {0,0};
static const uint08 scTlkMdiBtAttOtaSrvUUID[16] = {0x12,0x19,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};
static const uint08 scTlkMdiBtAttOtaDatUUID[16] = {0x12,0x2B,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};
static const uint08 sTlkMdiBtAttOtaName[] = {'O', 'T', 'A'};
static const uint08 sTlkMdiBtAttOtaCharVal[19] = {
	BTP_GATT_PROPERTY_WRITE_WITHOUT_RSP | BTP_GATT_PROPERTY_NOTIFY,
	SPLIT_WORD(TLKMDI_BTATT_OTA_CMD_OUT_DP_H),
	0x12,0x2B,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00
};
#endif

static const btp_gattItem_t scTlkMdiBtAttItems[TLKMDI_BTATT_HANDLE_MAX+1] = {
	{TLKMDI_BTATT_HANDLE_MAX-1,0,0,0,0,0,0,0},
	//Gap
	{7,BTP_ATT_PERMIT_READ,2,2,(uint08*)cBtpUUID16Primary,(uint08*)cBtpUUID16GapService, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(scLepGapsDevNameChar),(uint08*)cBtpUUID16Charact,(uint08*)scLepGapsDevNameChar, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(sLepGapsDevNameValue),(uint08*)cBtpUUID16DeviceName,(uint08*)sLepGapsDevNameValue, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(scLepGapsAppearanceChar),(uint08*)cBtpUUID16Charact,(uint08*)scLepGapsAppearanceChar, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(sLepGapsAppearanceValue),(uint08*)cBtpUUID16Appearance,(uint08*)sLepGapsAppearanceValue, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(scLepGapsConnParamChar),(uint08*)cBtpUUID16Appearance,(uint08*)scLepGapsConnParamChar, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(sLepGapsConnParamValue),(uint08*)cBtpUUID16PeriConnParam,(uint08*)sLepGapsConnParamValue, 0, 0},
	//Gatt
	{4,BTP_ATT_PERMIT_READ,2,2,(uint08*)cBtpUUID16Primary,(uint08*)cBtpUUID16GattService, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(scTlkMdiBtAttGattServiceChangedChar),(uint08*)cBtpUUID16Charact,(uint08*)scTlkMdiBtAttGattServiceChangedChar, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(scTlkMdiBtAttGattServiceChangedValue),(uint08*)cBtpUUID16ServiceChanged,(uint08*)scTlkMdiBtAttGattServiceChangedValue, 0, 0},
	{0,BTP_ATT_PERMIT_RDWR,2,sizeof(scTlkMdiBtAttGattServiceChangedCCC),(uint08*)cBtpUUID16ClientCharCfg,(uint08*)&scTlkMdiBtAttGattServiceChangedCCC, 0, 0},
	//Device
	{3,BTP_ATT_PERMIT_READ,2,2,(uint08*)cBtpUUID16Primary,(uint08*)cBtpUUID16DevService, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(scTlkMdiBtAttDevPnpIDChar),(uint08*)cBtpUUID16Charact, (uint08*)scTlkMdiBtAttDevPnpIDChar, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(scTlkMdiBtAttDevPnpIDValue),(uint08*)cBtpUUID16PnpID, (uint08*)scTlkMdiBtAttDevPnpIDValue, 0, 0},
	//OTA
	{5,BTP_ATT_PERMIT_READ,2,16,(uint08*)cBtpUUID16Primary,(uint08*)scTlkMdiBtAttOtaSrvUUID, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(sTlkMdiBtAttOtaCharVal),(uint08*)cBtpUUID16Charact,(uint08*)sTlkMdiBtAttOtaCharVal, 0, 0},
	{0,BTP_ATT_PERMIT_READ|BTP_ATT_PERMIT_WRITE_WITHOUT_RSP,2,sizeof(sTlkMdiBtAttOtaData),(uint08*)scTlkMdiBtAttOtaDatUUID,(uint08*)&sTlkMdiBtAttOtaData, 0, tlkmdi_btatt_otaWriteCB},
	{0,BTP_ATT_PERMIT_RDWR,2,sizeof(sTlkMdiBtAttOtaDataCCC),(uint08*)cBtpUUID16ClientCharCfg,(uint08*)sTlkMdiBtAttOtaDataCCC, 0, 0},
	{0,BTP_ATT_PERMIT_READ,2,sizeof(sTlkMdiBtAttOtaName),(uint08*)cBtpUUID16CharUserDesc,(uint08*)sTlkMdiBtAttOtaName, 0, 0},
};


#endif //#if (TLK_MDI_BTATT_ENABLE)

