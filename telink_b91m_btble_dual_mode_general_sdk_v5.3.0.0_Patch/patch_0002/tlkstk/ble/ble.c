/********************************************************************************************************
 * @file	ble.c
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
#include "ble_common.h"
#include "ble.h"
#include "tlkstk/inner/tlkstk_inner.h"
#include "drivers.h"
#include "ble.h"




/********************* ACL connection LinkLayer TX & RX data FIFO allocation, Begin *******************************/

/**
 * @brief	ACL RX buffer, shared by all connections to hold LinkLayer RF RX data.
 * 			user should define and initialize this buffer if either ACL Central or ACL Peripheral is used.
 */
_attribute_ble_data_retention_	u8	app_acl_rx_fifo[ACL_RX_FIFO_SIZE * ACL_RX_FIFO_NUM] = {0};

/**
 * @brief	ACL Central TX buffer, shared by all central connections to hold LinkLayer RF TX data.
 *  		ACL Central TX buffer should be defined only when ACl connection central role is used.
 */
#if ACL_CENTRAL_MAX_NUM
_attribute_ble_data_retention_	u8	app_acl_cen_tx_fifo[ACL_CENTRAL_TX_FIFO_SIZE * ACL_CENTRAL_TX_FIFO_NUM * ACL_CENTRAL_MAX_NUM] = {0};
#endif
/**
 * @brief	ACL Peripheral TX buffer, shared by all peripheral connections to hold LinkLayer RF TX data.
 *  		ACL Peripheral TX buffer should be defined only when ACl connection peripheral role is used.
 */
#if ACL_PERIPHR_MAX_NUM
_attribute_ble_data_retention_	u8	app_acl_per_tx_fifo[ACL_PERIPHR_TX_FIFO_SIZE * ACL_PERIPHR_TX_FIFO_NUM * ACL_PERIPHR_MAX_NUM] = {0};
#endif
/******************** ACL connection LinkLayer TX & RX data FIFO allocation, End ***********************************/



/***************** ACL connection L2CAP RX & TX data Buffer allocation, Begin **************************************/
/**
 * @brief	L2CAP RX Data buffer for ACL Central
 */
#if ACL_CENTRAL_MAX_NUM
_attribute_ble_data_retention_	u8 app_cen_l2cap_rx_buf[ACL_CENTRAL_MAX_NUM * CENTRAL_L2CAP_BUFF_SIZE];
#endif
/**
 * @brief	L2CAP TX Data buffer for ACL Central
 * 			if GATT server on ACL Central used, this buffer must be defined and initialized.
 * 			if GATT server on ACL Central not used, this buffer can be saved.
 */
//_attribute_ble_data_retention_	u8 app_cen_l2cap_tx_buf[ACL_CENTRAL_MAX_NUM * CENTRAL_L2CAP_BUFF_SIZE];

/**
 * @brief	L2CAP RX Data buffer for ACL Peripheral
 */
#if ACL_PERIPHR_MAX_NUM
_attribute_ble_data_retention_	u8 app_per_l2cap_rx_buf[ACL_PERIPHR_MAX_NUM * PERIPHR_L2CAP_BUFF_SIZE];
#endif
/**
 * @brief	L2CAP TX Data buffer for ACL Peripheral
 *		    GATT server on ACL Peripheral use this buffer.
 */
#if ACL_PERIPHR_MAX_NUM
_attribute_ble_data_retention_	u8 app_per_l2cap_tx_buf[ACL_PERIPHR_MAX_NUM * PERIPHR_L2CAP_BUFF_SIZE];
#endif
/***************** ACL connection L2CAP RX & TX data Buffer allocation, End ****************************************/




