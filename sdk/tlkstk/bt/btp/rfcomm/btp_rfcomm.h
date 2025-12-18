/********************************************************************************************************
 * @file	btp_rfcomm.h
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
#ifndef BTP_RFCOMM_H
#define BTP_RFCOMM_H


// DLC parameter negotiation (PN)
// Power Saving Control (PSC)
// Multiplexer close down (CLD)
// Test Command (Test)
// Flow Control On Command (FCon)
// Flow Control Off Command (FCoff)
// Modem Status Command (MSC)
// Non Supported Command Response (NSC)
// Remote Port Negotiation Command (RPN)
// Remote Line Status Command(RLS)
// Service Negotiation Command (SNC)


#define RFCOMM_DEFAULT_CREDITS    0
#define RFCOMM_RX_CREDITS_MAX     100


#define RFCOMM_SABM               0x2f
#define RFCOMM_DISC               0x43
#define RFCOMM_UA                 0x63
#define RFCOMM_DM                 0x0f
#define RFCOMM_UIH                0xef

#define RFCOMM_TEST               0x08
#define RFCOMM_FCON               0x28
#define RFCOMM_FCOFF              0x18
#define RFCOMM_MSC                0x38
#define RFCOMM_RPN                0x24
#define RFCOMM_RLS                0x14
#define RFCOMM_PN                 0x20
#define RFCOMM_NSC                0x04

#define RFCOMM_V24_FC             0x02
#define RFCOMM_V24_RTC            0x04
#define RFCOMM_V24_RTR            0x08
#define RFCOMM_V24_IC             0x40
#define RFCOMM_V24_DV             0x80

#define RFCOMM_RPN_BR_2400        0x0
#define RFCOMM_RPN_BR_4800        0x1
#define RFCOMM_RPN_BR_7200        0x2
#define RFCOMM_RPN_BR_9600        0x3
#define RFCOMM_RPN_BR_19200       0x4
#define RFCOMM_RPN_BR_38400       0x5
#define RFCOMM_RPN_BR_57600       0x6
#define RFCOMM_RPN_BR_115200      0x7
#define RFCOMM_RPN_BR_230400      0x8

#define RFCOMM_RPN_DATA_5         0x0
#define RFCOMM_RPN_DATA_6         0x1
#define RFCOMM_RPN_DATA_7         0x2
#define RFCOMM_RPN_DATA_8         0x3

#define RFCOMM_RPN_STOP_1         0
#define RFCOMM_RPN_STOP_15        1

#define RFCOMM_RPN_PARITY_NONE    0x0
#define RFCOMM_RPN_PARITY_ODD     0x1
#define RFCOMM_RPN_PARITY_EVEN    0x3
#define RFCOMM_RPN_PARITY_MARK    0x5
#define RFCOMM_RPN_PARITY_SPACE   0x7

#define RFCOMM_RPN_FLOW_NONE      0x00

#define RFCOMM_RPN_XON_CHAR       0x11
#define RFCOMM_RPN_XOFF_CHAR      0x13

#define RFCOMM_RPN_PM_BITRATE     0x0001
#define RFCOMM_RPN_PM_DATA        0x0002
#define RFCOMM_RPN_PM_STOP        0x0004
#define RFCOMM_RPN_PM_PARITY      0x0008
#define RFCOMM_RPN_PM_PARITY_TYPE 0x0010
#define RFCOMM_RPN_PM_XON         0x0020
#define RFCOMM_RPN_PM_XOFF        0x0040
#define RFCOMM_RPN_PM_FLOW        0x3F00

#define RFCOMM_RPN_PM_ALL         0x3F7F


typedef struct rfcomm_hdr {
    uint08 addr;
    uint08 ctrl;
    uint08 len; // Actual size can be 2 bytes
} __attribute__((__packed__)) rfcomm_hdr_t;

typedef struct rfcomm_cmd {
    uint08 addr;
    uint08 ctrl;
    uint08 len;
    uint08 fcs;
} __attribute__((__packed__)) rfcomm_cmd_t;

typedef struct rfcomm_mcc {
    uint08 type;
    uint08 len;
} __attribute__((__packed__)) rfcomm_mcc_t;

typedef struct rfcomm_pn {
    uint08 dlci;
    uint08 flow_ctrl;
    uint08 priority;
    uint08 ack_timer;
    uint16 mtu;
    uint08 max_retrans;
    uint08 credits;
} __attribute__((__packed__)) rfcomm_pn_t;

typedef struct rfcomm_rpn {
    uint08 dlci;
    uint08 bit_rate;
    uint08 line_settings;
    uint08 flow_ctrl;
    uint08 xon_char;
    uint08 xoff_char;
    uint16 param_mask;
} __attribute__((__packed__)) rfcomm_rpn_t;

typedef struct rfcomm_rls {
    uint08 dlci;
    uint08 status;
} __attribute__((__packed__)) rfcomm_rls_t;

typedef struct rfcomm_msc {
    uint08 dlci;
    uint08 v24_sig;
} __attribute__((__packed__)) rfcomm_msc_t;


enum rfcomm_session_state_enum {
    SESSION_CLOSE,        /*l2cap connection closed, wait for opening, and then can send out sabm request*/
    SESSION_CONNECT,      /*l2cap channel created*/
    SESSION_WAITING_OPEN, /*has sent sabm of dlci 0, wait ack*/
    SESSION_OPEN,         /* rfcomm session created, means dlci 0 created ok */
    SESSION_WAITING_DISCON
};
enum rfcomm_dlc_state_enum {
    DLC_CLOSE,
    DLC_CONFIG,     /*in dlc parameter config process*/
    DLC_CONNECTING, /* config passed, then send sabm,waiting for ack */
    DLC_OPEN,
    DLC_DISCONNECT  /*in dlc disconnection process*/
};


