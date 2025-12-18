/********************************************************************************************************
 * @file	tlkats_auth.c
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_TEST_ATS_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmmi_atsDefine.h"
#include "tlkmmi_ats.h"
#include "tlkmmi_atsAuth.h"
#include "drivers.h"

extern int rand(void);
static void tlkmdi_ats_generateRandom(uint08 *pBuffer, uint16 buffLen);

static void tlkmmi_ats_recvShakeConfirmCmd(uint08 *pData, uint16 dataLen);
static void tlkmmi_ats_recvAuthenRequestCmd(uint08 *pData, uint16 dataLen);
static void tlkmmi_ats_recvAuthenConfirmCmd(uint08 *pData, uint16 dataLen);

static int tlkmmi_ats_sendShakeConfirmRsp(void);
static int tlkmmi_ats_sendAuthenRequestRsp(void);
static int tlkmmi_ats_sendAuthenConfirmRsp(bool isSucc);


static tlkmmi_ats_auth_t sTlkMmiAtsAuth;



int tlkmmi_ats_authInit(void)
{
	tmemset(&sTlkMmiAtsAuth, 0, sizeof(tlkmmi_ats_auth_t));
	tlkmdi_comm_enableAuthen(true);
	tlkmdi_comm_regResetCompleteCB(tlkmmi_ats_authReset);
	
	return TLK_ENONE;
}

int tlkmmi_ats_authReset(void)
{
	tmemset(&sTlkMmiAtsAuth, 0, sizeof(tlkmmi_ats_auth_t));

	sTlkMmiAtsAuth.waitFlags |= TLKMMI_ATS_AUTH_FLAG_AUTHEN_REQUEST;

	return TLK_ENONE;
}

void tlkmmi_ats_recvHandler(uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKPRT_COMM_CMDID_SHAKE_CONFIRM){
		tlkmmi_ats_recvShakeConfirmCmd(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUTHEN_REQUEST){
		tlkmmi_ats_recvAuthenRequestCmd(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUTHEN_CONFIRM){
		tlkmmi_ats_recvAuthenConfirmCmd(pData, dataLen);
	}
}

static void tlkmmi_ats_recvShakeConfirmCmd(uint08 *pData, uint16 dataLen)
{
	uint16 offset;
	uint32 version;
	uint32 vendorID;
    uint16 extInfoLen;
	uint08 infoLens;
	uint08 infoType;
	
	if(dataLen < 10){
		tlkapi_error(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "RecvCdShakeConfirmCmd: error format!");
		return;
	}

	offset = 0;
	version = (((uint32)pData[offset+3])<<24) | (((uint32)pData[offset+2])<<16) 
		| (((uint32)pData[offset+1])<<8) | ((uint32)pData[offset+0]);
	offset += 4;
	vendorID = (((uint32)pData[offset+3])<<24) | (((uint32)pData[offset+2])<<16) 
		| (((uint32)pData[offset+1])<<8) | ((uint32)pData[offset+0]);
	offset += 4;
	extInfoLen = (((uint16)pData[offset+1])<<8) | ((uint16)pData[offset+0]);
	offset += 2;
	if(dataLen < offset+extInfoLen){
		return;
	}
	if(version > TLKMMI_ATS_DEVICE_VERSION || vendorID != TLKMMI_ATS_CLIENT_VENDOR){
		return;
	}

	pData += 10;
	offset = 0;
	while(offset < extInfoLen)
	{
		infoLens = pData[offset++];
		if(infoLens == 0) continue;
		if(offset+infoLens > extInfoLen) break;

		infoType = pData[offset++];
		infoLens -= 1;
		if(infoType == TLKMMI_ATS_FIELD_ID_CLIENT_NAME)
		{
//			QString peerName = "";
//            if(infoLens != 0) peerName = TlkString::Utf8StrToQString((char*)(pData+offset), infoLens);
//			m_pNetSock->SetPeerName(peerName);
		}

		offset += infoLens;
	}

	tlkmmi_ats_sendShakeConfirmRsp();
}
static void tlkmmi_ats_recvAuthenRequestCmd(uint08 *pData, uint16 dataLen)
{
	int length;
	const char *pInfo;

	if(dataLen < 36){
		tlkapi_error(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_recvAuthenRequestCmd: error format!");
		return;
	}
	if((sTlkMmiAtsAuth.waitFlags & TLKMMI_ATS_AUTH_FLAG_AUTHEN_REQUEST) == 0){
		tlkapi_error(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_recvAuthenRequestCmd: unexpected!");
		return;
	}
    sTlkMmiAtsAuth.waitFlags &= ~TLKMMI_ATS_AUTH_FLAG_AUTHEN_REQUEST;
	
	memcpy(sTlkMmiAtsAuth.peerRandom, pData+4, 32);
	
	pInfo = TLKMMI_ATS_AUTHEN_INFO;
    length = strlen(pInfo);
    if(length > 32) length = 32;
	memset(sTlkMmiAtsAuth.mineConfirm, 0, 32);
	memcpy(sTlkMmiAtsAuth.mineConfirm, pInfo, length);
	tlkmdi_ats_generateRandom(sTlkMmiAtsAuth.mineRandom, 32);
	
	sTlkMmiAtsAuth.waitFlags |= TLKMMI_ATS_AUTH_FLAG_AUTHEN_CONFIRM;
	
	tlkmmi_ats_sendAuthenRequestRsp();
}
static void tlkmmi_ats_recvAuthenConfirmCmd(uint08 *pData, uint16 dataLen)
{
	int length;
	uint08 authStatus;
	const char *pInfo;

	if(dataLen < 33){
		tlkapi_error(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_recvAuthenConfirmCmd: error format!");
		return;
	}
	if((sTlkMmiAtsAuth.waitFlags & TLKMMI_ATS_AUTH_FLAG_AUTHEN_CONFIRM) == 0){
		tlkapi_error(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_recvAuthenConfirmCmd: unexpected!");
		return;
	}
    sTlkMmiAtsAuth.waitFlags &= ~TLKMMI_ATS_AUTH_FLAG_AUTHEN_CONFIRM;
	
	authStatus = pData[0];
	memcpy(sTlkMmiAtsAuth.peerConfirm, pData+1, 32);

    if(authStatus == 0x00){ //Success
        pInfo = TLKMMI_ATS_AUTHEN_INFO;
        length = strlen(pInfo);
        if(length > 32) length = 32;
        if(memcmp(sTlkMmiAtsAuth.peerConfirm, pInfo, length) != 0){
			authStatus = 0x01;
		}
    }
	
	if(authStatus != 0x00){ //Failure
		tlkmmi_ats_sendAuthenConfirmRsp(false);
		tlkmdi_comm_setAuthenStatus(TLKMDI_COMM_AUTH_STATUS_FAILURE);
	}else{ //Success
		tlkmmi_ats_sendAuthenConfirmRsp(true);
		tlkmdi_comm_setAuthenStatus(TLKMDI_COMM_AUTH_STATUS_SUCCESS);
	}
}


static int tlkmmi_ats_sendShakeConfirmRsp(void)
{
	uint08 fieldID;
	uint16 length;
	uint16 buffLen;
    uint32 version;
    uint32 vendorID;
	uint08 buffer[256];
    const char *pInfo;
	
    pInfo = TLKMMI_ATS_DEVICE_INFO;
	fieldID = TLKMMI_ATS_FIELD_ID_DEVICE_NAME;
    version = TLKMMI_ATS_DEVICE_VERSION;
    vendorID = TLKMMI_ATS_DEVICE_VENDOR;
    length = strlen(pInfo);
    if(12+4+strlen(pInfo) > 256){
		tlkapi_error(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_sendShakeConfirmRsp: overflow!");
    	return -TLK_EOVERFLOW;
    }

//	tlkapi_trace(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_sendShakeConfirmRsp");
//	tlkapi_trace(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_sendShakeConfirmRsp: %d!", sTlkMmiAtsAuth.sendNumb);
	
	buffLen = 0;
    buffer[buffLen++] = (version & 0XFF);
    buffer[buffLen++] = (version & 0XFF00) >> 8;
    buffer[buffLen++] = (version & 0XFF0000) >> 16;
    buffer[buffLen++] = (version & 0XFF000000) >> 24;
    buffer[buffLen++] = (vendorID & 0XFF);
    buffer[buffLen++] = (vendorID & 0XFF00) >> 8;
    buffer[buffLen++] = (vendorID & 0XFF0000) >> 16;
    buffer[buffLen++] = (vendorID & 0XFF000000) >> 24;
	buffer[buffLen++] = (sTlkMmiAtsAuth.mineRole & 0XFF);
	buffer[buffLen++] = (sTlkMmiAtsAuth.mineChID & 0XFF);
	buffer[buffLen++] = ((length+2) & 0xFF);
	buffer[buffLen++] = ((length+2) & 0xFF00) >> 8;
	buffer[buffLen++] = ((length+1) & 0xFF);
	buffer[buffLen++] = fieldID;
    memcpy(buffer+buffLen, pInfo, length);
	buffLen += length;
    return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_AUTHEN, TLKPRT_COMM_CMDID_SHAKE_CONFIRM,
		TLKPRT_COMM_RSP_STATUE_SUCCESS, 0, buffer, buffLen);
}
static int tlkmmi_ats_sendAuthenRequestRsp(void)
{
    uint16 buffLen;
    uint08 buffer[64];
	
//	tlkapi_trace(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_sendAuthenRequestRsp");
	
    buffLen = 0;
    memcpy(buffer+buffLen, sTlkMmiAtsAuth.mineRandom, 32);
    buffLen += 32;
	memcpy(buffer+buffLen, sTlkMmiAtsAuth.mineConfirm, 32);
    buffLen += 32;
    return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_AUTHEN, TLKPRT_COMM_CMDID_AUTHEN_REQUEST, 
		TLKPRT_COMM_RSP_STATUE_SUCCESS, 0, buffer, buffLen);
}
static int tlkmmi_ats_sendAuthenConfirmRsp(bool isSucc)
{
    uint16 buffLen;
    uint08 buffer[64];

//	tlkapi_trace(TLKMMI_ATS_DBG_FLAG, TLKMMI_ATS_DBG_SIGN, "tlkmmi_ats_sendAuthenConfirmRsp");
	
    buffLen = 0;
    if(isSucc) buffer[buffLen++] = 0x00;
	else buffer[buffLen++] = 0x01;
    return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_AUTHEN, TLKPRT_COMM_CMDID_AUTHEN_CONFIRM, 
    	TLKPRT_COMM_RSP_STATUE_SUCCESS, 0, buffer, buffLen);
}

static void tlkmdi_ats_generateRandom(uint08 *pBuffer, uint16 buffLen)
{
    int index;
    int offset;
    uint32 value;

    if(pBuffer == nullptr || buffLen == 0) return;

    offset = 0;
    for(index=0; index<buffLen; index++){
        value = (uint32)rand();
        pBuffer[offset++] = value & 0xFF;
        if(offset >= buffLen) break;
        value >>= 8;
        pBuffer[offset++] = value & 0xFF;
        if(offset >= buffLen) break;
        value >>= 8;
        pBuffer[offset++] = value & 0xFF;
        if(offset >= buffLen) break;
        value >>= 8;
        pBuffer[offset++] = value & 0xFF;
        if(offset >= buffLen) break;
    }
}





#endif //#if (TLK_TEST_ATS_ENABLE)