/***HCI ACL buffer define***/
#define HCI_ACL_DATA_LEN              (CENTRAL_L2CAP_BUFF_SIZE > PERIPHR_L2CAP_BUFF_SIZE ? CENTRAL_L2CAP_BUFF_SIZE : PERIPHR_L2CAP_BUFF_SIZE)
#define HCI_ACL_BUF_SIZE              (4 + HCI_ACL_DATA_LEN)
#define HCI_ACL_BUF_NUM               8
_attribute_ble_data_retention_ u8 bleHciAclBuf[HCI_ACL_BUF_SIZE * HCI_ACL_BUF_NUM];

#if  BLE_DELAY_REPORT_DISCONNECT_EV
//data
#define APP_CONNECT_COMPLETE_EVT_DELAY_US   150000
#define APP_EVT_MAX_NUM    4
#define APP_EVT_MAX_SIZE   36 //(8 + 5 + 65)
my_fifo_t app_ble_txfifo;
//MYFIFO_INIT(app_ble_txfifo, APP_EVT_MAX_SIZE, APP_EVT_MAX_NUM);

//flag
struct APP_BLE_EVT_C_T {
   bool  Discon_flag;	//Mark disconnect timeout
   bool  write_flag;     //After writing, clean it manually  (true and false)
   u32   tick;           //Record the current time
   u32   delay_us;       //set Size of time delay value
   u8    p_buf[APP_EVT_MAX_NUM*APP_EVT_MAX_SIZE];
} app_ble_evt_c;        //Control related data and marks
#endif


int app_controller_event_cback(u32 h, u8 *p, int n)
{
	int re_val = 0;
#if  BLE_DELAY_REPORT_DISCONNECT_EV
	if(h &HCI_FLAG_EVENT_BT_STD)
	{
		u8 evtCode = h & 0xff;
		if(evtCode == HCI_EVT_DISCONNECTION_COMPLETE)  //connection terminate
		{
			if (HCI_ERR_CONN_TIMEOUT == p[3]) {
				app_ble_evt_c.Discon_flag = true;
				//my_dump_str_data(BLE_ACL_CONN_DBG_EN, "#timeout 0x08", NULL, 0);
			}
			else if (HCI_ERR_CONN_FAILED_TO_ESTABLISH == p[3]) {
				//my_dump_str_data(BLE_ACL_CONN_DBG_EN, "#connect fail 0x3E#", NULL, 0);
			}
			if (app_ble_evt_c.tick > 0) {
				app_ble_evt_c.write_flag = true;
			}
		}
		/* LE Event Handle */
		else if(evtCode == HCI_EVT_LE_META)
		{
			u8 subEvtCode = p[0];
			switch(subEvtCode)
			{
			case HCI_SUB_EVT_LE_ADVERTISING_REPORT:
			{
				break;
			}
			case HCI_SUB_EVT_LE_CONNECTION_COMPLETE: {
				if (app_ble_evt_c.Discon_flag == true) {
					app_ble_evt_c.Discon_flag = false;  // clear flag
					app_ble_evt_c.write_flag = true;
					//my_dump_str_data(BLE_ACL_CONN_DBG_EN, "#connect start#", NULL, 0);
					app_ble_evt_c.tick = clock_time() | 1;
				}
			}
			break;
			default:
				break;
			}
		}
	}

	if (app_ble_evt_c.write_flag == true)
	{
		u8 *app_p = my_fifo_wptr (&app_ble_txfifo);
		*app_p++ = h;
		*app_p++ = h >> 8;
		*app_p++ = h >> 16;
		*app_p++ = h >> 24;
		*app_p++ = n;
		*app_p++ = n >> 8;
		*app_p++ = n >> 16;
		*app_p++ = n >> 24;
		 smemcpy (app_p, p, n);
		 app_p += n;
		 //my_dump_str_data(BLE_ACL_CONN_DBG_EN, "#start h = #", app_ble_txfifo.p, 128);
		 app_ble_txfifo.wptr++;
		 app_ble_evt_c.write_flag = false;
		 re_val = 0;
	}
	else
#endif
	{
		re_val = blc_hci_send_data(h, p, n);
	}

	return re_val;
}

