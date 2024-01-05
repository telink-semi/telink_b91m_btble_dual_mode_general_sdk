/********************************************************************************************************
 * @file	bth_l2cap.h
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
#ifndef BTH_L2CAP_H
#define BTH_L2CAP_H


#define BTH_L2CAP_CHN_TIMEOUT      (100000) // 100ms
#define BTH_L2CAP_CHN_CONN_TIMEOUT (10000000 / BTH_L2CAP_CHN_TIMEOUT)
#define BTH_L2CAP_CHN_DISC_TIMEOUT (3000000 / BTH_L2CAP_CHN_TIMEOUT)

#define BTH_L2CAP_MTU_DEF_SIZE     672 // The default value is 672 octets. Spec5.2 P1083
#define BTH_L2CAP_MTU_MAX_SIZE     1024
#define BTH_L2CAP_MPS_MAX_SIZE     668 //

/* base frame */
#define BTH_L2CAP_SIG_CID            0x0001
#define BTH_L2CAP_CONNECTIONLESS_CID 0x0002


#define BTH_L2CAP_CFG_EFS_ENABLE     0
#define BTH_L2CAP_CFG_QOS_ENABLE     0

typedef int (*bth_l2cap_eventCallback_t)(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);
typedef void (*bth_l2cap_rdataCallback_t)(uint16 psmID, uint16 chnID, uint16 handle, uint08 *pData, uint16 dataLen);


typedef enum {
    BTH_L2CAP_SAR_UNSEGMENTED = 0x00,
    BTH_L2CAP_SAR_START       = 0x01,
    BTH_L2CAP_SAR_END         = 0x02,
    BTH_L2CAP_SAR_CONTINUE    = 0x03,
} BTH_L2CAP_SAR_ENUM;

typedef enum {
    BTH_L2CAP_SIG_RTNMODE_BASIC     = 0x00, // L2CAP Basic Mode
    BTH_L2CAP_SIG_RTNMODE_RTN       = 0x01, // Retransmission mode
    BTH_L2CAP_SIG_RTNMODE_FLOW      = 0x02, // Flow control mode
    BTH_L2CAP_SIG_RTNMODE_ENHANCE   = 0x03, // Enhanced Retransmission mode
    BTH_L2CAP_SIG_RTNMODE_STREAMING = 0x04, // Streaming mode
} BTH_L2CAP_SIG_RTNMODE_ENUM;
typedef enum {
    BTH_L2CAP_SIG_FCSTYPE_NOFCS  = 0x00,
    BTH_L2CAP_SIG_FCSTYPE_16bFCS = 0x01,
} BTH_L2CAP_SIG_FCSTYPE_ENUM;
typedef enum {
    BTH_L2CAP_OPTION_TYPE_MTU           = 0x01,
    BTH_L2CAP_OPTION_TYPE_FLUSH_TIMEOUT = 0x02,
    BTH_L2CAP_OPTION_TYPE_QOS           = 0x03,
    BTH_L2CAP_OPTION_TYPE_RTN           = 0x04,
    BTH_L2CAP_OPTION_TYPE_FCS           = 0x05,
    BTH_L2CAP_OPTION_TYPE_EXT_FLOW      = 0x06,
    BTH_L2CAP_OPTION_TYPE_EXT_WINSIZE   = 0x07,
} BTH_L2CAP_OPTION_TYPE_ENUM;
typedef enum {
    BTH_L2CAP_SERVICE_TYPE_NO_TRAFFIC  = 0x00,
    BTH_L2CAP_SERVICE_TYPE_BEST_EFFORT = 0x01, // Default
    BTH_L2CAP_SERVICE_TYPE_GUARANTEED  = 0x02,
} BTH_L2CAP_SERVICE_TYPE_ENUM;

