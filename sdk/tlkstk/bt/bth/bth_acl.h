/********************************************************************************************************
 * @file	bth_acl.h
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
#ifndef BTH_ACL_H
#define BTH_ACL_H


#define BTH_ACL_TIMEOUT             (100000)
#define BTH_ACL_TIMEOUT_MS          (100)
#define BTH_ACL_CONN_TIMEOUT        (5000000 / BTH_ACL_TIMEOUT)
#define BTH_ACL_RSW_TIMEOUT         (3000000 / BTH_ACL_TIMEOUT)
#define BTH_ACL_DISC_TIMEOUT        (5000000 / BTH_ACL_TIMEOUT)
#define BTH_ACL_CANCEL_TIMEOUT      (2000000 / BTH_ACL_TIMEOUT)
#define BTH_ACL_AUTH_SECR_TIMEOUT   (3000000 / BTH_ACL_TIMEOUT)
#define BTH_ACL_EXIT_TIMEOUT        (5000000 / BTH_ACL_TIMEOUT)
#define BTH_ACL_ENTER_SNIFF_TIMEOUT (1500000 / BTH_ACL_TIMEOUT)
#define BTH_ACL_LEAVE_SNIFF_TIMEOUT (200000 / BTH_ACL_TIMEOUT)

typedef enum {
    BTH_ACL_ATTR_NONE         = 0x00,
    BTH_ACL_ATTR_CONN         = 0x01,
    BTH_ACL_ATTR_PAIR_DONE    = 0x02,
    BTH_ACL_ATTR_AUTH_DONE    = 0x04,
    BTH_ACL_ATTR_ENC_DONE     = 0x08,
    BTH_ACL_ATTR_CONN_EVT     = 0x80,
    BTH_ACL_ATTR_LK_NOTI_DONE = 0x10,
    BTH_ACL_ATTR_ALLOW_SNIFF  = 0x20,
} BTH_ACL_ATTRS_ENUM;
typedef enum {
    BTH_ACL_BUSY_NONE               = 0x0000,
    BTH_ACL_BUSY_SEND_PAGE_TIMEOUT  = 0x8000,
    BTH_ACL_BUSY_SEND_CONN_REQUEST  = 0x0001,
    BTH_ACL_BUSY_SEND_CONN_CANCEL   = 0x0002,
    BTH_ACL_BUSY_SEND_CONN_ACCEPT   = 0x0004,
    BTH_ACL_BUSY_SEND_LINK_POLICY   = 0x0008,
    BTH_ACL_BUSY_SEND_SWITCH_ROLE   = 0x0010,
    BTH_ACL_BUSY_SEND_AUTH_REQUEST  = 0x0020,
    BTH_ACL_BUSY_SEND_CONN_ENCRYPT  = 0x0040,
    BTH_ACL_BUSY_SEND_DISC_REQUEST  = 0x0080,
    BTH_ACL_BUSY_SEND_PINCODE_REPLY = 0x0100,
    BTH_ACL_BUSY_SEND_LINKKEY_REPLY = 0x0200,
    BTH_ACL_BUSY_SEND_PINCODE_NEG   = 0x0400,
    BTH_ACL_BUSY_SEND_LINKKEY_NEG   = 0x0800,
    BTH_ACL_BUSY_SEND_IOCAP_REPLY   = 0x1000,
    BTH_ACL_BUSY_SEND_USERCFM_REPLY = 0x2000,
} BTH_ACL_BUSYS_ENUM;
typedef enum {
    BTH_ACL_FLAG_NONE                = 0x0000,
    BTH_ACL_FLAG_WAIT_CONN_CANCEL    = 0x0001,
    BTH_ACL_FLAG_WAIT_CONN_RESULT    = 0x0002,
    BTH_ACL_FLAG_WAIT_AUTH_RESULT    = 0x0004,
    BTH_ACL_FLAG_WAIT_CRYP_RESULT    = 0x0008,
    BTH_ACL_FLAG_WAIT_DISC_RESULT    = 0x0010,
    BTH_ACL_FLAG_WAIT_UNSNIFF_RESULT = 0x0020,
    BTH_ACL_FLAG_WAIT_GET_NAME       = 0x0040,
    BTH_ACL_FLAG_CONN_CANCEL_BY_USER = 0x0080,
} BTH_ACL_FLAGS_ENUM;
typedef enum {
    BTH_ACL_OTH_BUSY_NONE              = 0x00,
    BTH_ACL_OTH_BUSY_SEND_INFO_REQ     = 0x01,
    BTH_ACL_OTH_BUSY_SEND_INFO_RSP     = 0x02,
    BTH_ACL_OTH_BUSY_SEND_SNIFF_REQ    = 0x04,
    BTH_ACL_OTH_BUSY_SEND_UNSNIFF_REQ  = 0x08,
    BTH_ACL_OTH_BUSY_ENTER_SNIFF       = 0x10,
    BTH_ACL_OTH_BUSY_LEAVE_SNIFF       = 0x20,
    BTH_ACL_OTH_BUSY_SEND_GET_NAME_REQ = 0x40,
} BTH_ACL_OTH_BUSYS_ENUM;

typedef enum {
    BTH_ACL_SNIFF_BUSY_NONE = 0,
    BTH_ACL_SNIFF_BUSY_SCO  = 0x01,
} BTH_ACL_SNIFF_BUSYS_ENUM;

/******************************************************************************
 * Function: bth_acl_setWaitTimeout
 * Descript: Set the time from establishment of the ACL to connection timeout.
 * Params:
 *     @timeout[IN]--The time of connection timeout. Unit-ms, Range[3000~15000].
 * Return: None.
 *******************************************************************************/
