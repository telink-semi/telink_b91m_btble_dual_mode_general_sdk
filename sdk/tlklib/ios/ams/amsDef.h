/********************************************************************************************************
 * @file	amsDef.h
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
#ifndef _AMS_DEF_H_
#define _AMS_DEF_H_


#include "../ios_srvComm/ios_srvComm.h"
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"
#define MAX_AMS_NOTIFY_DATA_LIST_NUM	4

//#define DATA_MAX_LEN				0x20
#define ATT_AMS_ID_NUM					0x03///该值必须与上面设置的APP attribute个数及长度对应，对APP attribute不懂的，自行百度AMS相关知识
//#define MAX_DATA_SRC_NUM			(((ATT_CMD_TITLE_LENGTH + 3) * (ATT_ID_NUM - 1)) + ATT_CMD_MESSAGE_LENGTH + 30)
#define MAX_AMS_DATA_SRC_NUM			((3 * ATT_AMS_ID_NUM) + 30)///最大长度，不允许修改
#define MAX_VALID_DATA_LEN				128
#define MAX_ENTITY_ATT_ID_NUM			4

typedef enum{
	AMS_DEMO_ID_TYP,
}AMS_ATTS_ID_TYPE;///内部APP attribute标志位，无须修改

typedef enum{
	AMS_NO_STATE = 0,					//init state , not allow to send packet about AMS, expect adv packet.
	AMS_GET_START_HANDLE,			//have send packet about get AMS starting handle
	AMS_GET_START_HANDLE_FINISH,		//finish get start handle
	AMS_WRITE_NOTISRC_CCC,				// write CCC finish , and wait provider send information.
	AMS_CONNECTION_ESTABLISHED,
}ams_service_state_t;///AMS在连接初始用于建立AMS服务时会用到的各种状态

typedef enum{
	AMS_REMOTE_CMD_ID_PLAY					= 0,
	AMS_REMOTE_CMD_ID_PAUSE					= 1,
	AMS_REMOTE_CMD_ID_TOGGLE_PLAY_PAUSE		= 2,
	AMS_REMOTE_CMD_ID_NEXT_TRACK			= 3,
	AMS_REMOTE_CMD_ID_PREVIOUS_TRACK		= 4,
	AMS_REMOTE_CMD_ID_VOLUME_UP				= 5,
	AMS_REMOTE_CMD_ID_VOLUME_DOWN			= 6,
	AMS_REMOTE_CMD_ID_ADVANCE_REPEAT_MODE	= 7,
	AMS_REMOTE_CMD_ID_ADVANCE_SHUFFLE_MODE	= 8,
	AMS_REMOTE_CMD_ID_SKIP_FORWARD			= 9,
	AMS_REMOTE_CMD_ID_SKIP_BACKWARD			= 10,
	AMS_REMOTE_CMD_ID_LIKE_TRACK			= 11,
	AMS_REMOTE_CMD_ID_DISLIKE_TRACK			= 12,
	AMS_REMOTE_CMD_ID_BOOKMARK_TRACK		= 13,
	AMS_REMOTE_CMD_ID_RESERVED				= 14,
}ams_remoteID_t;///AMS状态参数，详见AMS服务介绍

typedef enum{
	AMS_ENTITY_ID_PLAYER					= 0,
	AMS_ENTITY_ID_QUEUE						= 1,
	AMS_ENTITY_ID_TRACK						= 2,
	AMS_ENTITY_ID_RESERVED					= 3,
}ams_entityID_t;

typedef enum{
	AMS_ENTITY_UPDATE_FLAG_TRUNCATED		= (1 << 0),
	AMS_ENTITY_UPDATE_FLAG_RESERVED			= (1 << 1),
}ams_entity_update_flags_t;///

typedef enum{
	AMS_PLAYER_ATT_ID_NAME					= 0,
	AMS_PLAYER_ATT_ID_PLAYBACK_INFO			= 1,
	AMS_PLAYER_ATT_ID_VOLUME				= 2,
	AMS_PLAYER_ATT_ID_RESERVED				= 3,
}ams_playerAttributeId_val;///

typedef enum{
	AMS_QUEUE_ATT_ID_INDEX					= 0,
	AMS_QUEUE_ATT_ID_COUNT					= 1,
	AMS_QUEUE_ATT_ID_SHUFFLE_MODE			= 2,
	AMS_QUEUE_ATT_ID_REPEAT_MODE			= 3,
	AMS_QUEUE_ATT_ID_RESERVED				= 4,
}ams_queueAttributeId_val;///

typedef enum{
	AMS_SHUFFLE_MODE_OFF					= 0,
	AMS_SHUFFLE_MODE_ONE					= 1,
	AMS_SHUFFLE_MODE_ALL					= 2,
	AMS_SHUFFLE_MODE_RESERVED				= 3,
}ams_shuffleModeConstants;

typedef enum{
	AMS_REPEAT_MODE_OFF						= 0,
	AMS_REPEAT_MODE_ONE						= 1,
	AMS_REPEAT_MODE_ALL						= 2,
	AMS_REPEAT_MODE_RESERVED				= 3,
}ams_repeatModeConstants;

typedef enum{
	AMS_TRACK_ATT_ID_ARTIST					= 0,
	AMS_TRACK_ATT_ID_ALBUM					= 1,
	AMS_TRACK_ATT_ID_TITLE					= 2,
	AMS_TRACK_ATT_ID_DURATION				= 3,
	AMS_TRACK_ATT_ID_RESERVED				= 4,
}ams_trackAttributeId_val;

typedef enum{
	AMS_PLAYBACK_STATE_PAUSED				= 0,
	AMS_PLAYBACK_STATE_PLAYING				= 1,
	AMS_PLAYBACK_STATE_REWINDING			= 2,
	AMS_PLAYBACK_STATE_FORWARDING			= 3,
}ams_playbackState_t;

typedef enum{
	AMS_DEMO_CMD_SEQ,
}ams_cmdSequence_flag;///内部流程处理标识

typedef enum{
	AMS_ACTION_ID_VAL,
}ams_actionIdValue;///内部流程处理标识

typedef struct{///8bytes
	u8 demoTyp;
}ams_notifySrc_type_t;///收到AMS通知的CMD ID固定格式，参考AMS官方文档

typedef struct{
	u8 dat[MAX_ENTITY_ATT_ID_NUM * 2];
}ams_getNotify_atts;///向IOS索求AMS通知详情时固定格式，参考AMS官方文档


/////////////////////////ATT HANDLE RELEVANT STRUCTURE DEFINITION///////////////////////////////
/**************************AMS 服务建立流程用到的各种内部设定 不许修改**************************************/

