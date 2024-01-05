/********************************************************************************************************
 * @file	ancsDef.h
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
#ifndef ANCSCONSTANTDEF_H_
#define ANCSCONSTANTDEF_H_


#include "../ios_srvComm/ios_srvComm.h"
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"

#define MAX_NOTIFY_DATA_LIST_NUM	4

#define ATT_CMD_APP_ID_LENGTH							48///APP ID的长度设置，32或以上
#define ATT_CMD_TITLE_LENGTH							48///APP Title的长度设置，32或以上
#define ATT_CMD_SUB_TITLE_LENGTH						48///APP SubTitle的长度设置，32或以上
#define ATT_CMD_MESSAGE_LENGTH							256///APP Message的长度设置，32或以上，占比越大，ram开销越?//@03/29/2022:The max length was 256 bytes?
#define	ATT_CMD_MESSAGE_SIZE_LENGTH						2///Message Date大小长度，固定2
#define ATT_CMD_DATE_LENGTH								15///Message Date大小，固定15，格式：yyyymmdd'T'hhmmss
#define ATT_CMD_POSITIVE_ACTION_LABEL_LENGTH			48///固定长度，主动消息
#define ATT_CMD_NEGATIVE_ACTION_LABEL_LENGTH			48///固定长度，主动消息

//#define DATA_MAX_LEN				0x20
#define ATT_ID_NUM					0x08///该值必须与上面设置的APP attribute个数及长度对应，对APP attribute不懂的，自行百度ANCS相关知识
//#define MAX_DATA_SRC_NUM			(((ATT_CMD_TITLE_LENGTH + 3) * (ATT_ID_NUM - 1)) + ATT_CMD_MESSAGE_LENGTH + 30)
#define MAX_DATA_SRC_NUM			((3 * ATT_ID_NUM) + ATT_CMD_APP_ID_LENGTH + ATT_CMD_TITLE_LENGTH \
									+ ATT_CMD_SUB_TITLE_LENGTH + ATT_CMD_MESSAGE_LENGTH + ATT_CMD_MESSAGE_SIZE_LENGTH \
									+ ATT_CMD_DATE_LENGTH + ATT_CMD_POSITIVE_ACTION_LABEL_LENGTH \
									+ ATT_CMD_NEGATIVE_ACTION_LABEL_LENGTH + 30)///最大长度，不允许修改

typedef enum{
	BUNDLE_ID_NULL = 0,
	BUNDLE_ID_CALL = 1,
	BUNDLE_ID_WECHAT,
	BUNDLE_ID_SMS,
	BUNDLE_ID_QQ,
	BUNDLE_ID_TWITTER,
	//BUNDLE_ID_FACEBOOK_MSGR,
	BUNDLE_ID_WHATSAPP,
	BUNDLE_ID_INSTAGRAM,
	//BUNDLE_ID_SNAPCHAT,
	//BUNDLE_ID_GOOGLE_TERMINAL,
	//BUNDLE_ID_YOUTUBE,
	BUNDLE_ID_SKYPE,
	BUNDLE_ID_FACEBOOK,
	//BUNDLE_ID_LINKEDIN,
	BUNDLE_ID_LINE,
	//BUNDLE_ID_VENMO,
	//BUNDLE_ID_PINTEREST,
}BUNDLE_ID_INDEX;///对应的APP ID枚举，建议枚举值的顺序按appBundleId的顺序排序，这样方便程序索引，参考demo.c的做法

typedef enum{
	ANCS_ATTS_ID_APP_IDENTIFIER 		= BIT(0),
	ANCS_ATTS_ID_TITLE					= BIT(1),
	ANCS_ATTS_ID_SUB_TITLE				= BIT(2),
	ANCS_ATTS_ID_MESSAGE				= BIT(3),
	ANCS_ATTS_ID_MESSAGE_SIZE			= BIT(4),
	ANCS_ATTS_ID_DATE					= BIT(5),
	ANCS_ATTS_ID_POSITIVE_ACTION_LABEL	= BIT(6),
	ANCS_ATTS_ID_NEGATIVE_ACTION_LABEL	= BIT(7),
}ANCS_ATTS_ID_TYPE;///内部APP attribute标志位，无须修改


typedef enum{
	ANCS_NO_STATE = 0,					//init state , not allow to send packet about ancs, expect adv packet.
	ANCS_GET_START_HANDLE,			//have send packet about get ancs starting handle
	ANCS_GET_START_HANDLE_FINISH,		//finish get start handle
	ANCS_WRITE_NOTISRC_CCC,				// write CCC finish , and wait provider send information.
	ANCS_CONNECTION_ESTABLISHED,
}ancs_service_state_t;///ANCS在连接初始用于建立ANCS服务时会用到的各种状态

typedef enum{
	CategroyConnErr = -1,
	CategroyIDOther = 0,
	CategroyIDIncomingCall = 1,
	CategroyIDMissCall = 2,
	CategroyIDVoiceMail = 3,
	CategroyIDSocial	= 4,
	CategroyIDSchedule,
	CategroyIDEmail,
	CategroyIDNews,
	CategroyIDHealthyAndFitness,
	CategroyIDBunissAndFinance,
	CategroyIDLocation,
	CategroyIDEntertainment,
}ancs_categroyID_t;///ANCS状态参数，详见ANCS服务介绍

typedef enum{
	EventIDNotificationAdded,///表示当前的命令是一个添加新通知命令
	EventIDNotificationModified,///表示之前的特定命令需要修改
	EventIDNotificationRemoved,///表示之前的特定命令需要移除为无效命令
}ancs_eventID_t;

typedef enum{
	EventFlagSilent = BIT(0),
	EventFlagImportant = BIT(1),
}ancs_event_flags_t;///消息类型是重要还是不重要的

typedef enum{
	ATT_ID_APP_IDENTIFIER = 0,
	ATT_ID_TITLE,
	ATT_ID_SUB_TITLE,
	ATT_ID_MESSAGE,
	ATT_ID_MESSAGE_SIZE,
	ATT_ID_DATE,
	ATT_ID_POSITIVE_ACTION_LABEL,
	ATT_ID_NEGATTIVE_ACTION_LABEL,
	ATT_ID_RESERVED,
}ancs_notifyAttIdValues;///APP attribute，目前根据官方文档就只有以上这么多，时间2020_05_29_15_25

typedef enum{
	CMD_ID_GET_NOTIFY_ATTS = 0,
	CMD_ID_GET_APP_ATTS,
	CMD_ID_PERFORM_NOTIFY_ACTION,
}ancs_cmdIdValues;///CMD ID的类型，目前获取消息只用到第一个枚举

typedef enum{
	CMD_RSP_PKT_START = BIT(0),
	CMD_RSP_PKT_END,
}ancs_cmdSequence_flag;///内部流程处理标识

typedef enum{
	ACTION_ID_POSITIVE 	= 0,
	ACTION_ID_NEGATIVE	= 1,
}ancs_actionIdValue;///内部流程处理标识

typedef enum{
	ANCS_SET_NO_NEW_NOTICE	= 0,
	ANCS_HAVE_NEW_NOTICE	= 1,
//	ANCS_HAVE_NEW_NOTICE = 1,
//	ANCS_SET_NO_NEW_NOTICE = 0,
}ancs_newsReceivedStatus;///内部流程处理标识

typedef enum{
	ANCS_ATT_TYPE_LITE_INCLUDE 	= 0,
	ANCS_ATT_TYPE_ALL_INCLUDE 	= 1,
}ancs_attTypeSelect;

typedef enum{
	INCOMING_CALL_NO_NEED_REMOVED	= 0,
	INCOMING_CALL_NEED_REMOVED		= 1,
}ancs_incomingCallHandleState;

typedef enum{
	CALL_EV_INCOMING_CALL			= 1,
	CALL_EV_MASTER_HANG_UP_CALL		= 2,
}ancs_call_ev_t;
#if 0
typedef struct{///7bytes
	ancs_eventID_t 			EventID;
	ancs_event_flags_t 		EventFlags;
	ancs_categroyID_t 		CategroyID;
	u8 						CategroyCnt;
	u8 						NotifyUID[4];
}ancs_notifySrc_type_t;///收到ANCS通知的CMD ID固定格式，参考ANCS官方文档
#else
typedef struct{///8bytes
	u8			 			EventID;
	u8				 		EventFlags;
	s8				 		CategroyID;
	u8 						CategroyCnt;
	u8 						NotifyUID[4];
}ancs_notifySrc_type_t;///收到ANCS通知的CMD ID固定格式，参考ANCS官方文档

#endif
typedef struct{
	u8 cmdId;
	u8 notifyUid[4];
	u8 attIds[14];///this array size can be varied
}ancs_getNotify_atts;///向IOS索求ANCS通知详情时固定格式，参考ANCS官方文档

typedef struct{
	u8 cmdId;

}ancs_getApp_atts;///暂无用到
/*************固定格式，参考ANCS官方文档，勿动***************/
typedef struct{
	u8 	attId;
	u16 len;
	u8 	data[ATT_CMD_TITLE_LENGTH];
}ancs_attId_AppId_Title;///appId,title,subTitle

