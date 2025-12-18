/********************************************************************************************************
 * @file	tlkmmi_rdt_leCore.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../../tlkmmi_testStdio.h"
#if (TLK_TEST_RDT_ENABLE)
#include "../tlkmmi_rdtStd.h"
#include "tlkmmi_rdt_le.h"
#include "tlkmmi_rdt_leCore.h"
#include "tlkstk/ble/ble.h"
#include "tlkmmi_rdt_leAtt.h"


extern void ext_btrf_setLeTxTpSlice(uint08 slice);

static int tlkmmi_rdt_leCoreEventCB(uint32 evtID, uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_leHostEventCB(uint32 evtID, uint08 *pData, int dataLen);
static int tlkmmi_rdt_leGattDataCB(uint16 connHandle, uint08 *pkt);
static int tlkmmi_rdt_leConnectCompleteEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_leEncryptCompleteEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_leDisconnCompleteEvt(uint08 *pData, uint16 dataLen);


static uint08 sTlkmmiRdtLeAclRxFifo[TLKMMI_RDT_LE_RX_FIFO_SIZE * TLKMMI_RDT_LE_RX_FIFO_NUMB] = {0};
static uint08 sTlkmmiRdtLeAclSlvTxFifo[TLKMMI_RDT_LE_S_TX_FIFO_SIZE * TLKMMI_RDT_LE_S_TX_FIFO_NUMB * TLKMMI_RDT_LE_S_ROLE_NUMB] = {0};
static uint08 sTlkmmiRdtLeMtuSlvRxFifo[TLKMMI_RDT_LE_S_ROLE_NUMB * TLKMMI_RDT_LE_S_MTU_SIZE];
static uint08 sTlkmmiRdtLeMtuSlvTxFifo[TLKMMI_RDT_LE_S_ROLE_NUMB * TLKMMI_RDT_LE_S_MTU_SIZE];
/***************** ACL connection L2CAP layer MTU TX & RX data FIFO allocation, End **********************************/
static uint08 sTlkMmiRdtLeAdvDataLen = 24;
static uint08 sTlkMmiRdtLeAdvData[31] = {
	 10, DT_COMPLETE_LOCAL_NAME,   'T','E','L','I','N','K','R','D','T',
	 2,	 DT_FLAGS, 								0x05, 					// BLE limited discoverable mode and BR/EDR not supported
	 3,  DT_APPEARANCE, 						0xC1, 0x03, 			// Generic Keyboard Generic category
	 5,  DT_INCOMPLT_LIST_16BIT_SERVICE_UUID,	0x12, 0x18, 0x0F, 0x18,	// incomplete list of service class UUIDs (0x1812, 0x180F)
};
static uint08 sTlkMmiRdtLeScanRspLen = 11;
static uint08 sTlkMmiRdtLeScanRsp[] = {
	 10, DT_COMPLETE_LOCAL_NAME,   'T','E','L','I','N','K','R','D','T',
};

static uint16 sTlkMmiRdtLeConnHandle;
static RdtLeConnectCB sTlkMmiRdtLeConnectCB;
static RdtLeConnectCB sTlkMmiRdtLeEncryptCB;
static RdtLeDisconnCB sTlkMmiRdtLeDisconnCB;