#define AMS_NEWS_GET_NOTIFY_ATT			0x01

typedef enum{
	AMS_SET_PLAYER_ATT_ENABLE 			= 0x80,
	AMS_SET_QUEUE_ATT_ENABLE			= 0x40,
	AMS_SET_TRACK_ATT_ENABLE			= 0x20,
	AMS_SEND_INIT_WRITE_REQ				= 0x10,
}ams_enable_attribute_t;

typedef enum{
	OBTAIN_AMS_ATT_SERVICE_DISABLE = 0,
	OBTAIN_AMS_ATT_READ_BY_TYPE,
	OBTAIN_AMS_ATT_FIND_INFO,
	OBTAIN_AMS_ATT_WRITE_NOTIFY_SRC_HANDLE,
	OBTAIN_AMS_ATT_SERVICE_ENABLED,
}att_enableAmsFlow;

typedef struct {
	u16	startHandle;
	u16	endingHandle;
} ams_att_db_uuid128_t;			//20-byte


typedef struct{
//	u32 dma_len;
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  data[1];
}rf_packet_l2cap_req_t_special_ams;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust
							//with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u16 handle;
	u8 	attIdInfo[MAX_ENTITY_ATT_ID_NUM * 2];///8
}rf_pkt_write_req_ams;

/////////////////////////ATT HANDLE RELEVANT STRUCTURE DEFINITION///////////////////////////////
typedef struct{
	u8 						ams_func_valid;
	u8						ams_dataSrcRcvNews;
	u8						ams_notifySrcRcvNews;

	ams_service_state_t  	ams_system_state;
}ams_state_flag;

typedef struct{
	u8 						i128;
	u8						hdEntityAttribute;
	u8						writeReqFlow;
	u8						writeRspFlag;

	u8 						findInfoFlow;
	u8						hdEntityUpdate;
	u8						hdRemoteCmd;

	u16 					curStartHandle;
	u16 					curEndingHandle;

	ams_att_db_uuid128_t	db128[ATT_DB_UUID128_NUM];
	ams_att_db_uuid128_t	dbHandleAmsSrv;
}ams_find_srv_flow;

typedef struct{
	u8 len;
	u8 entityId;
	u8 attIds[MAX_ENTITY_ATT_ID_NUM];
}ams_sentEntityIdPkt_param;

typedef struct{
	u8	len;
	u8 	isTruncated;
	u8	data[MAX_VALID_DATA_LEN];
}ams_storageFormat;

typedef struct{
	ams_storageFormat			index;
	ams_storageFormat			totalCount;
	u8							shuffleMode;
	u8							repeatMode;
}ams_queueInfo_t;

typedef struct{
	u8	state;
	u8	rateIntPart;
	u8	rateFloatPart;
	u32 elapsedTimeIntPart;
	u32 elapsedTimeFloatPart;
}ams_playerPlaybackInfo_t;

typedef struct{
	ams_storageFormat			name;
	ams_storageFormat			playbackInfo;
	ams_storageFormat			volume;
}ams_playerInfo_t;

typedef struct{
	ams_storageFormat			artist;
	ams_storageFormat			album;
	ams_storageFormat			title;
	ams_storageFormat			duration;
}ams_trackInfo_t;

typedef struct{
	ams_trackInfo_t				trackInfo;
	ams_playerInfo_t			playerInfo;
	ams_queueInfo_t				queueInfo;
	ams_playerPlaybackInfo_t	playbackParam;
}ams_notifyEntity_param;

typedef struct{
	u8							flagInfoUpdate;
	u32 						tick_reqTimeOut;
	rf_pkt_write_req_ams		getNotifyAtt_writeReq;///25
}ams_data_src_param;



#endif /* _AMS_DEF_H_ */
