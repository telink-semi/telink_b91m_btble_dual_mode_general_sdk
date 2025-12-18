/********************************************************************************************************
 * @file	bth_event.h
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
#ifndef BTH_EVENT_H
#define BTH_EVENT_H


typedef int (*bth_event_func)(uint08 *pData, uint16 dataLen);


typedef enum {
    BTH_EVTID_NONE = 0,

    // Basic Event
    BTH_EVTID_INQUIRY_RESULT,
    BTH_EVTID_INQUIRY_COMPLETE,
    BTH_EVTID_GETNAME_COMPLETE,
    BTH_EVTID_ACLCONN_REQUEST,
    BTH_EVTID_SCOCONN_REQUEST,
    BTH_EVTID_ACLCONN_COMPLETE,
    BTH_EVTID_SCOCONN_COMPLETE,
    BTH_EVTID_ACLDISC_COMPLETE,
    BTH_EVTID_SCODISC_COMPLETE,
    BTH_EVTID_AUTHEN_COMPLETE,
    BTH_EVTID_ENCRYPT_COMPLETE,
    BTH_EVTID_SCOCODEC_CHANGED,
    BTH_EVTID_ROLE_CHANGED,
    BTH_EVTID_MODE_CHANGED,
    BTH_EVTID_PINCODE_REQUEST,
    BTH_EVTID_LINKKEY_REQUEST,
    BTH_EVTID_LINKKEY_NOTIFY,
    BTH_EVTID_ACL_ESTABLISH, // Private Protocol
    BTH_EVTID_EXT_FEATURE_CHANGED,
    BTH_EVTID_ACL_GETNAME_REPORT,
    BTH_EVTID_ACL_GETRSSI_REPORT,

    // Signal Event
    BTH_EVTID_SIGNAL_CONNREQ,
    BTH_EVTID_SIGNAL_CONNRSP,
    BTH_EVTID_SET_SCAN_CMD_COMPLETE,

    BTH_EVTID_MAX,
} BTH_EVTID_ENUM;


typedef struct {
    uint08 mac[6];
    uint08 rssi;
    uint08 smode;   // Scan Mode
    uint08 nameLen; // Name Length
    uint16 clock;
    uint16 eirLen;
    uint32 dtype;
    uint08 *pName;
    uint08 *pEird;
} bth_inquiryResultEvt_t;
typedef struct {
    uint08 status;
    uint08 nameLen;
    uint08 btaddr[6];
    uint08 *pName;
} bth_getNameCompleteEvt_t;

typedef struct {
    uint32 devClass;
    uint08 peerMac[6];
} bth_aclConnRequestEvt_t;
typedef struct {
    uint08 linkType;
    uint32 devClass;
    uint08 peerMac[6];
} bth_scoConnRequestEvt_t;
typedef struct {
    uint08 status;
    uint08 active; // True - The link is initiated by us.
    uint16 handle;
    uint08 isEncrypt;
    uint08 peerMac[6];
} bth_aclConnCompleteEvt_t;
typedef struct {
    uint08 status;
    uint08 linkType; // Refer BTH_SCO_LINK_TYPE_ENUM.
    uint08 airMode;  // Refer BTH_SCO_AIRMODE_ENUM.
    uint16 aclHandle;
    uint16 scoHandle;
    uint08 peerMac[6];
} bth_scoConnCompleteEvt_t;
typedef struct {
    uint16 handle;
    uint08 reason;
    uint08 peerMac[6];
} bth_aclDiscCompleteEvt_t;
typedef struct {
    uint08 reason;
    uint08 linkType;
    uint16 aclHandle;
    uint16 scoHandle;
    uint08 peerMac[6];
} bth_scoDiscCompleteEvt_t;

typedef struct {
    uint08 status;
    uint16 handle;
} bth_authenCompleteEvt_t;
typedef struct {
    uint08 status;
    uint16 handle;
    uint08 enable;
} bth_encryptCompleteEvt_t;

typedef struct {
    uint08 codec; // 0/1-CVSD, 2-MSBC
} bth_scoCodecChangedEvt_t;

typedef struct {
    uint08 status;  // 0x00-A role change has occurred;
    uint08 newRole; // 0x00-Master, 0x01-Slave
    uint08 peerMac[6];
} bth_roleChangedEvt_t;
typedef struct {
    uint08 status;  // 0x00 -A Mode Change has occurred;
    uint08 curMode; // 0x00-Active Mode; 0x01-Hold Mode; 0x02-Sniff Mode.
    uint16 handle;
    uint16 interval;
} bth_modeChangedEvt_t;

typedef struct {
    uint16 aclHandle;
    uint08 peerMac[6];
} bth_pinCodeRequestEvt_t;
typedef struct {
    uint16 aclHandle;
    uint08 peerMac[6];
} bth_linkKeyRequestEvt_t;
typedef struct {
    uint08 keyType;
    uint08 peerMac[6];
    uint16 linkKey[16];
} bth_linkKeyNotifyEvt_t;
typedef struct {
    uint16 aclHandle;
    uint32 peerExtFeature; // Refer BTH_L2CAP_EXT_FEATURE_ENUM
    uint32 mineExtFeature; // Refer BTH_L2CAP_EXT_FEATURE_ENUM
} bth_extFeatureChangedEvt_t;

typedef struct {
    uint16 handle;
    uint08 status;
    uint08 nameLen;
    uint08 btaddr[6];
    uint08 *pName;
} bth_aclGetNameReportEvt_t;

typedef struct {
    uint16 handle;
    uint08 status;
    uint08 rssi;
} bth_aclGetRssiReportEvt_t;


typedef struct {
    uint16 handle;
    uint16 psm;
    uint16 dcid;
} bth_signalConnReqEvt_t;


/******************************************************************************
 * Function: bth_event_init
 * Descript: Initial the event control block.
 * Params: None.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_event_init(void);

/******************************************************************************
 * Function: bth_event_regCB
 * Descript: Register the event callback.
 * Params:
 *        @evtID[IN]--The event id.
 *        @func[IN]--The callback function.
 * Return: None.
 *******************************************************************************/