int tlkmmi_rdt_leCoreInit(void)
{
	/* for 1M Flash, flash_sector_mac_address equals to 0xFF000
	 * for 2M Flash, flash_sector_mac_address equals to 0x1FF000 */
	uint08 leAddr[12];
	uint08 publicAddr[6] = TLKMMI_RDT_LE_PUBLIC_ADDR;
	uint08 randomAddr[6] = TLKMMI_RDT_LE_RANDOM_ADDR;
	unsigned int addr;

	sTlkMmiRdtLeConnectCB = nullptr;
	sTlkMmiRdtLeDisconnCB = nullptr;

	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_ADDR_ADDR);
	if(addr == 0) return -TLK_EFAIL;

	tlkapi_flash_write(addr, leAddr, 12);
	if(leAddr[0] != 0xFF || leAddr[1] != 0xFF || leAddr[2] != 0xFF){
		tmemcpy(publicAddr, leAddr, 6);
	}
	if(leAddr[6] != 0xFF || leAddr[7] != 0xFF || leAddr[8] != 0xFF){
		tmemcpy(randomAddr, leAddr+6, 6);
	}
	
	//////////// LinkLayer Initialization  Begin /////////////////////////
	blc_ll_initBasicMCU();
	blc_ll_initStandby_module(publicAddr);		//mandatory
	blc_ll_initLegacyAdvertising_module();		//mandatory for BLE slave
	blc_ll_initAclConnection_module();			//mandatory for BLE slave & master
	blc_ll_initAclSlaveRole_module();			//mandatory for BLE slave
	
	//blc_ll_init2MPhyCodedPhy_feature();		//This feature is not supported by default
	
	blc_ll_setMaxConnectionNumber(TLKMMI_RDT_LE_M_ROLE_NUMB, TLKMMI_RDT_LE_S_ROLE_NUMB);
	blc_ll_setAclConnMaxOctetsNumber(TLKMMI_RDT_LE_MAX_RX_SIZE, TLKMMI_RDT_LE_M_MAX_RX_SIZE, TLKMMI_RDT_LE_S_MAX_RX_SIZE);
	
	/* all ACL connection share same RX FIFO */
	blc_ll_initAclConnRxFifo(sTlkmmiRdtLeAclRxFifo, TLKMMI_RDT_LE_RX_FIFO_SIZE, TLKMMI_RDT_LE_RX_FIFO_NUMB);
	/* ACL Slave TX FIFO */
	blc_ll_initAclConnSlaveTxFifo(sTlkmmiRdtLeAclSlvTxFifo, TLKMMI_RDT_LE_S_TX_FIFO_SIZE, TLKMMI_RDT_LE_S_TX_FIFO_NUMB, TLKMMI_RDT_LE_S_ROLE_NUMB);
	
	//////////// LinkLayer Initialization  End /////////////////////////
	
	//////////// HCI Initialization  Begin /////////////////////////
	blc_hci_registerControllerDataHandler(blt_l2cap_pktHandler);
	blc_hci_registerControllerEventHandler(tlkmmi_rdt_leCoreEventCB); //controller hci event to host all processed in this func
	
	/* bluetooth event */
	blc_hci_setEventMask_cmd(HCI_EVT_MASK_DISCONNECTION_COMPLETE);
	/* bluetooth low energy(LE) event */
	blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_CONNECTION_COMPLETE | HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE);
	//////////// HCI Initialization  End /////////////////////////
	
	uint08 check_status = blc_controller_check_appBufferInitialization();
	if(check_status != BLE_SUCCESS){
		/* here user should set some log to know which application buffer incorrect*/
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "application buffer incorrect");
	}
	
	//////////// Host Initialization  Begin /////////////////////////
	/* Host Initialization */
	/* GAP initialization must be done before any other host feature initialization !!! */
	blc_gap_init();
	
	/* L2CAP Initialization */
	bls_l2cap_initMtuBuffer(sTlkmmiRdtLeMtuSlvRxFifo, TLKMMI_RDT_LE_S_MTU_SIZE, sTlkmmiRdtLeMtuSlvTxFifo, TLKMMI_RDT_LE_S_MTU_SIZE);
	blc_att_setSlaveRxMTUSize(TLKMMI_RDT_LE_MTU_SIZE); //Do not call this API, the default MTU_SIZE is 23

	/* GATT Initialization */
	blc_gatt_register_data_handler(tlkmmi_rdt_leGattDataCB);
	
	/* SMP Initialization */
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_SMP_PAIRING_ADDR);
	blc_smp_configPairingSecurityInfoStorageAddressAndSize(addr, TLK_CFG_FLASH_LE_SMP_PAIRING_SIZE);
	blc_smp_setSecurityLevel_slave(Unauthenticated_Pairing_with_Encryption);  //LE_Security_Mode_1_Level_2
	
	//	blc_smp_setPairingMethods(LE_Secure_Connection); //TLKMMI_RDT_LE_MTU_SIZE or TLKMMI_LEMGR_ATT_MTU_MASTER_RX_MAX_SIZE >= 64
	blc_smp_smpParamInit();
	// Hid device on android7.0/7.1 or later version
	// New paring: send security_request immediately after connection complete
	// reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
	blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection)
	
	/* Host (GAP/SMP/GATT/ATT) event process: register host event callback and set event mask */
	blc_gap_registerHostEventHandler(tlkmmi_rdt_leHostEventCB );
	blc_gap_setEventMask( GAP_EVT_MASK_SMP_PAIRING_BEGIN			|  \
						  GAP_EVT_MASK_SMP_PAIRING_SUCCESS			|  \
						  GAP_EVT_MASK_SMP_PAIRING_FAIL 			|  \
						  GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE);
	//////////// Host Initialization  End /////////////////////////
	
	//////////////////////////// BLE stack Initialization  End //////////////////////////////////
	
	#if (TLK_CFG_PM_ENABLE)
	blc_ll_initPowerManagement_module();
	#endif
	
	//////////////////////////// User Configuration for BLE application ////////////////////////////	
	blc_ll_setAdvData((uint08 *)sTlkMmiRdtLeAdvData, sTlkMmiRdtLeAdvDataLen);
	blc_ll_setScanRspData((uint08 *)sTlkMmiRdtLeScanRsp, sTlkMmiRdtLeScanRspLen);
	blc_ll_setAdvParam(ADV_INTERVAL_50MS, ADV_INTERVAL_50MS, ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL, BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	blc_ll_setAdvEnable(BLC_ADV_DISABLE);
	//	blc_ll_setAdvCustomedChannel(37, 37, 37); //debug
	
	#if (MCU_CORE_TYPE == MCU_CORE_B91)
	ext_btrf_setLeTxTpSlice(RF_POWER_P9p11dBm);
	#endif
	#if (MCU_CORE_TYPE == MCU_CORE_B92)
	ext_btrf_setLeTxTpSlice(RF_POWER_P9p15dBm);
	#endif
	return TLK_ENONE;
}