// EXTENDED FEATURE MASK <Core5.2.pdf>P1063
typedef enum {
    BTH_L2CAP_EXT_FEATURE_NONE              = 0x0000,
    BTH_L2CAP_EXT_FEATURE_FLOW_CTRL_MODE    = 0x0001, // Flow control mode
    BTH_L2CAP_EXT_FEATURE_RTN_MODE          = 0x0002, // Retransmission mode
    BTH_L2CAP_EXT_FEATURE_BI_DIR_QOS        = 0x0004, // Bi-directional QoS
    BTH_L2CAP_EXT_FEATURE_ENHANCED_RTN_MODE = 0x0008, // Enhanced Retransmission Mode
    BTH_L2CAP_EXT_FEATURE_STREAMING_MODE    = 0x0010, // Streaming Mode
    BTH_L2CAP_EXT_FEATURE_FCS_OPTION        = 0x0020, // FCS Option
    BTH_L2CAP_EXT_FEATURE_EXT_FLOW_SPEC     = 0x0040, // Extended Flow Specification for BR/EDR
    BTH_L2CAP_EXT_FEATURE_FIXED_CHANNELS    = 0x0080, // Fixed Channels
    BTH_L2CAP_EXT_FEATURE_EXT_WIN_SIZE      = 0x0100, // Extended Window Size
    BTH_L2CAP_EXT_FEATURE_UNICAST_CONN_DATA = 0x0200, // Unicast Connectionless Data Reception
    BTH_L2CAP_EXT_FEATURE_ENHANCED_CREDIT   = 0x0400, // Enhanced Credit Based Flow Control Mode
    BTH_L2CAP_EXT_FEATURE_DEFAULT           = BTH_L2CAP_EXT_FEATURE_FIXED_CHANNELS,
} BTH_L2CAP_EXT_FEATURE_ENUM;
// EXTENDED CHANNEL MASK <Core5.2.pdf>P1063
typedef enum {
    BTH_L2CAP_EXT_CHANNEL_NONE              = 0x0000,
    BTH_L2CAP_EXT_CHANNEL_SIGNAL            = 0x0002, // BIT(1)
    BTH_L2CAP_EXT_CHANNEL_CONNECTIONLESS    = 0x0004, // BIT(2)
    BTH_L2CAP_EXT_CHANNEL_AMP_MANAGER       = 0x0008, // BIT(3)
    BTH_L2CAP_EXT_CHANNEL_SERCURITY_MANAGER = 0x0080, // BIT(7)
    BTH_L2CAP_EXT_CHANNEL_DEFAULT           = BTH_L2CAP_EXT_CHANNEL_SIGNAL,
} BTH_L2CAP_EXT_CHANNEL_ENUM;

typedef enum {
    BTP_L2CAP_INFO_TYPE_CONNLESS_MTU      = 0x01,
    BTP_L2CAP_INFO_TYPE_EXTENDED_FEATURE  = 0x02,
    BTP_L2CAP_INFO_TYPE_EXTENDED_CHANNELS = 0x03, // FIXED CHANNELS SUPPORTED   <Core5.2.pdf>P1061 and 1064
} BTH_L2CAP_INFO_TYPE_ENUM;


typedef enum {
    BTH_L2CAP_EVTID_NONE = 0,
    BTH_L2CAP_EVTID_CLOSED,
    BTH_L2CAP_EVTID_REQUEST,
    BTH_L2CAP_EVTID_DISCONN,
    BTH_L2CAP_EVTID_CONNECT,
} BTH_L2CAP_EVTID_ENUM;

typedef enum {
    BTP_L2CAP_OPTION_FLAG_MTU = 0x01,
    BTP_L2CAP_OPTION_FLAG_FCS = 0x02, // It is non-negotiable. The FCS option shall only be used when the mode is being,
                                      // or is already configured to Enhanced Retransmission mode or Streaming mode.
    BTP_L2CAP_OPTION_FLAG_RTN         = 0x04,
    BTP_L2CAP_OPTION_FLAG_QOS         = 0x08,
    BTP_L2CAP_OPTION_FLAG_TIMEOUT     = 0x10,
    BTP_L2CAP_OPTION_FLAG_EXT_FLOW    = 0x20,
    BTP_L2CAP_OPTION_FLAG_EXT_WINSIZE = 0x40,
    BTP_L2CAP_OPTION_FLAG_RESERVE     = 0x80,
} BTH_L2CAP_OPTION_FLAG_ENUM;
typedef enum {
    BTP_L2CAP_CONFIG_FLAG_NONE     = 0x0000,
    BTP_L2CAP_CONFIG_FLAG_CONTINUE = 0x0001,
} BTH_L2CAP_CONFIG_FLAG_ENUM;


