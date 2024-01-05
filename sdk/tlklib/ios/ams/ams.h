/********************************************************************************************************
 * @file	ams.h
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
#ifndef _AMS_H_
#define _AMS_H_
///type

#include "amsDef.h"
///all the string format was 'UTF-8'
void amsInit(u16 attsIdType);
void amsEventHandle ();
void amsStackCallback(u8 *p);
void amsFuncSetEnable(u8 enable);

u8 amsFuncIsEn();
bool ams_findAmsService();
ams_service_state_t amsGetConnState();

/*
 * get current ams library version
 *
 */
u8 *getAmsVersion();

/*
 * translate the packet format with BLE-Stack interface as AMS old packet format
 */
rf_packet_l2cap_req_t_special_ams* ams_attPktTrans(u8 *p);

/*
 * send the entity id packet to MS
 * reference the AMS protocol to send or the demo code
 * param.entityId and param.attsIds can be followed as: entityId + attributeId or entityId + attributeId + attributeId...
 */
void ams_sendEntityIdRequestPkt(ams_sentEntityIdPkt_param param);

/*
 * send the remote command id packet to MS
 * reference the AMS protocol to send or the demo code
 */
void ams_sendRemoteCmdIdRequestPkt(u8 remoteCmdId);


/*
 * getting the flag about info update,
 * for notice that info would updated
 * after it received the newest notice from MS
 */
u8 ams_getInfoUpdateFlag();

/*
 * setting the info update flag,
 * for notice that info would updated
 * after it received the newest notice from MS
 */
void ams_setInfoUpdateFlag(u8 flag);

/*
 * getting current player shuffle mode state
 */
u8 ams_getQueueShuffleModeState();

/*
 * getting current player repeat mode state
 */
u8 ams_getQueueRepeatMoedState();

/*
 * getting current track artist string
 */
ams_storageFormat ams_getTrackArtist();

/*
 * getting current track album string
 */
ams_storageFormat ams_getTrackAlbum();

/*
 * getting current track title string
 */
ams_storageFormat ams_getTrackTitle();

/*
 * getting current track duration
 */
ams_storageFormat ams_getTrackDuration();

/*
 * getting current player APP name
 */
ams_storageFormat ams_getPlayerAppName();

/*
 * getting current volume value
 */
ams_storageFormat ams_getPlayerVolume();

/*
 * getting the player information, current player application name, playing state or others
 */
ams_playerPlaybackInfo_t ams_getPlayerPlaybackInfo();

#endif /* _AMS_H_ */
