/********************************************************************************************************
 * @file	tlkmdi_audhfpag.h
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
#ifndef TLKMDI_BTAG_H
#define TLKMDI_BTAG_H


#define TLKMDI_BTAG_TIMEOUT        100000 //us
#define TLKMDI_BTAG_TIMEOUT_MS     (TLKMDI_BTAG_TIMEOUT/5000)	// 20ms
#define TLKMDI_BTAG_RING_TIMEOUT   (3000000/(TLKMDI_BTAG_TIMEOUT/2))

#define TLKMDI_AG_CALL_SETUP			  (1 && TLK_MDI_AUDAG_ENABLE)
#define TLKMDI_PHONE_NUMBER_MAX_LEN       64
#define TLKMDI_PHONE_SETUP_MAX_NUMB       2

typedef struct{
	uint08 state;
	uint08 busys;
	uint08 pBtAddr[6];
	uint32_t feature;
	tlkapi_timer_t timer;

}tlkmdi_btag_ctrl_t;

typedef enum{
	TLKMDI_BTAG_ATTR_NONE = 0x00,
	TLKMDI_BTAG_ATTR_CONN = 0x01,
	TLKMDI_BTAG_ATTR_CIEV = 0x02,
	TLKMDI_BTAG_ATTR_CLIP = 0x04,
	TLKMDI_BTAG_ATTR_NREC = 0x08,
	TLKMDI_BTAG_ATTR_CMEE = 0x10,
}TLKMDI_HFPAG_ATTR_ENUM;

typedef enum{
	TLKMDI_BTAG_BUSY_NONE = 0x00,
	TLKMDI_BTAG_BUSY_OPEN = 0x02,
	TLKMDI_BTAG_BUSY_RING = 0x04,
	TLKMDI_BTAG_BUSY_CHG_CODEC  = 0x08,
	TLKMDI_BTAG_BUSY_CHG_MICVOL = 0x10,
	TLKMDI_BTAG_BUSY_CHG_SPKVOL = 0x20,
}TLKMDI_HFPAG_BUSY_ENUM;

typedef enum{
	TLKMDI_BTAG_INDIC_NONE = 0x00,
	TLKMDI_BTAG_INDIC_SERVICE,
	TLKMDI_BTAG_INDIC_CALL,
	TLKMDI_BTAG_INDIC_CALLSETUP,
	TLKMDI_BTAG_INDIC_BATTCHG,
	TLKMDI_BTAG_INDIC_SIGNAL,
	TLKMDI_BTAG_INDIC_ROAM,
	TLKMDI_BTAG_INDIC_CALLHELD,
}TLKMDI_HFPAG_INDICATOR_ENUM;

typedef enum{
	TLKMDI_BTAG_SERVICE_NON_EXIST = 0x00,
	TLKMDI_BTAG_SERVICE_EXIST,
}TLKMDI_HFPAG_SERVICE_STATE_ENUM;

typedef enum{
	TLKMDI_BTAG_CALL_NOT_PROGRESS = 0x00,
	TLKMDI_BTAG_CALL_IN_PROGRESS,
}TLKMDI_HFPAG_CALLS_STATE_ENUM;

typedef enum{
	TLKMDI_BTAG_CALLSETUP_NOT_SETUP = 0x00,
	TLKMDI_BTAG_CALLSETUP_INCOMMING,
	TLKMDI_BTAG_CALLSETUP_OUTGOING,
	TLKMDI_BTAG_CALLSETUP_REMOTE_ALERTED,
}TLKMDI_HFPAG_CALLSETUP_STATE_ENUM;

typedef enum{
	TLKMDI_BTAG_CALLHELD_NOT_HELD = 0x00,
	TLKMDI_BTAG_CALLHELD_ACTIVE_HELD_SWAPPED,	// The AG has both an active AND a held call
	TLKMDI_BTAG_CALLHELD_HOLD_NO_ACTIVE,		// Call on hold, no active call
}TLKMDI_HFPAG_CALLHELD_STATE_ENUM;

typedef enum{
	TLKMDI_BTAG_SSIGNAL_STRENGTH_0 = 0x00,
	TLKMDI_BTAG_SSIGNAL_STRENGTH_20P,
	TLKMDI_BTAG_SSIGNAL_STRENGTH_40P,
	TLKMDI_BTAG_SSIGNAL_STRENGTH_60P,
	TLKMDI_BTAG_SSIGNAL_STRENGTH_80P,
	TLKMDI_BTAG_SSIGNAL_STRENGTH_100P,
}TLKMDI_HFPAG_SIGNAL_STATE_ENUM;

typedef enum{
	TLKMDI_BTAG_ROAM_NOT_ACTIVE = 0x00,
	TLKMDI_BTAG_ROAM_ACTIVE,
}TLKMDI_HFPAG_ROAM_STATE_ENUM;

typedef enum{
	TLKMDI_BTAG_BATTERY_CHARGE_0 = 0x00,
	TLKMDI_BTAG_BATTERY_CHARGE_20P,
	TLKMDI_BTAG_BATTERY_CHARGE_40P,
	TLKMDI_BTAG_BATTERY_CHARGE_60P,
	TLKMDI_BTAG_BATTERY_CHARGE_80P,
	TLKMDI_BTAG_BATTERY_CHARGE_100P,
}TLKMDI_HFPAG_BATTERY_STATE_ENUM;

typedef enum{
	TLKMDI_BTAG_STATE_IDLE = 0,
	TLKMDI_BTAG_STATE_CONNECTING,
	TLKMDI_BTAG_STATE_CONNECTED,
	TLKMDI_BTAG_STATE_DISCONNECT,
}TLKMDI_BTAG_STATE_ENUM;

typedef enum{
	TLKMDI_HFPAG_CALL_STATE_NONE = 0,
	TLKMDI_HFPAG_CALL_STATE_PAUSED,
	TLKMDI_HFPAG_CALL_STATE_OPENED,
	TLKMDI_HFPAG_CALL_STATE_RINGING,
	TLKMDI_HFPAG_CALL_STATE_CONNECT,
}TLKMDI_HFPAG_CALL_STATE_ENUM;

typedef enum {
    TLKMDI_HFP_CALL_HOLD_NO_HELD            = 0x30,
    TLKMDI_HFP_CALL_HOLD_HELD_AND_ACTIVE    = 0x31,
    TLKMDI_HFP_CALL_HOLD_HELD_AND_NO_ACTIVE = 0x32,
}TLKMDI_HFP_CALL_HOLD_ENUM;
	
typedef enum {
    TLKMDI_HFP_CALL_STATE_HUNGUP = 0x30,
    TLKMDI_HFP_CALL_STATE_ACTIVE = 0x31,
}TLKMDI_HFP_CALL_STATE_ENUM;


typedef enum {
    TLKMDI_HFP_CALL_SETUP_IDLE     = 0x30,
    TLKMDI_HFP_CALL_SETUP_INCOMING = 0x31,
    TLKMDI_HFP_CALL_SETUP_OUTGOING = 0x32,
    TLKMDI_HFP_CALL_SETUP_ALERTING = 0x33,
}TLKMDI_HFP_CALL_SETUP_ENUM;

typedef enum{
	TLKMDI_HFP_CLCC_DIR_OUTGOING = 0x30,
	TLKMDI_HFP_CLCC_DIR_INCOMING = 0x31,
}TLKMDI_HFP_CLCC_DIR_ENUM;
	
typedef enum{
	TLKMDI_HFP_CLCC_MODE_VOICE = 0x30,
	TLKMDI_HFP_CLCC_MODE_DATA  = 0x31,
	TLKMDI_HFP_CLCC_MODE_FAX   = 0x32,
}TLKMDI_HFP_CLCC_MODE_ENUM;
	
typedef enum{
	TLKMDI_HFP_CLCC_MPTY_NOT_MULTI_PARTY = 0x30,
	TLKMDI_HFP_CLCC_MPTY_MULTI_PARTY     = 0x31,
}TLKMDI_HFP_CLCC_MPTY_ENUM;
	
typedef enum{
	TLKMDI_HFP_CLCC_STATUS_NONE     = 0x00,
	TLKMDI_HFP_CLCC_STATUS_ACTIVE   = 0x30, //Active
	TLKMDI_HFP_CLCC_STATUS_HELD     = 0x31, //Held
	TLKMDI_HFP_CLCC_STATUS_DIALING  = 0x32, //Dialing (outgoing calls only)
	TLKMDI_HFP_CLCC_STATUS_ALERTING = 0x33, //Alerting (outgoing calls only)
	TLKMDI_HFP_CLCC_STATUS_INCOMING = 0x34, //Incoming (incoming calls only)
	TLKMDI_HFP_CLCC_STATUS_WAITING  = 0x35, //Waiting (incoming calls only)
	TLKMDI_HFP_CLCC_STATUS_CALLHELD = 0x36, //Call held by Response and Hold
}TLKMDI_HFP_CLCC_STATUS_ENUM;
	
typedef struct{
	uint08 status;    //Refer TLKMDI_HFP_CLCC_STATUS_ENUM.
	uint08 numbLen;
	uint08 callDir;   //Refer TLKMDI_HFP_CLCC_DIR_ENUM.
	uint08 callSetup; //Refer TLKMDI_HFP_CALL_SETUP_ENUM.
	uint08 callIsHold;
	uint08 number[TLKMDI_PHONE_NUMBER_MAX_LEN];
}tlkmdi_hfpag_unit_t;

typedef struct{
	uint16 busys;
	uint08 callHold;  //Refer TLKMDI_HFP_CALL_HOLD_ENUM.
	uint08 callState; //Refer TLKMDI_HFP_CALL_STATE_ENUM.
	tlkmdi_hfpag_unit_t unit[TLKMDI_PHONE_SETUP_MAX_NUMB];
	tlkapi_timer_t timer;
}tlkmdi_hfpag_ctrl_t;


/******************************************************************************
 * Function: tlkmdi_bthfpag_init
 * Descript: Query the status of the phone in the mobile phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_init(void);

/******************************************************************************
 * Function: tlkmdi_bthfpag_createSco
 * Descript: Query the status of the phone in the mobile phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_createSco(uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmdi_bthfag_disconnSco
 * Descript: Query the status of the phone in the mobile phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfag_disconnSco(uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmdi_bthfpag_reset
 * Descript: Query the status of the phone in the mobile phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_reset(void);

/******************************************************************************
 * Function: tlkmdi_bthfpag_sendMute
 * Descript: Query the status of the phone in the mobile phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_sendMute(uint08 *pBtAddr,  uint08 micSpk, uint08 enable);

/******************************************************************************
 * Function: tlkmdi_bthfpag_sendVgm
 * Descript: Query the status of the phone in the mobile phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_setVgm(uint08 *pBtAddr, uint08 volume);

/******************************************************************************
 * Function: tlkmdi_bthfpag_getVgs
 * Descript: Query the status of the phone in the mobile phone.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_setVgs(uint08 *pBtAddr, uint08 volume);

/******************************************************************************
 * Function: tlkmdi_bthfpag_insertCall
 * Descript: Define a passthrough data interface.
 * Params:
 *     @aclHandle[IN]--The acl handle.
 *     @pData[IN]--Data to be sent.
 *     @dataLen[IN]--Length of data to be sent.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_insertCall(uint08 *pNumber, uint08 numbLen, uint08 isIncoming);

/******************************************************************************
 * Function: tlkmdi_bthfpag_removeCall
 * Descript: Define a passthrough data interface.
 * Params:
 *     @aclHandle[IN]--The acl handle.
 *     @pData[IN]--Data to be sent.
 *     @dataLen[IN]--Length of data to be sent.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_removeCall(uint08 *pNumber, uint08 numbLen);

/******************************************************************************
 * Function: tlkmdi_bthfpag_activeCall
 * Descript: Define a passthrough data interface.
 * Params:
 *     @aclHandle[IN]--The acl handle.
 *     @pData[IN]--Data to be sent.
 *     @dataLen[IN]--Length of data to be sent.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_activeCall(uint08 *pNumber, uint08 numbLen);

/******************************************************************************
 * Function: tlkmdi_bthfpag_hungupCall
 * Descript: Define a passthrough data interface.
 * Params:
 *     @aclHandle[IN]--The acl handle.
 *     @pData[IN]--Data to be sent.
 *     @dataLen[IN]--Length of data to be sent.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_hungupCall(void);

/******************************************************************************
 * Function: HFP Trigger Reject a call in wait and active status interface
 * Descript: Defines trigger the hfp reject a Call in wait and keep active.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_rejectWaitAndKeepActive(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger accept a call in wait and active status interface
 * Descript: Defines trigger the hfp accept a Call in wait and keep active.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_acceptWaitAndHoldActive(uint16 aclHandle);

/******************************************************************************
 * Function: HFP Trigger hungup a call in active and restart another call in hold.
 * Descript: Defines trigger the hfp hungup a Call in active and resume a hold call.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
*******************************************************************************/
int tlkmdi_bthfpag_hungUpActiveAndResumeHold(uint16 aclHandle);


#endif //TLKMDI_BTHFP_H