typedef enum {
    BTH_L2CAP_REASON_NONE = 0,
    BTH_L2CAP_REASON_DISC_BY_USER,
    BTH_L2CAP_REASON_DISC_BY_REMOTE,
    BTH_L2CAP_REASON_DISC_BY_REJECT, // The remote does not accept the given parameters.
    BTH_L2CAP_REASON_DISC_BY_TIMEOUT,
    BTH_L2CAP_REASON_DISC_BY_ERR_PRAMA,
    BTH_L2CAP_REASON_DISC_BY_ERR_MTU,
    BTH_L2CAP_REASON_DISC_BY_ERR_STATUS,
} BTH_L2CAP_REASON_ENUM;


typedef struct {
    uint16 scid;
    uint16 handle;
    uint08 isActive;
} bth_l2cap_closedEvt_t;
typedef struct {
    uint16 scid;
    uint16 handle;
} bth_l2cap_requestEvt_t;
typedef struct {
    uint16 scid;
    uint16 dcid;
    uint16 handle;
    uint08 userID;
    uint16 mtuSize;
    uint08 isActive; // True-The connection was initiated by us; False-The connection was initiated by peer.
} bth_l2cap_connectEvt_t;
typedef struct {
    uint16 scid;
    uint16 dcid;
    uint16 handle;
    uint08 userID;
    uint16 reason;
} bth_l2cap_disconnEvt_t;


typedef struct {
    uint16 psmID;
    uint16 resv0;
    bth_l2cap_eventCallback_t eventCB;
    bth_l2cap_rdataCallback_t rdataCB;
} bth_l2cap_service_t;
typedef struct {
    uint16 scid;
    uint16 dcid;

    uint08 state; // TLK_STATE_CLOSED, TLK_STATE_OPENED, TLK_STATE_CONNING, TLK_STATE_CONNECT, TLK_STATE_DISCING
    uint08 busys;
    uint08 flags;
    uint08 usrID;
    uint08 attrs; // Refer to BTH_SIGNAL_ATTRS_ENUM.
    uint08 resv01;
    uint16 resv02;

    uint16 psmID;
    uint16 mtuSize;
    uint16 timeout;
    uint16 aclHandle;

    uint08 rspResult;
    uint08 rejReason; // reject reason
    uint08 disReason; // disconnect reason
    uint08 cmdIdentify;
    uint08 rejIdentify;
    uint08 rspIdentify;
    uint16 resv03;

    tlkapi_timer_t timer;

    // Options
    uint08 negIsNext; //
    uint08 cfgOption; // Refer to BTH_L2CAP_OPTION_FLAG_ENUM.
    uint08 negOption; // negotiation option by both. Refer to BTH_L2CAP_OPTION_FLAG_ENUM.
    uint08 curOption; //
    // Flush Timeout.
    uint16 flushTimeout;
    // RETRANSMISSION AND FLOW CONTROL OPTION
    uint16 rtnMps;
    uint08 rtnMode; //
    uint08 rtnTxWin;
    uint08 rtnMaxTx;
    uint08 resv04;
    uint16 rtnTimeout;
    uint16 monTimeout;
    // FCS
    uint08 fcsType;
    uint08 resv05;
    uint16 resv06;
// EXTENDED FLOW SPECIFICATION OPTION
#if (BTH_L2CAP_CFG_EFS_ENABLE)
    uint08 efsIdentifier;     // Default: 0x01
    uint08 efsServiceType;    // Default: 0x01
    uint16 efsMaxSduSize;     // Default: 0xFFFF
    uint32 efsSduArrivalTime; // Default: 0xFFFFFFFF
    uint32 efsAccessLatency;  // Default: 0xFFFFFFFF
    uint32 efsFlushTimeout;   // Default: 0xFFFFFFFF
#endif
#if (BTH_L2CAP_CFG_QOS_ENABLE)
    uint08 qosFlag;
    uint08 qosServiceType;     // Default: 0x01
    uint16 resv07;
    uint32 qosTokenRate;       //
    uint32 qosTokenBucketSize; //
    uint32 qosPeekBandWidth;   //
    uint32 qosLatency;         //
    uint32 qosDelayVariation;  //
#endif
} bth_l2cap_channel_t;
typedef struct {
    uint16 handle;
    uint16 chnID;
    uint16 sumLen;
    uint16 curLen;
    uint08 buffer[BTH_L2CAP_MTU_MAX_SIZE];
} bth_l2cap_acldata_t;

