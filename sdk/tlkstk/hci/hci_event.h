/********************************************************************************************************
 * @file	hci_event.h
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
#ifndef HCI_EVENT_H_
#define HCI_EVENT_H_


/**
 *  @brief  Definition for general HCI event packet
 */
typedef struct {
	uint8_t	 	   type;
	uint8_t         eventCode;
	uint8_t         paraLen;
	uint8_t         parameters[1];
} hci_event_t;


typedef struct {
	uint8_t         numHciCmds;
	uint8_t         opCode_OCF;
	uint8_t		   opCode_OGF;
	uint8_t         returnParas[1];
} hci_cmdCompleteEvt_evtParam_t;


typedef struct {
	uint8_t         status;
	uint8_t         numHciCmds;
	uint8_t         opCode_OCF;
	uint8_t		   opCode_OGF;
} hci_cmdStatusEvt_evtParam_t;

typedef struct{
	uint16_t 		connHandle;
	uint16_t 		numOfCmpPkts;
}numCmpPktParamRet_t;

typedef struct {
	uint8_t         numHandles;
	numCmpPktParamRet_t retParams[1];//TODO
} hci_numOfCmpPktEvt_t;

typedef struct{
	uint8_t  status;
	uint16_t connHandle;
	uint8_t  verNr;
	uint16_t compId;
	uint16_t subVerNr;
}hci_readRemVerInfoCmplEvt_t;

typedef struct {
	uint8_t		    type;
	uint8_t         eventCode;
	uint8_t         paraLen;
	uint8_t         subEventCode;
	uint8_t         parameters[1];
} hci_le_metaEvt_t;



/**
 *  @brief  Event Parameters for "7.7.5 Disconnection Complete event"
 */
typedef struct {
	uint8_t	 	    status;
	uint16_t	    connHandle;
	uint8_t		    reason;
} event_disconnection_t;

typedef struct {
	uint8_t         status;
	uint16_t        connHandle;
	uint8_t         reason;
} hci_disconnectionCompleteEvt_t;

/**
 *  @brief  Event Parameters for "7.7.8 Encryption Change event"
 */
typedef struct {
	uint8_t	        status;
	uint16_t	    handle;
	uint8_t         enc_enable;
} event_enc_change_t;

typedef struct {
	uint8_t         status;
	uint16_t        connHandle;
	uint8_t         encryption_enable;
} hci_le_encryptEnableEvt_t;

/**
 *  @brief  Event Parameters for "7.7.39 Encryption Key Refresh Complete event"
 */
typedef struct {
	uint8_t	        status;
	uint16_t	    handle;
} event_enc_refresh_t;

typedef struct {
	uint8_t         status;
	uint16_t        connHandle;
} hci_le_encryptKeyRefreshEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.1 LE Connection Complete event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint8_t         status;
	uint16_t        connHandle;
	uint8_t         role;
	uint8_t         peerAddrType;
	uint8_t         peerAddr[6];
	uint16_t        connInterval;
	uint16_t        slaveLatency;
	uint16_t        supervisionTimeout;
	uint8_t         masterClkAccuracy;
} hci_le_connectionCompleteEvt_t;


/* ACL Connection Role */
typedef enum {
	LL_ROLE_MASTER 	= 0,
	LL_ROLE_SLAVE 	= 1,
} acl_connection_role_t;

/* compatible with previous released SDK */
#define 	ACL_ROLE_CENTRAL    LL_ROLE_MASTER
#define 	ACL_ROLE_PERIPHERAL LL_ROLE_SLAVE

/**
 *  @brief  Event Parameters for "7.7.65.2 LE Advertising Report event"
 */
typedef struct {
	uint8_t	        subcode;
	uint8_t	        nreport;
	uint8_t	        event_type;
	uint8_t	        adr_type;
	uint8_t	        mac[6];
	uint8_t	        len;
	uint8_t	        data[1];
} event_adv_report_t;

/* Advertise report event type */
typedef enum {
	ADV_REPORT_EVENT_TYPE_ADV_IND 		= 0x00,
	ADV_REPORT_EVENT_TYPE_DIRECT_IND 	= 0x01,
	ADV_REPORT_EVENT_TYPE_SCAN_IND 		= 0x02,
	ADV_REPORT_EVENT_TYPE_NONCONN_IND 	= 0x03,
	ADV_REPORT_EVENT_TYPE_SCAN_RSP 		= 0x04,
} advReportEventType_t;



