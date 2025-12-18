/********************************************************************************************************
 * @file	l2cap_signal.h
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
#ifndef L2CAP_COC_H_
#define L2CAP_COC_H_

/**
 * @brief MPS - Maximum PDU Payload Size
 *        MTU - Maximum Transmission Unit
 *        CID - Channel Identifier
 *        SPSM - Simplified Protocol/Service Multiplexer
 */
#define SIGNAL_MINIMUM_MTU          23
#define SIGNAL_MINIMUM_MPS          23
#define SIGNAL_MAXIMUM_MPS          65533

#define SIGNAL_CREDIT_MINIMUM_MTU   64
#define SIGNAL_CREDIT_MINIMUM_MPS   64
#define SIGNAL_CREDIT_MAXIMUM_MPS   65533

/**
 * @brief   data structure of CoC channel
 */
typedef struct{
    u16 connHandle;
    u16 SPSM;
    u16 srcCID;
    u16 dstCID;
    u16 mtu;
    u16 mps;
    u16 sduLen;
    u16 recvLen;
    u16 sendTotalLen;
    u16 sendOffsetLen;
    u16 recvCredits;
    u16 sendCredits;

    u8* pRxSdu;
    u8* pTxSdu;
}l2cap_coc_cid_t;

/**
 * @brief   data structure of CoC CID
 *          LE Only supported CID 0x0040 to 0x007F
 *          0x0020 to 0x003E LE-U Reserved for future use, 0xFFFF for send reconfigure request
 */
typedef struct{
    u16 connHandle;
    u16 SPSM;
    u16 dstCIDNum;
    u16 dstCID[5];
}l2cap_coc_acl_t;


/**
 * @brief   tx or rx packet, for MTU size
 */
#define COC_MODULE_BUFFER_SIZE(createConnCnt, cocCidCnt, eattCidCnt, mtu)           \
        (\
            (createConnCnt)*sizeof(l2cap_coc_acl_t)  \
            +(cocCidCnt+eattCidCnt)*sizeof(l2cap_coc_cid_t) \
            +(mtu)*(cocCidCnt+2*eattCidCnt)  \
        )

/**
 * @brief   data structure of CoC initial parameter
 */
typedef struct{
    u16 SPSM;   //supported SPSM.
    u16 MTU;    //CID receive MTU.
    u16 createConnCnt;  //supported maximum ACL connect, create COC connect.
    u16 cocCidCnt;  //supported maximum COC CID count.
    u16 eattCidCnt; //supported maximum EATT CID count.
}blc_coc_initParam_t;

/**
 * @brief   This function is used to register CoC module
 * @param   param - CoC initialized parameters
 * @param   pBuffer - CoC buffer
 * @param   buffLen - CoC buffer length
 * @return  state - 0: command succeeded; others: failed
 */

int blc_l2cap_registerCocModule(blc_coc_initParam_t* param, u8 *pBuffer, u16 buffLen);

/**
 * @brief   This function is used to disconnect CoC channel
 * @param   connHandle - connection handle
 * @param   srcCID - source channel identifier
 * @return  BLE_SUCCESS
 */

ble_sts_t blc_l2cap_disconnectCocChannel(u16 connHandle, u16 srcCID);

/**
 * @brief   This function is used to create LE credit based connection
 * @param   connHandle - connection handle
 * @return  BLE_SUCCESS
 */

ble_sts_t blc_l2cap_createLeCreditBasedConnect(u16 connHandle);

/**
 * @brief   This function is used to create credit based connection
 * @param   connHandle - connection handle
 * @param   srcCnt - source channel count
 * @return  BLE_SUCCESS
 */

ble_sts_t blc_l2cap_createCreditBasedConnect(u16 connHandle, u8 srcCnt);

/**
 * @brief   This function is used to send CoC data
 * @param   connHandle - connection handle
 * @param   srcCID - source channel identifier
 * @param   data - data to be sent
 * @param   dataLen - data length
 * @return  BLE_SUCCESS
 */

ble_sts_t blc_l2cap_sendCocData(u16 connHandle, u16 srcCID, u8* data, u16 dataLen);

/**
 * @brief   CoC main loop
 * @param   none
 * @return  none
 */