/**
 * @brief		user initialization for BLE mode when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return      none
 */
void ble_core_init(void)
{

//////////////////////////// BLE stack Initialization  Begin //////////////////////////////////

	/* for 1M Flash, flash_sector_mac_address equals to 0xFF000
	 * for 2M Flash, flash_sector_mac_address equals to 0x1FF000 */
	uint08 mac_public[6] = {0x20,0xCD,0xC3,0x67,0xb5,00};

	//////////// Controller Initialization  Begin /////////////////////////
	blc_ll_initBasicMCU();

	blc_ll_initStandby_module(mac_public);		//mandatory

#if ACL_PERIPHR_MAX_NUM
	blc_ll_initLegacyAdvertising_module();		//mandatory for BLE slave
#endif

#if ACL_CENTRAL_MAX_NUM
	blc_ll_initLegacyScanning_module(); 		//scan module: 		 mandatory for BLE master

	blc_ll_initLegacyInitiating_module();		//initiate module: 	 mandatory for BLE master
#endif
	blc_ll_initAclConnection_module();			//mandatory for BLE slave & master

#if ACL_PERIPHR_MAX_NUM
	blc_ll_initAclSlaveRole_module();			//mandatory for BLE slave
#endif

#if ACL_CENTRAL_MAX_NUM
	blc_ll_initAclMasterRole_module();			//mandatory for BLE master
#endif
	//blc_ll_init2MPhyCodedPhy_feature();		//This feature is not supported by default

	blc_ll_disableDefaultInitTimeout(); //disable default initiator timeout

	blc_ll_setMaxConnectionNumber(ACL_CENTRAL_MAX_NUM, ACL_PERIPHR_MAX_NUM);

	blc_ll_setAclConnMaxOctetsNumber(ACL_CONN_MAX_RX_OCTETS, ACL_CENTRAL_MAX_TX_OCTETS, ACL_PERIPHR_MAX_TX_OCTETS);

	/* all ACL connection share same RX FIFO */
	blc_ll_initAclConnRxFifo(app_acl_rx_fifo, ACL_RX_FIFO_SIZE, ACL_RX_FIFO_NUM);
	/* ACL Central TX FIFO */
#if ACL_CENTRAL_MAX_NUM
	blc_ll_initAclConnMasterTxFifo(app_acl_cen_tx_fifo, ACL_CENTRAL_TX_FIFO_SIZE, ACL_CENTRAL_TX_FIFO_NUM, ACL_CENTRAL_MAX_NUM);
#endif
	/* ACL Peripheral TX FIFO */
#if ACL_PERIPHR_MAX_NUM
	blc_ll_initAclConnSlaveTxFifo(app_acl_per_tx_fifo, ACL_PERIPHR_TX_FIFO_SIZE, ACL_PERIPHR_TX_FIFO_NUM, ACL_PERIPHR_MAX_NUM);
#endif

	#if( (MASTER_MAX_NUM + SLAVE_MAX_NUM ) > MAX_BLE_LINK)
		#error MAX BLE lINK not enough
	#endif


	blc_ll_setAclMasterConnectionInterval(CONN_INTERVAL_7P5MS);
#if (TLKMMI_LEMGR_WORKAROUND_TX_FIFO_4K_LIMITATION_EN && (ACL_PERIPHR_TX_FIFO_SIZE > 230 || ACL_CENTRAL_TX_FIFO_SIZE > 230))
/* extend TX FIFO size for MAX_TX_OCTETS > 230 if user want use 16 or 32 FIFO */
	blc_ll_initAclConnCacheTxFifo(sTlkmmiLemgrAclCacheTxFifo, 260, 16);
#endif
	//////////// Controller Initialization  End /////////////////////////



	//////////// HCI Initialization  Begin /////////////////////////
	/* HCI ACL buffer config. */
	blc_ll_initHciAclDataFifo(bleHciAclBuf, HCI_ACL_BUF_SIZE, HCI_ACL_BUF_NUM);

	blc_hci_registerControllerDataHandler (blc_hci_sendACLData2Host);
	blc_hci_registerControllerEventHandler(blc_hci_send_data); //controller hci event to host all processed in this func
	/* bluetooth event */
	blc_hci_setEventMask_cmd (0xffffffff);

	/* bluetooth low energy(LE) event */
	blc_hci_le_setEventMask_cmd(0xffffffff);  //connection establish: telink private event   TODO: delete this cause we will support standard HCI
	//////////// HCI Initialization  End /////////////////////////

	u8 check_status = blc_controller_check_appBufferInitialization();
	if(check_status != BLE_SUCCESS){
		/* here user should set some log to know which application buffer incorrect*/
		write_log32(0x88880000 | check_status);
		my_dump_str_data(1, "application buffer error", &check_status, 1);
		while(1);
	}
//////////////////////////// BLE stack Initialization  End //////////////////////////////////

}


