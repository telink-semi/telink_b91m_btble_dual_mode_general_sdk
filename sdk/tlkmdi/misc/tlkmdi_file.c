/********************************************************************************************************
 * @file	tlkmdi_file.c
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
#if (TLK_MDI_FILE_ENABLE)
#include "tlkmdi/misc/tlkmdi_file.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlksys/prt/tlkpto_file.h"
#include "tlkalg/digest/md5/tlkalg_md5.h"
#include "tlkalg/digest/crc/tlkalg_crc.h"
#include "tlksys/tlksys_stdio.h"


#define TLKMDI_FILE_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_FILE << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_FILE_DBG_SIGN       "[MDI]"

extern void bth_acl_leaveSniff(uint16 aclHandle, uint16 busyTime);

static int tlkmdi_file_authSignDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit);
static int tlkmdi_file_authCrypDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit, bool isEnc, uint08 *pData, uint16 dataLen);
static int tlkmdi_file_crypSignDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit, uint08 stage);
static int tlkmdi_file_crypCrypDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit, bool isEnc, uint08 *pData, uint16 dataLen);
static int tlkmdi_file_fastSignDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit);


static void tlkmdi_file_digestInit(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_digestUpdate(tlkmdi_file_unit_t *pUnit, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_digestFinish(tlkmdi_file_unit_t *pUnit);


static tlkmdi_file_auth_t *tlkmdi_file_getIdleAuthSch(void);
static tlkmdi_file_auth_t *tlkmdi_file_getUsedAuthSch(uint16 schCode);
static tlkmdi_file_cryp_t *tlkmdi_file_getIdleCrypSch(void);
static tlkmdi_file_cryp_t *tlkmdi_file_getUsedCrypSch(uint16 schCode);
static tlkmdi_file_fast_t *tlkmdi_file_getIdleFastSch(void);
static tlkmdi_file_fast_t *tlkmdi_file_getUsedFastSch(uint16 schCode);


static void tlkmdi_file_resetRecvUnit(tlkmdi_file_unit_t *pUnit);
static tlkmdi_file_unit_t *tlkmdi_file_getIdleRecvUnit(void);
static tlkmdi_file_unit_t *tlkmdi_file_getUsedRecvUnitByFilePort(uint08 optChn, uint16 handle, uint08 filePort);
static tlkmdi_file_unit_t *tlkmdi_file_getUsedRecvUnitByDataPort(uint08 optChn, uint16 handle, uint08 dataPort);

static void tlkmdi_file_recvCmdDeal(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvRspDeal(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvEvtDeal(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvDatDeal(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvGetVersionCmdDeal(uint08 optChn, uint16 handle, uint08 csign, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvStartTranCmdDeal(uint08 optChn, uint16 handle, uint08 csign, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvStartAuthCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvCryptShakeCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvSetDataSchCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvSetFileNameCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvFastStartCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvStartDataCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen);
static void tlkmdi_file_recvCloseTranCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen);

static bool tlkmdi_file_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmdi_file_clientSendProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_serverSendProc(tlkmdi_file_unit_t *pUnit);

static int tlkmdi_file_sendRspPkt(uint08 optChn, uint16 handle, uint08 status, uint08 reason,
	uint08 cmdID, uint08 *pData, uint08 dataLen, uint08 *pSerial);
static int tlkmdi_file_sendEvtPkt(uint08 optChn, uint16 handle, uint08 evtID, 
	uint08 *pData, uint08 dataLen, uint08 *pSerial);



static void tlkmdi_file_sendStartTranRspProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendStartAuthRspProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendCryptShakeRspProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendSetDataSchRspProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendSetFileNameRspProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendFastStartRspProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendStartDataRspProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendCloseTranRspProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendCloseTranEvtProc(tlkmdi_file_unit_t *pUnit);
static void tlkmdi_file_sendSyncShakeEvtProc(tlkmdi_file_unit_t *pUnit, uint32 packNumb);
static void tlkmdi_file_sendSyncStallEvtProc(tlkmdi_file_unit_t *pUnit);



static tlkmdi_file_ctrl_t sTlkMdiFileCtrl;


int tlkmdi_file_init(void)
{
	tmemset(&sTlkMdiFileCtrl, 0, sizeof(tlkmdi_file_ctrl_t));
	tlkmdi_file_setAuthSch(0x8101, tlkmdi_file_authSignDefault, tlkmdi_file_authCrypDefault);
	tlkmdi_file_setCrypSch(0x9101, tlkmdi_file_crypSignDefault, tlkmdi_file_crypCrypDefault);
	tlkmdi_file_setFastSch(0x7101, tlkmdi_file_fastSignDefault);
	
	return TLK_ENONE;
}


void tlkmdi_file_setAuthCode(uint08 authCode[16])
{
	tmemcpy(sTlkMdiFileCtrl.usrAuthCode, authCode, 16);
}
void tlkmdi_file_setCrypCode(uint08 crypCode[16])
{
	tmemcpy(sTlkMdiFileCtrl.usrCrypCode, crypCode, 16);
}

void tlkmdi_file_setAuthSch(uint16 schCode, TlkmdiFileAuthSignFunc sign, TlkmdiFileAuthCrypFunc cryp)
{
	tlkmdi_file_auth_t *pNode;
	pNode = tlkmdi_file_getUsedAuthSch(schCode);
	if(pNode == nullptr) pNode = tlkmdi_file_getIdleAuthSch();
	if(pNode == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setAuthSch: failure - no valid node");
	}else if(sign == nullptr || cryp == nullptr){
		pNode->sign = nullptr;
		pNode->cryp = nullptr;
		pNode->schCode = 0;
	}else{
		pNode->sign = sign;
		pNode->cryp = cryp;
		pNode->schCode = schCode;
	}
}
void tlkmdi_file_setCrypSch(uint16 schCode, TlkmdiFileCrypSignFunc sign, TlkmdiFileCrypCrypFunc cryp)
{
	tlkmdi_file_cryp_t *pNode;
	pNode = tlkmdi_file_getUsedCrypSch(schCode);
	if(pNode == nullptr) pNode = tlkmdi_file_getIdleCrypSch();
	if(pNode == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setCrypSch: failure - no valid node");
	}else if(sign == nullptr || cryp == nullptr){
		pNode->sign = nullptr;
		pNode->cryp = nullptr;
		pNode->schCode = 0;
	}else{
		pNode->sign = sign;
		pNode->cryp = cryp;
		pNode->schCode = schCode;
	}
}
void tlkmdi_file_setFastSch(uint16 schCode, TlkmdiFileFastSignFunc sign)
{
	tlkmdi_file_fast_t *pNode;
	pNode = tlkmdi_file_getUsedFastSch(schCode);
	if(pNode == nullptr) pNode = tlkmdi_file_getIdleFastSch();
	if(pNode == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setFastSch: failure - no valid node");
	}else if(sign == nullptr){
		pNode->sign = nullptr;
		pNode->schCode = 0;
	}else{
		pNode->sign = sign;
		pNode->schCode = schCode;
	}
}

/******************************************************************************
 * Function: tlkmdi_file_setRecvParam.
 * Descript: Sets the parameters for receiving file transfers.
 * Params: 
 *     @pUnit - The structure of Transmission control unit;
 *     @IntvNumb - 
 *     @timeout - 
 *     @tranIntv - 
 *     @unitLens - 
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/


void tlkmdi_file_setRecvInfs(TlkmdiFileStartFunc start, TlkmdiFileCloseFunc close,
	 TlkmdiFileParamFunc param, TlkmdiFileSaveFunc save, TlkmdiFileSendFunc send)
{
	uint08 numb = 0;
	if(start != nullptr) numb ++;
	if(param != nullptr) numb ++;
	if(close != nullptr) numb ++;
	if(save != nullptr)  numb ++;
	if(send != nullptr)  numb ++;
	if(numb != 0 && numb != 5){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvInfs: failure - param error");
		return;
	}
	sTlkMdiFileCtrl.recvInfs.start = start;
	sTlkMdiFileCtrl.recvInfs.param = param;
	sTlkMdiFileCtrl.recvInfs.close = close;
	sTlkMdiFileCtrl.recvInfs.load = nullptr;
	sTlkMdiFileCtrl.recvInfs.save = save;
	sTlkMdiFileCtrl.recvInfs.send = send;
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvInfs: success");
}
void tlkmdi_file_setSendInfs(TlkmdiFileStartFunc start, TlkmdiFileCloseFunc close,
	 TlkmdiFileParamFunc param, TlkmdiFileLoadFunc load, TlkmdiFileSendFunc send)
{
	#if (TLKMDI_FILE_TRAN_SEND_NUMB == 0)
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setSendInfs: failure - not support");
	#else
	uint08 numb = 0;
	if(start != nullptr) numb ++;
	if(param != nullptr) numb ++;
	if(close != nullptr) numb ++;
	if(load != nullptr)  numb ++;
	if(send != nullptr)  numb ++;
	if(numb != 0 && numb != 5){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setSendInfs: failure - param error");
		return;
	}
	sTlkMdiFileCtrl.sendInfs.start = start;
	sTlkMdiFileCtrl.sendInfs.param = param;
	sTlkMdiFileCtrl.sendInfs.close = close;
	sTlkMdiFileCtrl.sendInfs.load  = load;
	sTlkMdiFileCtrl.sendInfs.save  = nullptr;
	sTlkMdiFileCtrl.sendInfs.send  = send;
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setSendInfs: success");
	#endif
}

int tlkmdi_file_getRecvSaveParam(tlkmdi_file_unit_t *pUnit, tlkmdi_file_fastParam_t *pParam)
{
	if(pUnit == nullptr || pParam == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_getRecvSaveParam: failure - param error");
		return -TLK_EPARAM;
	}
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_getRecvSaveParam: dealSize[%x]", pUnit->dealSize);

	UINT32H_TO_ARRAY(pUnit->digest.crc32, pUnit->param.code.realSign, 0);
	
	pParam->optChn = pUnit->optChn;
	pParam->cprSch = pUnit->cprSch; //Compress Scheme
	pParam->digSch = pUnit->digSch; //Digest Scheme
	pParam->tranRole = pUnit->tranRole;
	pParam->dataPort = pUnit->dataPort;
	pParam->waitMask = pUnit->waitMask;
	pParam->fileType = pUnit->fileType;
	pParam->unitLens = pUnit->unitLens;
	pParam->tranIntv = pUnit->tranIntv;
	pParam->authSch  = pUnit->authSch;
	pParam->fastSch  = pUnit->fastSch;
	pParam->cryptSch = pUnit->cryptSch;
	pParam->dealSize = pUnit->dealSize; //RecvSize or SendSize
	pParam->fileSize = pUnit->fileSize;
	pParam->fileVers = pUnit->fileVers;
	tmemcpy(pParam->authCode, pUnit->authCode, 16);
	tmemcpy(pParam->fileSign, pUnit->param.code.fileSign, 16);
	tmemcpy(pParam->realSign, pUnit->param.code.realSign, 16);
	tmemcpy(pParam->crypCode, pUnit->param.code.crypCode, 16);
	tmemcpy(pParam->fastCode, pUnit->param.code.fastCode, 16);
	return TLK_ENONE;
}
int tlkmdi_file_setRecvPortParam(tlkmdi_file_unit_t *pUnit, tlkmdi_file_portParam_t *pParam)
{
	if(pUnit == nullptr || pParam == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvPortParam: failure - param error");
		return -TLK_EPARAM;
	}

	pUnit->dataPort = pParam->dataPort;

	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvPortParam: dataPort[%d]", pParam->dataPort);

	return TLK_ENONE;
}
int tlkmdi_file_setRecvFastParam(tlkmdi_file_unit_t *pUnit, tlkmdi_file_fastParam_t *pParam)
{
	if(pUnit == nullptr || pParam == nullptr || pParam->unitLens == 0){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvFastParam: failure - param error");
		return -TLK_EPARAM;
	}
	
	pUnit->attrs = TLKMDI_FILE_ATTR_NONE;
	if(pParam->authSch != 0) pUnit->attrs |= TLKMDI_FILE_ATTR_AUTH;
	if(pParam->cryptSch != 0) pUnit->attrs |= TLKMDI_FILE_ATTR_CRYP;
	if(pParam->cprSch != 0) pUnit->attrs |= TLKMDI_FILE_ATTR_COMP;
	pUnit->optChn = pParam->optChn;
	pUnit->cprSch = pParam->cprSch; //Compress Scheme
	pUnit->digSch = pParam->digSch; //Digest Scheme
	pUnit->tranRole = pParam->tranRole;
	pUnit->dataPort = pParam->dataPort;
	pUnit->waitMask = pParam->waitMask;
	pUnit->fileType = pParam->fileType;
	pUnit->unitLens = pParam->unitLens;
	pUnit->tranIntv = pParam->tranIntv;
	pUnit->authSch  = pParam->authSch;
	pUnit->fastSch  = pParam->fastSch;
	pUnit->cryptSch = pParam->cryptSch;
	pUnit->packNumb = pParam->dealSize/pParam->unitLens;
	pUnit->dealSize = pParam->dealSize;
	pUnit->fileSize = pParam->fileSize;
	pUnit->fileVers = pParam->fileVers;
	tmemcpy(pUnit->authCode, pParam->authCode, 16);
	tmemcpy(pUnit->param.code.fileSign, pParam->fileSign, 16);
	tmemcpy(pUnit->param.code.realSign, pParam->realSign, 16);
	tmemcpy(pUnit->param.code.crypCode, pParam->crypCode, 16);
	tmemcpy(pUnit->param.code.fastCode, pParam->fastCode, 16);
	
	ARRAY_TO_UINT32H(pUnit->param.code.realSign, 0, pUnit->digest.crc32);
	
	return TLK_ENONE;
}
int tlkmdi_file_setRecvStartParam(tlkmdi_file_unit_t *pUnit, tlkmdi_file_startParam_t *pParam)
{
	if(pUnit == nullptr || pParam == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvStartParam: error param");
		return -TLK_EPARAM;
	}

	if(pUnit->tranRole != TLKMDI_FILE_TRAN_ROLE_CLIENT){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvStartParam: error role");
		return -TLK_EROLE;
	}
	if(pParam->intvNumb != 0 && (pParam->intvNumb & (pParam->intvNumb-1)) != 0){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvStartParam: error intvNumb[%d]", pParam->intvNumb);
		return -TLK_EPARAM;
	}
	if(pParam->unitLens == 0 || (pParam->unitLens & 0x0F) != 0 || pParam->unitLens > pUnit->unitLens){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_setRecvStartParam: error unitLens[%d - %d]", pParam->unitLens, pUnit->unitLens);
		return -TLK_EPARAM;
	}
	
	if(pParam->tranIntv < pUnit->tranIntv) pUnit->tranIntv = pParam->tranIntv;
	if(pParam->intvNumb == 0) pUnit->waitMask = 0;
	else pUnit->waitMask = pParam->intvNumb-1;
	pUnit->tranIntv = pParam->tranIntv;
	pUnit->unitLens = pParam->unitLens;
	
	return TLK_ENONE;
}

int tlkmdi_file_enableDataDecode(tlkmdi_file_unit_t *pUnit, bool isDecrypt, bool isUncompress)
{
	if(pUnit == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_enableDataDecode: error param");
		return -TLK_EPARAM;
	}
	if(pUnit->state == TLKMDI_FILE_STATE_IDLE){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_enableDataDecode: error status");
		return -TLK_ESTATUS;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_DATA) != 0){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_enableDataDecode: error busy");
		return -TLK_EBUSY;
	}

	if(isDecrypt) pUnit->attrs |= TLKMDI_FILE_ATTR_DECRYPT;
	if(isUncompress) pUnit->attrs |= TLKMDI_FILE_ATTR_UNCOMPR;
	
	return TLK_ENONE;
}

int tlkmdi_file_closeTrans(tlkmdi_file_unit_t *pUnit)
{
	if(pUnit == nullptr) return -TLK_EPARAM;
	if(pUnit->state == TLKMDI_FILE_STATE_IDLE) return -TLK_ESTATUS;
	pUnit->state = TLKMDI_FILE_STATE_CLOSE;
	pUnit->flags = TLKMDI_FILE_FLAG_NONE;
	pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
	pUnit->status = TLKPRT_FILE_STATUS_TRAN_STOP;
	tlkmdi_file_sendCloseTranEvtProc(pUnit);
	return TLK_ENONE;
}


void tlkmdi_file_recvHandler(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	uint08 pktType;	
	
	if(pHead == nullptr || headLen < 4){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recv: length error - %d %d", headLen, dataLen);
		return;
	}
	
	pktType = pHead[0];
	if(pktType == TLKPRT_COMM_PTYPE_CMD){
		tlkmdi_file_recvCmdDeal(optChn, handle, pHead, headLen, pData, dataLen);
	}else if(pktType == TLKPRT_COMM_PTYPE_RSP){
		tlkmdi_file_recvRspDeal(optChn, handle, pHead, headLen, pData, dataLen);
	}else if(pktType == TLKPRT_COMM_PTYPE_EVT){
		tlkmdi_file_recvEvtDeal(optChn, handle, pHead, headLen, pData, dataLen);
	}else if(pktType == TLKPRT_COMM_PTYPE_DAT){
		tlkmdi_file_recvDatDeal(optChn, handle, pHead, headLen, pData, dataLen);
	}else{
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recv: pktType not support - %d", pktType);
	}
}



static void tlkmdi_file_recvCmdDeal(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	uint08 csign;
	uint08 cmdID;
	uint08 pktLen;
	uint08 dataPort;
	tlkmdi_file_unit_t *pUnit;

	tlkapi_array(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal[Head]: ", pHead, headLen);
	tlkapi_array(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal[Data]: ", pData, dataLen);
	
	cmdID  = pHead[2];
	pktLen = pHead[4];
	csign  = pHead[5];
	dataPort = pHead[6];
	if(headLen != 7 || pktLen < 2 || pktLen-2 > dataLen){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal: error length - headLen[%d],pktLen[%d]", headLen, pktLen);
		return;
	}
	
	if(cmdID == TLKPRT_COMM_CMDID_FILE_START_TRAN){
		if(csign != 0 || dataPort != 0){
			tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal: error format - csign[%d],port[%d]", csign, dataPort);
			return;
		}
		tlkmdi_file_recvStartTranCmdDeal(optChn, handle, csign, pData, dataLen);
		return;
	}else if(cmdID == TLKPRT_COMM_CMDID_FILE_GET_VERSION){
		tlkmdi_file_recvGetVersionCmdDeal(optChn, handle, csign, pData, dataLen);
		return;
	}

	if(dataPort == 0){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal: error format");
		return;
	}
	pUnit = tlkmdi_file_getUsedRecvUnitByDataPort(optChn, handle, dataPort);
	if(pUnit == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal: not node");
		return;
	}

	if(csign == TLKPRT_FILE_CSIGN_NONE){
		
	}
	else if(csign == TLKPRT_FILE_CSIGN_AUTH){
		int ret;
		tlkmdi_file_auth_t *pAuth;
		pAuth = tlkmdi_file_getUsedAuthSch(pUnit->authSch);
		if(pAuth == nullptr || (pUnit->attrs & TLKMDI_FILE_ATTR_AUTH) == 0) ret = -TLK_ENOSUPPORT;
		else ret = pAuth->cryp(pUnit->authSch, pUnit, false, pData, dataLen);
		if(ret < 0){
			tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal: auth decrypt fail");
			return;
		}
	}
	else if(csign == TLKPRT_FILE_CSIGN_CRYP){
		int ret;
		tlkmdi_file_cryp_t *pCryp;
		pCryp = tlkmdi_file_getUsedCrypSch(pUnit->cryptSch);
		if(pCryp == nullptr || (pUnit->attrs & TLKMDI_FILE_ATTR_AUTH) == 0) ret = -TLK_ENOSUPPORT;
		else ret = pCryp->cryp(pUnit->authSch, pUnit, false, pData, dataLen);
		if(ret < 0){
			tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal: cryp decrypt fail");
			return;
		}
	}else{
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal: error sign[%d]", csign);
	}

	if(cmdID == TLKPRT_COMM_CMDID_FILE_START_AUTH){
		tlkmdi_file_recvStartAuthCmdDeal(pUnit, csign, pData, dataLen);
	}else if(cmdID == TLKPRT_COMM_CMDID_FILE_CRYPT_SHAKE){
		tlkmdi_file_recvCryptShakeCmdDeal(pUnit, csign, pData, dataLen);
	}else if(cmdID == TLKPRT_COMM_CMDID_FILE_SET_DATA_SCH){
		tlkmdi_file_recvSetDataSchCmdDeal(pUnit, csign, pData, dataLen);
	}else if(cmdID == TLKPRT_COMM_CMDID_FILE_SET_FILE_NAME){
		tlkmdi_file_recvSetFileNameCmdDeal(pUnit, csign, pData, dataLen);
	}else if(cmdID == TLKPRT_COMM_CMDID_FILE_FAST_START){
		tlkmdi_file_recvFastStartCmdDeal(pUnit, csign, pData, dataLen);
	}else if(cmdID == TLKPRT_COMM_CMDID_FILE_START_DATA){
		tlkmdi_file_recvStartDataCmdDeal(pUnit, csign, pData, dataLen);
	}else if(cmdID == TLKPRT_COMM_CMDID_FILE_CLOSE_TRAN){
		tlkmdi_file_recvCloseTranCmdDeal(pUnit, csign, pData, dataLen);
	}else{
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCmdDeal: cmdID not support - %d", cmdID);
	}
}
static void tlkmdi_file_recvRspDeal(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	
}
static void tlkmdi_file_recvEvtDeal(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	
}
static void tlkmdi_file_recvDatDeal(uint08 optChn, uint16 handle, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	int ret;
	uint08 dataPort;
	uint32 dataNumb;
	uint16 packNumb;
	uint16 packLens;
	tlkmdi_file_unit_t *pUnit;

	if(headLen != 6){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvDatDeal: error format dataLen[%d]", dataLen);
		return;
	}

//	tlkapi_array(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "DataBody0:", pHead, headLen);
//	tlkapi_array(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "DataBody1:", pData, dataLen);
//	tlkapi_array(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "DataBody2:", pData+64, dataLen);

	dataPort = pHead[1];
	ARRAY_TO_UINT16L(pHead, 2, dataNumb);
	ARRAY_TO_UINT16L(pHead, 4, packLens);
	dataNumb |= (packLens & 0xF000) << 4;
	packLens = packLens & 0x0FFF;
	pUnit = tlkmdi_file_getUsedRecvUnitByDataPort(optChn, handle, dataPort);
	if(pUnit == nullptr){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvDatDeal: not have node-optChn[%d],dataPort[%d]", optChn, dataPort);
		return;
	}
	if(pUnit->tranRole != TLKMDI_FILE_TRAN_ROLE_CLIENT || (pUnit->flags & TLKMDI_FILE_FLAG_DATA) == 0){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvDatDeal: unexpected packet");
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_WAIT_DATA) != 0){
		pUnit->flags &= ~TLKMDI_FILE_FLAG_WAIT_WAIT_DATA;
	}
		
	packNumb = (pUnit->packNumb+1) & 0xFFFF;
	
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvDatDeal: dataNumb[%d],expNumb[%d],packLens[%d],waitMask[%x] dealSize[%d] fileSize[%d]", 
		dataNumb, packNumb, packLens, pUnit->waitMask, pUnit->dealSize, pUnit->fileSize);
	if(dataNumb > packNumb){
		tlkapi_warn(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN,
			"tlkmdi_file_recvDatDeal: stall packet, curNumb[%d],expNumb[%d]",
			dataNumb, packNumb);
		if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_SYNC_STALL_EVT) == 0){
			pUnit->busys |= TLKMDI_FILE_BUSY_SEND_SYNC_STALL_EVT;
			tlkmdi_file_sendSyncStallEvtProc(pUnit);
			tlkapi_timer_insertNode(&pUnit->timer);
		}
		return;
	}

	if(pUnit->waitMask != 0){
		if((dataNumb & pUnit->waitMask) == 0 || pUnit->dealSize == pUnit->fileSize){
			if(packNumb == dataNumb) tlkmdi_file_sendSyncShakeEvtProc(pUnit, dataNumb);
			else tlkmdi_file_sendSyncShakeEvtProc(pUnit, pUnit->packNumb);
		}
	}else{
		if(packNumb == dataNumb) tlkmdi_file_sendSyncShakeEvtProc(pUnit, packNumb);
		else tlkmdi_file_sendSyncShakeEvtProc(pUnit, pUnit->packNumb);
	}
	if(packNumb != dataNumb){
		return;
	}

	if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_SYNC_STALL_EVT) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_SYNC_STALL_EVT;
	}
	if(pUnit->dealSize == pUnit->fileSize){
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		tlkmdi_file_sendSyncShakeEvtProc(pUnit, dataNumb);
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		return;
	}
	
//	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN,
//		"tlkmdi_file_recvDatDeal: dealSize[%d],fileSize[%d],packNumb[%d],packLens[%d],unitLens[%d]",
//		pUnit->dealSize, pUnit->fileSize, packNumb, packLens, pUnit->unitLens);
	if(pUnit->dealSize+packLens > pUnit->fileSize || pUnit->unitLens < packLens
		|| (pUnit->dealSize+packLens != pUnit->fileSize && pUnit->unitLens != packLens)){
		tlkapi_fatal(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN,
			"tlkmdi_file_recvDatDeal: transfer fault, dealSize[%d],fileSize[%d],packNumb[%d],packLens[%d],unitLens[%d]",
			pUnit->dealSize, pUnit->fileSize, packNumb, packLens, pUnit->unitLens);
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		pUnit->flags &= ~TLKMDI_FILE_FLAG_DATA;
		pUnit->status = TLKPRT_FILE_STATUS_ERR_LENGTH;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		return;
	}
	
	tlkmdi_file_digestUpdate(pUnit, pData, packLens);
	if((pUnit->attrs & TLKMDI_FILE_ATTR_CRYP) != 0 && (pUnit->attrs & TLKMDI_FILE_ATTR_DECRYPT) != 0){
		tlkmdi_file_cryp_t *pCryp;
		pCryp = tlkmdi_file_getUsedCrypSch(pUnit->cryptSch);
		if(pCryp == nullptr || pCryp->cryp == nullptr){
			tlkapi_fatal(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvDatDeal: decrypt fault!");
			pUnit->state = TLKMDI_FILE_STATE_CLOSE;
			pUnit->flags &= ~TLKMDI_FILE_FLAG_DATA;
			pUnit->status = TLKPRT_FILE_STATUS_DEC_FAILURE;
			tlkmdi_file_sendCloseTranEvtProc(pUnit);
			return;
		}
		if(pCryp->cryp(pUnit->cryptSch, pUnit, false, pData, dataLen) != TLK_ENONE){
			tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvDatDeal: decrypt error!");
			pUnit->state = TLKMDI_FILE_STATE_CLOSE;
			pUnit->flags &= ~TLKMDI_FILE_FLAG_DATA;
			pUnit->status = TLKPRT_FILE_STATUS_DEC_FAILURE;
			tlkmdi_file_sendCloseTranEvtProc(pUnit);
			return;
		}
	}
	if((pUnit->attrs & TLKMDI_FILE_ATTR_COMP) != 0 && (pUnit->attrs & TLKMDI_FILE_ATTR_UNCOMPR) != 0){
		
	}
	ret = sTlkMdiFileCtrl.recvInfs.save(pUnit, pUnit->packNumb*pUnit->unitLens, pData, packLens);
	if(ret != TLK_ENONE){
		tlkapi_warn(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvDatDeal: save error!");
		return;
	}

	pUnit->idleTime = TLKMDI_FILE_IDLE_TIMEOUT;
	pUnit->packNumb = dataNumb;
	pUnit->dealSize += packLens;	
	if(pUnit->dealSize == pUnit->fileSize){
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
	}
}


static void tlkmdi_file_recvGetVersionCmdDeal(uint08 optChn, uint16 handle, uint08 csign, uint08 *pData, uint16 dataLen)
{
	uint08 buffLen;
	uint08 buffer[32];

	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvGetVersionCmdDeal");

	buffLen = 0;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = (TLK_APP_VERSION & 0xFF000000) >> 24;
	buffer[buffLen++] = (TLK_APP_VERSION & 0xFF0000) >> 16;
	buffer[buffLen++] = (TLK_APP_VERSION & 0xFF00) >> 8;
	buffer[buffLen++] = (TLK_APP_VERSION & 0xFF);
	if(tlkmdi_file_sendRspPkt(optChn, handle, 0x00, 0x00, TLKPRT_COMM_CMDID_FILE_GET_VERSION,
		buffer, buffLen, nullptr) != TLK_ENONE){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvGetVersionCmdDeal: Send Failure");
	}
}
static void tlkmdi_file_recvStartTranCmdDeal(uint08 optChn, uint16 handle, uint08 csign, uint08 *pData, uint16 dataLen)
{
	int ret;
	uint08 reason;
	uint16 authSch;
	uint08 fastMode;
	uint08 filePort;
	uint16 protVers;
	uint16 fileType;
	uint32 fileVers;
	tlkmdi_file_unit_t *pUnit;
	
	if(dataLen < 12){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartTranCmdDeal: error format dataLen[%d]", dataLen);
		return;
	}

	bth_acl_leaveSniff(handle, 10000);

	pUnit = nullptr;
	reason = TLKPRT_FILE_STATUS_SUCCESS;
	
	filePort = pData[0];
	fastMode = pData[1];
	ARRAY_TO_UINT16L(pData, 2, protVers);
	ARRAY_TO_UINT16L(pData, 4, fileType);
	ARRAY_TO_UINT16L(pData, 6, authSch);
	ARRAY_TO_UINT32H(pData, 8, fileVers);
	if(reason == 0 && csign != 0) reason = TLKPRT_FILE_STATUS_AUTH_FAILURE;
	if(reason == 0 && filePort == 0) reason = TLKPRT_FILE_STATUS_ERR_PORT;
	if(reason == 0){
		pUnit = tlkmdi_file_getUsedRecvUnitByFilePort(optChn, handle, filePort);
		if(pUnit != nullptr) reason = TLKPRT_FILE_STATUS_REPEAT;
	}
	if(reason == 0){
		pUnit = tlkmdi_file_getIdleRecvUnit();
		if(pUnit == nullptr){
			reason = TLKPRT_FILE_STATUS_NO_QUOTA;
			tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartTranCmdDeal: no quota");
		}
	}
	
	if(reason == 0 && authSch != 0 && tlkmdi_file_getUsedAuthSch(authSch) == nullptr){
		reason = TLKPRT_FILE_STATUS_AUTH_FAILURE;
	}
	if(reason == 0 && sTlkMdiFileCtrl.recvInfs.start == nullptr){
		reason = TLKPRT_FILE_STATUS_FAILURE;
	}
	
	if(reason == 0){
		pUnit->fileType = fileType;
		ret = sTlkMdiFileCtrl.recvInfs.param(pUnit, TLKMDI_FILE_PARAM_TYPE_GET_PORT, nullptr, 0);
		if(ret != TLK_ENONE || pUnit->dataPort == 0) reason = TLKPRT_FILE_STATUS_NO_QUOTA;
	}
	if(reason != 0 || pUnit == nullptr){
		uint08 status;
		uint08 buffLen;
		uint08 buffer[32];
		
		status = 0x01;
		buffLen = 0;
		buffer[buffLen++] = 0x00;
		buffer[buffLen++] = filePort;
		buffer[buffLen++] = filePort;
		buffer[buffLen++] = 0x00;
		buffer[buffLen++] = (TLKPRT_FILE_VERSION & 0xFF);
		buffer[buffLen++] = (TLKPRT_FILE_VERSION & 0xFF00) >> 8;
		buffer[buffLen++] = (TLKPRT_FILE_COMPR_FEATURE_MASK & 0xFF);
		buffer[buffLen++] = (TLKPRT_FILE_COMPR_FEATURE_MASK & 0xFF00) >> 8;
		buffer[buffLen++] = (TLKPRT_FILE_COMPR_FEATURE_MASK & 0xFF0000) >> 16;
		buffer[buffLen++] = (TLKPRT_FILE_COMPR_FEATURE_MASK & 0xFF000000) >> 24;
		buffer[buffLen++] = (TLKPRT_FILE_DIGEST_FEATURE_MASK & 0xFF);
		buffer[buffLen++] = (TLKPRT_FILE_DIGEST_FEATURE_MASK & 0xFF00) >> 8;
		buffer[buffLen++] = (TLKPRT_FILE_DIGEST_FEATURE_MASK & 0xFF0000) >> 16;
		buffer[buffLen++] = (TLKPRT_FILE_DIGEST_FEATURE_MASK & 0xFF000000) >> 24;
		tmemcpy(buffer+buffLen, pUnit->param.rand.acpRand0, 16);
		buffLen += 16;
		tlkmdi_file_sendRspPkt(optChn, handle, status, reason, TLKPRT_COMM_CMDID_FILE_START_TRAN,
			buffer, buffLen, nullptr);
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN,
			"tlkmdi_file_recvStartTranCmdDeal: failure - status[%d],reason[%x]",
			status, reason);
		return;
	}

	pUnit->status = TLKPRT_FILE_STATUS_SUCCESS;
	pUnit->optChn = optChn;
	pUnit->handle = handle;
	pUnit->tranRole = TLKMDI_FILE_TRAN_ROLE_CLIENT;
	pUnit->authSch = authSch;
	pUnit->fileType = fileType;
	pUnit->filePort = filePort;
	pUnit->fileVers = fileVers;
	tlkapi_random(pUnit->param.rand.acpRand0, 16);

//	tlkapi_array(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, ": rand", pUnit->param.rand.acpRand0, 16);

	pUnit->state = TLKMDI_FILE_STATE_START;
	pUnit->idleTime = TLKMDI_FILE_IDLE_TIMEOUT;
	tlkapi_timer_initNode(&pUnit->timer, tlkmdi_file_timer, (uint32)pUnit, TLKMDI_FILE_TIMEOUT);
	tlkapi_timer_insertNode(&pUnit->timer);

	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartTranCmdDeal");

	if(fastMode == 1){
		pUnit->flags = TLKMDI_FILE_FLAG_WAIT_FAST_START;
	}else if(pUnit->authSch != 0){
		pUnit->flags = TLKMDI_FILE_FLAG_WAIT_START_AUTH;
	}else{
		pUnit->flags = TLKMDI_FILE_FLAG_WAIT_SET_DATA_SCH | TLKMDI_FILE_FLAG_WAIT_SET_FILE_NAME;
	}

	pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
	tlkmdi_file_sendStartTranRspProc(pUnit);
}


static void tlkmdi_file_recvStartAuthCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen)
{
	int ret;
	tlkmdi_file_auth_t *pAuth;

	if(dataLen < 32){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartAuthCmdDeal: error format dataLen[%d]", dataLen);
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_START_AUTH) == 0 && (pUnit->flags & TLKMDI_FILE_FLAG_WAIT_CRYPT_SHAKE) == 0){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartAuthCmdDeal: unexpected packet");
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_START_AUTH) == 0){
		tlkmdi_file_sendStartAuthRspProc(pUnit);
		return;
	}
	pUnit->flags &= ~TLKMDI_FILE_FLAG_WAIT_START_AUTH;
	
	
	tmemcpy(pUnit->param.rand.iniRand0, pData, 16);
	
	pAuth = tlkmdi_file_getUsedAuthSch(pUnit->authSch);
	if(pAuth == nullptr || pAuth->sign == nullptr) ret = -TLK_ENOSUPPORT;
	else ret = pAuth->sign(pUnit->authSch, pUnit);
	if(ret == TLK_ENONE && tmemcmp(pUnit->authCode, pData+16, 16) != 0){
		ret = -TLK_EFAIL;
	}
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartAuthCmdDeal: auth failure");
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		pUnit->flags = TLKMDI_FILE_FLAG_NONE;
		pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
		pUnit->status = TLKPRT_FILE_STATUS_AUTH_FAILURE;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		return;
	}

	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartAuthCmdDeal");
	
	pUnit->attrs |= TLKMDI_FILE_ATTR_AUTH;
	pUnit->flags = TLKMDI_FILE_FLAG_WAIT_CRYPT_SHAKE;
	pUnit->idleTime = TLKMDI_FILE_IDLE_TIMEOUT;
	tlkapi_random(pUnit->param.rand.acpRand1, 16);
	
	pUnit->csign = TLKPRT_FILE_CSIGN_AUTH;
	tlkmdi_file_sendStartAuthRspProc(pUnit);
}
static void tlkmdi_file_recvCryptShakeCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen)
{
	int ret;
	
	if(dataLen < 34){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCryptShakeCmdDeal: error format dataLen[%d]", dataLen);
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_CRYPT_SHAKE) == 0 && (pUnit->flags & TLKMDI_FILE_FLAG_WAIT_START_DATA) == 0){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCryptShakeCmdDeal: unexpected packet");
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_CRYPT_SHAKE) == 0){
		tlkmdi_file_sendCryptShakeRspProc(pUnit);
		return;
	}
	pUnit->flags &= ~TLKMDI_FILE_FLAG_WAIT_CRYPT_SHAKE;
	
	ARRAY_TO_UINT16L(pData, 0, pUnit->cryptSch);
	tmemcpy(pUnit->param.rand.iniRand1, pData+2,  16);
	
	ret = TLK_ENONE;
	if(pUnit->cryptSch == 0){
		tmemcpy(pUnit->param.sign.iniCrypt, pData+18, 16);
	}else{
		tlkmdi_file_cryp_t *pCryp;
		pCryp = tlkmdi_file_getUsedCrypSch(pUnit->cryptSch);
		if(pCryp == nullptr || pCryp->sign == nullptr) ret = -TLK_ENOSUPPORT;
		else ret = pCryp->sign(pUnit->cryptSch, pUnit, TLKMDI_FILE_CRYP_STAGE_GEN_INI_SIGN);
		if(ret == TLK_ENONE) ret = pCryp->sign(pUnit->cryptSch, pUnit, TLKMDI_FILE_CRYP_STAGE_GEN_ACP_SIGN);
		if(ret == TLK_ENONE) ret = pCryp->sign(pUnit->cryptSch, pUnit, TLKMDI_FILE_CRYP_STAGE_GEN_CRYPT_CODE);
	}
	
	if(ret != TLK_ENONE || csign != TLKPRT_FILE_CSIGN_AUTH){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartAuthCmdDeal: auth failure");
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		pUnit->flags = TLKMDI_FILE_FLAG_NONE;
		pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
		pUnit->status = TLKPRT_FILE_STATUS_NOSUPP_CRYPT;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		return;
	}
	
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCryptShakeCmdDeal");

	pUnit->flags = TLKMDI_FILE_FLAG_WAIT_SET_DATA_SCH | TLKMDI_FILE_FLAG_WAIT_SET_FILE_NAME
		| TLKMDI_FILE_FLAG_WAIT_START_DATA;
	if(pUnit->cryptSch == 0){
		pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
	}else{
		pUnit->attrs |= TLKMDI_FILE_ATTR_CRYP;
		pUnit->csign = TLKPRT_FILE_CSIGN_CRYP;
	}
	pUnit->idleTime = TLKMDI_FILE_IDLE_TIMEOUT;
	
	tlkmdi_file_sendCryptShakeRspProc(pUnit);
}
static void tlkmdi_file_recvSetDataSchCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen)
{
	if(dataLen < 2){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetDataSchCmdDeal: error format dataLen[%d]", dataLen);
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_SET_DATA_SCH) == 0 && (pUnit->flags & TLKMDI_FILE_FLAG_WAIT_START_DATA) == 0){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetDataSchCmdDeal: unexpected packet");
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_SET_DATA_SCH) == 0){
		tlkmdi_file_sendSetDataSchRspProc(pUnit);
		return;
	}
	pUnit->flags &= ~TLKMDI_FILE_FLAG_WAIT_SET_DATA_SCH;
		
	pUnit->cprSch = pData[0];
	pUnit->digSch = pData[1];
	if(pUnit->status == 0 && (pUnit->attrs & TLKMDI_FILE_ATTR_CRYP) != 0 && csign != TLKPRT_FILE_CSIGN_CRYP){
		pUnit->status = TLKPRT_FILE_STATUS_DEC_FAILURE;
	}
	if(pUnit->status == 0 && pUnit->cprSch != 0 && ((1 << (pUnit->cprSch-1)) & TLKPRT_FILE_COMPR_FEATURE_MASK) == 0){
		pUnit->status = TLKPRT_FILE_STATUS_NOSUPP_COMPR;
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetDataSchCmdDeal: not support compr");
	}
	if(pUnit->status == 0 && pUnit->digSch != 0 && ((1 << (pUnit->digSch-1)) & TLKPRT_FILE_DIGEST_FEATURE_MASK) == 0){
		pUnit->status = TLKPRT_FILE_STATUS_NOSUPP_DIGEST;
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetDataSchCmdDeal: not support digest");
	}
	if(pUnit->status == 0 && pUnit->digSch == 0){
		pUnit->status = TLKPRT_FILE_STATUS_ERR_PARAM;
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetDataSchCmdDeal: must have digest");
	}
	if(pUnit->status != TLKPRT_FILE_STATUS_SUCCESS){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetDataSchCmdDeal: error - reason[%d]", pUnit->status);
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		pUnit->flags = TLKMDI_FILE_FLAG_NONE;
		pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		return;
	}
	
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetDataSchCmdDeal");

	if(pUnit->cprSch != 0) pUnit->attrs |= TLKMDI_FILE_ATTR_COMP;
	pUnit->flags |= TLKMDI_FILE_BUSY_SEND_START_DATA;
		
	pUnit->idleTime = TLKMDI_FILE_IDLE_TIMEOUT;
	tlkmdi_file_sendSetDataSchRspProc(pUnit);
}
static void tlkmdi_file_recvSetFileNameCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen)
{
	if(dataLen < 1){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetFileNameCmdDeal: error format dataLen[%d],nameLen[%d]", dataLen, pData[0]);
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_SET_FILE_NAME) == 0 && (pUnit->flags & TLKMDI_FILE_FLAG_WAIT_START_DATA) == 0){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetFileNameCmdDeal: unexpected packet");
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_SET_FILE_NAME) == 0){
		tlkmdi_file_sendSetFileNameRspProc(pUnit);
		return;
	}
	pUnit->flags &= ~TLKMDI_FILE_FLAG_WAIT_SET_FILE_NAME;
			
	if(pUnit->status == 0 && pData[0] > TLKMDI_FILE_NAME_SIZE){
		pUnit->status = TLKPRT_FILE_STATUS_ERR_LENGTH;
	}
	if(pUnit->status == 0 && (pUnit->attrs & TLKMDI_FILE_ATTR_CRYP) != 0 && csign != TLKPRT_FILE_CSIGN_CRYP){
		pUnit->status = TLKPRT_FILE_STATUS_DEC_FAILURE;
	}
	if(pUnit->status != TLKPRT_FILE_STATUS_SUCCESS){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetFileNameCmdDeal: not support ");
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		pUnit->flags = TLKMDI_FILE_FLAG_NONE;
		pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		return;
	}
	
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvSetFileNameCmdDeal");
	
	pUnit->idleTime = TLKMDI_FILE_IDLE_TIMEOUT;
	pUnit->nameLens = pData[0];
	tmemcpy(pUnit->fileName, pData+1, pUnit->nameLens);
	tlkmdi_file_sendSetFileNameRspProc(pUnit);
}
static void tlkmdi_file_recvFastStartCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen)
{
	int ret;
		
	if(dataLen < 18){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvFastStartCmdDeal: error format dataLen[%d]", dataLen);
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_FAST_START) == 0 && (pUnit->flags & TLKMDI_FILE_FLAG_WAIT_WAIT_DATA) == 0){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvFastStartCmdDeal: unexpected packet");
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_FAST_START) == 0){
		tlkmdi_file_sendStartDataRspProc(pUnit);
		return;
	}
	pUnit->flags = TLKMDI_FILE_FLAG_NONE;

	ARRAY_TO_UINT16L(pData, 0, pUnit->fastSch);
	tmemcpy(pUnit->param.code.fastCode, pData+2,  16);

	if(pUnit->status == 0 && (pUnit->attrs & TLKMDI_FILE_ATTR_CRYP) != 0 && csign != TLKPRT_FILE_CSIGN_CRYP){
		pUnit->status = TLKPRT_FILE_STATUS_DEC_FAILURE;
	}
	
	if(pUnit->status == 0){
		ret = sTlkMdiFileCtrl.recvInfs.param(pUnit, TLKMDI_FILE_PARAM_TYPE_GET_FAST, nullptr, 0);
		if(ret != TLK_ENONE) pUnit->status = TLKPRT_FILE_STATUS_FAILURE;
	}
	if(pUnit->status == 0){
		ret = sTlkMdiFileCtrl.recvInfs.start(pUnit, true);
		if(ret != TLK_ENONE) pUnit->status = TLKPRT_FILE_STATUS_FAILURE;
	}
	if(pUnit->status != TLKPRT_FILE_STATUS_SUCCESS){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvFastStartCmdDeal: not support ");
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		pUnit->flags = TLKMDI_FILE_FLAG_NONE;
		pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		return;
	}
	
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvFastStartCmdDeal");
	
	if(pUnit->authSch != 0) pUnit->attrs |= TLKMDI_FILE_ATTR_AUTH;
	if(pUnit->cryptSch != 0){
		pUnit->attrs |= TLKMDI_FILE_ATTR_CRYP;
		pUnit->csign = TLKPRT_FILE_CSIGN_CRYP;
	}
	if(pUnit->cprSch != 0) pUnit->attrs |= TLKMDI_FILE_ATTR_COMP;
	
	pUnit->idleTime = TLKMDI_FILE_IDLE_TIMEOUT;
	pUnit->nameLens = pData[0];
	pUnit->flags |= TLKMDI_FILE_FLAG_START;
	pUnit->flags |= TLKMDI_FILE_FLAG_DATA;
	pUnit->flags |= TLKMDI_FILE_FLAG_WAIT_WAIT_DATA;
	tlkmdi_file_sendFastStartRspProc(pUnit);
}
static void tlkmdi_file_recvStartDataCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen)
{
	int ret;
	
	if(dataLen < 26){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartDataCmdDeal: error format dataLen[%d]", dataLen);
		return;
	}
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_START_DATA) == 0 && (pUnit->flags & TLKMDI_FILE_FLAG_WAIT_WAIT_DATA) == 0){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartDataCmdDeal: unexpected packet");
		return;
	}

	tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_CLOSE_PAIR, nullptr, 0);
	tlksys_sendInnerMsg(TLKSYS_TASKID_LEMGR, TLKPTI_LE_MSGID_CLOSE_ADV, nullptr, 0);
	
	if((pUnit->flags & TLKMDI_FILE_FLAG_WAIT_START_DATA) == 0){
		tlkmdi_file_sendStartDataRspProc(pUnit);
		return;
	}
	pUnit->flags = TLKMDI_FILE_FLAG_NONE;

	ARRAY_TO_UINT16L(pData, 0, pUnit->fastSch);
	ARRAY_TO_UINT16L(pData, 2, pUnit->tranIntv);
	ARRAY_TO_UINT16L(pData, 4, pUnit->unitLens);
	ARRAY_TO_UINT32L(pData, 6, pUnit->fileSize);	
	tmemcpy(pUnit->param.code.fileSign, pData+10,  16);

	if(pUnit->status == 0 && (pUnit->attrs & TLKMDI_FILE_ATTR_CRYP) != 0 && csign != TLKPRT_FILE_CSIGN_CRYP){
		pUnit->status = TLKPRT_FILE_STATUS_DEC_FAILURE;
	}
	if(pUnit->status == 0){
		tlkmdi_file_fast_t *pSch;
		pSch = tlkmdi_file_getUsedFastSch(pUnit->fastSch);
		if(pSch == nullptr || pSch->sign == nullptr) ret = -TLK_EFAIL;
		else ret = pSch->sign(pUnit->fastSch, pUnit);
		if(ret != TLK_ENONE) pUnit->status = TLKPRT_FILE_STATUS_FAILURE;
	}
		
	if(pUnit->status == 0){
		ret = sTlkMdiFileCtrl.recvInfs.param(pUnit, TLKMDI_FILE_PARAM_TYPE_GET_START, nullptr, 0);
		if(ret != TLK_ENONE) pUnit->status = TLKPRT_FILE_STATUS_FAILURE;
	}
	if(pUnit->status == 0){
		ret = sTlkMdiFileCtrl.recvInfs.start(pUnit, false);
		if(ret != TLK_ENONE) pUnit->status = TLKPRT_FILE_STATUS_FAILURE;
	}

	if(pUnit->status != TLKPRT_FILE_STATUS_SUCCESS){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartDataCmdDeal: not support ");
		pUnit->state = TLKMDI_FILE_STATE_CLOSE;
		pUnit->flags = TLKMDI_FILE_FLAG_NONE;
		pUnit->csign = TLKPRT_FILE_CSIGN_NONE;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		return;
	}
	
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvStartDataCmdDeal");
	
	tlkmdi_file_digestInit(pUnit);

	pUnit->flags |= TLKMDI_FILE_FLAG_WAIT_WAIT_DATA;
	
	pUnit->idleTime = TLKMDI_FILE_IDLE_TIMEOUT;
	pUnit->nameLens = pData[0];
	pUnit->flags |= TLKMDI_FILE_FLAG_START;
	pUnit->flags |= TLKMDI_FILE_FLAG_DATA;
	tlkmdi_file_sendStartDataRspProc(pUnit);
}
static void tlkmdi_file_recvCloseTranCmdDeal(tlkmdi_file_unit_t *pUnit, uint08 csign, uint08 *pData, uint16 dataLen)
{
	uint08 status;
	
	if(dataLen < 2){
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCloseTranCmdDeal: error format dataLen[%d]", dataLen);
		return;
	}
	
	status = pData[0];
	if(status == TLKMDI_FILE_TRAN_STATUS_SUCCESS){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCloseTranCmdDeal: Trans Complete");
		if(pUnit->status == 0 && (pUnit->flags & TLKMDI_FILE_FLAG_START) == 0){
			pUnit->status = TLKPRT_FILE_STATUS_FAILURE;
		}
		if(pUnit->status == 0 && pUnit->dealSize != pUnit->fileSize){
			pUnit->status = TLKPRT_FILE_STATUS_ERR_LENGTH;
		}
		if(pUnit->status == 0) tlkmdi_file_digestFinish(pUnit);
		if(pUnit->status == 0 && tmemcmp(pUnit->param.code.fileSign, pUnit->param.code.fileSign, 16) != 0){
			pUnit->status = TLKPRT_FILE_STATUS_ERR_DIGEST;
		}
		if(pUnit->status == 0) pUnit->flags |= TLKMDI_FILE_FLAG_COMPELE;
	}else if(status == TLKMDI_FILE_TRAN_STATUS_PAUSED){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCloseTranCmdDeal: Trans Stop");
	}else{
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCloseTranCmdDeal: Trans Fail");
		pUnit->status = TLKPRT_FILE_STATUS_FAILURE;
	}
	
	tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_recvCloseTranCmdDeal");
	
	pUnit->state = TLKMDI_FILE_STATE_CLOSE;
	tlkmdi_file_sendCloseTranRspProc(pUnit);
}


static bool tlkmdi_file_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkmdi_file_unit_t *pUnit;

	pUnit = (tlkmdi_file_unit_t*)userArg;
	if(pUnit == nullptr || pUnit->state == TLKMDI_FILE_STATE_IDLE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_timer: error UsrArg or State");
		return false;
	}
	
	if(pUnit->busys != TLKMDI_FILE_BUSY_NONE && pUnit->tranRole == TLKMDI_FILE_TRAN_ROLE_CLIENT){
		tlkmdi_file_clientSendProc(pUnit);
	}else if(pUnit->busys != TLKMDI_FILE_BUSY_NONE && pUnit->tranRole == TLKMDI_FILE_TRAN_ROLE_CLIENT){
		tlkmdi_file_serverSendProc(pUnit);
	}else{
		pUnit->busys = TLKMDI_FILE_BUSY_NONE;
	}
	if(pUnit->idleTime != 0) pUnit->idleTime --;
	if(pUnit->idleTime == 0){
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
		if(sTlkMdiFileCtrl.recvInfs.close != nullptr && (pUnit->flags & TLKMDI_FILE_FLAG_START) != 0){
			if(pUnit->status == 0 && (pUnit->flags & TLKMDI_FILE_FLAG_COMPELE) != 0){
				sTlkMdiFileCtrl.recvInfs.close(pUnit, TLKMDI_FILE_TRAN_STATUS_SUCCESS);
			}else{
				sTlkMdiFileCtrl.recvInfs.close(pUnit, TLKMDI_FILE_TRAN_STATUS_FAILURE);
			}
		}
		tlkmdi_file_resetRecvUnit(pUnit);
		return false;
	}
	
	return true;
}
static void tlkmdi_file_clientSendProc(tlkmdi_file_unit_t *pUnit)
{
	if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_START_TRAN) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_START_TRAN;
		tlkmdi_file_sendStartTranRspProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_START_AUTH) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_START_AUTH;
		tlkmdi_file_sendStartAuthRspProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_CRYPT_SHAKE) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_CRYPT_SHAKE;
		tlkmdi_file_sendCryptShakeRspProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_SET_DATA_SCH) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_SET_DATA_SCH;
		tlkmdi_file_sendSetDataSchRspProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_SET_FILE_NAME) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_SET_FILE_NAME;
		tlkmdi_file_sendSetFileNameRspProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_FAST_START) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_FAST_START;
		tlkmdi_file_sendFastStartRspProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_START_DATA) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_START_DATA;
		tlkmdi_file_sendStartDataRspProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_CLOSE_TRAN) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_CLOSE_TRAN;
		tlkmdi_file_sendCloseTranRspProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_CLOSE_TRAN_EVT) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_CLOSE_TRAN_EVT;
		tlkmdi_file_sendCloseTranEvtProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_SYNC_STALL_EVT) != 0){
//		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_SYNC_STALL_EVT;
		tlkmdi_file_sendSyncStallEvtProc(pUnit);
	}else if((pUnit->busys & TLKMDI_FILE_BUSY_SEND_SYNC_SHAKE_EVT) != 0){
		pUnit->busys &= ~TLKMDI_FILE_BUSY_SEND_SYNC_SHAKE_EVT;
		tlkmdi_file_sendSyncShakeEvtProc(pUnit, pUnit->packNumb);
	}else{
		pUnit->busys = TLKMDI_FILE_BUSY_NONE;
	}
}
static void tlkmdi_file_serverSendProc(tlkmdi_file_unit_t *pUnit)
{
	pUnit->busys = TLKMDI_FILE_BUSY_NONE;
}


static int tlkmdi_file_sendRspPkt(uint08 optChn, uint16 handle, uint08 status, uint08 reason,
	uint08 cmdID, uint08 *pData, uint08 dataLen, uint08 *pSerial)
{
	int ret;
	uint08 headLen;
	uint08 header[12];

	headLen = 0;
	header[headLen++] = TLKPRT_COMM_PTYPE_RSP;
	header[headLen++] = TLKPRT_COMM_MTYPE_FILE; //MsgType
	header[headLen++] = cmdID;//MsgID
	if(pSerial == nullptr) header[headLen++] = 0x00;
	else header[headLen++] = *pSerial;
	header[headLen++] = dataLen+2;
	header[headLen++] = status;
	header[headLen++] = reason;
	ret = sTlkMdiFileCtrl.recvInfs.send(optChn, handle, header, headLen, pData, dataLen);
	if(ret == TLK_ENONE && pSerial != nullptr) *pSerial = (*pSerial)+1;
	return ret;
}
static int tlkmdi_file_sendEvtPkt(uint08 optChn, uint16 handle, uint08 evtID, 
	uint08 *pData, uint08 dataLen, uint08 *pSerial)
{
	int ret;
	uint08 headLen;
	uint08 header[12];

	headLen = 0;
	header[headLen++] = TLKPRT_COMM_PTYPE_EVT;
	header[headLen++] = TLKPRT_COMM_MTYPE_FILE; //MsgType
	header[headLen++] = evtID;//MsgID
	if(pSerial == nullptr) header[headLen++] = 0x00;
	else header[headLen++] = *pSerial;
	header[headLen++] = dataLen;
	ret = sTlkMdiFileCtrl.recvInfs.send(optChn, handle, header, headLen, pData, dataLen);
	if(ret == TLK_ENONE && pSerial != nullptr) *pSerial = (*pSerial)+1;
	return ret;
}

static void tlkmdi_file_sendStartTranRspProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 status;
	uint08 reason;
	uint08 dataLen;
	uint08 buffer[36];

	if(pUnit->status == TLKPRT_FILE_STATUS_SUCCESS) status = 0x00;
	else status = 0x01;
	reason = pUnit->status;
	
	dataLen = 0;
	buffer[dataLen++] = TLKPRT_FILE_CSIGN_NONE;
	buffer[dataLen++] = 0x00;
	buffer[dataLen++] = pUnit->filePort;
	buffer[dataLen++] = pUnit->dataPort;
	buffer[dataLen++] = (TLKPRT_FILE_VERSION & 0xFF);
	buffer[dataLen++] = (TLKPRT_FILE_VERSION & 0xFF00) >> 8;
	buffer[dataLen++] = (TLKPRT_FILE_COMPR_FEATURE_MASK & 0xFF);
	buffer[dataLen++] = (TLKPRT_FILE_COMPR_FEATURE_MASK & 0xFF00) >> 8;
	buffer[dataLen++] = (TLKPRT_FILE_COMPR_FEATURE_MASK & 0xFF0000) >> 16;
	buffer[dataLen++] = (TLKPRT_FILE_COMPR_FEATURE_MASK & 0xFF000000) >> 24;
	buffer[dataLen++] = (TLKPRT_FILE_DIGEST_FEATURE_MASK & 0xFF);
	buffer[dataLen++] = (TLKPRT_FILE_DIGEST_FEATURE_MASK & 0xFF00) >> 8;
	buffer[dataLen++] = (TLKPRT_FILE_DIGEST_FEATURE_MASK & 0xFF0000) >> 16;
	buffer[dataLen++] = (TLKPRT_FILE_DIGEST_FEATURE_MASK & 0xFF000000) >> 24;
	tmemcpy(buffer+dataLen, pUnit->param.rand.acpRand0, 16);
	dataLen += 16;
	if(tlkmdi_file_sendRspPkt(pUnit->optChn, pUnit->handle, status, reason, TLKPRT_COMM_CMDID_FILE_START_TRAN, 
		buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendStartTranRspProc %d", pUnit->filePort);
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_START_TRAN;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}
static void tlkmdi_file_sendStartAuthRspProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 count;
	uint08 index;
	uint08 status;
	uint08 reason;
	uint08 dataLen;
	uint08 buffer[36];
	uint16 safeSch[3] = {0};
	
	count = 0;
	for(index=0; index<TLKMDI_FILE_CRYP_SCH_NUMB; index++){
		if(sTlkMdiFileCtrl.cryp[index].schCode != 0){
			safeSch[count++] = sTlkMdiFileCtrl.cryp[index].schCode;
			if(count == 3) break;
		}
	}

	if(pUnit->status == TLKPRT_FILE_STATUS_SUCCESS) status = 0x00;
	else status = 0x01;
	reason = pUnit->status;
	
	dataLen = 0;
	buffer[dataLen++] = TLKPRT_FILE_CSIGN_AUTH;
	buffer[dataLen++] = pUnit->filePort;
	buffer[dataLen++] = (safeSch[0] & 0xFF);
	buffer[dataLen++] = (safeSch[0] & 0xFF00) >> 8;
	buffer[dataLen++] = (safeSch[1] & 0xFF);
	buffer[dataLen++] = (safeSch[1] & 0xFF00) >> 8;
	buffer[dataLen++] = (safeSch[2] & 0xFF);
	buffer[dataLen++] = (safeSch[2] & 0xFF00) >> 8;
	tmemcpy(buffer+dataLen, pUnit->param.rand.acpRand1, 16);
	dataLen += 16;
	if(tlkmdi_file_sendRspPkt(pUnit->optChn, pUnit->handle, status, reason, 
		TLKPRT_COMM_CMDID_FILE_START_AUTH, buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendStartAuthRspProc: %d", pUnit->filePort);
		if(pUnit->authSch != 0) pUnit->flags |= TLKMDI_FILE_FLAG_WAIT_CRYPT_SHAKE;
		else pUnit->flags |= TLKMDI_FILE_FLAG_WAIT_SET_DATA_SCH | TLKMDI_FILE_FLAG_WAIT_SET_FILE_NAME;
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_START_TRAN;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}
static void tlkmdi_file_sendCryptShakeRspProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 csign;
	uint08 status;
	uint08 reason;
	uint08 dataLen;
	uint08 buffer[32];

	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;

	if(pUnit->status == TLKPRT_FILE_STATUS_SUCCESS) status = 0x00;
	else status = 0x01;
	reason = pUnit->status;
		
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	tmemcpy(buffer+dataLen, pUnit->param.sign.acpCrypt, 16);
	dataLen += 16;
	if(tlkmdi_file_sendRspPkt(pUnit->optChn, pUnit->handle, status, reason, TLKPRT_COMM_CMDID_FILE_CRYPT_SHAKE, 
		buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendCryptShakeRspProc");
		pUnit->flags |= TLKMDI_FILE_FLAG_WAIT_SET_DATA_SCH | TLKMDI_FILE_FLAG_WAIT_SET_FILE_NAME;
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_CRYPT_SHAKE;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}
static void tlkmdi_file_sendSetDataSchRspProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 csign;
	uint08 status;
	uint08 reason;
	uint08 dataLen;
	uint08 buffer[32];
	
	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;

	if(pUnit->status == TLKPRT_FILE_STATUS_SUCCESS) status = 0x00;
	else status = 0x01;
	reason = pUnit->status;
	
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	if(tlkmdi_file_sendRspPkt(pUnit->optChn, pUnit->handle, status, reason, TLKPRT_COMM_CMDID_FILE_SET_DATA_SCH, 
		buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendSetDataSchRspProc");
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_SET_DATA_SCH;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}
static void tlkmdi_file_sendSetFileNameRspProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 csign;
	uint08 status;
	uint08 reason;
	uint08 dataLen;
	uint08 buffer[32];
	
	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;

	if(pUnit->status == TLKPRT_FILE_STATUS_SUCCESS) status = 0x00;
	else status = 0x01;
	reason = pUnit->status;
	
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	if(tlkmdi_file_sendRspPkt(pUnit->optChn, pUnit->handle, status, reason, TLKPRT_COMM_CMDID_FILE_SET_FILE_NAME, 
		buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendSetFileNameRspProc");
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_SET_FILE_NAME;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}
static void tlkmdi_file_sendFastStartRspProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 csign;
	uint08 status;
	uint08 reason;
	uint08 dataLen;
	uint08 buffer[32];
	
	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;

	if(pUnit->status == TLKPRT_FILE_STATUS_SUCCESS) status = 0x00;
	else status = 0x01;
	reason = pUnit->status;
	
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	if(tlkmdi_file_sendRspPkt(pUnit->optChn, pUnit->handle, status, reason, TLKPRT_COMM_CMDID_FILE_FAST_START, 
		buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendFastStartRspProc");
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_FAST_START;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}
static void tlkmdi_file_sendStartDataRspProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 csign;
	uint08 status;
	uint08 reason;
	uint08 dataLen;
	uint08 buffer[36];
	
	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;

	if(pUnit->status == TLKPRT_FILE_STATUS_SUCCESS) status = 0x00;
	else status = 0x01;
	reason = pUnit->status;
	
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	if(pUnit->waitMask == 0) buffer[dataLen++] = 0x00;
	else buffer[dataLen++] = pUnit->waitMask+1;
	buffer[dataLen++] = 0x00;
	buffer[dataLen++] = 0xFF;
	buffer[dataLen++] = 0xFF;
	buffer[dataLen++] = (pUnit->tranIntv & 0xFF);
	buffer[dataLen++] = (pUnit->tranIntv & 0xFF00) >> 8;
	buffer[dataLen++] = (pUnit->unitLens & 0xFF);
	buffer[dataLen++] = (pUnit->unitLens & 0xFF00) >> 8;
	if(tlkmdi_file_sendRspPkt(pUnit->optChn, pUnit->handle, status, reason, TLKPRT_COMM_CMDID_FILE_START_DATA, 
		buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendStartDataRspProc");
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_START_DATA;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}
static void tlkmdi_file_sendCloseTranRspProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 csign;
	uint08 status;
	uint08 reason;
	uint08 dataLen;
	uint08 buffer[32];
	
	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;

	if(pUnit->status == TLKPRT_FILE_STATUS_SUCCESS) status = 0x00;
	else status = 0x01;
	reason = pUnit->status;
	
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	if(tlkmdi_file_sendRspPkt(pUnit->optChn, pUnit->handle, status, reason, TLKPRT_COMM_CMDID_FILE_CLOSE_TRAN, 
		buffer, dataLen, &pUnit->serial) != TLK_ENONE){
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_CLOSE_TRAN;
		tlkapi_timer_insertNode(&pUnit->timer);
	}else{
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendCloseTranRspProc");
		if((pUnit->flags & TLKMDI_FILE_FLAG_START) != 0){
			if(pUnit->status != 0) status = TLKMDI_FILE_TRAN_STATUS_FAILURE;
			else if((pUnit->flags & TLKMDI_FILE_FLAG_COMPELE) != 0) status = TLKMDI_FILE_TRAN_STATUS_SUCCESS;
			else status = TLKMDI_FILE_TRAN_STATUS_PAUSED;
			if(sTlkMdiFileCtrl.recvInfs.close != nullptr) sTlkMdiFileCtrl.recvInfs.close(pUnit, status);
			tlkmdi_file_resetRecvUnit(pUnit);
		}
	}
}
static void tlkmdi_file_sendCloseTranEvtProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 csign;
	uint08 dataLen;
	uint08 buffer[32];
	
	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;
	
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	if(pUnit->status != 0x00) buffer[dataLen++] = 0x01;
	else if(pUnit->dealSize != pUnit->fileSize) buffer[dataLen++] = 0x02;
	else buffer[dataLen++] = 0x00;
	if(tlkmdi_file_sendEvtPkt(pUnit->optChn, pUnit->handle, TLKPRT_COMM_EVTID_FILE_CLOSE, 
		buffer, dataLen, &pUnit->serial) != TLK_ENONE){
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_CLOSE_TRAN_EVT;
		tlkapi_timer_insertNode(&pUnit->timer);
	}else{
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_sendCloseTranEvtProc");
	}
}
static void tlkmdi_file_sendSyncShakeEvtProc(tlkmdi_file_unit_t *pUnit, uint32 packNumb)
{
	uint08 csign;
	uint08 dataLen;
	uint08 buffer[32];
	
	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;
	
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	buffer[dataLen++] = (packNumb & 0xFF);
	buffer[dataLen++] = (packNumb & 0xFF00) >> 8;
	buffer[dataLen++] = (packNumb & 0xFF0000) >> 16;
	buffer[dataLen++] = (packNumb & 0xFF000000) >> 24;
	if(tlkmdi_file_sendEvtPkt(pUnit->optChn, pUnit->handle, TLKPRT_COMM_EVTID_FILE_SHAKE, 
		buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN,
			"tlkmdi_file_sendSyncShakeEvtProc: packNumb[%d]",
			packNumb);
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_SYNC_SHAKE_EVT;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}
static void tlkmdi_file_sendSyncStallEvtProc(tlkmdi_file_unit_t *pUnit)
{
	uint08 csign;
	uint08 dataLen;
	uint08 buffer[32];
	
	if((pUnit->attrs & TLKMDI_FILE_STATE_CRYPT) == 0) csign = 0;
	else csign = TLKPRT_FILE_CSIGN_CRYP;
	
	dataLen = 0;
	buffer[dataLen++] = csign;
	buffer[dataLen++] = pUnit->filePort;
	buffer[dataLen++] = ((pUnit->packNumb+1) & 0xFF);
	buffer[dataLen++] = ((pUnit->packNumb+1) & 0xFF00) >> 8;
	buffer[dataLen++] = ((pUnit->packNumb+1) & 0xFF0000) >> 16;
	buffer[dataLen++] = ((pUnit->packNumb+1) & 0xFF000000) >> 24;
	if(tlkmdi_file_sendEvtPkt(pUnit->optChn, pUnit->handle, TLKPRT_COMM_EVTID_FILE_SYNC, 
		buffer, dataLen, &pUnit->serial) == TLK_ENONE){
		tlkapi_trace(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, 
			"tlkmdi_file_sendSyncStallEvtProc: packNumb[%d]", 
			pUnit->packNumb+1);
	}else{
		pUnit->busys |= TLKMDI_FILE_BUSY_SEND_SYNC_STALL_EVT;
		tlkapi_timer_insertNode(&pUnit->timer);
	}
}






static tlkmdi_file_auth_t *tlkmdi_file_getIdleAuthSch(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_FILE_AUTH_SCH_NUMB; index++){
		if(sTlkMdiFileCtrl.auth[index].schCode == 0) break;
	}
	if(index == TLKMDI_FILE_AUTH_SCH_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.auth[index];
}
static tlkmdi_file_auth_t *tlkmdi_file_getUsedAuthSch(uint16 schCode)
{
	uint08 index;
	if(schCode == 0) return nullptr;
	for(index=0; index<TLKMDI_FILE_AUTH_SCH_NUMB; index++){
		if(sTlkMdiFileCtrl.auth[index].schCode == schCode) break;
	}
	if(index == TLKMDI_FILE_AUTH_SCH_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.auth[index];
}
static tlkmdi_file_cryp_t *tlkmdi_file_getIdleCrypSch(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_FILE_CRYP_SCH_NUMB; index++){
		if(sTlkMdiFileCtrl.cryp[index].schCode == 0) break;
	}
	if(index == TLKMDI_FILE_CRYP_SCH_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.cryp[index];
}
static tlkmdi_file_cryp_t *tlkmdi_file_getUsedCrypSch(uint16 schCode)
{
	uint08 index;
	if(schCode == 0) return nullptr;
	for(index=0; index<TLKMDI_FILE_CRYP_SCH_NUMB; index++){
		if(sTlkMdiFileCtrl.cryp[index].schCode == schCode) break;
	}
	if(index == TLKMDI_FILE_CRYP_SCH_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.cryp[index];
}
static tlkmdi_file_fast_t *tlkmdi_file_getIdleFastSch(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_FILE_FAST_SCH_NUMB; index++){
		if(sTlkMdiFileCtrl.fast[index].schCode == 0) break;
	}
	if(index == TLKMDI_FILE_FAST_SCH_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.fast[index];
}
static tlkmdi_file_fast_t *tlkmdi_file_getUsedFastSch(uint16 schCode)
{
	uint08 index;
	if(schCode == 0) return nullptr;
	for(index=0; index<TLKMDI_FILE_FAST_SCH_NUMB; index++){
		if(sTlkMdiFileCtrl.fast[index].schCode == schCode) break;
	}
	if(index == TLKMDI_FILE_FAST_SCH_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.fast[index];
}



static void tlkmdi_file_resetRecvUnit(tlkmdi_file_unit_t *pUnit)
{
	tlkapi_timer_removeNode(&pUnit->timer);
	tmemset(pUnit, 0, sizeof(tlkmdi_file_unit_t));
}
static tlkmdi_file_unit_t *tlkmdi_file_getIdleRecvUnit(void)
{
	uint08 index;
	tlkmdi_file_unit_t *pUnit;
	for(index=0; index<TLKMDI_FILE_TRAN_RECV_NUMB; index++){
		pUnit = &sTlkMdiFileCtrl.recvUnit[index];
		if(pUnit->state == TLKMDI_FILE_STATE_IDLE) break;
	}
	if(index == TLKMDI_FILE_TRAN_RECV_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.recvUnit[index];
}
static tlkmdi_file_unit_t *tlkmdi_file_getUsedRecvUnitByFilePort(uint08 optChn, uint16 handle, uint08 filePort)
{
	uint08 index;
	tlkmdi_file_unit_t *pUnit;
	if(optChn == 0 || handle == 0 || filePort == 0) return nullptr;
	for(index=0; index<TLKMDI_FILE_TRAN_RECV_NUMB; index++){
		pUnit = &sTlkMdiFileCtrl.recvUnit[index];
		if(pUnit->optChn == optChn || pUnit->handle == handle || pUnit->filePort == filePort) break;
	}
	if(index == TLKMDI_FILE_TRAN_RECV_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.recvUnit[index];
}
static tlkmdi_file_unit_t *tlkmdi_file_getUsedRecvUnitByDataPort(uint08 optChn, uint16 handle, uint08 dataPort)
{
	uint08 index;
	tlkmdi_file_unit_t *pUnit;
	if(optChn == 0 || handle == 0 || dataPort == 0) return nullptr;
	for(index=0; index<TLKMDI_FILE_TRAN_RECV_NUMB; index++){
		pUnit = &sTlkMdiFileCtrl.recvUnit[index];
		if(pUnit->optChn == optChn || pUnit->handle == handle || pUnit->dataPort == dataPort) break;
	}
	if(index == TLKMDI_FILE_TRAN_RECV_NUMB) return nullptr;
	return &sTlkMdiFileCtrl.recvUnit[index];
}


static int tlkmdi_file_authSignDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit)
{
	if(schCode == 0x8101){
		uint08 sign0[16] = {0};
		uint08 sign1[16] = {0};
		tlkalg_md5_context_t context;
		tlkalg_md5_digest_t digest;
		tlkalg_md5_init(&context);
		tlkalg_md5_update(&context, pUnit->param.rand.iniRand0, 16);
		tlkalg_md5_update(&context, sTlkMdiFileCtrl.usrAuthCode, 16);
		tlkalg_md5_finish(&context, &digest);
		tmemcpy(sign0, digest.value, 16);
		tlkalg_md5_init(&context);
		tlkalg_md5_update(&context, pUnit->param.rand.acpRand0, 16);
		tlkalg_md5_update(&context, sTlkMdiFileCtrl.usrAuthCode, 16);
		tlkalg_md5_finish(&context, &digest);
		tmemcpy(sign1, digest.value, 16);
		tlkalg_md5_init(&context);
		tlkalg_md5_update(&context, sign0, 16);
		tlkalg_md5_update(&context, sign1, 16);
		tlkalg_md5_finish(&context, &digest);
		tmemcpy(pUnit->authCode, digest.value, 16);
	}
	else{
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_authDefault: not support scheme code[0x%x].", schCode);
 		return -TLK_EFAIL;
	}
	return TLK_ENONE;
}
static int tlkmdi_file_authCrypDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit, bool isEnc, uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}

static int tlkmdi_file_crypSignDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit, uint08 stage)
{
	if(schCode == 0x9101){
		tlkalg_md5_context_t context;
		tlkalg_md5_digest_t digest;
		if(stage == TLKMDI_FILE_CRYP_STAGE_GEN_INI_SIGN){
			tlkalg_md5_init(&context);
			tlkalg_md5_update(&context, pUnit->authCode, 16);
			tlkalg_md5_update(&context, pUnit->param.rand.iniRand1, 16);
			tlkalg_md5_update(&context, sTlkMdiFileCtrl.usrCrypCode, 16);
			tlkalg_md5_finish(&context, &digest);
			tmemcpy(pUnit->param.sign.iniCrypt, digest.value, 16);
		}
		else if(stage == TLKMDI_FILE_CRYP_STAGE_GEN_ACP_SIGN){
			tlkalg_md5_init(&context);
			tlkalg_md5_update(&context, pUnit->authCode, 16);
			tlkalg_md5_update(&context, pUnit->param.rand.acpRand1, 16);
			tlkalg_md5_update(&context, sTlkMdiFileCtrl.usrCrypCode, 16);
			tlkalg_md5_finish(&context, &digest);
			tmemcpy(pUnit->param.sign.acpCrypt, digest.value, 16);
		}
		else if(stage == TLKMDI_FILE_CRYP_STAGE_GEN_CRYPT_CODE){
			uint08 sign0[16] = {0};
			uint08 sign1[16] = {0};
			tlkalg_md5_init(&context);
			tlkalg_md5_update(&context, pUnit->param.sign.iniCrypt, 16);
			tlkalg_md5_update(&context, pUnit->param.sign.acpCrypt, 16);
			tlkalg_md5_finish(&context, &digest);
			tmemcpy(sign0, digest.value, 16);
			tlkalg_md5_init(&context);
			tlkalg_md5_update(&context, pUnit->authCode, 16);
			tlkalg_md5_update(&context, sTlkMdiFileCtrl.usrCrypCode, 16);
			tlkalg_md5_finish(&context, &digest);
			tmemcpy(sign1, digest.value, 16);
			tlkalg_md5_init(&context);
			tlkalg_md5_update(&context, sign0, 16);
			tlkalg_md5_update(&context, sign1, 16);
			tlkalg_md5_finish(&context, &digest);
			tmemcpy(pUnit->param.code.crypCode, digest.value, 16);
		}
		else{
			tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_crypDefault: not support stage[%d].", stage);
			return -TLK_EFAIL;
		}
	}
	else{
		tlkapi_error(TLKMDI_FILE_DBG_FLAG, TLKMDI_FILE_DBG_SIGN, "tlkmdi_file_crypDefault: not support scheme code[0x%x].", schCode);
		return -TLK_EFAIL;
	}
	return TLK_ENONE;
}
static int tlkmdi_file_crypCrypDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit, bool isEnc, uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static int tlkmdi_file_fastSignDefault(uint16 schCode, tlkmdi_file_unit_t *pUnit)
{
	tlkalg_md5_context_t context;
	tlkalg_md5_digest_t digest;
	tlkalg_md5_init(&context);
	tlkalg_md5_update(&context, pUnit->fileName, pUnit->nameLens);
	tlkalg_md5_update(&context, pUnit->param.sign.iniCrypt, 16);
	tlkalg_md5_update(&context, pUnit->param.sign.acpCrypt, 16);
	tlkalg_md5_finish(&context, &digest);
	tmemcpy(pUnit->param.code.fastCode, digest.value, 16);
	return TLK_ENONE;
}


static void tlkmdi_file_digestInit(tlkmdi_file_unit_t *pUnit)
{
	if(pUnit->digSch == TLKPRT_FILE_DIGEST_FEATURE_CRC32){
		tlkalg_crc32_init(&pUnit->digest.crc32);
	}
}
static void tlkmdi_file_digestUpdate(tlkmdi_file_unit_t *pUnit, uint08 *pData, uint16 dataLen)
{
	if(pUnit->digSch == TLKPRT_FILE_DIGEST_FEATURE_CRC32){
		tlkalg_crc32_update(&pUnit->digest.crc32, pData, dataLen);
	}
}
static void tlkmdi_file_digestFinish(tlkmdi_file_unit_t *pUnit)
{
	if(pUnit->digSch == TLKPRT_FILE_DIGEST_FEATURE_CRC32){
		tlkalg_crc32_finish(&pUnit->digest.crc32);
		pUnit->param.code.fileSign[0] = (pUnit->digest.crc32 & 0xFF);
		pUnit->param.code.fileSign[1] = (pUnit->digest.crc32 & 0xFF00);
		pUnit->param.code.fileSign[2] = (pUnit->digest.crc32 & 0xFF0000);
		pUnit->param.code.fileSign[3] = (pUnit->digest.crc32 & 0xFF000000);
	}
}


#endif //#if (TLK_MDI_FILE_ENABLE)

