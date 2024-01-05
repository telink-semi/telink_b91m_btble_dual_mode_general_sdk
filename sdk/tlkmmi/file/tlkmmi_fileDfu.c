/********************************************************************************************************
 * @file	tlkmmi_fileDfu.c
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
#include "tlk_config.h" 
#include "tlkapi/tlkapi_stdio.h"
#include "tlkmdi/misc/tlkmdi_file.h"
#include "tlkmmi_fileConfig.h"
#include "tlkmmi_file.h"
#if (TLKMMI_FILE_DFU_ENABLE)
#include "tlkmmi_fileAdapt.h"
#include "tlkmmi_fileCtrl.h"
#include "tlkmmi_fileDfu.h"

#include "tlksys/prt/tlkpto_stdio.h"
#include "tlksys/prt/tlkpto_file.h"
#include "tlkalg/digest/md5/tlkalg_md5.h"
#include "tlklib/fs/tlkfs.h"
#include "tlkapi/tlkapi_file.h"
#include "drivers.h"
#if (TLK_CFG_COMM_ENABLE)
#include "tlkmdi/misc/tlkmdi_comm.h"
#endif

#define TLKMMI_FILE_DFU_SAVE_SIGN        0x3A
#define TLKMMI_FILE_DFU_SAVE_VERS        0x03
#define TLKMMI_FILE_DFU_SAVE_ADDR        TLK_CFG_FLASH_OTA_PARAM_ADDR
#define TLKMMI_FILE_DFU_SAVE_SIZE        sizeof(tlkmdi_file_saveParam_t)

extern unsigned int tlkcfg_getFlashAddr(unsigned int offset);
extern void bt_ll_schedule_acl_bandwidth_policy_enter(uint16 con_handle);
extern void bt_ll_schedule_acl_bandwidth_policy_exit(void);


static int tlkmmi_file_dfuInit(void);
static int tlkmmi_file_dfuStart(tlkmdi_file_unit_t *pUnit, bool isFast);
static int tlkmmi_file_dfuClose(tlkmdi_file_unit_t *pUnit, uint08 status);
static int tlkmmi_file_dfuParam(tlkmdi_file_unit_t *pUnit, uint08 paramType, uint08 *pParam, uint08 paramLen);
static int tlkmmi_file_dfuSave(tlkmdi_file_unit_t *pUnit, uint32 offset, uint08 *pData, uint16 dataLen);
#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
#if (TLK_CFG_COMM_ENABLE)
static void tlkmmi_file_dfuData(uint08 datID, uint32 number, uint08 *pData, uint16 dataLen);
#endif
#endif

static void tlkmmi_file_dfuOverHandler(bool isSucc);



const tlkmmi_file_recvIntf_t gcTlkMmiFileDfuIntf = {
	TLKPRT_FILE_TYPE_DFU,
	tlkmmi_file_dfuInit,
	tlkmmi_file_dfuStart,
	tlkmmi_file_dfuClose,
	tlkmmi_file_dfuParam,
	tlkmmi_file_dfuSave,
};
#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
static uint08 sTlkMdiFileDfuPort = 0;
#endif

typedef struct{
	uint08 isStart;
	uint08 saveIsOK;
	uint16 reserve1;
	#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
	uint16 updateNum;
	uint16 updateCnt;
	tlkapi_save_ctrl_t saveCtrl;
	tlkmdi_file_saveParam_t saveParam;
	#endif
}tlkmmi_file_dfuCtrl_t;


static tlkmmi_file_dfuCtrl_t sTlkMmiFileDfuCtrl;


bool tlkmmi_file_dfuIsStart(void)
{
	return sTlkMmiFileDfuCtrl.isStart;
}


static int tlkmmi_file_dfuInit(void)
{
	#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
	int ret;
	#endif
	
	tmemset(&sTlkMmiFileDfuCtrl, 0, sizeof(tlkmmi_file_dfuCtrl_t));
	#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
		unsigned int dfuAddr = tlkcfg_getFlashAddr(TLKMMI_FILE_DFU_SAVE_ADDR);
		if(dfuAddr == 0) return -TLK_EFAIL;

		tlkapi_save2_init(&sTlkMmiFileDfuCtrl.saveCtrl, TLKMMI_FILE_DFU_SAVE_SIGN, TLKMMI_FILE_DFU_SAVE_VERS,
			TLKMMI_FILE_DFU_SAVE_SIZE, dfuAddr);
		ret = tlkapi_save2_load(&sTlkMmiFileDfuCtrl.saveCtrl, (uint08*)&sTlkMmiFileDfuCtrl.saveParam, TLKMMI_FILE_DFU_SAVE_SIZE);
		if(ret > 0) sTlkMmiFileDfuCtrl.saveIsOK = true;
		sTlkMmiFileDfuCtrl.saveParam.status = 0xFF;
		sTlkMmiFileDfuCtrl.saveParam.datPos = TLKMMI_FILE_DFU_SAVE_METHOD;
		sTlkMmiFileDfuCtrl.saveParam.dataAddr = TLKMMI_FILE_DFU_SAVE_OFFSET;
	#else
		#if (TLKMMI_FILE_DFU_SAVE_METHOD == TLKMMI_FILE_DFU_SAVE_INNER)
		uint32 offset;
		uint08 buffer[16] = {0};
		tlkapi_flash_read(TLKMMI_FILE_DFU_SAVE_OFFSET, buffer, 16);
		if(buffer[0] != 0xFF || buffer[1] != 0xFF || buffer[2] != 0xFF || buffer[13] != 0xFF || buffer[15] != 0xFF){
			for(offset=0; offset<TLKMMI_FILE_DFU_MAX_SIZE; offset+=4096){
				tlkapi_flash_eraseSector(TLKMMI_FILE_DFU_SAVE_OFFSET+offset);
			}
		}
		#endif
	#endif
	
	#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
		#if (TLK_CFG_COMM_ENABLE)
		tlkmdi_comm_getValidDatID(&sTlkMdiFileDfuPort);
		tlkmdi_comm_regDatCB(sTlkMdiFileDfuPort, tlkmmi_file_dfuData, true);
		#endif
	#endif
	
	return TLK_ENONE;
}

#if (TLKMMI_FILE_CHN_SERIAL_ENABLE)
#if (TLK_CFG_COMM_ENABLE)
static void tlkmmi_file_dfuData(uint08 datID, uint32 number, uint08 *pData, uint16 dataLen)
{
	uint08 headLen;
	uint08 header[12];
		
	headLen = 0;
	header[headLen++] = TLKPRT_COMM_PTYPE_DAT;
	header[headLen++] = datID;
	header[headLen++] = (number & 0xFF);
	header[headLen++] = (number & 0xFF00) >> 8;
	header[headLen++] = (dataLen & 0xFF);
	header[headLen++] = ((dataLen & 0x0F00) >> 8) | ((number & 0x0F0000) >> 12);
	tlkmdi_file_recvHandler(TLKMDI_FILE_OPTCHN_SERIAL, 0xFFFF, header, headLen, pData, dataLen);
}
#endif
#endif


static int tlkmmi_file_dfuStart(tlkmdi_file_unit_t *pUnit, bool isFast)
{
	if(sTlkMmiFileDfuCtrl.isStart){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuStart: reject - busying");
		return -TLK_EBUSY;
	}
	
	if(isFast){
		#if !(TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
		return -TLK_EFAIL;
		#else
		if(!sTlkMmiFileDfuCtrl.saveIsOK){
			tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuStart: reject - fast is not ready");
			return -TLK_EBUSY;
		}
		if(sTlkMmiFileDfuCtrl.saveParam.fastSch != pUnit->fastSch || tmemcmp(pUnit->param.code.fastCode, sTlkMmiFileDfuCtrl.saveParam.fastCode, 16) != 0){
			tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuStart: reject - fast code is not peer");
			return -TLK_EBUSY;
		}
		tlkmdi_file_setRecvFastParam(pUnit, &sTlkMmiFileDfuCtrl.saveParam);
		#endif
	}

	#if (TLKMMI_FILE_DFU_VERSION_CHECK_ENABLE)
	if(pUnit->fileVers <= TLK_APP_VERSION){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuStart: reject - version is overdue");
		return -TLK_EFAIL;
	}
	#endif
	#if (TLKMMI_FILE_DFU_FORCE_AUTH_ENABLE)
	if(pUnit->authSch == 0){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuStart: reject - not authentication");
		return -TLK_EFAIL;
	}
	#endif
	#if (TLKMMI_FILE_DFU_FORCE_CRYP_ENABLE)
	if(pUnit->cryptSch == 0){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuStart: reject - not encryption");
		return -TLK_EFAIL;
	}
	#endif
	if(pUnit->fileSize > TLKMMI_FILE_DFU_MAX_SIZE){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuStart: reject - file size is too large");
		return -TLK_EFAIL;
	}
	
	sTlkMmiFileDfuCtrl.isStart = true;
	#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
	if(sTlkMmiFileDfuCtrl.saveIsOK){
		tlkapi_save2_clean(&sTlkMmiFileDfuCtrl.saveCtrl);
		sTlkMmiFileDfuCtrl.saveIsOK = false;
	}
	#endif
	#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
	if(pUnit->optChn == TLKMDI_FILE_OPTCHN_SERIAL){
		sTlkMmiFileDfuCtrl.updateNum = 0;
		sTlkMmiFileDfuCtrl.updateCnt = TLKMMI_FILE_DFU_SERIAL_SAVE_INTV;
	}else if(pUnit->optChn == TLKMDI_FILE_OPTCHN_BT_SPP){
		sTlkMmiFileDfuCtrl.updateNum = 0;
		sTlkMmiFileDfuCtrl.updateCnt = TLKMMI_FILE_DFU_BT_SPP_SAVE_INTV;
	}else if(pUnit->optChn == TLKMDI_FILE_OPTCHN_BT_ATT){
		sTlkMmiFileDfuCtrl.updateNum = 0;
		sTlkMmiFileDfuCtrl.updateCnt = TLKMMI_FILE_DFU_BT_ATT_SAVE_INTV;
	}else if(pUnit->optChn == TLKMDI_FILE_OPTCHN_LE_ATT){
		sTlkMmiFileDfuCtrl.updateNum = 0;
		sTlkMmiFileDfuCtrl.updateCnt = TLKMMI_FILE_DFU_LE_ATT_SAVE_INTV;
	}else{
		sTlkMmiFileDfuCtrl.updateNum = 0;
		sTlkMmiFileDfuCtrl.updateCnt = 0;
	}
	if(sTlkMmiFileDfuCtrl.updateCnt != 0){
		if(tlkmdi_file_getRecvSaveParam(pUnit, &sTlkMmiFileDfuCtrl.saveParam) != TLK_ENONE){
			sTlkMmiFileDfuCtrl.updateNum = 0;
			sTlkMmiFileDfuCtrl.updateCnt = 0;
			sTlkMmiFileDfuCtrl.saveParam.status = 0x00;
			sTlkMmiFileDfuCtrl.saveParam.datPos = 0x00;
			sTlkMmiFileDfuCtrl.saveParam.dataAddr = 0;
		}
	}
	#endif

	if(pUnit->optChn == TLKMDI_FILE_OPTCHN_BT_SPP || pUnit->optChn == TLKMDI_FILE_OPTCHN_BT_ATT){
		bt_ll_schedule_acl_bandwidth_policy_enter(pUnit->handle);
		tlksys_switch_clock(SYS_CHIP_CLOCK_96M);
	}
			
	return TLK_ENONE;
}
static int tlkmmi_file_dfuClose(tlkmdi_file_unit_t *pUnit, uint08 status)
{
	unsigned int dfuAddr;

	dfuAddr = tlkcfg_getFlashAddr(TLKMMI_FILE_DFU_SAVE_ADDR);
	if(dfuAddr == 0) return -TLK_EFAIL;

	if(pUnit->optChn == TLKMDI_FILE_OPTCHN_BT_SPP || pUnit->optChn == TLKMDI_FILE_OPTCHN_BT_ATT){
		bt_ll_schedule_acl_bandwidth_policy_exit();
		tlksys_switch_clock(SYS_CHIP_CLOCK_48M);
	}

	sTlkMmiFileDfuCtrl.isStart = false;
	if(status == TLKMDI_FILE_TRAN_STATUS_SUCCESS){
		uint08 flag = TLKMMI_FILE_DFU_START_FLAG;
		tlkapi_flash_write(TLKMMI_FILE_DFU_SAVE_OFFSET+TLKMMI_FILE_DFU_START_OFFSET, &flag, 1);
		#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
		tlkmdi_file_getRecvSaveParam(pUnit, &sTlkMmiFileDfuCtrl.saveParam);
		#else
		tlkapi_save_ctrl_t ctrl;
		tlkmdi_file_saveParam_t param;
		tlkapi_save2_init(&ctrl, TLKMMI_FILE_DFU_SAVE_SIGN, TLKMMI_FILE_DFU_SAVE_VERS,
			TLKMMI_FILE_DFU_SAVE_SIZE, dfuAddr);
		tlkapi_save2_clean(&ctrl);
		tlkapi_save2_load(&ctrl, (uint08*)&param, TLKMMI_FILE_DFU_SAVE_SIZE);
		param.status = 0x00;
		param.datPos = TLKMMI_FILE_DFU_SAVE_METHOD;
		param.dataAddr = TLKMMI_FILE_DFU_SAVE_OFFSET;
		tlkmdi_file_getRecvSaveParam(pUnit, &param);
		#endif
		tlkmmi_file_dfuOverHandler(true);
	}else if(status == TLKMDI_FILE_TRAN_STATUS_FAILURE){
		tlkmmi_file_dfuOverHandler(false);
	}else{
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuClose: error status");
		tlkmmi_file_dfuOverHandler(false);
		return -TLK_EPARAM;
	}
	return TLK_ENONE;
}
static int tlkmmi_file_dfuParam(tlkmdi_file_unit_t *pUnit, uint08 paramType, uint08 *pParam, uint08 paramLen)
{
	uint08 optChn;
	
	if(pUnit == nullptr){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuParam: error param");
		return -TLK_EPARAM;
	}
	
//	tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuParam: ...");
	optChn = pUnit->optChn;
	if(paramType == TLKMDI_FILE_PARAM_TYPE_GET_PORT){
		tlkmdi_file_portParam_t param;
//		tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuParam: 000: %d", sTlkMdiFileDfuPort);
		if(optChn == TLKMDI_FILE_OPTCHN_SERIAL){
			param.dataPort = sTlkMdiFileDfuPort;
		}else{
			param.dataPort = 1;
		}
		return tlkmdi_file_setRecvPortParam(pUnit, &param);
	}
	else if(paramType == TLKMDI_FILE_PARAM_TYPE_GET_FAST){
		#if !(TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
		return -TLK_EFAIL;
		#else
		if(sTlkMmiFileDfuCtrl.saveIsOK && pUnit->optChn == sTlkMmiFileDfuCtrl.saveParam.optChn){
			tlkmdi_file_setRecvFastParam(pUnit, &sTlkMmiFileDfuCtrl.saveParam);
		}else{
			tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuParam: fast start failure");
			return -TLK_EFAIL;
		}
		#endif
	}
	else if(paramType == TLKMDI_FILE_PARAM_TYPE_GET_START){
		tlkmdi_file_startParam_t param;
		if(optChn == TLKMDI_FILE_OPTCHN_SERIAL){
			param.intvNumb = TLKMMI_FILE_DFU_SERIAL_WAIT_INTV;
			param.tranIntv = TLKMMI_FILE_DFU_SERIAL_TRAN_INTV;
			param.unitLens = TLKMMI_FILE_DFU_SERIAL_UNIT_LENS;
		}else if(optChn == TLKMDI_FILE_OPTCHN_BT_SPP){
			param.intvNumb = TLKMMI_FILE_DFU_BT_SPP_WAIT_INTV;
			param.tranIntv = TLKMMI_FILE_DFU_BT_SPP_TRAN_INTV;
			param.unitLens = TLKMMI_FILE_DFU_BT_SPP_UNIT_LENS;
		}else if(optChn == TLKMDI_FILE_OPTCHN_BT_ATT){
			param.intvNumb = TLKMMI_FILE_DFU_BT_ATT_WAIT_INTV;
			param.tranIntv = TLKMMI_FILE_DFU_BT_ATT_TRAN_INTV;
			param.unitLens = TLKMMI_FILE_DFU_BT_ATT_UNIT_LENS;
		}else if(optChn == TLKMDI_FILE_OPTCHN_LE_ATT){
			param.intvNumb = TLKMMI_FILE_DFU_LE_ATT_WAIT_INTV;
			param.tranIntv = TLKMMI_FILE_DFU_LE_ATT_TRAN_INTV;
			param.unitLens = TLKMMI_FILE_DFU_LE_ATT_UNIT_LENS;
		}else{
			tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuParam[GET_START]: error optChn[%d]", optChn);
			return -TLK_ENOSUPPORT;
		}
		if(param.unitLens > pUnit->unitLens){
			param.unitLens = pUnit->unitLens & 0xFFF0;
		}
		return tlkmdi_file_setRecvStartParam(pUnit, &param);
	}	
	
	return -TLK_ENOSUPPORT;
}

static int tlkmmi_file_dfuSave(tlkmdi_file_unit_t *pUnit, uint32 offset, uint08 *pData, uint16 dataLen)
{
	if(offset <= TLKMMI_FILE_DFU_START_OFFSET && offset+dataLen >= TLKMMI_FILE_DFU_START_OFFSET){
		if(pData[TLKMMI_FILE_DFU_START_OFFSET-offset] != TLKMMI_FILE_DFU_START_FLAG){
			tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_dfuSave: rejected -- not telink DFU");
			tlkmdi_file_closeTrans(pUnit);
			return TLK_ENONE;
		}
	}

	#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
	if(sTlkMmiFileDfuCtrl.updateCnt != 0){
		sTlkMmiFileDfuCtrl.updateNum ++;
		if(sTlkMmiFileDfuCtrl.updateNum >= sTlkMmiFileDfuCtrl.updateCnt){
			int ret;
			sTlkMmiFileDfuCtrl.updateNum = 0;
			ret = tlkapi_save2_smartSave(&sTlkMmiFileDfuCtrl.saveCtrl, (uint08*)&sTlkMmiFileDfuCtrl.saveParam, TLKMMI_FILE_DFU_SAVE_SIZE);
			if(ret == TLK_ENONE && !sTlkMmiFileDfuCtrl.saveIsOK){
				sTlkMmiFileDfuCtrl.saveIsOK = true;
			}
		}
	}
	#endif
	
	#if (TLKMMI_FILE_DFU_SAVE_METHOD == TLKMMI_FILE_DFU_SAVE_INNER)
		#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
		if((offset & 0xFFF) == 0){
			tlkapi_flash_eraseSector(TLKMMI_FILE_DFU_SAVE_OFFSET+offset);
		}
		#endif
		if(offset < TLKMMI_FILE_DFU_START_OFFSET && offset+dataLen > TLKMMI_FILE_DFU_START_OFFSET){
			pData[TLKMMI_FILE_DFU_START_OFFSET-offset] = 0xFF;
		}
		if(offset != 0x00 || dataLen < 256){
			tlkapi_flash_write(TLKMMI_FILE_DFU_SAVE_OFFSET+offset, pData, dataLen);
		}else{
			tlkapi_flash_write(TLKMMI_FILE_DFU_SAVE_OFFSET+offset, pData, 64);
			tlkapi_flash_write(TLKMMI_FILE_DFU_SAVE_OFFSET+offset+64, pData+64, dataLen-64);
		}
	#else
		return -TLK_ENOSUPPORT;
	#endif
	return TLK_ENONE;
}



static void tlkmmi_file_dfuOverHandler(bool isSucc)
{
	#if (TLKMMI_FILE_DFU_BK_RESUME_ENABLE)
	if(isSucc) sTlkMmiFileDfuCtrl.saveParam.status = 0x00; //Success
	else sTlkMmiFileDfuCtrl.saveParam.status = 0x01; //Failure
	sTlkMmiFileDfuCtrl.saveParam.datPos = TLKMMI_FILE_DFU_SAVE_METHOD;
	sTlkMmiFileDfuCtrl.saveParam.dataAddr = TLKMMI_FILE_DFU_SAVE_OFFSET;
	tlkapi_save2_smartSave(&sTlkMmiFileDfuCtrl.saveCtrl, (uint08*)&sTlkMmiFileDfuCtrl.saveParam, TLKMMI_FILE_DFU_SAVE_SIZE);
	#endif
	#if (TLK_DBG_LOG_ENABLE)
	tlkdbg_delayForPrint(100000);
	#else
	delay_ms(100);
	#endif
	
	core_reboot();
}





#endif //#if (TLKMMI_FILE_ENABLE)