/**
 * @brief      BLE host event handler call-back.
 * @param[in]  h       event type
 * @param[in]  para    Pointer point to event parameter buffer.
 * @param[in]  n       the length of event parameter.
 * @return
 */
int app_gap_event_callback(u32 h, u8 *para, int n)
{
	return 0;
}

/**
 * @brief      BLE GATT data handler call-back.
 * @param[in]  connHandle     connection handle.
 * @param[in]  pkt             Pointer point to data packet buffer.
 * @return
 */
int app_gatt_data_handler(u16 connHandle, u8 *pkt)
{
	//app add
	return 0;
}

int bleh_init(void)
{
	#if (TLK_CFG_PM_ENABLE)
	blc_ll_initPowerManagement_module();
	#endif

	//////////// Host Initialization  Begin /////////////////////////
	/* Host Initialization */
	/* GAP initialization must be done before any other host feature initialization !!! */
	blc_gap_init();


	/* L2CAP data buffer Initialization */
#if ACL_CENTRAL_MAX_NUM
	blm_l2cap_initMtuBuffer(app_cen_l2cap_rx_buf, CENTRAL_L2CAP_BUFF_SIZE, NULL,	0);
	blc_att_setMasterRxMTUSize(CENTRAL_ATT_RX_MTU); ///must be placed after "blc_gap_init"
#endif
#if ACL_PERIPHR_MAX_NUM
	bls_l2cap_initMtuBuffer(app_per_l2cap_rx_buf, PERIPHR_L2CAP_BUFF_SIZE, app_per_l2cap_tx_buf, PERIPHR_L2CAP_BUFF_SIZE);
	blc_att_setSlaveRxMTUSize(PERIPHR_ATT_RX_MTU);   ///must be placed after "blc_gap_init"
#endif
	/* GATT Initialization */
    //tlkmmi_lemgr_attInit();
	blc_gatt_register_data_handler(app_gatt_data_handler);


	/* SMP Initialization */
	#if (TLK_LE_PERIPHR_SMP_ENABLE || TLK_BLE_CENTRAL_SMP_ENABLE)
	extern unsigned int tlkcfg_getFlashAddr(unsigned int offset);
	unsigned int addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_SMP_PAIRING_ADDR);
	blc_smp_configPairingSecurityInfoStorageAddressAndSize(addr, TLK_CFG_FLASH_LE_SMP_PAIRING_SIZE);
	#endif

	#if (TLK_LE_PERIPHR_SMP_ENABLE)
	blc_smp_setSecurityLevel_slave(Unauthenticated_Pairing_with_Encryption);  //LE_Security_Mode_1_Level_2
	#else
	blc_smp_setSecurityLevel(No_Security);  //LE_Security_Mode_1_Level_2
	#endif

	//	blc_smp_setPairingMethods(LE_Secure_Connection); //TLKMMI_LEMGR_ATT_MTU_SLAVE_RX_MAX_SIZE or TLKMMI_LEMGR_ATT_MTU_MASTER_RX_MAX_SIZE >= 64
	blc_smp_smpParamInit();
	// Hid device on android7.0/7.1 or later version
	// New paring: send security_request immediately after connection complete
	// reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
	blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection)

	/* Host (GAP/SMP/GATT/ATT) event process: register host event callback and set event mask */
	blc_gap_registerHostEventHandler(app_gap_event_callback );
	blc_gap_setEventMask( GAP_EVT_MASK_SMP_PAIRING_BEGIN			|  \
						  GAP_EVT_MASK_SMP_PAIRING_SUCCESS			|  \
						  GAP_EVT_MASK_SMP_PAIRING_FAIL 			|  \
						  GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE);
	//////////// Host Initialization  End /////////////////////////

	return 0;
}