typedef struct{
	u8 	attId;
	u16 len;
	u8 	data[ATT_CMD_MESSAGE_LENGTH];
}ancs_attId_Message;///message

typedef struct{
	u8 	attId;
	u16 len;
	u8 	data[ATT_CMD_MESSAGE_SIZE_LENGTH];
}ancs_attId_Message_Size;

typedef struct{
	u8 	attId;
	u16 len;
	u8 	data[ATT_CMD_DATE_LENGTH];
}ancs_attId_Date;

typedef struct{
	u8 	attId;
	u16 len;
	u8 	data[ATT_CMD_POSITIVE_ACTION_LABEL_LENGTH];
}ancs_attId_Positive_Action_Label;

typedef struct{
	u8 	attId;
	u16 len;
	u8 	data[ATT_CMD_NEGATIVE_ACTION_LABEL_LENGTH];
}ancs_attId_Negative_Action_Label;

typedef struct{
	u8 cmdId;
	u8 notifyUid[4];
	u8 actionIdValue;
}ancs_performNotifyAction;
/*************固定格式，参考ANCS官方文档，勿动***************/

typedef struct{
	ancs_attId_AppId_Title 				appId;
	ancs_attId_AppId_Title 				title;
	ancs_attId_AppId_Title				subTitle;
	ancs_attId_Message					message;
	ancs_attId_Message_Size				messageSize;
	ancs_attId_Date						date;
	ancs_attId_Positive_Action_Label	positiveActionLabel;
	ancs_attId_Negative_Action_Label	negativeActionLabel;
}ancs_notifyAtts;///具体的消息整合后存储结构，不建议修改

