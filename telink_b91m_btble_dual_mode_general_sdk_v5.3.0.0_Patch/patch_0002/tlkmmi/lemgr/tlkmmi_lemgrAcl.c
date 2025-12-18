/********************************************************************************************************
 * @file	tlkmmi_lemgrAcl.c
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
#if (TLKMMI_LEMGR_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"
#include "tlkmdi/le/tlkmdi_le_device_manage.h"
#include "tlkmdi/le/tlkmdi_le_simple_sdp.h"
#include "tlkmdi/le/tlkmdi_le_custom_pair.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAdapt.h"
#include "tlkmmi/lemgr/tlkmmi_lemgr.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrMsgOuter.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrCtrl.h"
#include "tlkmdi/le/tlkmdi_lemgrAtt.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAcl.h"
#include "tlkmdi/le/tlkmdi_leAcl.h"
#include "tlkmdi/le/tlkmdi_le_command.h"
#include "tlkmdi/le/tlkmdi_le_event.h"
#if BLE_IOS_ANCS_ENABLE
	#include "tlklib/ios_service/ancs/ancs.h"
	#include "tlklib/ios_service/ancs/ancsDemo.h"
#endif

#if BLE_IOS_AMS_ENABLE
	#include "tlklib/ios_service/ams/ams.h"
	#include "tlklib/ios_service/ams/amsDemo.h"
#endif


extern void ext_btrf_setLeTxTpSlice (uint08 slice);
static int tlkmmi_lemgr_connectCompleteEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_lemgr_disconnCompleteEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_lemgr_advReportEvt(uint08 *pData, uint16 dataLen);
#if (TLKMMI_LEMGR_OTA_SERVER_ENABLE)
static void tlkmmi_lemgr_enterOTAEvt(void);
static void tlkmmi_lemgr_leaveOTAEvt(int result);
#endif



/**
 * @brief	ACL TX cache FIFO, used to workaround problem caused by
 *          "(ACL_xxx_TX_FIFO_SIZE * (ACL_xxx_TX_FIFO_NUM - 1)) must be less than 4096 (4K)"
 *          CAL_LL_ACL_TX_FIFO_SIZE(230) = 240, CAL_LL_ACL_TX_FIFO_SIZE(231) = 256
 *          when ACL_xxx_MAX_TX_OCTETS > 230, ACL_xxx_TX_FIFO_SIZE >= 256, ACL_xxx_TX_FIFO_NUM can only be 9
 *          if you want TX FIFO 16 or 32, can use Cache TX FIFO to extend this function.
 *
 */
#if (TLKMMI_LEMGR_WORKAROUND_TX_FIFO_4K_LIMITATION_EN && (TLKMMI_LEMGR_MASTER_MAX_TX_OCTETS > 230 || TLKMMI_LEMGR_SLAVE_MAX_TX_OCTETS > 230))
	//_attribute_ble_data_retention_  //iRam not enough now, add later
static uint08 sTlkmmiLemgrAclCacheTxFifo[260*16] = {0}; //size must 260, number must be 16 or 32
#endif
/******************** ACL connection LinkLayer TX & RX data FIFO allocation, End ***********************************/
#if (TLKMMI_LEMGR_OTA_SERVER_ENABLE)
_attribute_ble_data_retention_	int sTlkMmiLemgrOtaIsWorking = 0;
#endif


static bool tlkmmi_lemgr_timer(tlkapi_timer_t *pTimer, uint32 userArg);


static tlkmmi_lemgr_acl_t sTlkMmiLemgrAcl;

uint16 tlkmmi_lemgr_aclGetSlaveHandle(void)
{
	return sTlkMmiLemgrAcl.connHandle;
}

