/********************************************************************************************************
 * @file	tlkmdi_file.h
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
#ifndef TLKMDI_FILE_H
#define TLKMDI_FILE_H

#if (TLK_MDI_FILE_ENABLE)





#define TLKMDI_FILE_TIMEOUT            100000 //us
#define TLKMDI_FILE_TIMEOUT_MS         100
#define TLKMDI_FILE_IDLE_TIMEOUT       (3000/TLKMDI_FILE_TIMEOUT_MS)//(3000/TLKMDI_FILE_TIMEOUT_MS) //3s

#define TLKMDI_FILE_NAME_SIZE           64

#define TLKMDI_FILE_TRAN_RECV_NUMB      2
#define TLKMDI_FILE_TRAN_SEND_NUMB      0

#define TLKMDI_FILE_AUTH_SCH_NUMB       2
#define TLKMDI_FILE_CRYP_SCH_NUMB       2
#define TLKMDI_FILE_FAST_SCH_NUMB       2



typedef enum{
	TLKMDI_FILE_TRAN_ROLE_NONE = 0,
	TLKMDI_FILE_TRAN_ROLE_CLIENT,
	TLKMDI_FILE_TRAN_ROLE_SERVER,
}TLKMDI_FILE_TRAN_ROLE_ENUM;
typedef enum{
	TLKMDI_FILE_OPTCHN_NONE = 0x00,
	TLKMDI_FILE_OPTCHN_SERIAL,
	TLKMDI_FILE_OPTCHN_BT_SPP,
	TLKMDI_FILE_OPTCHN_BT_ATT,
	TLKMDI_FILE_OPTCHN_LE_ATT,
}TLKMDI_FILE_OPTCHN_ENUM;
	
typedef enum{
	TLKMDI_FILE_STATE_IDLE = 0x00,
	TLKMDI_FILE_STATE_START,
	TLKMDI_FILE_STATE_CRYPT,
	TLKMDI_FILE_STATE_CLOSE,
	TLKMDI_FILE_STATE_STREAM,
}TLKMDI_FILE_STATE_ENUM;

typedef enum{
	TLKMDI_FILE_BUSY_NONE               = 0x0000,
	TLKMDI_FILE_BUSY_SEND_START_TRAN    = 0x0001,
	TLKMDI_FILE_BUSY_SEND_START_AUTH    = 0x0002,
	TLKMDI_FILE_BUSY_SEND_CRYPT_SHAKE   = 0x0004,
	TLKMDI_FILE_BUSY_SEND_SET_DATA_SCH  = 0x0008,
	TLKMDI_FILE_BUSY_SEND_SET_FILE_NAME = 0x0010,
	TLKMDI_FILE_BUSY_SEND_FAST_START    = 0x0020,
	TLKMDI_FILE_BUSY_SEND_START_DATA    = 0x0040,
	TLKMDI_FILE_BUSY_SEND_CLOSE_TRAN    = 0x0080,
	TLKMDI_FILE_BUSY_SEND_CLOSE_TRAN_EVT = 0x0100,
	TLKMDI_FILE_BUSY_SEND_SYNC_SHAKE_EVT = 0x0200,
	TLKMDI_FILE_BUSY_SEND_SYNC_STALL_EVT = 0x0400,
}TLKMDI_FILE_BUSYS_ENUM;
typedef enum{
	TLKMDI_FILE_FLAG_NONE               = 0x0000,
	TLKMDI_FILE_FLAG_DATA               = 0x8000,
	TLKMDI_FILE_FLAG_START              = 0x4000,
	TLKMDI_FILE_FLAG_COMPELE            = 0x2000,
	
	TLKMDI_FILE_FLAG_WAIT_START_TRAN    = 0x0001,
	TLKMDI_FILE_FLAG_WAIT_START_AUTH    = 0x0002,
	TLKMDI_FILE_FLAG_WAIT_CRYPT_SHAKE   = 0x0004,
	TLKMDI_FILE_FLAG_WAIT_SET_DATA_SCH  = 0x0008,
	TLKMDI_FILE_FLAG_WAIT_SET_FILE_NAME = 0x0010,
	TLKMDI_FILE_FLAG_WAIT_FAST_START    = 0x0020,
	TLKMDI_FILE_FLAG_WAIT_START_DATA    = 0x0040,
	TLKMDI_FILE_FLAG_WAIT_WAIT_DATA     = 0x0080,
}TLKMDI_FILE_FLAGS_ENUM;
typedef enum{
	TLKMDI_FILE_ATTR_NONE = 0x00,
	TLKMDI_FILE_ATTR_AUTH = 0x01,
	TLKMDI_FILE_ATTR_CRYP = 0x02,
	TLKMDI_FILE_ATTR_COMP = 0x04,
	TLKMDI_FILE_ATTR_DECRYPT = 0x80,
	TLKMDI_FILE_ATTR_UNCOMPR = 0x40,
}TLKMDI_FILE_ATTRS_ENUM;

typedef enum{
	TLKMDI_FILE_TRAN_STATUS_SUCCESS = 0x00,
	TLKMDI_FILE_TRAN_STATUS_FAILURE = 0x01,
	TLKMDI_FILE_TRAN_STATUS_PAUSED  = 0x02,
}TLKMDI_FILE_TRAN_STATUS_ENUM;
typedef enum{
	TLKMDI_FILE_CRYP_STAGE_GEN_INI_SIGN = 0x01,
	TLKMDI_FILE_CRYP_STAGE_GEN_ACP_SIGN = 0x02,
	TLKMDI_FILE_CRYP_STAGE_GEN_CRYPT_CODE = 0x03,
}TLKMDI_FILE_CRYP_STAGE_ENUM;

typedef enum{
	TLKMDI_FILE_PARAM_TYPE_NONE = 0,
	TLKMDI_FILE_PARAM_TYPE_GET_PORT,
	TLKMDI_FILE_PARAM_TYPE_GET_FAST,
	TLKMDI_FILE_PARAM_TYPE_GET_START,
}TLKMDI_FILE_PARAM_TYPE_ENUM;


typedef struct tlkmdi_file_unit_s tlkmdi_file_unit_t;
typedef int(*TlkmdiFileAuthSignFunc)(uint16 schCode, tlkmdi_file_unit_t *pUnit);
typedef int(*TlkmdiFileAuthCrypFunc)(uint16 schCode, tlkmdi_file_unit_t *pUnit, bool isEnc, uint08 *pData, uint16 dataLen);
typedef int(*TlkmdiFileCrypSignFunc)(uint16 schCode, tlkmdi_file_unit_t *pUnit, uint08 stage);
typedef int(*TlkmdiFileCrypCrypFunc)(uint16 schCode, tlkmdi_file_unit_t *pUnit, bool isEnc, uint08 *pData, uint16 dataLen);
typedef int(*TlkmdiFileFastSignFunc)(uint16 schCode, tlkmdi_file_unit_t *pUnit);

typedef int(*TlkmdiFileStartFunc)(tlkmdi_file_unit_t *pUnit, bool isFast);
typedef int(*TlkmdiFileCloseFunc)(tlkmdi_file_unit_t *pUnit, uint08 status); //0x00-Success, 0x01-Failure, 0x02-Paused
typedef int(*TlkmdiFileParamFunc)(tlkmdi_file_unit_t *pUnit, uint08 paramType, uint08 *pParam, uint08 paramLen);
typedef int(*TlkmdiFileLoadFunc)(tlkmdi_file_unit_t *pUnit, uint32 offset, uint08 *pBuff, uint16 buffLen);
typedef int(*TlkmdiFileSaveFunc)(tlkmdi_file_unit_t *pUnit, uint32 offset, uint08 *pData, uint16 dataLen);
typedef int(*TlkmdiFileSendFunc)(uint08 optChn, uint16 handle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen);


typedef struct{
	uint16 schCode;
	TlkmdiFileAuthSignFunc sign;
	TlkmdiFileAuthCrypFunc cryp;
}tlkmdi_file_auth_t;
typedef struct{
	uint16 schCode;
	TlkmdiFileCrypSignFunc sign;
	TlkmdiFileCrypCrypFunc cryp;
}tlkmdi_file_cryp_t;
typedef struct{
	uint16 schCode;
	TlkmdiFileFastSignFunc sign;
}tlkmdi_file_fast_t;
typedef struct{
	TlkmdiFileStartFunc start;
	TlkmdiFileCloseFunc close;
	TlkmdiFileParamFunc param;
	TlkmdiFileLoadFunc  load;
	TlkmdiFileSaveFunc  save;
	TlkmdiFileSendFunc  send;
}tlkmdi_file_infs_t;

typedef struct{
	uint08 dataPort;
}tlkmdi_file_portParam_t;
typedef struct{
	uint08 intvNumb;
	uint16 tranIntv;
	uint16 unitLens;
}tlkmdi_file_startParam_t;

typedef struct{ //32+80=112
	uint08 status;
	uint08 datPos; //Data Position: 1-Inner,
	uint08 optChn;
	uint08 cprSch; //Compress Scheme
	uint08 digSch; //Digest Scheme
	uint08 tranRole;
	uint08 dataPort;
	uint08 waitMask;
	uint16 fileType;
	uint16 unitLens;
	uint16 tranIntv;
	uint16 authSch;
	uint16 fastSch;
	uint16 cryptSch;
	uint32 dataAddr;
	uint32 dealSize; //RecvSize or SendSize
	uint32 fileSize;
	uint32 fileVers;
	uint08 authCode[16];
	uint08 fileSign[16];
	uint08 realSign[16];
	uint08 crypCode[16];
	uint08 fastCode[16];
}tlkmdi_file_saveParam_t, tlkmdi_file_fastParam_t;


struct tlkmdi_file_unit_s{
	uint08 state;
	uint08 attrs;
	uint16 flags;
	uint16 busys;
	uint08 csign;
	uint08 serial;
	uint08 optChn;
	uint08 status;
	uint08 cprSch; //Compress Scheme
	uint08 digSch; //Digest Scheme
	uint16 handle;
	uint16 fileType;
	uint08 tranRole;
	uint08 filePort;
	uint08 dataPort;
	uint08 waitMask;
	uint08 idleTime;
	uint08 nameLens; //Length of File Name
	uint16 unitLens;
	uint16 tranIntv;
	uint16 authSch;
	uint16 fastSch;
	uint16 cryptSch;
	uint32 packNumb;
	uint32 dealSize; //RecvSize or SendSize
	uint32 fileSize;
	uint32 fileVers;
	uint08 authCode[16];
	uint08 fileName[TLKMDI_FILE_NAME_SIZE];
	union{
		struct{
			uint08 iniRand0[16];
			uint08 acpRand0[16];
			uint08 iniRand1[16];
			uint08 acpRand1[16];
		}rand;
		struct{
			uint08 iniCrypt[16];
			uint08 acpCrypt[16];
		}sign;
		struct{
			uint08 fileSign[16];
			uint08 realSign[16];
			uint08 crypCode[16];
			uint08 fastCode[16];
		}code;
	}param;
	union{
		uint32 crc32;
	}digest;
	tlkapi_timer_t timer;
};

typedef struct{
	uint08 usrAuthCode[16];
	uint08 usrCrypCode[16];
	tlkmdi_file_infs_t recvInfs;
	#if (TLKMDI_FILE_TRAN_SEND_NUMB != 0)
	tlkmdi_file_infs_t sendInfs;
	#endif
	tlkmdi_file_auth_t auth[TLKMDI_FILE_AUTH_SCH_NUMB];
	tlkmdi_file_cryp_t cryp[TLKMDI_FILE_CRYP_SCH_NUMB];
	tlkmdi_file_fast_t fast[TLKMDI_FILE_FAST_SCH_NUMB];
	#if (TLKMDI_FILE_TRAN_RECV_NUMB != 0)
	tlkmdi_file_unit_t recvUnit[TLKMDI_FILE_TRAN_RECV_NUMB];
	#endif
	#if (TLKMDI_FILE_TRAN_SEND_NUMB != 0)
	tlkmdi_file_unit_t sendUnit[TLKMDI_FILE_TRAN_SEND_NUMB];
	#endif
}tlkmdi_file_ctrl_t;


int  tlkmdi_file_init(void);


void tlkmdi_file_setAuthCode(uint08 authCode[16]);
void tlkmdi_file_setCrypCode(uint08 crypCode[16]);

void tlkmdi_file_setAuthSch(uint16 schCode, TlkmdiFileAuthSignFunc sign, TlkmdiFileAuthCrypFunc cryp);
void tlkmdi_file_setCrypSch(uint16 schCode, TlkmdiFileCrypSignFunc sign, TlkmdiFileCrypCrypFunc cryp);
void tlkmdi_file_setFastSch(uint16 schCode, TlkmdiFileFastSignFunc sign);

void tlkmdi_file_setRecvPort(tlkmdi_file_unit_t *pUnit, uint08 dataPort);

int tlkmdi_file_closeTrans(tlkmdi_file_unit_t *pUnit);

int tlkmdi_file_getRecvSaveParam(tlkmdi_file_unit_t *pUnit, tlkmdi_file_fastParam_t *pParam);
int tlkmdi_file_setRecvPortParam(tlkmdi_file_unit_t *pUnit, tlkmdi_file_portParam_t *pParam);
int tlkmdi_file_setRecvFastParam(tlkmdi_file_unit_t *pUnit, tlkmdi_file_fastParam_t *pParam);
int tlkmdi_file_setRecvStartParam(tlkmdi_file_unit_t *pUnit, tlkmdi_file_startParam_t *pParam);

int tlkmdi_file_enableDataDecode(tlkmdi_file_unit_t *pUnit, bool isDecrypt, bool isUncompress);

void tlkmdi_file_setRecvInfs(TlkmdiFileStartFunc start, TlkmdiFileCloseFunc close,
	 TlkmdiFileParamFunc param, TlkmdiFileSaveFunc save, TlkmdiFileSendFunc send);
void tlkmdi_file_setSendInfs(TlkmdiFileStartFunc start, TlkmdiFileCloseFunc close,
	 TlkmdiFileParamFunc param, TlkmdiFileLoadFunc load, TlkmdiFileSendFunc send);


void tlkmdi_file_recvHandler(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);




#endif //#if (TLK_MDI_FILE_ENABLE)

#endif //TLK_MDI_FILE_ENABLE