void ble_customize_init(void)
{
#if  BLE_DELAY_REPORT_DISCONNECT_EV
	app_ble_evt_c.Discon_flag = false;
	app_ble_evt_c.tick = 0;
	app_ble_evt_c.delay_us = APP_CONNECT_COMPLETE_EVT_DELAY_US;
	app_ble_evt_c.write_flag = false;
	my_fifo_init (&app_ble_txfifo, APP_EVT_MAX_SIZE, APP_EVT_MAX_NUM, app_ble_evt_c.p_buf);
#endif

}


void ble_customize_main_loop(void)
{
#if  BLE_DELAY_REPORT_DISCONNECT_EV
	if (app_ble_evt_c.tick	&& clock_time_exceed(app_ble_evt_c.tick, app_ble_evt_c.delay_us))
	{
		app_ble_evt_c.tick = 0;
        u8 num;
        u32 h,n;
        num = 0;
		while (app_ble_txfifo.rptr != app_ble_txfifo.wptr){
			// process HCI_RX/UART_TX fifo
			uint8_t *main_p = app_ble_txfifo.p + (app_ble_txfifo.rptr & (app_ble_txfifo.num-1)) * app_ble_txfifo.size;
			h = main_p[0]|(main_p[1]<<8)|(main_p[2]<<16)|(main_p[3]<<24);
			n = main_p[4]|(main_p[5]<<8)|(main_p[6]<<16)|(main_p[7]<<24);
			blc_hci_send_data(h, &main_p[8],n);
			app_ble_txfifo.rptr++;
			num++;
			if(num >= APP_EVT_MAX_NUM)
			{
				//my_dump_str_data(BLE_ACL_CONN_DBG_EN, "#main loop overflow#", NULL, 0);
				break;
			}
		}
	}
#endif

}
int blec_init(void)
{
	ble_core_init();
	ble_customize_init();

	u8 Bd_addBuf[6] = {0x20, 0xcd, 0xc3, 0x67, 0xb5, 0x00};
	blc_ll_writeBDAddr(Bd_addBuf); //set ble addr

	return 0;
}

/******************************************************************************
 * Function: bleh_event_regCB
 * Descript: Register the event callback.
 * Params:
 *        @evtID[IN]--The event id.
 *        @func[IN]--The callback function.
 * Return: None.
*******************************************************************************/
void bleh_event_regCB(ble_evt_t * p_ble_evt)
{
	if(p_ble_evt->header >= BLE_EVT_MAX) return;
	switch(p_ble_evt->header)
	{
	case BLE_EVT_HCI_CMD_ID:
		blc_hci_registerControllerDataHandler(blt_l2cap_pktHandler);
		blc_hci_registerControllerEventHandler(p_ble_evt->evt.hci_cmd_evt); //controller hci event to host all processed in this func
		break;
	case BLE_EVT_GAP_ID:
		blc_gap_registerHostEventHandler(p_ble_evt->evt.gap_evt);
		break;
	case BLE_EVT_GATT_ID:
		blc_gatt_register_data_handler(p_ble_evt->evt.gatt_evt);
		break;
	default:
		break;
	}
}