void bth_acl_setWaitTimeout(uint16 timeout);

/******************************************************************************
 * Function: bth_acl_setInitRole
 * Descript: This interface be used to set the role of acl link.
 * Params: @btaddr[IN]--The device bt address.
 *         @initRole[IN]--The role of acl link.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_acl_setInitRole(uint08 btaddr[6], uint08 initRole);

/******************************************************************************
 * Function: bth_acl_setPinCode
 * Descript: This interface be used to set the pin code to do auth.
 * Params: @btaddr[IN]--The device bt address.
 *         @pinCode[IN]--The pin code.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_acl_setPinCode(uint08 btaddr[6], uint08 pinCode[6]);

/******************************************************************************
 * Function: bth_acl_setLinkKey
 * Descript: This interface be used to set the link key to auth.
 * Params: @btaddr[IN]--The device bt address.
 *         @linkKey[IN]--The link key.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_acl_setLinkKey(uint08 btaddr[6], uint08 linkKey[16]);

/******************************************************************************
 * Function: bth_acl_setSniffParam
 * Descript: Set sniff parameters.
 * Params:
 *     @maxInterval[IN]--The maximum interval. Default:HCI_CFG_SNIFF_MAX_INTERVAL.
 *     @minInterval[IN]--The minimum interval. Default:HCI_CFG_SNIFF_MIN_INTERVAL.
 *     @attempt[IN]--The sniff attempt times. Default:HCI_CFG_SNIFF_ATTEMPT.
 *     @timeout[IN]--The sniff time. Default:HCI_CFG_SNIFF_TIMEOUT.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_acl_setSniffParam(uint16 maxInterval, uint16 minInterval, uint16 attempt, uint16 timeout);

/******************************************************************************
 * Function: bth_acl_connect
 * Descript: This interface be used to start the acl link set up.
 * Params: @btaddr[IN]--The device bt address.
 *         @devClass[IN]--The device class.
 *         @initRole[IN]--The role of acl link.
 *         @timeout[IN]--The acl setup timeout. Unit:ms. Range:3000~3600000
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_acl_connect(uint08 btaddr[6], uint32 devClass, uint08 initRole, uint32 timeout); // timeout - ms

/******************************************************************************
 * Function: bth_acl_disconn
 * Descript: This interface be used to tear up the acl link which specify
 *           by aclhandle.
 * Params: @aclHandle[IN]--The acl link handle.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_acl_disconn(uint16 aclHandle, uint08 reason);

/******************************************************************************
 * Function: bth_acl_destroy
 * Descript: This interface be used to release the acl link resource which
 *           specify by aclhandle.
 * Params: @aclHandle[IN]--The acl link handle.
 * Return: None.
 *******************************************************************************/