void blc_l2cap_cocMainLoop(void);


/**
 * @brief   This function is used to send connection parameter update request
 * @param   connHandle - connection handle
 * @param   min_interval - minimum connection interval
 * @param   max_interval - maximum connection interval
 * @param   latency - connection latency
 * @param   timeout - connection timeout
 * @return  BLE_SUCCESS
 */

ble_sts_t blc_signal_sendConnectParameterUpdateReq(u16 connHandle, u16 min_interval, u16 max_interval, u16 latency, u16 timeout);

/**
 * @brief   This function is used to send connection parameter update response
 * @param   connHandle - connection handle
 * @param   identifier - request packet identifier
 * @param   result - connection parameter update result
 * @return  BLE_SUCCESS
 */

ble_sts_t blc_signal_sendConnectParameterUpdateRsp(u16 connHandle, u8 identifier, conn_para_up_rsp result);


/**
 * @brief		This function is used to host set connect parameter process pending in master
 * @param[in]	connHandle - connection handle
 * @param[in]	min_interval - connect interval minimum
 * @param[in]	max_interval - connect interval maximum
 * @param[in]	latency - connect latency
 * @param[in]	timeout - connect timeout
 * @return		none.
 */
void	blm_l2cap_processConnParamUpdatePending(u16 connHandle, u16 min_interval, u16 max_interval, u16 latency, u16 timeout);


















#if (L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN)


#define L2CAP_CHANNEL_MAX_COUNT    64

#define L2CAP_COC_CID_START		   0X0040
#define L2CAP_COC_CID_END		   0X007F


/**
 * @brief		Gets the size of the buffer needed to register the service.
 * @param[in]	srvCount - Maximum number of Services supported.
 * @param[in]	chCount  - Maximum number of Channels supported. (<L2CAP_CHANNEL_MAX_COUNT)
 * @param[in]	mtu      - Maximum Transmission Unit.
 * @return		The size of the buffer needed to register the service.
 */
#define BLC_L2CAP_COC_MTU_NEEDLEN(srvCount,chCount,mtu)   (srvCount*chCount*mtu)
/**
 * @brief		Gets the size of the buffer needed to initialize the service.
 * @param[in]	connCount - Maximum number of connections supported.
 * @param[in]	srvCount  - Maximum number of Services supported.
 * @param[in]	chCount   - Maximum number of Channels supported. (<L2CAP_CHANNEL_MAX_COUNT)
 * @return		The size of the buffer needed to initialize the service.
 */
#define BLC_L2CAP_COC_INIT_NEEDLEN(connCount,srvCount,chCount)  \
			(\
				(connCount)*sizeof(l2cap_coc_handle_t)  \
				+(srvCount)*sizeof(l2cap_coc_service_t) \
				+(chCount)*sizeof(l2cap_coc_channel_t)  \
			)


enum L2CAP_COC_PSM_ENUM{
	L2CAP_COC_SPSM_EATT = 0x0027, //L2CAP_COC_START_CREADIT_CONNECT
	L2CAP_COC_SPSM_OTS  = 0x0025, //L2CAP_COC_START_LE_CREADIT_CONNECT
	//L2CAP_COC_LE_PSM   = L2CAP_COC_SPSM_OTS,
};
enum L2CAP_COC_START_METHOD_ENUM{
	L2CAP_COC_START_CREADIT_CONNECT    = 0, //Send blc_l2cap_sendCreditBasedConnectReq
	L2CAP_COC_START_LE_CREADIT_CONNECT = 1, //Send blc_l2cap_sendLECreditBasedConnectReq
};

/**
 * @brief		The interface type for the Event callback of the COC service.
 * @param[in]	evtID - The event ID. Refer to "gap_event.h".
 * @param[in]	pData - The included data of the event.
 * @param[in]	dataLen - The length of the pData.
 */
typedef int(*blc_l2cap_coc_evtcb)(u16 evtID, u8 *pData, u16 dataLen);
/**
 * @brief		The interface type for the RecvData callback of the COC service.
 * @param[in]	aclHandle - The connection handle
 * @param[in]	scid      - Source Channel ID.
 * @param[in]	dcid      - Destination Channel ID.
 * @param[in]	pData     - The data received.
 * @param[in]	dataLen   - The length of the data received.
 */
