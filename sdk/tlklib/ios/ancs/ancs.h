/********************************************************************************************************
 * @file	ancs.h
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
#ifndef ANCS_H_
#define ANCS_H_
///type

#include "ancsDef.h"

void ancsInit(u16 attsIdType);
void ancsEventHandle ();
void latency_turn_off_once();
void ancsStackCallback(u8 *p);
void ancsFuncSetEnable(u8 enable);

///controlled the incoming call by bracelet
///input : accept-->1 to send a positive action command to the phone, accept the incoming call
///accept-->0 to send a negative action command to the phone, reject the incoming call
void ancs_handleIncomingCall(u8 accept);

///output : ancs_incomingCallHandleState
///-->INCOMING_CALL_NO_NEED_REMOVED:current incoming call never be canceled by user
///-->INCOMING_CALL_NEED_REMOVED:current incoming call was canceled by user
unsigned char ancs_isIncomingCallNeedRemoved();

u8 ancsFuncIsEn();
bool ancs_findAncsService();
ancs_service_state_t ancsGetConnState();

//setting the attributes ID for obtain the news detail when received the CMD Id from the IOS.
void ancs_setAttsIdType(u16 attsIdType);

u16	ancs_getAttsIdType();

//setting news received status after handle the ancs news
void ancs_setNewsReceivedStatus(u8 en);

u8	ancs_getNewsReceivedStatus();

/*
 * get current CMD Id from IOS
 * return : pointing to the struct_type ancs_notifySrc_type_t
 */
u8* ancs_getLastNotifyCmdId(u8 arLastCmdId[], u8 arLen);

/*
 * get the news APP Id,comes from QQ or others application.
 * return : pointing to the struct_type ancs_attId_AppId_Title
 */
u8* ancs_getNewsAppId();

/*
 * get the news title
 * return : pointing to the struct_type ancs_attId_AppId_Title
 */
u8* ancs_getNewsTitle();

/*
 * get the news sub title
 * return : pointing to the struct_type ancs_attId_AppId_Title
 */
u8* ancs_getNewsSubTitle();

/*
 * get the news message,it always include the text comes from the application
 * return : pointing to the struct_type ancs_attId_Message
 */
u8* ancs_getNewsMessage();

/*
 * get the news message size, two bytes in string.
 * return : pointing to the struct_type ancs_attId_Message_Size
 */
u8* ancs_getNewsMessageSize();

/*
 * get the date of the news, the date was the time which IOS received, not the bracelet received.
 * return : pointing to the struct_type ancs_attId_Date
 */
u8* ancs_getNewsDate();

/*
 * get the positive action label
 * return : pointing to the struct_type ancs_attId_Positive_Action_Label
 */
u8* ancs_getNewsPositiveActionLabel();

/*
 * get the negative action label
 * return : pointing to the struct_type ancs_attId_Negative_Action_Label
 */
u8* ancs_getNewsNegativeActionLabel();

/*
 * get current ancs library version
 *
 */
u8 *getAncsVersion();

/*
 * translate the packet format with BLE-Stack interface as ANCS old packet format
 */
rf_packet_l2cap_req_t_special_ancs* ancs_attPktTrans(u8 *p);

#endif /* ANCS_H_ */