/**
 *  @brief  Event Parameters for "7.7.65.3 LE Connection Update Complete event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint8_t         status;
	uint16_t        connHandle;
	uint16_t        connInterval;
	uint16_t        connLatency;
	uint16_t        supervisionTimeout;
} hci_le_connectionUpdateCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.4 LE Read Remote Features Complete event"
 */
#define LL_FEATURE_SIZE                                      			8
typedef struct {
	uint8_t         subEventCode;
	uint8_t         status;
	uint16_t        connHandle;
	uint8_t		    feature[LL_FEATURE_SIZE];
} hci_le_readRemoteFeaturesCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.5 LE Long Term Key Request event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint16_t        connHandle;
	uint8_t         random[8];
	uint16_t        ediv;
} hci_le_longTermKeyRequestEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.6 LE Remote Connection Parameter Request event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint16_t        connHandle;
	uint16_t        IntervalMin;
	uint16_t        IntervalMax;
	uint16_t		latency;
	uint16_t		timeout;
} hci_le_remoteConnParamReqEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.7 LE Data Length Change event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint16_t        connHandle;  //no aligned, can not be used as pointer
	uint16_t  	    maxTxOct;
	uint16_t		maxTxtime;
	uint16_t  	    maxRxOct;
	uint16_t		maxRxtime;
} hci_le_dataLengthChangeEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.8 LE Read Local P-256 Public Key Complete event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint8_t         status;
	uint8_t         localP256Key[64];
} hci_le_readLocalP256KeyCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.9 LE Generate DHKey Complete event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint8_t         status;
	uint8_t         DHKey[32];
} hci_le_generateDHKeyCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.10 LE Enhanced Connection Complete event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint8_t         status;
	uint16_t		connHandle;
	uint8_t		    role;
	uint8_t		    PeerAddrType;
	uint8_t         PeerAddr[6];
	uint8_t         localRslvPrivAddr[6];
	uint8_t         Peer_RslvPrivAddr[6];
	uint16_t        connInterval;
	uint16_t        connLatency;
	uint16_t        superTimeout;
	uint8_t         masterClkAccuracy;
} hci_le_enhancedConnCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.12 LE PHY Update Complete event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint8_t         status;
	uint16_t        connHandle;
	uint8_t 		tx_phy;
	uint8_t		    rx_phy;
} hci_le_phyUpdateCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.13 LE Extended Advertising Report event"
 */
typedef struct{
	uint16_t		event_type;			// 2
	uint8_t		    address_type;       // 1
	uint8_t		    address[6];			// 6
	uint8_t		    primary_phy;		// 1
	uint8_t		    secondary_phy;		// 1
	uint8_t		    advertising_sid;	// 1
	uint8_t		    tx_power;			// 1
	uint8_t		    rssi;				// 1
	uint16_t		perd_adv_inter;		// 2   Periodic_Advertising_Interval
	uint8_t		    direct_address_type;// 1
	uint8_t		    direct_address[6];	// 6
	uint8_t		    data_length;		// 1
	uint8_t		    data[1];
} extAdvEvt_info_t;

#define EXTADV_INFO_LENGTH				24	 //byte number from "event_type" to "data_length"
#define EXTADV_RPT_DATA_LEN_MAX			229  //253 - 24 = 229

typedef struct{
	uint8_t			subEventCode;
	uint8_t			num_reports;
	uint8_t			advEvtInfo[1];
} hci_le_extAdvReportEvt_t;


/* Extended Advertising Report Event Event_Type mask*/
typedef enum{
	EXTADV_RPT_EVT_MASK_CONNECTABLE					=	BIT(0),
	EXTADV_RPT_EVT_MASK_SCANNABLE     				=	BIT(1),
	EXTADV_RPT_EVT_MASK_DIRECTED  				    =   BIT(2),
	EXTADV_RPT_EVT_MASK_SCAN_RESPONSE     			=	BIT(3),
	EXTADV_RPT_EVT_MASK_LEGACY          			=	BIT(4),

	EXTADV_RPT_DATA_COMPLETE     					=	0x00,
	EXTADV_RPT_DATA_INCOMPLETE_MORE_TO_COME         =	0x20,
	EXTADV_RPT_DATA_INCOMPLETE_TRUNCATED          	=	0x40,
	EXTADV_RPT_DATA_RFU				 	          	=	0x60,

	EXTADV_RPT_EVTTYPE_MASK		         			=	0x1F,
	EXTADV_RPT_DATA_STATUS_MASK		         		=	0x60,
}extAdvRptEvtMask_t;


