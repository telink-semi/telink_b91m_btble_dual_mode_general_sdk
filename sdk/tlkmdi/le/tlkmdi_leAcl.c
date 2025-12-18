/********************************************************************************************************
 * @file	tlkmdi_lemgrAcl.c
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
#if (tlkMDI_LEMGR_OTA_SERVER_ENABLE)
static void tlkmdi_lemgr_enterOTAEvt(void);
static void tlkmdi_lemgr_leaveOTAEvt(int result);
#endif

int	central_smp_pending = 0; 		// SMP: security & encryption;


#if (TLKMDI_LEMGR_OTA_SERVER_ENABLE)
_attribute_ble_data_retention_	int sTlkMdiLemgrOtaIsWorking = 0;
#endif


static int tlkmdi_le_coreEventCB(uint32 evtID, uint08 *pData, uint16 dataLen);
static int tlkmdi_le_gattDataCB(uint16 connHandle, uint08 *pkt);
static int tlkmdi_le_hostEventCB(uint32 evtID, uint08 *para, int dataLen);

#if (!BLE_IOS_ANCS_ENABLE)

void latency_turn_off_onceEx(){
	extern void 		bls_pm_setManualLatency(u16 latency);
	bls_pm_setManualLatency(0);
}
#else

_attribute_data_retention_ u8	bls_needSendSecurityReq = 0;
_attribute_data_retention_ u8	bls_is_ios = 0;
_attribute_data_retention_ u32 	tick_findAncsSrv;
_attribute_data_retention_ u32 	tick_encryptionFinish;
_attribute_data_retention_ u32 	tick_conn_update_param = 0;

enum{
	NO_NEED_SEND_SEC_REQ = 0,
	NEED_SEND_SEC_REQ = 1,
};

enum{
	SYSTEM_TYPE_ANDROID = 0,
	SYSTEM_TYPE_IOS		= 1,
};


typedef enum{
	BLE_DISCONNECTED	= 0,
	BLE_CONNECTED 		= 1,
}BLE_CONNECTION_STATE;
_attribute_ble_data_retention_ u8 bleConnState = BLE_DISCONNECTED;

void app_setCurBleConnState(u8 state){
	bleConnState = state;
}

u8	app_getCurBleConnState(){
	return bleConnState;
}


void app_setSystemType(u8 isIOS){
	bls_is_ios = isIOS;
}

u8 app_getSystemType(){
	return bls_is_ios;
}

void app_setSendSecReqFlag(u8 en){
	bls_needSendSecurityReq = en;
}

u8 app_getSendSecReqFlag(){
	return bls_needSendSecurityReq;
}

void task_ancs_proc(u16 connHandle, u8 *p)
{
	if(ancsFuncIsEn()){
		ancsStackCallback(p);
	}

	if(app_getSendSecReqFlag() == NO_NEED_SEND_SEC_REQ ){
		rf_packet_l2cap_req_t_special_ancs *req = (rf_packet_l2cap_req_t_special_ancs *)p;
		//req = ancs_attPktTrans(p);


		if((req->chanId == 0x04)
			&& (req->opcode == ATT_OP_FIND_BY_TYPE_VALUE_RSP)){///ATT
			app_setSendSecReqFlag(NEED_SEND_SEC_REQ);
			//blc_smp_sendSecurityRequest();
			blt_smp_sendSecurityRequest(connHandle);
			tick_findAncsSrv = 0x00;
			my_dump_str_data(1, "security request send", 0, 0);
			blc_smp_setSecurityLevel_slave(Unauthenticated_Pairing_with_Encryption);  //LE_Security_Mode_1_Level_2
		}else{
			if((req->opcode == ATT_OP_ERROR_RSP) ///op_code
				&& (req->data[0] == ATT_OP_FIND_BY_TYPE_VALUE_RSP)){///err_op_code
				//tick_findAncsSrv = clock_time() | 1;
				//att not found
				app_setSendSecReqFlag(NO_NEED_SEND_SEC_REQ);
			}
		}
	}

	return;
}

void task_findAncsService(){
	 _attribute_data_retention_ static u8 ancsRetry = 0;

	if((tick_findAncsSrv & 0x01) && (clock_time_exceed(tick_findAncsSrv, 2 * 1000 * 1000))){
		ancsRetry++;
		tick_findAncsSrv = clock_time() | 0x01;
		ancs_findAncsService();
	}else{
		if((ancsRetry == 3)
			|| (ancsRetry && (tick_findAncsSrv == 0))){
			ancsRetry = 0;
			tick_findAncsSrv = 0;
		}
	}
}
extern u8			blt_llms_getCurrentState(void);
void task_ancsServiceEstablish(){
	if(app_getCurBleConnState() != BLE_CONNECTED){
		return;  // no connection
	}

	_attribute_data_retention_ static u8 flag_ancsBuildSrv = 0;
	task_findAncsService();///for check ANCS service enable

	if((tick_encryptionFinish & 0x01) && (clock_time_exceed(tick_encryptionFinish, 2 * 1000 * 1000))){
		ancsFuncSetEnable(1);
		tick_encryptionFinish = 0x00;
		flag_ancsBuildSrv = 1;
		my_dump_str_data(DUMP_ACL_MSG,"ancs enable", 0, 0);
	}else{
		if(flag_ancsBuildSrv){
			if(ancsGetConnState() == ANCS_CONNECTION_ESTABLISHED){///must be enabled after the ANCS established
				#if BLE_IOS_AMS_ENABLE
				amsInit(1);
				amsFuncSetEnable(1);
				#endif
				flag_ancsBuildSrv = 0;
			}
		}
	}
}

void app_curSystemType(){
	if(app_getSendSecReqFlag() == NEED_SEND_SEC_REQ){
		app_setSystemType(SYSTEM_TYPE_IOS);
	}else{
		if(ancsGetConnState() == ANCS_CONNECTION_ESTABLISHED){
			app_setSystemType(SYSTEM_TYPE_IOS);
		}
	}

}

void app_ancsTask(){
	if(app_getCurBleConnState() == BLE_CONNECTED){
			task_ancsServiceEstablish();
			app_curSystemType();
			ancs_mainLoopTask();
	}
}

#endif


#if (BLE_IOS_AMS_ENABLE)

void app_amsTask(){
	if(app_getCurBleConnState() == BLE_CONNECTED){
		ams_mainLoopTask();
	}
}

#endif


int tlkmdi_le_aclInit(void)
{

	ble_evt_t evt_hci_params;
	evt_hci_params.header = BLE_EVT_HCI_CMD_ID;
	evt_hci_params.evt.hci_cmd_evt = tlkmdi_le_coreEventCB;
	bleh_event_regCB(&evt_hci_params);
	
	ble_evt_t evt_gap_params;
	evt_gap_params.header = BLE_EVT_GAP_ID;
	evt_gap_params.evt.gap_evt = tlkmdi_le_hostEventCB;
	bleh_event_regCB(&evt_gap_params);

	ble_evt_t evt_gatt_params;
	evt_gatt_params.header = BLE_EVT_GATT_ID;
	evt_gatt_params.evt.gatt_evt = tlkmdi_le_gattDataCB;
	bleh_event_regCB(&evt_gatt_params);

	/* GATT Initialization */
	#if (TLKMDI_LE_CENTRAL_SDP_ENABLE)
		host_att_register_idle_func (ble_sdk_main_loop);
	#endif
	tlkmdi_lemgr_attInit();
	//////////////////////////// User Configuration for BLE application ////////////////////////////
	#if (TLKMDI_LEMGR_OTA_SERVER_ENABLE)
	blc_ota_initOtaServer_module();
	blc_ota_setOtaProcessTimeout(200);
	blc_ota_registerOtaStartCmdCb(tlkmdi_lemgr_enterOTAEvt);
	blc_ota_registerOtaResultIndicationCb(tlkmdi_lemgr_leaveOTAEvt);  //debug
	
	extern unsigned int ota_program_offset;
	if(ota_program_offset){
		tlkapi_error(TLKMDI_LEMGR_DBG_FLAG, TLKMDI_LEMGR_DBG_SIGN, "Firmware start from 0");
		tlkapi_error(TLKMDI_LEMGR_DBG_FLAG, TLKMDI_LEMGR_DBG_SIGN, "Firmware start from 0");
		tlkapi_error(TLKMDI_LEMGR_DBG_FLAG, TLKMDI_LEMGR_DBG_SIGN, "Firmware start from 0");
	}
	else{
		tlkapi_error(TLKMDI_LEMGR_DBG_FLAG, TLKMDI_LEMGR_DBG_SIGN, "Firmware start from 512K");
		tlkapi_error(TLKMDI_LEMGR_DBG_FLAG, TLKMDI_LEMGR_DBG_SIGN, "Firmware start from 512K");
		tlkapi_error(TLKMDI_LEMGR_DBG_FLAG, TLKMDI_LEMGR_DBG_SIGN, "Firmware start from 512K");
	}
	#endif
	u8 name_buf[] = {'T','E','L','I','N','K','-','L','E'};
	tlkmdi_le_setAclName(name_buf,sizeof(name_buf));
	blc_ll_setAdvParam(ADV_INTERVAL_50MS, ADV_INTERVAL_50MS, ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL, BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	//	blc_ll_setAdvCustomedChannel(37, 37, 37); //debug
	
	blc_ll_setScanParameter(SCAN_TYPE_ACTIVE, SCAN_INTERVAL_100MS, SCAN_WINDOW_50MS, OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
	//blc_ll_setScanEnable (BLC_SCAN_ENABLE, DUP_FILTER_DISABLE);


	#if (MCU_CORE_TYPE == MCU_CORE_B91)
	ext_btrf_setLeTxTpSlice(RF_POWER_P9p11dBm);
	#endif
	#if (MCU_CORE_TYPE == MCU_CORE_B92)
	ext_btrf_setLeTxTpSlice(RF_POWER_P9p15dBm);
	#endif

	return TLK_ENONE;
}
_attribute_ble_retention_code_
void tlkmdi_le_aclDeepInit(void)
{
	rf_drv_ble_init();

	blc_ll_initBasicMCU();
	blc_ll_recoverDeepRetention();

	reg_rf_irq_status = 0xFFFF;
	plic_interrupt_claim();//claim,clear eip
	plic_interrupt_complete(IRQ15_ZB_RT);//complete
}
//////////////////////////////////////////////////////event//////////////////////////////////////////
extern TlkMdiLeAclCallback sTlkMdiLeAclDiscCB;
extern TlkMdiLeAclCallback sTlkMdiLeAclConCB;
extern TlkMdiLeAclCallback sTlkMdiLeAclConUpdateCB;
extern TlkMdiLeAclCallback sTlkMdiLeAclPhyUpdateCB;
extern TlkMdiLeAclCallback sTlkMdiLeAclAdvReportCB;

/**
 * @brief      BLE Connection complete event handler
 * @param[in]  p         Pointer point to event parameter buffer.
 * @return
 */
int tlkmdi_le_connection_complete_event_handle(u8 *p)
{
	hci_le_connectionCompleteEvt_t *pConnEvt = (hci_le_connectionCompleteEvt_t *)p;
	if(pConnEvt->status == BLE_SUCCESS){
		dev_char_info_insert_by_conn_event(pConnEvt);
		if(pConnEvt->role == ACL_ROLE_CENTRAL) // central role, process SMP and SDP if necessary
		{
			#if (TLKMDI_LE_CENTRAL_SMP_ENABLE)//
				central_smp_pending = pConnEvt->connHandle; // this connection need SMP
			#elif (ACL_CENTRAL_CUSTOM_PAIR_ENABLE)
				//manual pairing, device match, add this device to peripheral mac table
				if(blm_manPair.manual_pair && blm_manPair.mac_type == pConnEvt->peerAddrType && !memcmp(blm_manPair.mac, pConnEvt->peerAddr, 6)){
					blm_manPair.manual_pair = 0;
					user_tbl_peripheral_mac_add(pConnEvt->peerAddrType, pConnEvt->peerAddr);
				}
			#endif
			#if (1)
				memset(&cur_sdp_device, 0, sizeof(dev_char_info_t));
				cur_sdp_device.conn_handle = pConnEvt->connHandle;
				cur_sdp_device.peer_adrType = pConnEvt->peerAddrType;
				memcpy(cur_sdp_device.peer_addr, pConnEvt->peerAddr, 6);

				u8	temp_buff[sizeof(dev_att_t)];
				dev_att_t *pdev_att = (dev_att_t *)temp_buff;

				/* att_handle search in flash, if success, load char_handle directly from flash, no need SDP again */
				if( dev_char_info_search_peer_att_handle_by_peer_mac(pConnEvt->peerAddrType, pConnEvt->peerAddr, pdev_att) ){
					//cur_sdp_device.char_handle[1] = 									//Speaker
					cur_sdp_device.char_handle[2] = pdev_att->char_handle[2];			//OTA
					cur_sdp_device.char_handle[3] = pdev_att->char_handle[3];			//consume report
					cur_sdp_device.char_handle[4] = pdev_att->char_handle[4];			//normal key report
					//cur_sdp_device.char_handle[6] =									//BLE Module, SPP Server to Client
					//cur_sdp_device.char_handle[7] =									//BLE Module, SPP Client to Server

					/* add the peer device att_handle value to conn_dev_list */
					dev_char_info_add_peer_att_handle(&cur_sdp_device);
				}
				else
				{
					master_sdp_pending = pConnEvt->connHandle;  // mark this connection need SDP
					#if (TLKMDI_LE_CENTRAL_SMP_ENABLE)
						 //service discovery initiated after SMP done, trigger it in "GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE" event callBack.
					#else
						 app_register_service(&app_service_discovery); 	//No SMP, service discovery can initiated now
					#endif
				}
			#endif
		}
		else
		{
			bls_l2cap_requestConnParamUpdate(pConnEvt->connHandle, CONN_INTERVAL_20MS, CONN_INTERVAL_20MS, 49, CONN_TIMEOUT_4S);
		}
	}

	return 0;
}
/**
 * @brief      BLE Disconnection event handler
 * @param[in]  p         Pointer point to event parameter buffer.
 * @return
 */
int 	tlkmdi_le_disconnect_event_handle(u8 *p)
{
	hci_disconnectionCompleteEvt_t	*pDisConn = (hci_disconnectionCompleteEvt_t *)p;

	//terminate reason
	if(pDisConn->reason == HCI_ERR_CONN_TIMEOUT){  	//connection timeout

	}
	else if(pDisConn->reason == HCI_ERR_REMOTE_USER_TERM_CONN){  	//peer device send terminate command on link layer

	}
	else if(pDisConn->reason == HCI_ERR_CONN_TERM_BY_LOCAL_HOST){

	}
	else{

	}


	/* if previous connection SMP & SDP not finished, clear flag */
	#if (TLKMDI_LE_CENTRAL_SMP_ENABLE)
		if(central_smp_pending == pDisConn->connHandle){
			central_smp_pending = 0;
		}
	#endif
	#if (TLKMDI_LE_CENTRAL_NUM_ENABLE)
		if(master_sdp_pending == pDisConn->connHandle){
			master_sdp_pending = 0;
		}
	#endif

	dev_char_info_delete_by_connhandle(pDisConn->connHandle);


	return 0;
}

/**
 * @brief      BLE Adv report event handler
 * @param[in]  p         Pointer point to event parameter buffer.
 * @return
 */

int app_le_adv_report_event_handle(u8 *p)
{
#if	TLKMDI_LE_CENTRAL_NUM_ENABLE
	event_adv_report_t *pa = (event_adv_report_t *)p;
//	s8 rssi = pa->data[pa->len];/
    gpio_toggle(GPIO_PC4);

	/*********************** Central Create connection demo: Key press or ADV pair packet triggers pair  ********************/
	#if (TLKMDI_LE_CENTRAL_SMP_ENABLE)
		if(central_smp_pending){ 	 //if previous connection SMP not finish, can not create a new connection
			return 1;
		}
	#endif

	#if (TLKMDI_LE_CENTRAL_NUM_ENABLE)
		if(master_sdp_pending){ 	 //if previous connection SDP not finish, can not create a new connection
			return 1;
		}
	#endif


	#if (TLKMDI_LE_CENTRAL_SMP_ENABLE)
	u8	central_auto_connect = blc_smp_searchBondingSlaveDevice_by_PeerMacAddress(pa->adr_type, pa->mac);
	#else
	u8	central_auto_connect = 0;
	#endif
	//manual pairing methods 1: key press triggers
//	u8 user_manual_pairing =  (rssi > -40) ? 1:0;  //button trigger pairing(RSSI threshold, short distance)

	if((central_auto_connect) && (blc_ll_getCurrentMasterRoleNumber() < ACL_CENTRAL_MAX_NUM)){

		/* send create connection command to Controller, trigger it switch to initiating state. After this command, Controller
		 * will scan all the ADV packets it received but not report to host, to find the specified device(mac_adr_type & mac_adr),
		 * then send a "CONN_REQ" packet, enter to connection state and send a connection complete event
		 * (HCI_SUB_EVT_LE_CONNECTION_COMPLETE) to Host*/
		u8 status = blc_ll_createConnection( SCAN_INTERVAL_100MS, SCAN_WINDOW_100MS, INITIATE_FP_ADV_SPECIFY,  \
								 pa->adr_type, pa->mac, OWN_ADDRESS_PUBLIC, \
								 CONN_INTERVAL_31P25MS, CONN_INTERVAL_48P75MS, 0, CONN_TIMEOUT_4S, \
								 0, 0xFFFF);

		if(status == BLE_SUCCESS){ //create connection success
			tlkapi_trace(TLKMDI_LE_DBG_FLAG, TLKMDI_LE_DBG_SIGN, "blc_ll_createConnection mac:%02x %02x %02x %02x %02x %02x",\
																							pa->mac[0],pa->mac[1],pa->mac[2],\
																							pa->mac[3],pa->mac[4],pa->mac[5]);
		}
	}
	/*********************** Central Create connection demo code end  *******************************************************/
#endif
	return 0;
}
static int tlkmdi_le_coreEventCB(uint32 evtID, uint08 *pData, uint16 dataLen)
{
	if(evtID &HCI_FLAG_EVENT_BT_STD)		//Controller HCI event
	{
		uint08 evtCode = evtID & 0xff;

		//------------ disconnect -------------------------------------
		if(evtCode == HCI_EVT_DISCONNECTION_COMPLETE)  //connection terminate
		{
			tlkmdi_le_disconnect_event_handle(pData);
			if(sTlkMdiLeAclDiscCB)
			{
				sTlkMdiLeAclDiscCB(pData, dataLen);
			}
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
				tlkmdi_le_connection_complete_event_handle(pData);
				//TO MMI
				if(sTlkMdiLeAclConCB)
				{
					sTlkMdiLeAclConCB(pData, dataLen);
				}
			}
			//--------hci le event: le adv report event ----------------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
			{

				//after controller is set to scan state, it will report all the adv packet it received by this event
                  app_le_adv_report_event_handle(pData);
				if(sTlkMdiLeAclAdvReportCB)
				{
					sTlkMdiLeAclAdvReportCB(pData, dataLen);
				}
			}
			//------hci le event: le connection update complete event-------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
			{
				if(sTlkMdiLeAclConUpdateCB)
				{
					sTlkMdiLeAclConUpdateCB(pData, dataLen);
				}
			}
			else if(subEvt_code ==  HCI_SUB_EVT_LE_PHY_UPDATE_COMPLETE )
			{
				if(sTlkMdiLeAclPhyUpdateCB)
				{
					sTlkMdiLeAclPhyUpdateCB(pData, dataLen);
				}
			}
		}
	}

	
	return 0;
}