int tlkmmi_lemgr_aclInit(void)
{

	uint08 mac_public[6];
	uint08 mac_random_static[6];

	tmemset(&sTlkMmiLemgrAcl, 0, sizeof(tlkmmi_lemgr_acl_t));
	extern uint32 flash_sector_mac_address;
	blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);
	tlkmmi_lemgr_setAddr1(mac_public, mac_random_static);

	tlkmdi_le_aclInit();

	tlkmdi_le_aclRegDiscCB(tlkmmi_lemgr_disconnCompleteEvt);
	tlkmdi_le_aclRegConCB(tlkmmi_lemgr_connectCompleteEvt);

	tlkmdi_le_aclRegAdvReportCB(tlkmmi_lemgr_advReportEvt);

	tlkmmi_lemgr_adaptInitTimer(&sTlkMmiLemgrAcl.timer, tlkmmi_lemgr_timer, (uint32)&sTlkMmiLemgrAcl, TLKMMI_LEMGR_TIMEOUT);

	////////////////////////////////////////////////////////////////////////////////////////////////
	tlkmdi_le_setAclName(tlkmmi_lemgr_getName(), tlkmmi_lemgr_getNameLen());
	tlkmdi_le_setAclAddr(tlkmmi_lemgr_getAddr(), 6);
	
	#if (MCU_CORE_TYPE == MCU_CORE_B91)
	ext_btrf_setLeTxTpSlice(RF_POWER_P9p11dBm);
	#endif
	#if (MCU_CORE_TYPE == MCU_CORE_B92)
	ext_btrf_setLeTxTpSlice(RF_POWER_P9p15dBm);
	#endif

	return TLK_ENONE;
}

_attribute_ble_retention_code_
void tlkmmi_lemgr_aclDeepInit(void)
{
	tlkmdi_le_aclDeepInit();
}
int tlkmmi_lemgr_createconnect(uint08 *pData, uint08 dataLen)
{
    u8 pPeerAddr[6];
    tmemcpy(pPeerAddr, pData, 6); //MAC
    //pAddr_type:Need to be modified manually according to the actual
	if(tlkmdi_le_createConnection(OWN_ADDRESS_PUBLIC,pPeerAddr) == TLK_ENONE)
	{
			return TLK_ENONE;
	}
	return -TLK_EFAIL;
}
int tlkmmi_lemgr_disconn(uint16 handle, uint08 *pPeerAddr)
{
	if(sTlkMmiLemgrAcl.connHandle == 0) return -TLK_ESTATUS;
	if(tlkmdi_le_disconn(sTlkMmiLemgrAcl.connHandle, 0x13) == TLK_ENONE){
		return TLK_ENONE;
	}
	return -TLK_EFAIL;
}

int tlkmmi_lemgr_startAdv(uint32 timeout, uint08 advType)
{
	if(sTlkMmiLemgrAcl.connHandle != 0){
		return -TLK_ESTATUS;
	}
	if(timeout == 0) timeout = TLKMMI_LEMGR_ADV_TIMEOUT_DEF;
	else if(timeout < TLKMMI_LEMGR_ADV_TIMEOUT_MIN) timeout = TLKMMI_LEMGR_ADV_TIMEOUT_MIN;
	else if(timeout > TLKMMI_LEMGR_ADV_TIMEOUT_MAX) timeout = TLKMMI_LEMGR_ADV_TIMEOUT_MAX;
	sTlkMmiLemgrAcl.isStart = true;
	sTlkMmiLemgrAcl.advTime = timeout;
	sTlkMmiLemgrAcl.timeout = timeout/TLKMMI_LEMGR_TIMEOUT_MS;
	if(tlkmdi_le_startAdv() != TLK_ENONE)
	{
		return -TLK_ESTATUS;
	}
	tlkmmi_lemgr_sendAdvStartEvt();
	tlkmmi_lemgr_adaptInsertTimer(&sTlkMmiLemgrAcl.timer);
	return TLK_ENONE;
}
int tlkmmi_lemgr_closeAdv(void)
{
	sTlkMmiLemgrAcl.isStart = false;
	tlkmdi_le_closeAdv();
	tlkmmi_lemgr_sendAdvCompleteEvt();
	return TLK_ENONE;
}