/* Extended Advertising Report Event_Type */
typedef enum{
	//Legacy
	EXTADV_RPT_EVTTYPE_LEGACY_ADV_IND 				       		= 0x0013,		//  0001 0011'b
	EXTADV_RPT_EVTTYPE_LEGACY_ADV_DIRECT_IND			       	= 0x0015,		//  0001 0101'b
	EXTADV_RPT_EVTTYPE_LEGACY_ADV_SCAN_IND	 					= 0x0012,		//  0001 0010'b
	EXTADV_RPT_EVTTYPE_LEGACY_ADV_NONCONN_IND					= 0x0010,		//  0001 0000'b
	EXTADV_RPT_EVTTYPE_LEGACY_SCAN_RSP_2_ADV_IND				= 0x001B,		//  0001 1011'b
	EXTADV_RPT_EVTTYPE_LEGACY_SCAN_RSP_2_ADV_SCAN_IND			= 0x001A,		//  0001 1010'b

	//Extended
	EXTADV_RPT_EVTTYPE_EXT_NON_CONN_NON_SCAN_UNDIRECTED    	    = 0x0000,	// Extended, Non_Connectable Non_Scannable Undirected
	EXTADV_RPT_EVTTYPE_EXT_CONNECTABLE_UNDIRECTED       		= 0x0001,	// Extended, Connectable Undirected
	EXTADV_RPT_EVTTYPE_EXT_SCANNABLE_UNDIRECTED					= 0x0002,	// Extended, Scannable Undirected
	EXTADV_RPT_EVTTYPE_EXT_NON_CONN_NON_SCAN_DIRECTED			= 0x0004,	// Extended, Non_Connectable Non_Scannable Directed
	EXTADV_RPT_EVTTYPE_EXT_CONNECTABLE_DIRECTED			       	= 0x0005,	// Extended, Connectable Directed
	EXTADV_RPT_EVTTYPE_EXT_SCANNABLE_DIRECTED					= 0x0006,	// Extended, Scannable Directed
	EXTADV_RPT_EVTTYPE_EXT_SCAN_RESPONSE						= 0x0008,	// Extended, AUX_SCAN_RESPONSE
}extAdvRptEvtType_t;  //extended advertising report event type




/* Address type */
typedef enum{
	EXTADV_RPT_PUBLIC_DEVICE_ADDRESS				=	0x00,
	EXTADV_RPT_RANDOM_DEVICE_ADDRESS     			=	0x01,
	EXTADV_RPT_PUBLIC_IDENTITY_ADDRESS  			=   0x02,
	EXTADV_RPT_RANDOM_IDENTITY_ADDRESS     			=	0x03,
	EXTADV_RPT_ANONYMOUS_ADV     					=	0xFF,
}ext_adv_adr_type_t;


#define	PERIODIC_ADV_INTER_NO_PERIODIC_ADV							0
#define	SECONDARY_PHY_NO_PACKET_ON_SECONDARY_ADV_CHN				0
#define	ADVERTISING_SID_NO_ADI_FIELD								0xFF
#define TX_POWER_INFO_NOT_AVAILABLE									127



/**
 *  @brief  Event Parameters for "7.7.65.14 LE Periodic Advertising Sync Established event"
 */
typedef struct {
	uint8_t			subEventCode;
	uint8_t			status;
	uint16_t		syncHandle;
//	extadv_id_t	adverting_id;
	uint8_t			advSID;
	uint8_t			advAddrType;
	uint8_t			advAddr[6];
	uint8_t			advPHY;
	uint16_t		perdAdvItvl;
	uint8_t			advClkAccuracy;
}hci_le_periodicAdvSyncEstablishedEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.15 LE Periodic Advertising Report event"
 */
typedef struct {
	uint8_t			subEventCode;
	uint16_t		syncHandle;
	uint8_t			txPower;
	uint8_t			RSSI;
	uint8_t			cteType;
	uint8_t			dataStatus;
	uint8_t			dataLength;  // 0 to 247 Length of the Data field
	uint8_t			data[1];
} hci_le_periodicAdvReportEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.16 LE Periodic Advertising Sync Lost event"
 */