typedef struct {
    uint08 mpsBuffer[BTH_L2CAP_MPS_MAX_SIZE + 12];	// 10 -> 12
    bth_l2cap_service_t *service;
    bth_l2cap_channel_t *channel;
    bth_l2cap_acldata_t *aclData;
} bth_l2cap_ctrl_t;


// I-frame Standard Control Field  Core5.2:P1038
typedef union {
    uint16 value;
    struct {
        uint16 Type   : 1; // 0:I-Frame, 1-S-Frame
        uint16 TxSeq  : 6; // Send Sequence Number (Sequence Number Of Acknowledgement)
        uint16 R      : 1; // Final:The F-bit is set to 1 in response to an S-frame with the P bit set to 1.
        uint16 ReqSeq : 6; // Receive Sequence Number
        uint16 SAR    : 2; // Segmentation and Reassembly
    } field;
} bth_l2cap_iframeStdCtrlField_t;
// I-frame Enhanced Control Field
typedef union {
    uint16 value;
    struct {
        uint16 Type   : 1;
        uint16 TxSeq  : 6;
        uint16 F      : 1;
        uint16 ReqSeq : 6;
        uint16 SAR    : 2;
    } field;
} bth_l2cap_iframeEnhCtrlField_t;
// I-frame Extended Control Field
typedef union {
    uint32 value;
    struct {
        uint32 Type   : 1;
        uint32 F      : 1;
        uint32 TxSeq  : 14;
        uint32 SAR    : 2;
        uint32 ReqSeq : 14;
    } field;
} bth_l2cap_iframeExtCtrlField_t;

// S-frame Standard Control Field  Core5.2:P1038
typedef union {
    uint16 value;
    struct {
        uint16 Type : 1; // 0:I-Frame, 1-S-Frame
        uint16 RFU0 : 1; //
        uint16 S    : 2; // Supervisory function. 00-RR,Receiver Ready; 01-REJ,Reject; 10-RNR,Receiver Not Ready;
                      // 11-SREJ,Select Reject
        uint16 RFU1 : 3; //
        uint16 R    : 1; // Retransmission Disable Bit. 0-Normal operation; 1-Receiver side requests sender to postpone
                      // retransmission of I-frames.
        uint16 ReqSeq : 6; // Receive Sequence Number
        uint16 RFU2   : 2; //
    } field;
} bth_l2cap_sframeStdCtrlField_t;
// S-frame Enhanced Control Field
typedef union {
    uint16 value;
    struct {
        uint16 Type : 1; // 0:I-Frame, 1-S-Frame
        uint16 RFU0 : 1; //
        uint16 S    : 2; // Supervisory function. 00-RR,Receiver Ready; 01-REJ,Reject; 10-RNR,Receiver Not Ready;
                      // 11-SREJ,Select Reject
        uint16 P : 1; // Retransmission Disable Bit. 0-Normal operation; 1-Receiver side requests sender to postpone
                      // retransmission of I-frames.
        uint16 RFU1 : 2; // Poll. The P-bit is set to 1 to solicit a response from the receiver. The receiver
                         // shallrespond immediately with a frame with the F-bit set to 1.
        uint16 F      : 1; // Final. The F-bit is set to 1 in response to an S-frame with the P bit set to 1.
        uint16 ReqSeq : 6; // Receive Sequence Number
        uint16 RFU2   : 2; //
    } field;
} bth_l2cap_sframeEnhCtrlField_t;
// S-frame Extended Control Field
typedef union {
    uint32 value;
    struct {
        uint32 Type   : 1;
        uint32 F      : 1;
        uint32 TxSeq  : 14;
        uint32 S      : 2;
        uint32 P      : 1;
        uint32 ReqSeq : 13;
    } field;
} bth_l2cap_sframeExtCtrlField_t;