static int tlkmdi_le_hostEventCB(uint32 evtID, uint08 *para, int dataLen)
{
	uint08 event = evtID & 0xFF;

	switch(event)
	{
		case GAP_EVT_SMP_PAIRING_BEGIN:
		{
		}
		break;

		case GAP_EVT_SMP_PAIRING_SUCCESS:
		{
		}
		break;

		case GAP_EVT_SMP_PAIRING_FAIL:
		{
			#if (TLKMDI_LE_CENTRAL_SMP_ENABLE)
				gap_smp_pairingFailEvt_t *pEvt = (gap_smp_pairingFailEvt_t *)para;

				if( dev_char_get_conn_role_by_connhandle(pEvt->connHandle) == ACL_ROLE_CENTRAL){
					if(central_smp_pending == pEvt->connHandle){
						central_smp_pending = 0;
					}
				}
			#endif
		}
		break;

		case GAP_EVT_SMP_CONN_ENCRYPTION_DONE:
		{
			gap_smp_connEncDoneEvt_t* p = (gap_smp_connEncDoneEvt_t*)para;
			if(p->re_connect)
			{
				//
			}
		}
		break;
		
		case GAP_EVT_SMP_SECURITY_PROCESS_DONE:
		{
#if BLE_IOS_ANCS_ENABLE
			app_setSendSecReqFlag(NEED_SEND_SEC_REQ);
#endif
#if BLE_IOS_ANCS_ENABLE
			tick_findAncsSrv = 0x00;
			tick_encryptionFinish = clock_time() | 1;
#endif
#if (BLE_IOS_AMS_ENABLE && (!BLE_IOS_ANCS_ENABLE))
			amsInit(1);
			amsFuncSetEnable(1);
#endif
			gap_smp_connEncDoneEvt_t* pEvt = (gap_smp_connEncDoneEvt_t*)para;
			//tlkapi_send_string_data(APP_SMP_LOG_EN, "[APP][SMP] Security process done event", &pEvt->connHandle, sizeof(gap_smp_connEncDoneEvt_t));

			if( dev_char_get_conn_role_by_connhandle(pEvt->connHandle) == ACL_ROLE_CENTRAL)
			{
				#if (TLKMDI_LE_CENTRAL_SMP_ENABLE)
				  if( dev_char_get_conn_role_by_connhandle(pEvt->connHandle) == ACL_ROLE_CENTRAL){
						if(central_smp_pending == pEvt->connHandle){
							central_smp_pending = 0;
						}
					}
				#endif //(TLKMDI_LE_CENTRAL_SMP_ENABLE)
				#if (TLKMDI_LE_CENTRAL_NUM_ENABLE)  //SMP finish
					if(master_sdp_pending == pEvt->connHandle){  //SDP is pending
						app_register_service(&app_service_discovery);  //start SDP now
					}
				#endif
			}
		}
		break;
		
		case GAP_EVT_SMP_TK_DISPLAY:
		{


		}
		break;

		case GAP_EVT_SMP_TK_REQUEST_PASSKEY:
		{

		}
		break;

		case GAP_EVT_SMP_TK_REQUEST_OOB:
		{

		}
		break;

		case GAP_EVT_SMP_TK_NUMERIC_COMPARE:
		{

		}
		break;

		case GAP_EVT_ATT_EXCHANGE_MTU:
		{

		}
		break;

		case GAP_EVT_GATT_HANDLE_VALUE_CONFIRM:
		{

		}
		break;

		default:
		break;
	}

	return 0;
}
#define			HID_HANDLE_CONSUME_REPORT			25
#define			HID_HANDLE_KEYBOARD_REPORT			29
#define			AUDIO_HANDLE_MIC					52
#define			OTA_HANDLE_DATA						48
/**
 * @brief      BLE GATT data handler call-back.
 * @param[in]  connHandle     connection handle.
 * @param[in]  pkt             Pointer point to data packet buffer.
 * @return
 */