typedef int(*blc_l2cap_coc_datacb)(u16 aclHandle, u16 scid, u16 dcid, u16 dataLen, u8 *pData);


typedef struct{
	u8 stat : 4;
	u8 opts : 4;
	u8 chID; //channel identifier
	u8 supChs; //Bit[7~4]-Supported channel count; bit[3~0]-Now channel count.
	u8 nowChs;
	u8 *pBuffer; //Sdu Buffer

	u16 newMtu;
	u16 newMps;
	
	u16 spsm;
	u16 aclHandle; //connect aclHandle	
	
	u16 ownMtu;
	u16 ownMps;
	u16 ownCredits;
	u16 peerMtu;
	u16 peerMps;
	u16 peerCredits;
	
	u16 scid[5];
	u16 dcid[5];

	void *pUsrArg;
	blc_l2cap_coc_evtcb cb;
	blc_l2cap_coc_evtcb evtCb;
	blc_l2cap_coc_datacb dataCb;
}l2cap_coc_service_t;
typedef struct{
	u8  stat;
	u8  srvNum;
	u16 resv;
	
	u16 sduLen;
	u16 recvLen;
	
	u16 recvCredits;
	u16 sendCredits;
	
	u8* pSdu;
}l2cap_coc_channel_t;
typedef struct{
	u8 isUsed;
	u8 isCocPkt; ///Prev Is COC Packet
	u16 aclHandle;
	u16 cocPendLen;
	u16 cocPendScid;
}l2cap_coc_handle_t;



/**
 * @brief		Initialize the COC parameters and set the buffer required for the COC.
 * @param[in]	connCount    - Maximum number of connections supported.
 * @param[in]	serviceCount - Maximum number of Services supported.
 * @param[in]	channelCount - Maximum number of Channels supported. (<L2CAP_CHANNEL_MAX_COUNT)
 * @param[in]	*pBuffer     - The buffer required for the COC.
 * @param[in]	buffLen      - the size of of "pBuffer", Refer to BLC_L2CAP_COC_INIT_NEEDLEN.
 * @return		0: success;
 *             -1: pBuffer is not enough.
 */
int  blc_l2cap_cocInit(u8 connCount, u8 serviceCount, u8 channelCount, u8 *pBuffer, u16 buffLen);

/**
 * @brief		Bind a connection to a COC processing flow.The received data from the COC is not available until the binding is completed.
 * @param[in]	aclHandle - The connection handle to be bound.
 * @return		0: success;
 *             -1: The handle is already bound.
 *             -2: The resource is insufficient. (The maximum number of connections was exceeded)
 */
int  blc_l2cap_cocConnectHandler(u16 aclHandle);
/**
 * @brief		Release the connection from the COC processing flow.
 * @param[in]	aclHandle - The connection handle to be bound.
 * @return		none.
 */
void blc_l2cap_cocDisconnHandler(u16 aclHandle);


/**
 * @brief		Register the COC service into L2CAP. After registration, the corresponding service is activated, and then the user can call "startService" to start the service.
 * @param[in]	spsm - Simplified Protocol/Service Multiplexer. Refer to L2CAP_COC_PSM_ENUM.
 * @param[in]	srvCount - Number of registered services.
 * @param[in]	chCount - Number of Channels per service.
 * @param[in]	mtu     - Maximum Transmission Unit.
 * @param[in]	mps     - Maximum PDU Payload Size.
 * @param[in]	credits - Initial Credits. (>0)
 * @param[in]	pBuffer - A buffer to store data. You can use BLC_L2CAP_COC_MTU_NEEDLEN to calculate the required size.
 * @param[in]	evtCb   - The callback of Event.
 * @param[in]	dataCb  - The callback of RecvData.
 * @return    >=0: Number of services successfully registered;
 *             -1: Incorrect input parameter;
 *             -2: The resource is insufficient;
 *             -3: Unknown Error.
 */