/////////////////////////ATT HANDLE RELEVANT STRUCTURE DEFINITION///////////////////////////////
/**************************ANCS 服务建立流程用到的各种内部设定 不许修改**************************************/
#define ANCS_NEWS_GET_NOTIFY_ATT		0x01
#define ANCS_NEWS_GET_APP_ATT			0x02

#define ANCS_SEND_INIT_WRITE_REQ		0x80
#define ANCS_SEND_CMD_REQ				0x40

#define ANCS_REQ_GET_NOTIFY_ATT			0x20
#define ANCS_REQ_GET_APP_ATT			0x10

typedef enum{
	OBTAIN_ATT_ANCS_SERVICE_DISABLE = 0,
	OBTAIN_ATT_READ_BY_TYPE,
	OBTAIN_ATT_FIND_INFO,
	OBTAIN_ATT_WRITE_NOTIFY_SRC_HANDLE,
	OBTAIN_ATT_ANCS_SERVICE_ENABLED,
}att_enableAncsFlow;

typedef struct {
	u16	startHandle;
	u16	endingHandle;
} ancs_att_db_uuid128_t;			//20-byte

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust
							//with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u16 handle;
	ancs_getNotify_atts attIdInfo;///12
}rf_pkt_write_req;


typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust
							//with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u16 handle;
	ancs_performNotifyAction cmdInfo;
}rf_pkt_perform_action;

typedef struct{
//	u32 dma_len;
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  data[1];
}rf_packet_l2cap_req_t_special_ancs;

/////////////////////////ATT HANDLE RELEVANT STRUCTURE DEFINITION///////////////////////////////
typedef struct{
	u8 						ancs_func_valid;
	u8						ancs_dataSrcRcvNews;
	u8						ancs_notifySrcRcvNews;

	ancs_service_state_t  	ancs_system_state;
}ancs_state_flag;

typedef struct{
	u8 						i128;
	u8						hdDataSource;
	u8						writeReqFlow;
	u8						writeRspFlag;

	u8 						findInfoFlow;
	u8						hdNotifySource;
	u8						hdControlPoint;

	u16 					curStartHandle;
	u16 					curEndingHandle;

	ancs_att_db_uuid128_t	db128[ATT_DB_UUID128_NUM];
	ancs_att_db_uuid128_t	dbHandleAncsSrv;
}ancs_find_srv_flow;

typedef struct{
	u8 						notifyTblEndPtr;
	s32						cmdPktStartIdx;
	u16 					ancsRcvTmpBufLen;
	u32 					ancs_clrTick;
	ancs_notifyAtts 		curNotifyAttsIdTbl;
	ancs_notifySrc_type_t 	ancs_lastNotifySrcData;
	ancs_notifySrc_type_t 	ancs_phoneCallCmdIdCache;
	ancs_notifySrc_type_t 	notifyTbl[MAX_NOTIFY_DATA_LIST_NUM];
}ancs_notify_src_param;

typedef struct{
	u8 						ancsStable;
	u8 						ancs_no_latency;
	u8 						ancs_dataSrcData[MAX_DATA_SRC_NUM];
	u32 					tick_lastNotifyRev;
	u32 					tick_reqTimeOut;
	u32 					num_appBundleId;///4
	rf_pkt_write_req 		getNotifyAtt_writeReq;///25
}ancs_data_src_param;

typedef struct{
	u16						ancs_attsIdType;
	u8						ancs_numOfAttsIdType;
	u8						ancs_newsReceived;
}ancs_interface_param;

/**************************ANCS 服务建立流程用到的各种内部设定 不许修改**************************************/

extern const u8 appId_null[];///该空APP ID为占位ID,必须存在,不然程序无法清楚用户添加多少个APP ID.
extern const u8 * appBundleId[];///用于存放各个APP ID地址的table,用于底层根据用户先前定好的APP ID索引对应的APP ID枚举,并返回对应的枚举号
///////////////////////////////////Phone Call State////////////////////////////////////////
extern const u8 utf8_activeCall[];
extern const u8 utf8_incomingCall[];

#endif /* ANCSCONSTANTDEF_H_ */