/******************************************************************************
 * Function: bth_l2cap_init
 * Descript: Initial the l2cap control block.
 * Params:
 * 		@psmNumb[IN]--refer to TLK_STK_BTPSM_NUMB
 * 		@chnNumb[IN]--refer to TLK_STK_BTCHN_NUMB
 * 		@aclNumb[IN]--refer to TLK_STK_BTACl_NUMB
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *         If others value(-TLK_ENOMEM) is returned means the initial process fail.
 *******************************************************************************/
int bth_l2cap_init(uint08 psmNumb, uint08 chnNumb, uint08 aclNumb);

/******************************************************************************
 * Function: bth_l2cap_deinit
 * Descript: Deinitial the l2cap control block.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *******************************************************************************/
int bth_l2cap_deinit(void);

/******************************************************************************
 * Function: bth_l2cap_getMemLen
 * Descript: get the size of the required number of nodes interface.
 * Params:
 * 		@psmNumb[IN]--refer to TLK_STK_BTPSM_NUMB
 * 		@chnNumb[IN]--refer to TLK_STK_BTCHN_NUMB
 * 		@aclNumb[IN]--refer to TLK_STK_BTACl_NUMB
 * Return: Actual node memory size required(unit: byte).
 *******************************************************************************/
int bth_l2cap_getMemLen(uint08 psmNumb, uint08 chnNumb, uint08 aclNumb);

/******************************************************************************
 * Function: bth_l2cap_aclDisconn
 * Descript: Send l2cap disconnect.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_l2cap_aclDisconn(uint16 aclHandle);


/******************************************************************************
 * Function: bth_l2cap_setCurChnID
 * Descript: Initial sBthL2capCurChnID.
 * Params:
 *        @cur_cid[IN]--new set cid
 * Return: None.
 *******************************************************************************/
void bth_l2cap_setCurChnID(uint16 cur_cid);

/******************************************************************************
 * Function: bth_l2cap_setDefMtuSize
 * Descript: Set the Mtu size.
 * Params:
 *     @size[IN]--The mtu size.
 * Return: None.
 *******************************************************************************/
void bth_l2cap_setDefMtuSize(uint16 size);
uint bth_l2cap_getDefMtuSize(void);

void bth_l2cap_setExtFeature(uint32 feature);
uint bth_l2cap_getExtFeature(void);
void bth_l2cap_setExtFeatureBits(uint32 featBits);
void bth_l2cap_clsExtFeatureBits(uint32 featBits);
bool bth_l2cap_haveExtFeatureBits(uint32 featBits);

void bth_l2cap_setExtChannel(uint32 channel);
uint bth_l2cap_getExtChannel(void);
void bth_l2cap_setExtChannelBits(uint32 channel);
void bth_l2cap_clsExtChannelBits(uint32 channel);
bool bth_l2cap_haveExtChannelBits(uint32 channel);

uint bth_l2cap_getValidCID(void);


/******************************************************************************
 * Function: bth_l2cap_regServiceCB
 * Descript: Register the callback.
 * Params:
 *     @psmID[IN]--The psm id.
 *     @usrID[IN]--The user id.
 *     @eventCB[IN]--The l2cap event callback.
 *     @rdataCB[IN]--The read data callback.
 * Return: TLK_ENONE is success, other' value is failure.
 *******************************************************************************/