int tlkmmi_lemgr_startScan(void)
{
	if(tlkmdi_le_startScan()  != TLK_ENONE)
	{
		return -TLK_ESTATUS;
	}
	tlkmmi_lemgr_sendScanStartEvt();
	return TLK_ENONE;
}

int tlkmmi_lemgr_closeScan(void)
{
	tlkmdi_le_closeScan();
	tlkmmi_lemgr_sendScanCompleteEvt();
	return TLK_ENONE;
}


static bool tlkmmi_lemgr_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(!sTlkMmiLemgrAcl.isStart) return false;
	if(sTlkMmiLemgrAcl.timeout != 0) sTlkMmiLemgrAcl.timeout --;
	if(sTlkMmiLemgrAcl.timeout == 0){
		tlkmmi_lemgr_closeAdv();
		return false;
	}
	return true;
}


static int tlkmmi_lemgr_connectCompleteEvt(uint08 *pData, uint16 dataLen)
{
	hci_le_connectionCompleteEvt_t *pConnEvt = (hci_le_connectionCompleteEvt_t *)pData;
	sTlkMmiLemgrAcl.connHandle = 0;
	if(pConnEvt->status == BLE_SUCCESS){
		tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "BLE: conn complete: 0x%x connInterval-0X%02X connLatency-0X%02X Timeout-0X%02X", pConnEvt->connHandle,pConnEvt->connInterval,pConnEvt->slaveLatency,pConnEvt->supervisionTimeout);
		sTlkMmiLemgrAcl.connHandle = pConnEvt->connHandle;
		tmemcpy(sTlkMmiLemgrAcl.connAddr, pConnEvt->peerAddr, 6);

#if (BLE_IOS_ANCS_ENABLE || BLE_IOS_AMS_ENABLE)
		app_setCurBleConnState(BLE_CONNECTED);
#endif

#if BLE_IOS_ANCS_ENABLE
		//ancs_findAncsService(); //should not call when conn back
		app_setSendSecReqFlag(NO_NEED_SEND_SEC_REQ);
		tick_findAncsSrv = clock_time() | 0x01;
		ancs_initial(ANCS_ATT_TYPE_ALL_INCLUDE);
		app_setSystemType(SYSTEM_TYPE_ANDROID);
		ancs_setNewsReceivedStatus(ANCS_SET_NO_NEW_NOTICE);
		tick_conn_update_param = clock_time() | 1;
#endif
	}
	
	tlkmmi_lemgr_sendAclConnectEvt(pConnEvt->connHandle, pConnEvt->status, 0x02, pConnEvt->peerAddrType, pConnEvt->peerAddr);
	tlkmmi_lemgr_closeAdv();
	
	return 0;
}

static int tlkmmi_lemgr_disconnCompleteEvt(uint08 *pData, uint16 dataLen)
{
	event_disconnection_t *pDiscEvt = (event_disconnection_t *)pData;

	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "BLE: conn disconnect handle-0x%x, reason-%d", 
		pDiscEvt->connHandle, pDiscEvt->reason);

	sTlkMmiLemgrAcl.connHandle = 0;
	tlkmmi_lemgr_sendAclDisconnEvt(pDiscEvt->connHandle, pDiscEvt->reason, sTlkMmiLemgrAcl.connAddr);
	tlkmdi_le_startAdv();
	
	return 0;
}

static int tlkmmi_lemgr_advReportEvt(uint08 *pData, uint16 dataLen)
{
	event_adv_report_t *pa = (event_adv_report_t *)pData;
	u8 adr_type  = pa->adr_type;
	u8 mac[6];
	s8 rssi = pa->data[pa->len];
	if(rssi < -60)
	{
		return 0;
	}
	tmemcpy(mac, pa->mac, 6); //MAC
    tlkmmi_lemgr_sendAclAdvReportEvt(adr_type,mac,rssi);
	return 0;
}



#endif //TLKMMI_LEMGR_ENABLE