void tlkmmi_rdt_leStartAdv(void)
{
	blc_ll_setAdvEnable(BLC_ADV_ENABLE);
}
void tlkmmi_rdt_leCloseAdv(void)
{
	blc_ll_setAdvEnable(BLC_ADV_DISABLE);
}

void tlkmmi_rdt_leDisconn(uint16 handle, uint08 reason)
{
	if(handle == 0) handle = sTlkMmiRdtLeConnHandle;
	blc_ll_disconnect(handle, reason);
}
void tlkmmi_rdt_leClsPeerInfo(void)
{
	blc_smp_eraseAllBondingInfo();
}


void tlkmmi_rdt_leRegConnectCB(RdtLeConnectCB connectCB)
{
	sTlkMmiRdtLeConnectCB = connectCB;
}
void tlkmmi_rdt_leRegEncryptCB(RdtLeEncryptCB encryptCB)
{
	sTlkMmiRdtLeEncryptCB = encryptCB;
}
void tlkmmi_rdt_leRegDisconnCB(RdtLeDisconnCB disconnCB)
{
	sTlkMmiRdtLeDisconnCB = disconnCB;
}

void tlkmmi_rdt_leUpdateConnParam(uint16 handle, uint16 timeMs, uint08 latency)
{
	if(handle == 0) handle = sTlkMmiRdtLeConnHandle;
	bls_l2cap_requestConnParamUpdate(handle, timeMs*8/10, timeMs*8/10, latency, CONN_TIMEOUT_4S);
}