static int tlkmdi_le_gattDataCB(uint16 connHandle, uint08 *pkt)
{
#if (TLKMDI_LE_CENTRAL_NUM_ENABLE)

	if(dev_char_get_conn_role_by_connhandle(connHandle) == LL_ROLE_MASTER)
	{
		rf_packet_att_t *pAtt = (rf_packet_att_t*)pkt;
#if (TLKMDI_LE_CENTRAL_SDP_ENABLE)
	if(master_sdp_pending == connHandle ){  //ATT service discovery is ongoing on this conn_handle
		//when service discovery function is running, all the ATT data from peripheral
		//will be processed by it,  user can only send your own att cmd after  service discovery is over
		host_att_client_handler (connHandle, pkt); //handle this ATT data by service discovery process
	}
#endif
		dev_char_info_t* dev_info = dev_char_info_search_by_connhandle (connHandle);
		if(dev_info)
		{
			//-------	user process ------------------------------------------------
			u16 attHandle = pAtt->handle;
			if(pAtt->opcode == ATT_OP_HANDLE_VALUE_NOTI)  //slave handle notify
			{
				//---------------	consumer key --------------------------
				#if (TLKMDI_LE_CENTRAL_SDP_ENABLE)
				if(attHandle == dev_info->char_handle[3])  // Consume Report In (Media Key)
				#else
				if(attHandle == HID_HANDLE_CONSUME_REPORT)   //Demo device(825x_ble_sample) Consume Report AttHandle value is 25
				#endif
				{
					//printf
				}
				//---------------	keyboard key --------------------------
				#if (TLKMDI_LE_CENTRAL_SDP_ENABLE)
				else if(attHandle == dev_info->char_handle[4])     // Key Report In
				#else
				else if(attHandle == HID_HANDLE_KEYBOARD_REPORT)   // Demo device(825x_ble_sample) Key Report AttHandle value is 29
				#endif
				{
					//att_keyboard (connHandle, pAtt->dat);
					//printf
				}
				#if (TLKMDI_LE_CENTRAL_SDP_ENABLE)
				else if(attHandle == dev_info->char_handle[0])     // AUDIO Notify
				#else
				else if(attHandle == AUDIO_HANDLE_MIC)   // Demo device(825x_ble_remote) Key Report AttHandle value is 52
				#endif
				{

				}
				else
				{

				}
				
			}
			else if (pAtt->opcode == ATT_OP_HANDLE_VALUE_IND)
			{
				blc_gatt_pushAttHdlValueCfm(connHandle);
			}
		}

		/* The Master does not support GATT Server by default */
		if(!(pAtt->opcode & 0x01)){
			switch(pAtt->opcode){
				case ATT_OP_FIND_INFO_REQ:
				case ATT_OP_FIND_BY_TYPE_VALUE_REQ:
				case ATT_OP_READ_BY_TYPE_REQ:
				case ATT_OP_READ_BY_GROUP_TYPE_REQ:
					blc_gatt_pushErrResponse(connHandle, pAtt->opcode, pAtt->handle, ATT_ERR_ATTR_NOT_FOUND);
					break;
				case ATT_OP_READ_REQ:
				case ATT_OP_READ_BLOB_REQ:
				case ATT_OP_READ_MULTI_REQ:
				case ATT_OP_WRITE_REQ:
				case ATT_OP_PREPARE_WRITE_REQ:
					blc_gatt_pushErrResponse(connHandle, pAtt->opcode, pAtt->handle, ATT_ERR_INVALID_HANDLE);
					break;
				case ATT_OP_EXECUTE_WRITE_REQ:
				case ATT_OP_HANDLE_VALUE_CFM:
				case ATT_OP_WRITE_CMD:
				case ATT_OP_SIGNED_WRITE_CMD:
					//ignore
					break;
				default://no action
					break;
			}
		}
	}
	else
#endif
	{   //GATT data for Slave
		//todo
	}


#if BLE_IOS_ANCS_ENABLE
		task_ancs_proc(connHandle, pkt);
#endif
	
#if BLE_IOS_AMS_ENABLE
		if(amsFuncIsEn()){
			amsStackCallback(pkt);
		}
#endif


	return 0;
}


#if (TLKMDI_LEMGR_OTA_SERVER_ENABLE)

/**
 * @brief      this function is used to register the function for OTA start.
 * @param[in]  none
 * @return     none
 */
static void tlkmdi_le_enterOTAEvt(void)
{
	sTlkMmiLemgrOtaIsWorking = 1;

	tlkapi_trace(TLKMDI_LEMGR_DBG_FLAG, TLKMDI_LEMGR_DBG_SIGN, "OTA start: %d", ota_program_offset);
}

/**
 * @brief       no matter whether the OTA result is successful or fail.
 *              code will run here to tell user the OTA result.
 * @param[in]   result    OTA result:success or fail(different reason)
 * @return      none
 */
static void tlkmdi_le_leaveOTAEvt(int result)
{
	
}
#endif

void tlkmdi_le_sdpTask(void){
#if  TLKMDI_LE_CENTRAL_SDP_ENABLE
	simple_sdp_loop ();
#endif

}


#endif //TLKMDI_LEMGR_ENABLE

