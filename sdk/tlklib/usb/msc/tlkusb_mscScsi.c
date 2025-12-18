/********************************************************************************************************
 * @file	tlkusb_mscScsi.c
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
#include "tlklib/usb/tlkusb_stdio.h"
#if (TLK_USB_MSC_ENABLE)
#include "drivers.h"
#include "tlklib/usb/msc/tlkusb_mscDefine.h"
#include "tlklib/usb/msc/tlkusb_msc.h"
#include "tlklib/usb/msc/tlkusb_mscDesc.h"
#include "tlklib/usb/msc/tlkusb_mscCtrl.h"
#include "tlklib/usb/msc/tlkusb_mscScsi.h"

static int tlkusb_msc_scsiSendData(uint08 *pData, uint08 dataLen);

static void tlkusb_msc_scsiSetStatus(uint08 *pTag, uint32 residue, uint08 status);
static void tlkusb_msc_scsiRecvData(void);

static void tlkusb_msc_scsiStageIdleDeal(void);
static void tlkusb_msc_scsiStageDataDeal(void);
static void tlkusb_msc_scsiStageStatusDeal(void);
static void tlkusb_msc_scsiStageCompleteDeal(void);

static void tlkusb_msc_scsiStageDataSendDeal(void);
static void tlkusb_msc_scsiStageDataReadDeal(void);
static void tlkusb_msc_scsiStageDataRecvDeal(void);


static void tlkusb_msc_scsiRecvTestUnitDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvRequestSenseDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvInquiryDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvModeSense6Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvModeSense10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvStartStopUnitDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvAllowMediumRemoveDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvReadFormatCapacityDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvReadCapacity10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvRead10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvWrite10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);
static void tlkusb_msc_scsiRecvVerify10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen);



static tlkusb_msc_scsi_t sTlkUsbMscScsi;


int tlkusb_msc_scsiInit(void)
{
	tmemset(&sTlkUsbMscScsi, 0, sizeof(tlkusb_msc_scsi_t));
	
	sTlkUsbMscScsi.cswBuff[0] = 'U';
	sTlkUsbMscScsi.cswBuff[1] = 'S';
	sTlkUsbMscScsi.cswBuff[2] = 'B';
	sTlkUsbMscScsi.cswBuff[3] = 'S';

	sTlkUsbMscScsi.enable = true;
	
	return TLK_ENONE;
}


void tlkusb_msc_scsiReset(void)
{
	sTlkUsbMscScsi.enable = true;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_IDLE;
	sTlkUsbMscScsi.flags = TLKUSB_MSC_SCSI_FLAG_NONE;

	sTlkUsbMscScsi.optLen = 0;
	sTlkUsbMscScsi.datLen = 0;
	sTlkUsbMscScsi.blkCnt = 0;
	sTlkUsbMscScsi.blkNum = 0;
	sTlkUsbMscScsi.blkOff = 0;
}

void tlkusb_msb_scsiEnable(bool enable)
{
	sTlkUsbMscScsi.enable = true;
}

void tlkusb_msc_scsiHandler(void)
{
	if(sTlkUsbMscScsi.stage == TLKUSB_MSC_SCSI_STAGE_IDLE){
		tlkusb_msc_scsiStageIdleDeal();
	}

	if(sTlkUsbMscScsi.stage == TLKUSB_MSC_SCSI_STAGE_DATA){
		tlkusb_msc_scsiStageDataDeal();
	}else if(sTlkUsbMscScsi.stage == TLKUSB_MSC_SCSI_STAGE_STATUS){
		tlkusb_msc_scsiStageStatusDeal();
	}else if(sTlkUsbMscScsi.stage == TLKUSB_MSC_SCSI_STAGE_COMPLETE){
		tlkusb_msc_scsiStageCompleteDeal();
	}
}

static void tlkusb_msc_scsiSetStatus(uint08 *pTag, uint32 residue, uint08 status)
{
	if(pTag != nullptr){
		sTlkUsbMscScsi.cswBuff[4] = pTag[0];
		sTlkUsbMscScsi.cswBuff[5] = pTag[1];
		sTlkUsbMscScsi.cswBuff[6] = pTag[2];
		sTlkUsbMscScsi.cswBuff[7] = pTag[3];
	}
	sTlkUsbMscScsi.cswBuff[8] = residue & 0xFF;
	sTlkUsbMscScsi.cswBuff[9] = (residue>>8) & 0xFF;
	sTlkUsbMscScsi.cswBuff[10] = (residue>>16) & 0xFF;
	sTlkUsbMscScsi.cswBuff[11] = (residue>>24) & 0xFF;
	sTlkUsbMscScsi.cswBuff[12] = status;
}
static void tlkusb_msc_scsiRecvData(void)
{
	uint irq;
	uint08 index;

	sTlkUsbMscScsi.rcvLen = 0;
	irq = usbhw_get_eps_irq();
	if((irq & BIT(TLKUSB_MSC_EDP_OUT)) == 0) return;
	
	usbhw_clr_eps_irq(BIT(TLKUSB_MSC_EDP_OUT));
	sTlkUsbMscScsi.rcvLen = reg_usb_ep_ptr(TLKUSB_MSC_EDP_OUT);
	if(sTlkUsbMscScsi.rcvLen > 64) sTlkUsbMscScsi.rcvLen = 64;
	usbhw_reset_ep_ptr(TLKUSB_MSC_EDP_OUT);
	for(index=0; index<sTlkUsbMscScsi.rcvLen; index++){
		sTlkUsbMscScsi.rcvBuff[index] = reg_usb_ep_dat(TLKUSB_MSC_EDP_OUT);
	}
	usbhw_data_ep_ack(TLKUSB_MSC_EDP_OUT); // set the ack
}



static void tlkusb_msc_scsiStageIdleDeal(void)
{
	uint08 logNum;
	uint08 opcode;
	uint08 cmdLen;
	uint08 *pCmd;
	tlkusb_msc_disk_t *pUnit;
	
	tlkusb_msc_scsiRecvData();
	if(sTlkUsbMscScsi.rcvLen == 0) return;
	
	if(sTlkUsbMscScsi.rcvLen < TLKUSB_MSC_CBW_LENGTH || sTlkUsbMscScsi.rcvBuff[0] != 'U' || sTlkUsbMscScsi.rcvBuff[1] != 'S' 
		|| sTlkUsbMscScsi.rcvBuff[2] != 'B' || sTlkUsbMscScsi.rcvBuff[3] != 'C'){
		return; //Error Length
	}
	
	pCmd = sTlkUsbMscScsi.rcvBuff+TLKUSB_MSC_CBW_CB_OFFSET;
	
	opcode = pCmd[0];
	logNum = pCmd[1] >> 5;
	cmdLen = pCmd[TLKUSB_MSC_CBW_BLEN_OFFSET] & 0x1F;
	pUnit = tlkusb_msc_getDisk(logNum);
	if(pUnit == nullptr){
		tlkusb_msc_scsiSetStatus(sTlkUsbMscScsi.rcvBuff+4, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
		sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
		return;
	}
	
	sTlkUsbMscScsi.curLun = logNum;
	tlkusb_msc_scsiSetStatus(sTlkUsbMscScsi.rcvBuff+4, 0, 0);

	switch(opcode){
		case TLKUSB_MSC_SCSI_TEST_UNIT_READY:
			tlkusb_msc_scsiRecvTestUnitDeal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_REQUEST_SENSE:
			tlkusb_msc_scsiRecvRequestSenseDeal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_INQUIRY:
			tlkusb_msc_scsiRecvInquiryDeal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_MODE_SENSE6:
			tlkusb_msc_scsiRecvModeSense6Deal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_MODE_SENSE10:
			tlkusb_msc_scsiRecvModeSense10Deal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_START_STOP_UNIT:
			tlkusb_msc_scsiRecvStartStopUnitDeal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_ALLOW_MEDIUM_REMOVAL:
			tlkusb_msc_scsiRecvAllowMediumRemoveDeal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_READ_FORMAT_CAPACITIES:
			tlkusb_msc_scsiRecvReadFormatCapacityDeal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_READ_CAPACITY10:
			tlkusb_msc_scsiRecvReadCapacity10Deal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_READ10:
			tlkusb_msc_scsiRecvRead10Deal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_WRITE10:
			tlkusb_msc_scsiRecvWrite10Deal(pUnit, pCmd, cmdLen);
			break;
		case TLKUSB_MSC_SCSI_VERIFY10:
			tlkusb_msc_scsiRecvVerify10Deal(pUnit, pCmd, cmdLen);
			break;
		default:
			tlkusb_msc_scsiSetStatus(nullptr, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
			sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
			break;
	}
}



static void tlkusb_msc_scsiStageDataDeal(void)
{
	if((sTlkUsbMscScsi.flags & TLKUSB_MSC_SCSI_FLAG_SEND_DATA) != 0){
		tlkusb_msc_scsiStageDataSendDeal();
		if(sTlkUsbMscScsi.datLen == 0){
			sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
		}
	}else if((sTlkUsbMscScsi.flags & TLKUSB_MSC_SCSI_FLAG_READ_DATA) != 0){
		tlkusb_msc_scsiStageDataReadDeal();
	}else if((sTlkUsbMscScsi.flags & TLKUSB_MSC_SCSI_FLAG_RECV_DATA) != 0){
		tlkusb_msc_scsiStageDataRecvDeal();
	}
}
static void tlkusb_msc_scsiStageStatusDeal(void)
{
	int ret;	
	ret = tlkusb_msc_scsiSendData(sTlkUsbMscScsi.cswBuff, TLKUSB_MSC_CSW_LENGTH);
	if(ret == TLK_ENONE){
		sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_COMPLETE;
	}
}
static void tlkusb_msc_scsiStageCompleteDeal(void)
{
	if(usbhw_is_ep_busy(TLKUSB_MSC_EDP_IN)) return;
	
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_IDLE;
	sTlkUsbMscScsi.flags = TLKUSB_MSC_SCSI_FLAG_NONE;
	sTlkUsbMscScsi.optLen = 0;
	sTlkUsbMscScsi.datLen = 0;
	sTlkUsbMscScsi.blkCnt = 0;
	sTlkUsbMscScsi.blkNum = 0;
	sTlkUsbMscScsi.blkOff = 0;
}





static void tlkusb_msc_scsiStageDataSendDeal(void)
{
	int ret;
	uint08 sendLen;
	
	if(usbhw_is_ep_busy(TLKUSB_MSC_EDP_IN)) return;
	
	if(sTlkUsbMscScsi.optLen+64 <= sTlkUsbMscScsi.datLen) sendLen = 64;
	else sendLen = sTlkUsbMscScsi.datLen-sTlkUsbMscScsi.optLen;
	
	if(sendLen == 0) ret = TLK_ENONE;
	else ret = tlkusb_msc_scsiSendData(sTlkUsbMscScsi.buffer+sTlkUsbMscScsi.optLen, sendLen);
	if(ret == TLK_ENONE) sTlkUsbMscScsi.optLen += sendLen;
	if(sTlkUsbMscScsi.optLen < sTlkUsbMscScsi.datLen) return;
	
	sTlkUsbMscScsi.optLen = 0;
	sTlkUsbMscScsi.datLen = 0;
}
static void tlkusb_msc_scsiStageDataReadDeal(void)
{
	int ret;
	tlkusb_msc_disk_t *pUnit;
	
	if(sTlkUsbMscScsi.datLen != 0){
		tlkusb_msc_scsiStageDataSendDeal();
		return;
	}

	if(sTlkUsbMscScsi.blkNum >= sTlkUsbMscScsi.blkCnt){
		sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
		return;
	}
	
	pUnit = tlkusb_msc_getDisk(sTlkUsbMscScsi.curLun);
	if(pUnit == nullptr || pUnit->Read == nullptr || pUnit->blkSize > TLKUSB_MSC_BLOCK_SIZE){
		tlkusb_msc_scsiSetStatus(nullptr, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
		sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
		return;
	}
	
	ret = pUnit->Read(sTlkUsbMscScsi.buffer, sTlkUsbMscScsi.blkOff+sTlkUsbMscScsi.blkNum, 1);
	if(ret < 0){
		tlkusb_msc_scsiSetStatus(nullptr, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
		sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
		return;
	}
	
	sTlkUsbMscScsi.blkNum += 1;
	sTlkUsbMscScsi.datLen = pUnit->blkSize;
	sTlkUsbMscScsi.optLen = 0;
}
static void tlkusb_msc_scsiStageDataRecvDeal(void)
{
	tlkusb_msc_disk_t *pUnit;
	
	if(sTlkUsbMscScsi.blkNum >= sTlkUsbMscScsi.blkCnt){
		sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
		return;
	}
	
	tlkusb_msc_scsiRecvData();
	if(sTlkUsbMscScsi.rcvLen == 0) return;
	
	pUnit = tlkusb_msc_getDisk(sTlkUsbMscScsi.curLun);
	if(pUnit == nullptr || pUnit->Write == nullptr || pUnit->blkSize > TLKUSB_MSC_BLOCK_SIZE
		|| sTlkUsbMscScsi.optLen+sTlkUsbMscScsi.rcvLen > pUnit->blkSize){
		tlkusb_msc_scsiSetStatus(nullptr, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
		sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
		return;
	}
	
	tmemcpy(sTlkUsbMscScsi.buffer+sTlkUsbMscScsi.optLen, sTlkUsbMscScsi.rcvBuff, sTlkUsbMscScsi.rcvLen);
	sTlkUsbMscScsi.optLen += sTlkUsbMscScsi.rcvLen;
	
	if(sTlkUsbMscScsi.optLen == pUnit->blkSize){
		int ret;
		ret = pUnit->Write(sTlkUsbMscScsi.buffer, sTlkUsbMscScsi.blkOff+sTlkUsbMscScsi.blkNum, 1);
		if(ret < 0){
			tlkusb_msc_scsiSetStatus(nullptr, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
			sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
		}else{
			sTlkUsbMscScsi.optLen = 0;
			sTlkUsbMscScsi.blkNum += 1;
		}
	}
}



static void tlkusb_msc_scsiRecvTestUnitDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	if(!sTlkUsbMscScsi.enable){
		tlkusb_msc_scsiSetStatus(nullptr, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
	}
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
}
static void tlkusb_msc_scsiRecvRequestSenseDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	uint08 datLen = 0;
	sTlkUsbMscScsi.buffer[datLen++] = 0x70; //Error code, fixed as 0x70
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Reserved
	sTlkUsbMscScsi.buffer[datLen++] = 0x05; //Sense Key 0x05,Indicates an invalid command opcode(ILLEGAL REQUEST)
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Information as 0
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x0A; //The length of additional data is 10 bytes
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Reserved
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x20; //Additional Sense Code(ASC) 0x20,Indicates an invalid command opcode(INVALID COMMAND OPERATION CODE)
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Additional Sense Code Qualifier(ASCQ)
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Reserved
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.datLen = datLen;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_DATA;
	sTlkUsbMscScsi.flags |= TLKUSB_MSC_SCSI_FLAG_SEND_DATA;
}
static void tlkusb_msc_scsiRecvInquiryDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	char *pStr;
	uint08 tmpLen = 0;
	uint08 datLen = 0;
		
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Disk device
	if(pUnit->hotPlug) sTlkUsbMscScsi.buffer[datLen++] = 0x80; //The most significant D7 is RMB.  RMB=0, which means the device cannot be removed.  If RMB=1, it is a removable device.
	else sTlkUsbMscScsi.buffer[datLen++] = 0x00; //The most significant D7 is RMB.  RMB=0, which means the device cannot be removed.  If RMB=1, it is a removable device.
	sTlkUsbMscScsi.buffer[datLen++] = 0x02; //ISOVersion[7~6], ECMA Version[5~3], ANSI Version[2~1]-0x02 new version usb
	sTlkUsbMscScsi.buffer[datLen++] = 0x02; //Data response format
	sTlkUsbMscScsi.buffer[datLen++] = 0x1F; //Additional data length, 31 bytes
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Reserved
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Reserved
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Reserved
	tmemset(sTlkUsbMscScsi.buffer+datLen, 0x20, 8); //Manufacturer: 8 bytes
	if(pUnit->pVendorStr == nullptr) pStr = TLKUSB_MSC_VENDOR_STRING_DEF;
	else pStr = pUnit->pVendorStr;
	tmpLen = tstrlen(pStr);
	if(tmpLen > 8) tmpLen = 8;
	if(tmpLen != 0){
		tmemcpy(sTlkUsbMscScsi.buffer+datLen, pStr, tmpLen);
	}
	datLen += 8;
	tmemset(sTlkUsbMscScsi.buffer+datLen, 0x20, 16); //Product: 16 Bytes
	if(pUnit->pProductStr == nullptr) pStr = TLKUSB_MSC_PRODUCE_STRING_DEF;
	else pStr = pUnit->pProductStr;
	tmpLen = tstrlen(pStr);
	if(tmpLen > 16) tmpLen = 16;
	if(tmpLen != 0){
		tmemcpy(sTlkUsbMscScsi.buffer+datLen, pStr, tmpLen);
	}
	datLen += 16;
	tmemset(sTlkUsbMscScsi.buffer+datLen, 0x20, 4); //Version: 4 Bytes
	if(pUnit->pVersionStr == nullptr) pStr = TLKUSB_MSC_VERSION_STRING_DEF;
	else pStr = pUnit->pVersionStr;
	tmpLen = tstrlen(pStr);
	if(tmpLen > 4) tmpLen = 4;
	if(tmpLen != 0){
		tmemcpy(sTlkUsbMscScsi.buffer+datLen, pStr, tmpLen);
	}
	datLen += 4;
	
	sTlkUsbMscScsi.datLen = datLen;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_DATA;
	sTlkUsbMscScsi.flags |= TLKUSB_MSC_SCSI_FLAG_SEND_DATA;
}
static void tlkusb_msc_scsiRecvModeSense6Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	uint08 datLen = 0;
	sTlkUsbMscScsi.buffer[datLen++] = 0x03;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
//	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
//	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
//	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
//	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.datLen = datLen;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_DATA;
	sTlkUsbMscScsi.flags |= TLKUSB_MSC_SCSI_FLAG_SEND_DATA;
}
static void tlkusb_msc_scsiRecvModeSense10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	uint08 datLen = 0;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x06;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
//	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
//	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
//	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
//	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.datLen = datLen;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_DATA;
	sTlkUsbMscScsi.flags |= TLKUSB_MSC_SCSI_FLAG_SEND_DATA;
}
static void tlkusb_msc_scsiRecvStartStopUnitDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
}
static void tlkusb_msc_scsiRecvAllowMediumRemoveDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	//pCmd[4] bit0~1 <PREVENT>
	//PREVENT = 00b: Volume removal shall be allowed
	//PREVENT = 01b: Volume removal shall be prevented.
	//PREVENT = 10b: Obsolete
	//PREVENT = 11b: Obsolete
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
	if((pCmd[4] & 0x03) == 0x01){
		tlkusb_msc_scsiSetStatus(nullptr, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
	}
}
static void tlkusb_msc_scsiRecvReadFormatCapacityDeal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	uint08 datLen = 0;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00; //Reserved
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x00;
	sTlkUsbMscScsi.buffer[datLen++] = 0x08; //Capacity list length
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkCount >> 24) & 0xFF; //Number of blocks (maximum support 8GB)
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkCount >> 16) & 0xFF;
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkCount >> 8) & 0xFF;
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkCount & 0xFF);
	sTlkUsbMscScsi.buffer[datLen++] = 0x02; //The descriptor code is 2, indicating the maximum formatted capacity supported
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkSize >> 16) & 0xFF; //Each block size
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkSize >> 8) & 0xFF;
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkSize & 0xFF);
	sTlkUsbMscScsi.datLen = datLen;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_DATA;
	sTlkUsbMscScsi.flags |= TLKUSB_MSC_SCSI_FLAG_SEND_DATA;
}
static void tlkusb_msc_scsiRecvReadCapacity10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	uint08 datLen = 0;
//	sTlkUsbMscScsi.buffer[datLen++] = ((pUnit->blkCount-1) >> 24) & 0xFF; //Number of blocks (maximum support 8GB)
//	sTlkUsbMscScsi.buffer[datLen++] = ((pUnit->blkCount-1) >> 16) & 0xFF;
//	sTlkUsbMscScsi.buffer[datLen++] = ((pUnit->blkCount-1) >> 8) & 0xFF;
//	sTlkUsbMscScsi.buffer[datLen++] = ((pUnit->blkCount-1) & 0xFF);
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkCount >> 24) & 0xFF; //Number of blocks (maximum support 8GB)
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkCount >> 16) & 0xFF;
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkCount >> 8) & 0xFF;
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkCount & 0xFF);
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkSize >> 24) & 0xFF;
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkSize >> 16) & 0xFF; //Each block size
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkSize >> 8) & 0xFF;
	sTlkUsbMscScsi.buffer[datLen++] = (pUnit->blkSize & 0xFF);
	sTlkUsbMscScsi.datLen = datLen;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_DATA;
	sTlkUsbMscScsi.flags |= TLKUSB_MSC_SCSI_FLAG_SEND_DATA;
}
static void tlkusb_msc_scsiRecvRead10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	ARRAY_TO_UINT32H(pCmd, 2, sTlkUsbMscScsi.blkOff);
	ARRAY_TO_UINT16H(pCmd, 7, sTlkUsbMscScsi.blkCnt);
	sTlkUsbMscScsi.blkNum = 0;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_DATA;
	sTlkUsbMscScsi.flags |= TLKUSB_MSC_SCSI_FLAG_READ_DATA;
}
static void tlkusb_msc_scsiRecvWrite10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	ARRAY_TO_UINT32H(pCmd, 2, sTlkUsbMscScsi.blkOff);
	ARRAY_TO_UINT16H(pCmd, 7, sTlkUsbMscScsi.blkCnt);
	sTlkUsbMscScsi.blkNum = 0;
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_DATA;
	sTlkUsbMscScsi.flags |= TLKUSB_MSC_SCSI_FLAG_RECV_DATA;
}
static void tlkusb_msc_scsiRecvVerify10Deal(tlkusb_msc_disk_t *pUnit, uint08 *pCmd, uint08 cmdLen)
{
	if((pCmd[1] & 0x01) != 0){ //Evpd is set
		tlkusb_msc_scsiSetStatus(nullptr, 0, TLKUSB_MSC_CSW_STATUS_CMD_FAILED);
	}
	sTlkUsbMscScsi.stage = TLKUSB_MSC_SCSI_STAGE_STATUS;
}


/******************************************************************************
 * Function: tlkusb_msc_sendData
 * Descript: This function serves to send string data back to PC by bulk transfer.
 * Params: 
 *     @ptr - the data string.
 *     @ptr - the data string.
 * Return: None.
 * Others: None.
*******************************************************************************/  
static int tlkusb_msc_scsiSendData(uint08 *pData, uint08 dataLen)
{
	uint08 index;
	
	if(pData == nullptr || dataLen == 0) return -TLK_EPARAM;
	if(usbhw_is_ep_busy(TLKUSB_MSC_EDP_IN)) return -TLK_EBUSY;
		
	usbhw_reset_ep_ptr(TLKUSB_MSC_EDP_IN); // send status back to PC
	for(index=0; index<dataLen; index++){
		reg_usb_ep_dat(TLKUSB_MSC_EDP_IN) = pData[index];
	}
	usbhw_data_ep_ack(TLKUSB_MSC_EDP_IN); // set the ack
	return TLK_ENONE;
}





#endif //#if (TLK_USB_MSC_ENABLE)