static int tlkmmi_rdt_leCoreEventCB(uint32 evtID, uint08 *pData, uint16 dataLen)
{
	uint08 evtCode;
	
	if((evtID & HCI_FLAG_EVENT_BT_STD) == 0) return 0;
	
	evtCode = evtID & 0xff;
	//------------ disconnect -------------------------------------
	if(evtCode == HCI_EVT_DISCONNECTION_COMPLETE)  //connection terminate
	{
		tlkmmi_rdt_leDisconnCompleteEvt(pData, dataLen);
	}
	else if(evtCode == HCI_EVT_ENCRYPTION_CHANGE){
		tlkmmi_rdt_leEncryptCompleteEvt(pData, dataLen);
	}
	else if(evtCode == HCI_EVT_LE_META)  //LE Event
	{
		uint08 subEvt_code = pData[0];
		//------hci le event: le connection complete event---------------------------------
		if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)	// connection complete
		{
			//after controller is set to initiating state by host (blc_ll_createConnection(...) )
			//it will scan the specified device(adr_type & mac), when find this adv packet, send a connection request packet to slave
			//and enter to connection state, send connection complete event. but notice that connection complete not
			//equals to connection establish. connection complete means that master controller set all the ble timing
			//get ready, but has not received any slave packet, if slave rf lost the connection request packet, it will
			//not send any packet to master controller
			tlkmmi_rdt_leConnectCompleteEvt(pData, dataLen);
		}
		//--------hci le event: le adv report event ----------------------------------------
		else if (subEvt_code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
		{
			//after controller is set to scan state, it will report all the adv packet it received by this event
		}
		//------hci le event: le connection update complete event-------------------------------
		else if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
		{
			hci_le_connectionUpdateCompleteEvt_t *pUpt = (hci_le_connectionUpdateCompleteEvt_t *)pData;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN,
				"ble connect update: connInterval-0x%02X connLatency-0x%02X Timeout-0x%02X",
				pUpt->connInterval,pUpt->connLatency,pUpt->supervisionTimeout);
			//aa_connLatency_temp = pUpt->connLatency;
		}
	}

	
	return 0;
}
static int tlkmmi_rdt_leHostEventCB(uint32 evtID, uint08 *pData, int dataLen)
{
	uint08 event = evtID & 0xFF;
	switch(event)
	{
		case GAP_EVT_SMP_PAIRING_BEGIN:
			break;
		case GAP_EVT_SMP_PAIRING_SUCCESS:
			break;
		case GAP_EVT_SMP_PAIRING_FAIL:
			break;
		case GAP_EVT_SMP_CONN_ENCRYPTION_DONE:
			break;
		case GAP_EVT_SMP_SECURITY_PROCESS_DONE:
			break;
		case GAP_EVT_SMP_TK_DISPLAY:
			break;
		case GAP_EVT_SMP_TK_REQUEST_PASSKEY:
			break;
		case GAP_EVT_SMP_TK_REQUEST_OOB:
			break;
		case GAP_EVT_SMP_TK_NUMERIC_COMPARE:
			break;
		case GAP_EVT_ATT_EXCHANGE_MTU:
			break;
		case GAP_EVT_GATT_HANDLE_VALUE_CONFIRM:
			break;
		default:
			break;
	}
	return 0;
}

static int tlkmmi_rdt_leGattDataCB(uint16 connHandle, uint08 *pkt)
{
	
	return 0;
}


static int tlkmmi_rdt_leConnectCompleteEvt(uint08 *pData, uint16 dataLen)
{
	hci_le_connectionCompleteEvt_t *pConnEvt = (hci_le_connectionCompleteEvt_t *)pData;

	if(pConnEvt->status == BLE_SUCCESS){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "RDT-BLE: conn complete: 0x%x connInterval-0X%02X connLatency-0X%02X Timeout-0X%02X", pConnEvt->connHandle,pConnEvt->connInterval,pConnEvt->slaveLatency,pConnEvt->supervisionTimeout);
		sTlkMmiRdtLeConnHandle = pConnEvt->connHandle;
	}else{
		sTlkMmiRdtLeConnHandle = 0;
	}
	blc_ll_setAdvEnable(BLC_ADV_DISABLE);
	if(sTlkMmiRdtLeConnectCB != nullptr){
		sTlkMmiRdtLeConnectCB(sTlkMmiRdtLeConnHandle, pConnEvt->status);
	}
	
	return 0;
}
static int tlkmmi_rdt_leEncryptCompleteEvt(uint08 *pData, uint16 dataLen)
{
	if(sTlkMmiRdtLeEncryptCB != nullptr){
		sTlkMmiRdtLeEncryptCB(sTlkMmiRdtLeConnHandle, 0);
	}
	return 0;
}
static int tlkmmi_rdt_leDisconnCompleteEvt(uint08 *pData, uint16 dataLen)
{
	event_disconnection_t *pDiscEvt = (event_disconnection_t *)pData;

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "RDT-BLE: conn disconnect handle-0x%x, reason-%d",
		pDiscEvt->connHandle, pDiscEvt->reason);

	//terminate reason
	if(pDiscEvt->reason == HCI_ERR_CONN_TIMEOUT){  	//connection timeout

	}
	else if(pDiscEvt->reason == HCI_ERR_REMOTE_USER_TERM_CONN){  	//peer device send terminate command on link layer

	}
	//master host disconnect( blm_ll_disconnect(current_connHandle, HCI_ERR_REMOTE_USER_TERM_CONN) )
	else if(pDiscEvt->reason == HCI_ERR_CONN_TERM_BY_LOCAL_HOST){

	}
	else{

	}
	sTlkMmiRdtLeConnHandle = 0;
	if(sTlkMmiRdtLeDisconnCB != nullptr){
		sTlkMmiRdtLeDisconnCB(pDiscEvt->connHandle, pDiscEvt->reason);
	}
	
	return 0;
}



#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)