int bth_l2cap_regServiceCB(uint16 psmID, bth_l2cap_eventCallback_t eventCB, bth_l2cap_rdataCallback_t rdataCB);

/******************************************************************************
 * Function: bth_l2cap_sendEvent
 * Descript: Set the Mtu size.
 * Params:
 *     @size[IN]--The mtu size.
 * Return: None.
 *******************************************************************************/
int bth_l2cap_sendEvent(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: bth_l2cap_getIdleService
 * Descript: Get the l2cap service.
 * Params: None.
 * Return: Return L2cap service.
 *******************************************************************************/
bth_l2cap_service_t *bth_l2cap_getIdleService(void);

/******************************************************************************
 * Function: bth_l2cap_getUsedService
 * Descript: Get the Used l2cap service.
 * Params: @psmID[IN]--The psm id.
 * Return: Return L2cap service.
 *******************************************************************************/
bth_l2cap_service_t *bth_l2cap_getUsedService(uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getIdleChannel
 * Descript: Get the idle l2cap channel.
 * Params:
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getIdleChannel(void);

/******************************************************************************
 * Function: bth_l2cap_getInitChannel
 * Descript: Get the init l2cap channel.
 * Params:
 *     @scid[IN]--The channel id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getInitChannel(uint16 scid);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByScid
 * Descript: Get the Used l2cap channel by scid.
 * Params:
 *     @scid[IN]--The channel id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getUsedChannelByScid(uint16 scid);

/******************************************************************************
 * Function: bth_l2cap_getConnChannelByScid
 * Descript: Get the Connect l2cap channel by scid.
 * Params:
 *     @scid[IN]--The channel id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getConnChannelByScid(uint16 scid);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByDcid
 * Descript: Get the Used l2cap channel by data channel id.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @dcid[IN]--The data channel id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getUsedChannelByDcid(uint16 aclHandle, uint16 dcid);

/******************************************************************************
 * Function: bth_l2cap_getConnChannelByDcid
 * Descript: Get the Connect l2cap channel by data channel id.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @dcid[IN]--The data channel id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getConnChannelByDcid(uint16 aclHandle, uint16 dcid);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByUser
 * Descript: Get the Used l2cap channel by userid.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 *     @usrID[ID]--The user id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getUsedChannelByUser(uint16 aclHandle, uint16 psmID, uint08 usrID);

/******************************************************************************
 * Function: bth_l2cap_getConnChannelByUser
 * Descript: Get the Connect l2cap channel by userid.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 *     @usrID[ID]--The user id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getConnChannelByUser(uint16 aclHandle, uint16 psmID, uint08 usrID);

/******************************************************************************
 * Function: bth_l2cap_getInitChannelByPsm
 * Descript: Get the Init l2cap channel by psm.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getInitChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByPsm
 * Descript: Get the Used l2cap channel by psm.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getUsedChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getUsedChannelByPsm
 * Descript: Get the Connected l2cap channel by psm.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getConnChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getNoConnChannelByPsm
 * Descript: Get the no Connected l2cap channel by psm.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getNoConnChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getNoDiscChannelByPsm
 * Descript: Get the no DisConnected l2cap channel by psm.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @psmID[IN]--The psm id.
 * Return: Return L2cap Channel.
 *******************************************************************************/
bth_l2cap_channel_t *bth_l2cap_getNoDiscChannelByPsm(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_l2cap_getSendFifoNumb
 * Descript: Gets the number of TX-FIFOs that can be used.
 * Params: None.
 * Return: The number of TX-FIFOs.
 *******************************************************************************/
int bth_l2cap_getValidTxFifoNumb(void);

/******************************************************************************
 * Function: bth_l2cap_sendData
 * Descript: Send the l2cap data.
 * Params:
 *     @connHandle[IN]--The conntion handle.
 *     @pHead[IN]--The head data.
 *     @headLen[IN]--The headdata length.
 *     @pData[IN]--The payload data.
 *     @dataLen[IN]--The payload data length.
 * Return: Return TLK_ENONE is success or others value is failure.
 *******************************************************************************/
int bth_l2cap_sendData(uint16 connHandle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: bth_l2cap_sendChannelData
 * Descript: Send the l2cap data.
 * Params:
 *     @connHandle[IN]--The conntion handle.
 *     @scid[IN]--The channel id.
 *     @pHead[IN]--The head data.
 *     @headLen[IN]--The headdata length.
 *     @pData[IN]--The payload data.
 *     @dataLen[IN]--The payload data length.
 * Return: Return TLK_ENONE is success or others value is failure.
 *******************************************************************************/
int bth_l2cap_sendChannelData(uint16 connHandle,
                              uint16 scid,
                              uint08 *pHead,
                              uint16 headLen,
                              uint08 *pData,
                              uint16 dataLen);

/******************************************************************************
 * Function: bth_l2cap_sendChannelDataExt
 * Descript: Send the l2cap data with user extend data.
 * Params:
 *     @connHandle[IN]--The conntion handle.
 *     @scid[IN]--The channel id.
 *     @pUsrExt[IN]--The user extend data.
 *     @extLen[IN]--The data length.
 *     @pHead[IN]--The head data.
 *     @headLen[IN]--The headdata length.
 *     @pData[IN]--The payload data.
 *     @dataLen[IN]--The payload data length.
 * Return: Return TLK_ENONE is success or others value is failure.
 *******************************************************************************/
int bth_l2cap_sendChannelDataExt(uint16 connHandle,
                                 uint16 scid,
                                 uint08 *pUsrExt,
                                 uint08 extLen,
                                 uint08 *pHead,
                                 uint16 headLen,
                                 uint08 *pData,
                                 uint16 dataLen);

int bth_l2cap_sendIFrameSFrame(uint16 connHandle,
                               uint16 scid,
                               uint32 ctrl,
                               uint08 *pHead,
                               uint16 headLen,
                               uint08 *pData,
                               uint16 dataLen,
                               bool isAddFcs,
                               bool isExtCtr);
int bth_l2cap_sendIFrame(uint16 connHandle,
                         uint16 scid,
                         uint16 ictrl,
                         uint8_t *pHead,
                         uint16 headLen,
                         uint8_t *pData,
                         uint16 dataLen,
                         bool isAddFcs);
int bth_l2cap_sendExtIFrame(uint16 connHandle,
                            uint16 scid,
                            uint16 ictrl,
                            uint8_t *pHead,
                            uint16 headLen,
                            uint8_t *pData,
                            uint16 dataLen,
                            bool isAddFcs);
int bth_l2cap_sendIFrameWithoutCtrFcs(uint16 connHandle,
                                      uint16 scid,
                                      uint8_t *pHead,
                                      uint16 headLen,
                                      uint8_t *pData,
                                      uint16 dataLen);

int bth_l2cap_sendSFrame(uint16 connHandle,
                         uint16 scid,
                         uint16 sctrl,
                         uint8_t *pHead,
                         uint16 headLen,
                         uint8_t *pData,
                         uint16 dataLen,
                         bool isAddFcs);
int bth_l2cap_sendExtSFrame(uint16 connHandle,
                            uint16 scid,
                            uint16 sctrl,
                            uint8_t *pHead,
                            uint16 headLen,
                            uint8_t *pData,
                            uint16 dataLen,
                            bool isAddFcs);


/******************************************************************************
 * Function: bth_l2cap_recvHandler
 * Descript: Receive the l2cap data.
 * Params:
 *     @connHandle[IN]--The conntion handle.
 *     @llid[IN]--The ll id.
 *     @p[IN]--The data.
 *     @len[IN]--The data length.
 * Return: Return TLK_ENONE is success or others value is failure.
 *******************************************************************************/
void bth_l2cap_recvHandler(uint16 handle, uint08 *pData, uint16 dataLen);


#endif // BTH_L2CAP_H
