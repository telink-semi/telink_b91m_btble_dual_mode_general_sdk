/********************************************************************************************************
 * @file	amsDemo.c
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
#include "amsDemo.h"

/*
 * 用于发出获取当前歌曲状态信息包括artist,album,title,duration的请求
 */
void amsDemo_getTrackInfo(){
	ams_sentEntityIdPkt_param param;
	param.entityId 	= AMS_ENTITY_ID_TRACK;
	param.attIds[0]	= AMS_TRACK_ATT_ID_ARTIST;
	param.attIds[1]	= AMS_TRACK_ATT_ID_ALBUM;
	param.attIds[2]	= AMS_TRACK_ATT_ID_TITLE;
	param.attIds[3]	= AMS_TRACK_ATT_ID_DURATION;
	param.len		= 4;

	ams_sendEntityIdRequestPkt(param);
}

/*
 * 用于获取当前播放媒体队列状态的请求,
 * 经测试,主要返回AMS_QUEUE_ATT_ID_SHUFFLE_MODE,AMS_QUEUE_ATT_ID_REPEAT_MODE的状态
 */
void amsDemo_getQueueInfo(){
	ams_sentEntityIdPkt_param param;
	param.entityId 	= AMS_ENTITY_ID_QUEUE;
	param.attIds[0]	= AMS_QUEUE_ATT_ID_INDEX;
	param.attIds[1]	= AMS_QUEUE_ATT_ID_COUNT;
	param.attIds[2]	= AMS_QUEUE_ATT_ID_SHUFFLE_MODE;
	param.attIds[3]	= AMS_QUEUE_ATT_ID_REPEAT_MODE;
	param.len		= 4;

	ams_sendEntityIdRequestPkt(param);
}
/*
 * 用于发出控制多媒体的请求包,
 * 通过发送remote command id控制多媒体,
 * remote command id参考amsDef.h里的枚举ams_remoteID_t
 * 当前函数只是一个demo函数,仅用于演示,因为包含枚举所有值
 */
void amsDemo_sendRemoteIdControl(){
	static u8 addition = 0;
	ams_sendRemoteCmdIdRequestPkt(AMS_REMOTE_CMD_ID_PLAY + addition);
	addition++;
	if(addition % 13 == 0){
		addition = 0;
	}
}

/*************************************************
 * 函数说明:用于AMS建立服务及AMS能接收到的通知处理
 * 并处理整合为相对应的详细消息，放在main_loop()中
 * 调用
 * 执行
 * 输入:无
 * 输出:无
 * ***********************************************/
void ams_mainLoopTask(){

	_attribute_data_retention_ static u32 tick_amsTask = 1;
	extern u16 ams_getConnInterval();
	u32 connInterval = ams_getConnInterval() * 1250;

	if((tick_amsTask & 1) && (!clock_time_exceed(tick_amsTask, 1 * connInterval))){
		return;
	}else{
		tick_amsTask = clock_time() | 1;
	}

	amsEventHandle();///必须优先运行

	if(ams_getInfoUpdateFlag() == 1){//当有新的媒体信息从MS那边push notify过来时，该标志会标志为1
		ams_playerPlaybackInfo_t myPlaybackInfo = ams_getPlayerPlaybackInfo();

		tlkapi_trace(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"------------AMS------------");
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"playback state", &myPlaybackInfo.state, 1);

		///shuffle mode跟repeat mode只有在初始的时候会给个状态，后续手机状态有变化，暂时无法获取，但是设备端可以通过cmd id去改变状态

		//getting current player shuffle mode state
		u8 val = ams_getQueueShuffleModeState();
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"shuffle mode state", &val, 1);

		//getting current player repeat mode state
		val = ams_getQueueRepeatMoedState();
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"repeat mode state", &val, 1);

		//getting current track artist string
		ams_storageFormat trackArtist = ams_getTrackArtist();
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"track artist is",trackArtist.data, trackArtist.len);

		// getting current track album string
		ams_storageFormat trackAlbum = ams_getTrackAlbum();
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"track album is",trackAlbum.data, trackAlbum.len);

		//getting current track title string
		ams_storageFormat trackTitle = ams_getTrackTitle();
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"track title is",trackTitle.data, trackTitle.len);

		//getting current track duration
		ams_storageFormat trackDuration = ams_getTrackDuration();
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"track duration is",trackDuration.data, trackDuration.len);

		//getting current player APP name
		ams_storageFormat appName = ams_getPlayerAppName();
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"app name is",appName.data, appName.len);

		// getting current volume value
		ams_storageFormat vol = ams_getPlayerVolume();
		tlkapi_array(TLKMMI_LEMGR_IOS_FLAG, TLKMMI_LEMGR_IOS_SIGN,"volume is",vol.data, vol.len);

		ams_setInfoUpdateFlag(0);//每次信息变更后，必须把该标志清零，不然下次有信息变更时无法从此得知
	}
}