typedef enum {
    BTP_RFCOMM_EVTID_NONE = 0,
    BTP_RFCOMM_EVTID_REQUEST,
    BTP_RFCOMM_EVTID_CONNECT,
    BTP_RFCOMM_EVTID_DISCONN,
} BTP_RFCOMM_EVTID_ENUM;
typedef enum {
    BTP_RFCOMM_SRVID_NONE = 0,
    BTP_RFCOMM_SRVID_IAP,
    BTP_RFCOMM_SRVID_SPP,
    BTP_RFCOMM_SRVID_MAP,
    BTP_RFCOMM_SRVID_PBAPC,
    BTP_RFCOMM_SRVID_PBAPS,
    BTP_RFCOMM_SRVID_HFPHF,
    BTP_RFCOMM_SRVID_HFPAG,
    BTP_RFCOMM_SRVID_SPP_USER,
} BTP_RECOMM_SRVID_ENUM; // Service ID

typedef struct {
    uint08 status;
    uint08 reason;
    uint08 active;
} btp_rfcomm_connectEvt_t;
typedef struct {
    uint08 reason;
} btp_rfcomm_disconnEvt_t;


typedef int (*BtpRfcommEventCB)(uint08 evtID, uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
typedef void (*BtpRfcommRdataCB)(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
typedef void (*BtpRfcommFlowIndCB)(uint08 aclHandle, uint16 rfcHandle, uint08 flowNumb);


typedef enum {
    BTP_RFCOMM_SESSION_STAGE_NONE = 0,
    // TLK_STATE_CONNING
    BTP_RFCOMM_SESSION_CONNING_STAGE_WAIT_L2CAP = 1,
    BTP_RFCOMM_SESSION_CONNING_STAGE_SEND_SARM_CMD,
    BTP_RFCOMM_SESSION_CONNING_STAGE_WAIT_SARM_RSP,
    BTP_RFCOMM_SESSION_CONNING_STAGE_WAIT_SARM_CMD,
    // TLK_STATE_CONNECT
    BTP_RFCOMM_SESSION_CONNECT_STAGE_SEND_SARM_RSP = 1,
    // TLK_STATE_DISCING
    BTP_RFCOMM_SESSION_DISCING_STAGE_WAIT_IDLE = 1,
    BTP_RFCOMM_SESSION_DISCING_STAGE_DISC_L2CAP,
    BTP_RFCOMM_SESSION_DISCING_STAGE_WAIT_L2CAP,
} BTP_RFCOMM_SESSION_ENUM;

typedef enum {
    BTP_RFCOMM_CHANNEL_STAGE_NONE = 0,
    // TLK_STATE_CONNING
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_SEND_PN_CMD = 1, // PN: DLC parameter negotiation
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_WAIT_PN_RSP,
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_WAIT_PN_CMD,
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_SEND_PN_RSP,
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_SEND_CONN_CMD, // MS: Modem Status
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_WAIT_CONN_RSP,
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_WAIT_CONN_CMD,
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_SEND_CONN_RSP,
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_SEND_MS_CMD, // MS: Modem Status
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_WAIT_MS_RSP,
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_WAIT_MS_CMD,
    BTP_RFCOMM_CHANNEL_CONNING_STAGE_SEND_MS_RSP,
    // TLK_STATE_CONNECT
    BTP_RFCOMM_CHANNEL_CONNECT_STAGE_SEND_CREDIT = 1,
    // TLK_STATE_DISCING
    BTP_RFCOMM_CHANNEL_DISCING_STAGE_SEND_SARM_CMD = 1,
    BTP_RFCOMM_CHANNEL_DISCING_STAGE_WAIT_SARM_RSP,


} BTP_RFCOMM_CHANNEL_ENUM;

typedef enum {
    BTP_RFCOMM_MSFLAG_NONE     = 0,
    BTP_RFCOMM_MSFLAG_SEND_CMD = 0x01,
    BTP_RFCOMM_MSFLAG_WAIT_CMD = 0x02,
    BTP_RFCOMM_MSFLAG_SEND_RSP = 0x04,
    BTP_RFCOMM_MSFLAG_WAIT_RSP = 0x08,
} BTP_RFCOMM_MSFLAG_ENUM;


typedef struct { // 16Byte
    uint32 serviceID;
    BtpRfcommEventCB eventCB;
    BtpRfcommRdataCB rdataCB;
    BtpRfcommFlowIndCB flowIndCB;
} btp_rfcomm_service_t;

typedef struct { // 44
    uint08 state;
    uint08 stage;
    uint16 chnID;

    uint08 msFlag;
    uint08 active;
    uint16 timeout;

    uint08 reserve01;
    uint08 rfcHandle;
    uint16 aclHandle;

    uint08 sessionID;
    uint08 serviceID;
    uint08 dlci;
    uint08 addr;

    uint08 priority;
    uint08 v24_sig;
    uint08 cfc;        /* 0: no flow control; other: the credits we give remote;*/
    uint08 rx_credits; /*the remote device's tx credits now*/
    uint08 tx_credits; /*our tx credits*/
    uint08 cur_session;
    uint16 mtuSize;
    tlkapi_timer_t timer; // 20
} btp_rfcomm_channel_t;
typedef struct {          // 12+20=32
    uint08 state;
    uint08 stage;
    uint16 chnID;
    uint16 timeout;
    uint16 aclHandle;
    uint08 sessionID;
    uint08 initiator;
    uint16 reserve01;
    tlkapi_timer_t timer;
} btp_rfcomm_session_t;
typedef struct {
    uint08 chnID;
    uint08 srvID;
    uint16 aclHandle;
} btp_rfcomm_chndict_t;
typedef struct {
    uint08 serviceNumb;
    uint08 sessionNumb;
    uint08 channelNumb;
    uint08 chndictNumb;
    btp_rfcomm_service_t *service; 
    btp_rfcomm_session_t *session; 
    btp_rfcomm_channel_t *channel; 
    btp_rfcomm_chndict_t *chndict;
} btp_rfcomm_ctrl_t;


/******************************************************************************
 * Function: Rfcomm init interface
 * Descript: This interface be used by user to initial the Rfcomm resource
 *           of client/server before create a connection between the entity.
 * Params:
 * 		@srvNumb[IN]:  refer to TLK_BT_RFCOMM_SERVICE_MAX_NUMB
 * 		@sesNumb[IN]:  refer to TLK_BT_RFCOMM_SESSION_MAX_NUMB
 * 		@chnNumb[IN]:  refer to TLK_BT_RFCOMM_CHANNEL_MAX_NUMB
 * 		@dictNumb[IN]: refer to TLK_BT_RFCOMM_CHNDICT_MAX_NUMB
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value(-TLK_ENOMEM) is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_init(uint08 srvNumb, uint08 sesNumb, uint08 chnNumb, uint08 dictNumb);

/******************************************************************************
 * Function: RFOCMM Deinit interface
 * Descript: This interface be used by User to deinitial rfcomm resource.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *******************************************************************************/
int btp_rfcomm_deinit(void);

/******************************************************************************
 * Function: btp_rfcomm_getMemLen
 * Descript: rfcomm get the size of the required number of nodes interface.
 * Params:
 * 		@srvNumb[IN]:  refer to TLK_BT_RFCOMM_SERVICE_MAX_NUMB
 * 		@sesNumb[IN]:  refer to TLK_BT_RFCOMM_SESSION_MAX_NUMB
 * 		@chnNumb[IN]:  refer to TLK_BT_RFCOMM_CHANNEL_MAX_NUMB
 * 		@dictNumb[IN]: refer to TLK_BT_RFCOMM_CHNDICT_MAX_NUMB
 * Return: Actual node memory size required(unit: byte).
 *******************************************************************************/
int btp_rfcomm_getMemLen(uint08 srvNumb, uint08 sesNumb, uint08 chnNumb, uint08 dictNumb);

/******************************************************************************
 * Function: Rfcomm Connect interface
 * Descript: This interface be used by profile or user to setup a connection
 *           between the entity.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_connect(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm Disconnect interface
 * Descript: This interface be used by profile or user to disconnect a connection
 *           between the entity.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_disconn(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm Destroy interface
 * Descript: This interface be used by profile or user to release rfcomm resource
 *           which indicate by aclHandle.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return:
 *******************************************************************************/
void btp_rfcomm_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm Destroy Channel Dict interface
 * Descript: This interface be used by profile or user to release rfcomm resource
 *           which indicate by aclHandle.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return:
 *******************************************************************************/
void btp_rfcomm_destroyChndict(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm Destroy Session interface
 * Descript: This interface be used by profile or user to destroy rfcomm session.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return:
 *******************************************************************************/
void btp_rfcomm_destroySession(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm Destroy Channel interface
 * Descript: This interface be used by profile or user to destroy rfcomm channel.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return:
 *******************************************************************************/
void btp_rfcomm_destroyChannel(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm Destroy Channel interface
 * Descript: This interface be used by profile or user to destroy rfcomm channel.
 * Params:
 *         @rfcHandle[IN]--The rfcomm handle.
 * Return:
 *******************************************************************************/
void btp_rfcomm_destroyChannelByHandle(uint08 rfcHandle);

void btp_rfcomm_destroyChannelByService(uint16 aclHandle, uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm connect Session interface
 * Descript: This interface be used by profile or user to create a rfcomm session.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_connectSession(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm Disconnect Session interface
 * Descript: This interface be used by profile or user to disconnect rfcomm
 *           session.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_disconnSession(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm connect Rfcomm Channel interface
 * Descript: This interface be used by profile or user to create rfcomm channel.
 * Params:
 *         @serviceID[IN]--The Service Id
 *         @channelID[IN]--The channel id.
 *         @serviceID[IN]--The service id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_connectChannel(uint16 aclHandle, uint08 channel, uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm disconnect channel interface
 * Descript: This interface be used by profile or user to disconnect rfcomm channel.
 * Params:
 *         @rfcHandle[IN]--The rfcomm handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_disconnChannel(uint08 rfcHandle);
int btp_rfcomm_disconnChannelByService(uint16 aclHandle, uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm add Channel Info interface
 * Descript: This interface be used by profile or user to add Channel info into rfcomm.
 * Params:
 *         @serviceID[IN]--The Service Id
 *         @channelID[IN]--The channel id.
 *         @serviceID[IN]--The service id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_appendChnDict(uint16 aclHandle, uint08 channelID, uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm Remove Channel Dict interface
 * Descript: This interface be used by profile or user to release rfcomm resource
 *           which indicate by aclHandle.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 *         @channelID[IN]--The channel id.
 *         @serviceID[IN]--The service id.
 * Return:
 *******************************************************************************/
void btp_rfcomm_removeChnDict(uint16 aclHandle, uint08 channelID, uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm add Service interface
 * Descript: This interface be used by profile or user to add service into rfcomm.
 * Params:
 *         @serviceID[IN]--The Service Id
 *         @eventCB[IN]--The event callback.
 *         @rdataCB[IN]--The rfcomm data callback.
 *         @flowIndCB[IN]--The rfcomm indicate callback.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_appendService(uint08 serviceID,
                             BtpRfcommEventCB eventCB,
                             BtpRfcommRdataCB rdataCB,
                             BtpRfcommFlowIndCB flowIndCB);

/******************************************************************************
 * Function: Rfcomm Remove Service interface
 * Descript: This interface be used by profile or user to remove rfcomm service
 * Params:
 *         @serviceID[IN]--The Service id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_removeService(uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm Get Idle Service Count interface
 * Descript: This interface be used by profile or user to get rfcomm
 *           idle service count.
 * Params:
 * Return:The Idle Service count.
 *******************************************************************************/
uint08 btp_rfcomm_getIdleServiceCount(void);

/******************************************************************************
 * Function: Rfcomm Get Idle Session Count interface
 * Descript: This interface be used by profile or user to get rfcomm
 *           idle session count.
 * Params:
 * Return:The Used Session count.
 *******************************************************************************/
uint08 btp_rfcomm_getIdleSessionCount(void);

/******************************************************************************
 * Function: Rfcomm Get Used Session Count interface
 * Descript: This interface be used by profile or user to get rfcomm
 *           used session count.
 * Params:
 * Return:The Used Session count.
 *******************************************************************************/
uint08 btp_rfcomm_getUsedSessionCount(void);

/******************************************************************************
 * Function: Rfcomm Get Connect Session Count interface
 * Descript: This interface be used by profile or user to get rfcomm
 *           connect session count.
 * Params:
 * Return:The Connect Session count.
 *******************************************************************************/
uint08 btp_rfcomm_getConnSessionCount(void);

/******************************************************************************
 * Function: Rfcomm Get Idle Channel count interface
 * Descript: This interface be used by profile or user to get rfcomm Channel count.
 * Params:
 * Return: The Idle Rfcomm Channel.
 *******************************************************************************/
uint08 btp_rfcomm_getIdleChannelCount(void);

/******************************************************************************
 * Function: Rfcomm Get Used Channel count interface
 * Descript: This interface be used by profile or user to get rfcomm Channel count.
 * Params:
 * Return: The Used Rfcomm Channel count.
 *******************************************************************************/
uint08 btp_rfcomm_getUsedChannelCount(void);

/******************************************************************************
 * Function: Rfcomm Get Connect Channel Dict interface
 * Descript: This interface be used by profile or user to get rfcomm Channel count.
 * Params:
 * Return:The Connect Rfcomm Channel count.
 *******************************************************************************/
uint08 btp_rfcomm_getConnChannelCount(void);

/******************************************************************************
 * Function: Rfcomm Get Used Channel Dict interface
 * Descript: This interface be used by profile or user to get rfcomm Channel count.
 * Params:
 *         @sessionID[IN]--The Session Id.
 * Return:The Used Rfcomm Channel count.
 *******************************************************************************/
uint08 btp_rfcomm_getUsedChannelCountInSession(uint08 sessionID);

/******************************************************************************
 * Function: Rfcomm Get Idle Channel Dict interface
 * Descript: This interface be used by profile or user to get rfcomm Channel.
 * Params:
 * Return:The Rfcomm Channel Dict(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_chndict_t *btp_rfcomm_getIdleChndict(void);

/******************************************************************************
 * Function: Rfcomm Get Used Channel Dict interface
 * Descript: This interface be used by profile or user to get rfcomm Channel
 *           dict with aclhandle and channel Id.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 *         @channelID[IN]--The channel id.
 * Return:The Rfcomm Channel Dict(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_chndict_t *btp_rfcomm_getUsedChndictByChannel(uint16 aclHandle, uint08 channelID);

/******************************************************************************
 * Function: Rfcomm Get Used Channel Dict interface
 * Descript: This interface be used by profile or user to get rfcomm Channel
 *           dict with aclhandle and service Id.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 *         @serviceID[IN]--The service id.
 * Return:The Rfcomm Channel Dict(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_chndict_t *btp_rfcomm_getUsedChndictByService(uint16 aclHandle, uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm Get Service interface
 * Descript: This interface be used by profile or user to get rfcomm Service.
 * Params:
 *         @serviceID[IN]--The service id.
 * Return:The Rfcomm Service(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_service_t *btp_rfcomm_getService(uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm Get Idle Service interface
 * Descript: This interface be used by profile or user to get rfcomm Idle
 *           Service.
 * Params:
 *
 * Return:The Rfcomm Service(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_service_t *btp_rfcomm_getIdleService(void);

/******************************************************************************
 * Function: Rfcomm Get Session interface
 * Descript: This interface be used by profile or user to get rfcomm Session.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return:The Rfcomm Session(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_session_t *btp_rfcomm_getSession(uint08 sessionID);

/******************************************************************************
 * Function: Rfcomm Get Idle Session interface
 * Descript: This interface be used by profile or user to get rfcomm Session.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return:The Rfcomm Session(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_session_t *btp_rfcomm_getIdleSession(void);

/******************************************************************************
 * Function: Rfcomm Get Used Session interface
 * Descript: This interface be used by profile or user to get rfcomm session.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return:The Rfcomm Session(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_session_t *btp_rfcomm_getUsedSession(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm Get Ression interface
 * Descript: This interface be used by profile or user to get rfcomm Session.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return:The Rfcommc Session(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_session_t *btp_rfcomm_getConnSession(uint16 aclHandle);

/******************************************************************************
 * Function: Rfcomm get Channel interface
 * Descript: This interface be used by profile or user to get rfcomm channel.
 * Params:
 * Return: The Rfcommc Channel(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_channel_t *btp_rfcomm_getChannel(uint08 rfcHandle);

/******************************************************************************
 * Function: Rfcomm Get Idle Channel interface
 * Descript: This interface be used by profile or user to get rfcomm Idle channel.
 *           which indicate by aclHandle.
 * Params:
 * Return:The Rfcommc Channel(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_channel_t *btp_rfcomm_getIdleChannel(void);

/******************************************************************************
 * Function: Rfcomm Get Used Channel interface
 * Descript: This interface be used by profile or user to get rfcomm Used Channel.
 *           which indicate by aclHandle.
 * Params:
 *         @aclHandle[IN]--The Acl Link handle.
 * Return: The Rfcommc Channel(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_channel_t *btp_rfcomm_getUsedChannel(uint16 aclHandle, uint08 rfcHandle);

/******************************************************************************
 * Function: Rfcomm Get Channel interface
 * Descript: This interface be used by profile or user to get rfcomm Connect Channel.
 * Params:
 * Return:The Rfcommc Channel(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_channel_t *btp_rfcomm_getConnChannel(uint16 aclHandle, uint08 rfcHandle);

/******************************************************************************
 * Function: Rfcomm Get Channel interface
 * Descript: This interface be used by profile or user to get rfcomm Used Channel.
 * Params:
 *         @aclHandle[IN]---The Acl handle.
 *         @dlci[IN]--The dlci.
 * Return:The Rfcommc Channel(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_channel_t *btp_rfcomm_getUsedChannelByDlci(uint16 aclHandle, uint08 dlci);

/******************************************************************************
 * Function: Rfcomm Get Channel interface
 * Descript: This interface be used by profile or user to get rfcomm Connect Channel.
 * Params:
 *         @aclHandle[IN]---The Acl handle.
 *         @dlci[IN]--The dlci.
 * Return:The Rfcommc Channel(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_channel_t *btp_rfcomm_getConnChannelByDlci(uint16 aclHandle, uint08 dlci);

/******************************************************************************
 * Function: Rfcomm get Channel interface
 * Descript: This interface be used by profile or user to get rfcomm Used Channel.
 * Params:
 *         @aclHandle[IN]---The Acl handle.
 *         @serviceID[IN]--The Mtu Size.
 * Return:The Rfcommc Channel(success)/NULL(false).
 *******************************************************************************/
btp_rfcomm_channel_t *btp_rfcomm_getUsedChannelByService(uint16 aclHandle, uint08 serviceID);

/******************************************************************************
 * Function: Rfcomm Get Mtu Size interface
 * Descript: This interface be used by profile or user to get rfcomm Mtu Size.
 *         @rfchandle[IN]---The rfcomm channel handle.
 *         @pMtuSize[IN]--The Mtu Size.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_getMtuSize(uint08 rfchandle, uint16 *pMtuSize);

/******************************************************************************
 * Function: Rfcomm Get Channel Id interface
 * Descript: This interface be used by profile or user to get Rfcomm channel.
 * Params:
 *         @rfchandle[IN]---The rfcomm channel handle.
 *         @pChannel[IN]--The rfcomm Channel.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_getChannelID(uint08 rfchandle, uint08 *pChannel);

/******************************************************************************
 * Function: btp_rfcomm_sendRemoteLineStatusCmd
 * Descript: This interface be used by profile or user to send RLS CMD.
 * Params:
 *     @rfchandle[IN]---The rfcomm channel handle.
 *     @status[IN]--The L1-L4 bits indicates the Line Status.
 *         If L1 is set to 0, no error have occurred.
 *           L1 = 1 indicates the following errors:
 *         L2-L4:
 *           100 Overrun Error - Received character overwrote an unread character
 *           010 Parity Error - Received character's parity was incorrect
 *           001 Framing Error - a character did not terminate with a stop bit.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_sendRemoteLineStatusCmd(uint08 rfcHandle, uint08 status);

/******************************************************************************
 * Function: Rfcomm Destroy interface
 * Descript: This interface be used by profile or user to release rfcomm resource
 *           which indicate by aclHandle.
 * Params:
 *         @rfcHandle[IN]--The rfcomm channel handle.
 *         @pData[IN]--The payload data.
 *         @datalen[IN]--The payload length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_send(uint08 rfcHandle, uint08 *pData, uint16 datalen);

/******************************************************************************
 * Function: Rfcomm Destroy interface
 * Descript: This interface be used by profile or user to release rfcomm resource
 *           which indicate by aclHandle.
 * Params:
 *         @rfcHandle[IN]--The rfcomm channel handle.
 *         @pHead[IN]--The header data.
 *         @headLen[IN]--The header data length.
 *         @pBody[IN]--The payload data.
 *         @bodyLen[IN]--The payload length.
 *         @isFinal[IN]--The final packet.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_extendSend(uint08 rfcomm_handle,
                          uint08 *pHead,
                          uint16 headLen,
                          uint08 *pBody,
                          uint32 bodyLen,
                          bool isFinal);

/******************************************************************************
 * Function: Rfcomm Destroy interface
 * Descript: This interface be used by profile or user to release rfcomm resource
 *           which indicate by aclHandle.
 * Params:
 *         @rfcHandle[IN]--The rfcomm channel handle.
 *         @pHead[IN]--The header data.
 *         @headLen[IN]--The header data length.
 *         @pBody[IN]--The payload data.
 *         @bodyLen[IN]--The payload length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_rfcomm_sendData(uint08 rfcHandle, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);


#endif /* BTP_RFCOMM_H */