void bth_event_regCB(uint16 evtID, bth_event_func func);

/******************************************************************************
 * Function: bth_send_event
 * Descript: Send the event via callback.
 * Params:
 *        @evtID[IN]--The event id.
 *        @pData[IN]--The callback data.
 *        @dataLen[IN]--The data len.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_event(uint16 evtID, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * Function: bth_send_aclConnRequestEvt
 * Descript: Send the acl connect request event via callback.
 * Params:
 *        @devClass[IN]--The device class.
 *        @btaddr[IN]--The bt address.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_aclConnRequestEvt(uint32 devClass, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_send_aclConnCompleteEvt
 * Descript: Send the ACL link connect complete event via callback.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @status[IN]--The link status.
 *        @active[IN]--The acl link is active.
 *        @isEncrypt[IN]--is encrypt.
 *        @btaddr[IN]--The device bt address.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_aclConnCompleteEvt(uint16 handle, uint08 status, uint08 active, uint08 isEncrypt, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_send_aclDiscCompleteEvt
 * Descript: Send the ACL link disconnect complete event via callback.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 *        @reason[IN]--The disconnect reason.
 *        @btaddr[IN]--The device bt address.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_aclDiscCompleteEvt(uint16 handle, uint08 reason, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_send_scoConnRequestEvt
 * Descript: Send the Sco link connect request event via callback.
 * Params:
 *        @devClass[IN]--The device class.
 *        @linkType[IN]--The link type.
 *        @btaddr[IN]--The bt address.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_scoConnRequestEvt(uint32 devClass, uint08 linkType, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_send_scoConnCompleteEvt
 * Descript: Send the SCO link connect complete event via callback.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 *        @scoHandle[IN]--The Sco link handle.
 *        @status[IN]--The acl link status.
 *        @linkType[IN]--The link type. Refer BTH_SCO_LINK_TYPE_ENUM.
 *        @airMode[IN]--Refer BTH_SCO_AIRMODE_ENUM.
 *        @btaddr[IN]--The device bt address.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_scoConnCompleteEvt(uint16 aclHandle,
                                uint16 scoHandle,
                                uint08 status,
                                uint08 linkType,
                                uint08 airMode,
                                uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_send_scoDiscCompleteEvt
 * Descript: Send the SCO link disconnect complete event via callback.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 *        @scoHandle[IN]--The Sco link handle.
 *        @reason[IN]--The sco link disconnect reason.
 *        @linkType[IN]--The link type.
 *        @btaddr[IN]--The device bt address.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_scoDiscCompleteEvt(uint16 aclHandle, uint16 scoHandle, uint08 reason, uint08 linkType, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_send_authenCompleteEvt
 * Descript: Send the authenticate complete event via callback.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 *        @status[IN]--The status of authenticate.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_authenCompleteEvt(uint16 handle, uint08 status);

/******************************************************************************
 * Function: bth_send_encryptCompleteEvt
 * Descript: Send the encrypt complete event via callback.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 *        @status[IN]--The status of authenticate.
 *        @enable[IN]--is encrypt enable.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_encryptCompleteEvt(uint16 handle, uint08 status, uint08 enable);

/******************************************************************************
 * Function: bth_send_scoCodecChangedEvt
 * Descript: Send the SCO codec changed event via callback.
 * Params:
 *        @codec[IN]--The sco codec id.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
extern int bth_send_scoCodecChangedEvt(uint08 codec);

/******************************************************************************
 * Function: bth_send_aclRoleChangeEvt
 * Descript: Send the Acl role changed event via callback.
 * Params:
 *        @btaddr[IN]--The peer btaddr.
 *        @status[IN]--The role change event status.
 *        @newRole[IN]--The new role.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
extern int bth_send_aclRoleChangeEvt(uint08 btaddr[6], uint08 status, uint08 newRole);

/******************************************************************************
 * Function: bth_send_pinCodeRequestEvt
 * Descript: Send the Pin Code request event via callback.
 * Params:
 *        @handle[IN]--The handle of acl link.
 *        @btaddr[IN]--The bt addr.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_pinCodeRequestEvt(uint16 handle, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_send_linkKeyRequestEvt
 * Descript: Send the link key request event via callback.
 * Params:
 *        @handle[IN]--The handle of acl link.
 *        @btaddr[IN]--The bt addr.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_linkKeyRequestEvt(uint16 handle, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_send_linkKeyNotifyEvt
 * Descript: Send the link key notify event via callback.
 * Params:
 *        @keyType[IN]--The link key type.
 *        @btaddr[IN]--The bt addr.
 *        @linkKey[IN]--The link key.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_linkKeyNotifyEvt(uint08 keyType, uint08 btaddr[6], uint08 linkKey[16]);

/******************************************************************************
 * Function: bth_send_aclEstablishEvt
 * Descript: Send the link key notify event via callback.
 * Params:
 *        @pData[IN]--The event data.
 *        @dataLen[IN]--The data length.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_aclEstablishEvt(uint08 *pData, uint16_t dataLen);

/******************************************************************************
 * Function: bth_send_extFeatureChangedEvt
 * Descript:
 * Params:
 *     @handle[IN]--The handle of acl link.
 *     @peerFeature[IN]--Refer BTH_L2CAP_EXT_FEATURE_ENUM.
 *     @mineFeature[IN]--Refer BTH_L2CAP_EXT_FEATURE_ENUM.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_extFeatureChangedEvt(uint16 handle, uint32 peerFeature, uint32 mineFeature);


/******************************************************************************
 * Function: bth_send_aclGetNameReqEvt
 * Descript:
 * Params:
 *     @handle[IN]--The handle of acl link.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_aclGetNameReqEvt(uint16 handle, uint08 status, uint08 btaddr[6], uint08 *pName, uint08 nameLen);

/******************************************************************************
 * Function: bth_send_aclGetRssiReqEvt
 * Descript:
 * Params:
 *     @handle[IN]--The handle of acl link.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_aclGetRssiReqEvt(uint16 handle, uint08 status, uint08 rssi);

/******************************************************************************
 * Function: bth_send_set_scan_cmd_CompleteEvt
 * Descript:
 * Params:
 *     @handle[IN]--the resut of set bt scan.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_send_set_scan_cmd_CompleteEvt(uint08 status);

#endif // BTH_EVENT_H