void bth_acl_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: bth_acl_connectCancel
 * Descript: This interface be used to cancel the acl link connect which
 *           specify by bt address.
 * Params: @btaddr[IN]--The peer device address.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_acl_connectCancel(uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_acl_disconnByAddr
 * Descript: This interface be used to disconnect the acl link which
 *           specify by bt address.
 * Params:
 *     @btaddr[IN]--The peer device address.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_acl_disconnByAddr(uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_acl_enableSniff
 * Descript: This interface be used to enable sniff setting.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @enable[IN]--True-enable, false-disable.
 * Return: TLK_ENONE is set success, others means failure.
 * Note:
 *     1.Before using "bth_acl_enterSniff", you need to call "bth_acl_enableSniff"
 *       to enable a sniff mechanism.
 *******************************************************************************/
int bth_acl_enableSniff(uint16 aclHandle, bool enable);

/******************************************************************************
 * Function: bth_acl_systemManageSniff
 * Descript: This interface determine whether sniff exit and entry are managed
 * 			 by the system(Default system management).
 * Params: @enable[IN]--True-system, false-user.
 * Return: null.
*******************************************************************************/
void bth_acl_systemManageSniff(bool enable);

/******************************************************************************
 * Function: bth_acl_enterSniff
 * Descript:
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @isForce[IN]--Whether to force a switchover to enter mode when a sniff
 *         is in the exit state.
 * Return: NONE.
 * Note:
 *     1.Before using "bth_acl_enterSniff", you need to call "bth_acl_enableSniff"
 *       to enable a sniff mechanism.
 *     2.By default, the system enables a sniff mechanism and automatically
 *       manages in-and-out latency after encryption is complete.
 *     3.Users should be cautious about calling the interface with isForce=true,
 *       because it will clear the bth_acl_leaveSniff parameter.
 *******************************************************************************/
void bth_acl_enterSniff(uint16 aclHandle, bool isForce);

/******************************************************************************
 * Function: bth_acl_leaveSniff
 * Descript: Makes the system exit sniff. The BusyTime parameter can be used to
 *           preset the busy time of the system, so as to reasonably control
 *           the transmission of system data.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @busyTime[IN]--sniff estimated busy time after exiting. Unit:ms.
 * Return: NONE.
 * Note:
 *     1.Different from bth_acl_sendUnSniffReq, bth_acl_enterSniff is automatically
 *       managed by the system. It is recommended to sniff bth_acl_leaveSniff
 *       instead of bth_acl_sendUnSniffReq.
 *******************************************************************************/
void bth_acl_leaveSniff(uint16 aclHandle, uint16 busyTime);

/******************************************************************************
 * Function: bth_acl_getSniffBusy
 * Descript: Get the sniffer busy's flags.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @pBusyFlags[OUT]--Refer BTH_ACL_SNIFF_BUSYS_ENUM.
 * Return: TLK_ENONE is set success, others means failure.
 * Note: NONE.
 *******************************************************************************/
int bth_acl_getSniffBusy(uint16 aclHandle, uint08 *pBusyFlags);

/******************************************************************************
 * Function: bth_acl_setSniffBusy
 * Descript: Set the sniffer busy's flags. Once the busy location is up, the
 *           software does not sniff until it is cleared.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @busyFlags[IN]--Refer BTH_ACL_SNIFF_BUSYS_ENUM.
 * Return: NONE.
 *******************************************************************************/
void bth_acl_setSniffBusy(uint16 aclHandle, uint08 busyFlags);

/******************************************************************************
 * Function: bth_acl_setSniffBusy
 * Descript: Clear the sniffer busy's flags. Once the busy location is up, the
 *           software does not sniff until it is cleared.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @busyFlags[IN]--Refer BTH_ACL_SNIFF_BUSYS_ENUM.
 * Return: NONE.
 *******************************************************************************/
void bth_acl_clsSniffBusy(uint16 aclHandle, uint08 busyFlags);

/******************************************************************************
 * Function: bth_acl_clearSniffPolicy
 * Descript:
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @mask[IN]--
 * Return: NONE.
 *******************************************************************************/
void bth_acl_clsLinkPolicyBit(uint16 aclHandle, uint08 mask);
void bth_acl_setLinkPolicyBit(uint16 aclHandle, uint08 mask);

/******************************************************************************
 * Function: bth_acl_connCancelComplete
 * Descript: This function be used to handle the connect cancel event coming
 *           from peer device.
 * Params:
 *         @status[IN]--The status from HCI
 *         @btaddr[IN]--The peer device address.
 * Return: None.
 *******************************************************************************/
void bth_acl_connCancelComplete(uint08 status, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_hci_linkkeyReqReplyComplete
 * Descript: This function be used to handle the request linlkey repley cmd event coming
 *           from peer device.
 * Params:
 *         @status[IN]--The status from HCI
 *         @btaddr[IN]--The peer device address.
 * Return: None.
 *******************************************************************************/
void bth_hci_linkkeyReqReplyComplete(uint08 status, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_acl_connectRequestEvt
 * Descript: This function be used to handle the request connect event from
 *           peer device.
 * Params:
 *         @btaddr[IN]--The peer device address.
 *         @devClass[IN]--The device class type.
 * Return: None.
 *******************************************************************************/
void bth_acl_connectRequestEvt(uint08 btaddr[6], uint32 devClass);


/******************************************************************************
 * Function: bth_acl_connectCompleteEvt
 * Descript: This interface be used to handle the acl link connect complete event.
 * Params:
 *        @status[IN]--The status of the acl link.
 *        @handle[IN]--The handle of the acl link.
 *        @btaddr[IN]--The peer device address.
 *        @isEncrypt[IN]--is encrypt or not.
 * Return: None.
 *******************************************************************************/
void bth_acl_connectCompleteEvt(uint08 status, uint16 handle, uint08 btaddr[6], uint08 isEncrypt);


/******************************************************************************
 * Function: bth_acl_disconnCompleteEvt
 * Descript: This interface be used to handle the acl link disconnect
 *           complete event.
 * Params:
 *        @handle[IN]--The handle of acl link.
 *        @reason[IN]--The reason of disconnect.
 * Return: None.
 *******************************************************************************/
void bth_acl_disconnCompleteEvt(uint16 handle, uint08 reason);

/******************************************************************************
 * Function: bth_acl_roleChangeEvt
 * Descript: This interface be used to handle the acl link role switch event.
 * Params:
 *        @btaddr[IN]--The peer device address.
 *        @status[IN]--The status of acl link.
 *        @newRole[IN]--The role of acl link.
 * Return: None.
 *******************************************************************************/
void bth_acl_roleChangeEvt(uint08 btaddr[6], uint08 status, uint08 newRole);

/******************************************************************************
 * Function: bth_acl_modeChangeEvt
 * Descript: This interface be used to handle the acl link mode change event.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @status[IN]--The status of acl link.
 *        @curMode[IN]--The current mode of acl link.
 * Return: None.
 *******************************************************************************/
void bth_acl_modeChangeEvt(uint16 handle, uint08 status, uint08 curMode);
/******************************************************************************
 * Function: bth_acl_simplePairingCompleteEvt
 * Descript: This interface be used to handle the acl link simplepairing event.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @status[IN]--The status of acl link.
 * Return: None.
 *******************************************************************************/
void bth_acl_simplePairingCompleteEvt(uint08 btaddr[6], uint08 status);
/******************************************************************************
 * Function: bth_acl_authenCompleteEvt
 * Descript: This interface be used to handle the acl link authenticate event.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @status[IN]--The status of acl link.
 * Return: None.
 *******************************************************************************/
void bth_acl_authenCompleteEvt(uint16 handle, uint08 status);

/******************************************************************************
 * Function: bth_acl_encryptChangeEvt
 * Descript: This interface be used to handle the acl link encrypt change event.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @status[IN]--The status of acl link.
 *        @enable[IN]--enable or not
 * Return: None.
 *******************************************************************************/
void bth_acl_encryptChangeEvt(uint16 handle, uint08 status, uint08 enable);


/******************************************************************************
 * Function: bth_acl_pinCodeReqEvt
 * Descript: This interface be used to handle pincode request event.
 * Params:
 *        @btaddr[IN]--The bt address.
 * Return: None.
 *******************************************************************************/
void bth_acl_pinCodeReqEvt(uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_acl_linkKeyReqEvt
 * Descript: This interface be used to handle link key request event.
 * Params:
 *        @btaddr[IN]--The bt address.
 * Return: None.
 *******************************************************************************/
void bth_acl_linkKeyReqEvt(uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_acl_ioCapReqEvt
 * Descript: This interface be used to handle io capability request event.
 * Params:
 *        @btaddr[IN]--The bt address.
 * Return: None.
 *******************************************************************************/
void bth_acl_ioCapReqEvt(uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_acl_userCfmReqEvt
 * Descript: This interface be used to handle user confirm the numeric request event.
 * Params:
 *        @btaddr[IN]--The bt address.
 *        @number[IN]--The number use to pair.
 * Return: None.
 *******************************************************************************/
void bth_acl_userCfmReqEvt(uint08 btaddr[6], uint32 number);

void bth_acl_getNameEvt(uint08 status, uint08 btaddr[6], uint08 *pName, uint08 nameLen);

/******************************************************************************
 * Function: bth_acl_sendInfoRsp
 * Descript: Send l2cap info response.
 * Params:
 *     @handle[IN]--The acl link handle.
 *     @identify[IN]--The identify.
 *     @infoType[IN]--The info type.
 *     @result[IN]--The result.
 *     @pData[IN]--The payload.
 *     @dataLen[IN]--The payload length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
void bth_acl_sendInfoRsp(uint16 handle, uint08 identify, uint16 infoType, uint16 result, uint08 *pData, uint16 dataLen);

void bth_acl_sendGetNameReq(uint16 handle);

int bth_acl_setPeerExtFeature(uint16 handle, uint32 extFeature);
int bth_acl_getPeerExtFeature(uint16 handle, uint32 *pExtFeature);
int bth_acl_acl_establish(uint8_t *p, int len);


#endif // BTH_ACL_H