typedef struct {
	uint8_t			subEventCode;
	uint16_t		syncHandle;
} hci_le_periodicAdvSyncLostEvt_t;



typedef struct {
	//TODO
} hci_le_scanTimeoutEvt_t;




/**
 *  @brief  Event Parameters for "7.7.65.18 LE Advertising Set Terminated event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint8_t         status;
	uint8_t         advHandle;
	uint8_t		    num_compExtAdvEvt;
	uint16_t		connHandle;
} hci_le_advSetTerminatedEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.19 LE Scan Request Received event"
 */
typedef struct {
	//TODO
} hci_le_scanReqRcvdEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.20 LE Channel Selection Algorithm event"
 */
typedef struct {
	uint8_t         subEventCode;
	uint8_t		    channel_selection_algorithm;
	uint16_t        connHandle;

} hci_le_chnSelectAlgorithmEvt_t;






/**
 *  @brief  Event Parameters for "7.7.65.25 LE CIS Established event"
 */
typedef struct {
	uint8_t        	subEventCode;
	uint8_t        	status;
	uint16_t		cisHandle;
	uint8_t         cigSyncDly[3];
	uint8_t         cisSyncDly[3];
	uint8_t         transLaty_m2s[3];
	uint8_t         transLaty_s2m[3];
	uint8_t			phy_m2s;
	uint8_t			phy_s2m;
	uint8_t			nse;
	uint8_t			bn_m2s;
	uint8_t			bn_s2m;
	uint8_t			ft_m2s;
	uint8_t			ft_s2m;
	uint16_t		maxPDU_m2s;
	uint16_t		maxPDU_s2m;
	uint16_t		isoIntvl;
} hci_le_cisEstablishedEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.26 LE CIS Request event"
 */
typedef struct {
	uint8_t        	subEventCode;
	uint16_t        aclHandle;
	uint16_t        cisHandle;
	uint8_t			cigId;
	uint8_t			cisId;
} hci_le_cisReqEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.27 LE Create BIG Complete event"
 */
typedef struct {
	uint8_t        	subEventCode;
	uint8_t        	status;
	uint8_t			bigHandle;
	uint8_t        	bigSyncDly[3];
	uint8_t         transLatyBig[3];
	uint8_t			phy;
	uint8_t			nse;
	uint8_t			bn;
	uint8_t			pto;
	uint8_t			irc;
	uint16_t		maxPDU;
	uint16_t		isoIntvl;
	uint8_t			numBis;
	uint16_t		bisHandles[1];//LL_BIS_IN_PER_BIG_BCST_NUM_MAX];
} hci_le_createBigCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.28 LE Terminate BIG Complete event"
 */
typedef struct {
	uint8_t        	subEventCode;
	uint8_t			bigHandle;
	uint8_t			reason;
} hci_le_terminateBigCompleteEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.20 LE Channel Selection Algorithm event"
 */
typedef struct {
	uint8_t        	subEventCode;
	uint8_t        	status;
	uint8_t			bigHandle;
	uint8_t         transLatyBig[3];
	uint8_t			nse;
	uint8_t			bn;
	uint8_t			pto;
	uint8_t			irc;
	uint16_t		maxPDU;
	uint16_t		isoIntvl;
	uint8_t			numBis;
	uint16_t        bisHandles[1];//BIS_IN_PER_BIG_SYNC_NUM_MAX];
} hci_le_bigSyncEstablishedEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.29 LE BIG Sync Established event"
 */
typedef struct {
	uint8_t        	subEventCode;
	uint8_t			bigHandle;
	uint8_t			reason;
} hci_le_bigSyncLostEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.30 LE BIG Sync Lost event"
 */


/**
 *  @brief  Event Parameters for "7.7.65.34 LE BIGInfo Advertising Report event"
 */
typedef struct {
	uint8_t			subEventCode;
	uint16_t		syncHandle;
	uint8_t			numBis;
	uint8_t			nse;
	uint16_t		IsoItvl; //in units of 1.25 ms.
	uint8_t			bn;
	uint8_t			pto;
	uint8_t			irc;
	uint16_t		maxPdu;
	uint8_t			sduItvl[3];
	uint16_t		maxSdu;
	uint8_t			phy;
	uint8_t			framing;
	uint8_t			enc;
} hci_le_bigInfoAdvReportEvt_t;



#endif /* HCI_EVENT_H_ */