int blc_l2cap_registerService(u16 spsm, u8 srvCount, u8 chCount, u16 mtu, u16 mps, 
	u16 credits, u8 *pBuffer, blc_l2cap_coc_evtcb evtCb, blc_l2cap_coc_datacb dataCb);
/**
 * @brief		Start the COC service. The interface automatically finds the relevant SPSM service, binds it to ConnHandle, and sends the start instruction packet.
 * @param[in]	aclHandle - The connection handle to be bound.
 * @param[in]	spsm      - Simplified Protocol/Service Multiplexer. Refer to L2CAP_COC_PSM_ENUM.
 * @param[in]	method    - Refer to L2CAP_COC_START_METHOD_ENUM.
 * @return		0: success;
 *             -1: The AclHandle is not in a connected state;
 *             -2: There is no available service.
 *             -3: The service's state is error. (eg, Busy.)
 *             -4: There is no available channel. (This interface can be called multiple times so that all Channels are created.)
 *             -5: Failed to push data to Controller.
 */
int blc_l2cap_startService(u16 aclHandle, u16 spsm, u8 method);
/**
 * @brief		Stop the COC service.
 * @param[in]	aclHandle - The connection handle to be bound.
 * @param[in]	spsm      - Simplified Protocol/Service Multiplexer. Refer to L2CAP_COC_PSM_ENUM.
 * @return		0: success;
 *             -1: The Service is not started;
 *             -2: The Service's state is error. (eg, Busy.)
 */
int blc_l2cap_stopService(u16 aclHandle, u16 spsm);
/**
 * @brief		Disconnects the specified channel. Once disconnected, the user can also call "startService" to recreate it.
 * @param[in]	aclHandle - The connection handle.
 * @param[in]	scid      - Source Channel ID. (local CID)
 * @return		0: success;
 *             -1: The SCID is no available;
 *             -2: The Service is not started;
 *             -3: The Channel's state is error;
 *             -4: Failed to push data to Controller.
 */
int blc_l2cap_disconnScid(u16 aclHandle, u16 scid);

/**
 * @brief		Reconfigure the MTU and MPS of the COC service. This interface can only be invoked when the service is idle.
 *              It is not stable at the moment, please use with caution!
 * @param[in]	aclHandle - The connection handle.
 * @param[in]	srvNum    - Service ID.
 * @param[in]	mtu     - Maximum Transmission Unit.
 * @param[in]	mps     - Maximum PDU Payload Size.
 * @return		0: success;
 *             -1: The srvNum is no available;
 *             -2: The Service's state is error;
 *             >0: Failed to push data to Controller. Refer to ble_sts_t.
 */
int blc_l2cap_reconfigService(u8 srvNum, u16 mtu, u16 mps);
/**
 * @brief		Get the SPSM of the corresponding service.
 * @param[in]	aclHandle - The connection handle.
 * @param[in]	srvNum    - Service ID.
 * @return		SPSM. (0-fail)
 */
u16 blc_l2cap_getSpsmBySrvnum(u8 srvNum);
/**
 * @brief		Get the "SrvNum" by "ConnHandle" and "SPSM".
 * @param[in]	aclHandle - The connection handle.
 * @param[in]	spsm      - Simplified Protocol/Service Multiplexer. Refer to L2CAP_COC_PSM_ENUM.
 * @return		SrvNum. (0-fail)
 */
u8  blc_l2cap_getSrvnumBySpsm(u16 aclHandle, u16 spsm);


/**
 * @brief		Sends data to the specified channel.
 * @param[in]	aclHandle - The connection handle.
 * @param[in]	scid      - Source Channel ID. (local CID)
 * @param[in]	pHead     - The first part of the data to be sent.
 * @param[in]	headLen   - The length of the data's first part.
 * @param[in]	pData     - The second part of the data to be sent.
 * @param[in]	dataLen   - The length of the data's second part.
 * @return		0: success;
 *             -1: The parameter is error.
 *             -2: The SCID is no available.
 *             >0: Failed to push data to Controller. Refer to ble_sts_t.
 */
int blc_l2cap_cocSendData(u16 aclHandle, u16 scid, u8 *pHead, u8 headLen, u8 *pData, u16 dataLen);



#endif //#if (L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN)

#endif /* L2CAP_COC_H_ */
