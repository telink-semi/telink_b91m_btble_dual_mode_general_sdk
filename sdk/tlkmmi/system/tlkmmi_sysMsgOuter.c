/********************************************************************************************************
 * @file	tlkmmi_sysMsgOuter.c
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
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_sys.h"
#include "tlkmmi_sysCtrl.h"
#include "tlkmmi_sysMsgOuter.h"
#include "tlkmmi_sysMsgInner.h"
#include "tlkmmi_sysAdapt.h"
#if (TLK_CFG_DEV_ENABLE)
#include "tlkdev/tlkdev.h"
#endif
#include "tlklib/dbg/tlkdbg_config.h"

#if (TLK_CFG_USB_ENABLE)
extern void tlkcfg_setUsbMode(uint08 umode);
void tlkcfg_setSerialBaudrate(uint32 baudrate);
#endif
extern int bth_hci_sendResetCmd(void);
extern uint tlkcfg_getWorkMode(void);
extern void tlkcfg_setSerialBaudrate(uint32 baudrate);
#if (TLK_CFG_COMM_ENABLE)
extern void delay_ms(unsigned int millisec);
extern int tlkmdi_comm_setBaudrate(uint32 baudrate);
extern uint32 tlkmdi_comm_getBaudrate();
#endif
#if (TLK_CFG_TEST_ENABLE)
extern void tlkcfg_setWorkMode(TLK_WORK_MODE_ENUM wmode);
extern void core_reboot(void);
#endif
#if (TLK_CFG_USB_ENABLE && TLK_MDI_AUDUAC_ENABLE)
extern uint08 tlkusb_get_curMode();
#endif

static void tlkmmi_sys_recvGetVersionCmdDeal(void);
static void tlkmmi_sys_recvRebootCmdDeal(void);
static void tlkmmi_sys_recvPowerOffCmdDeal(void);
static void tlkmmi_sys_recvSetHeartCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvGetWorkModeCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvSetWorkModeCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvFormatUDiskCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvSetUSBModeCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_sys_recvSetbaudrateCmdDeal(uint08 *pData, uint08 dataLen);




int tlkmmi_sys_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_sysMsgHandler:msgID-%d", msgID);
	if(msgID == TLKPRT_COMM_CMDID_SYS_VERSION){
		tlkmmi_sys_recvGetVersionCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_REBOOT){
		tlkmmi_sys_recvRebootCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_POWER_OFF){
		tlkmmi_sys_recvPowerOffCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_HEART){
		tlkmmi_sys_recvSetHeartCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_GET_WORK_MODE){
		tlkmmi_sys_recvGetWorkModeCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE){
		tlkmmi_sys_recvSetWorkModeCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_FORMAT_U_DISK){
		tlkmmi_sys_recvFormatUDiskCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_USB_MODE){
		tlkmmi_sys_recvSetUSBModeCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_SYS_SET_BAUDRATE){
		tlkmmi_sys_recvSetbaudrateCmdDeal(pData,dataLen);
	}
	return TLK_ENONE;
}


static void tlkmmi_sys_recvGetVersionCmdDeal(void)
{
	uint08 buffLen;
	uint08 buffer[12];
	uint16 prtVersion;
	uint32 libVersion;
	uint32 appVersion;

	libVersion = TLK_LIB_VERSION;
	prtVersion = TLK_PRT_VERSION;
	appVersion = TLK_APP_VERSION;
	
	buffLen = 0;
	buffer[buffLen++] = (libVersion & 0xFF000000) >> 24;
	buffer[buffLen++] = (libVersion & 0x00FF0000) >> 16;
	buffer[buffLen++] = (libVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (libVersion & 0x000000FF);
	buffer[buffLen++] = (prtVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (prtVersion & 0x000000FF);
	buffer[buffLen++] = (appVersion & 0xFF000000) >> 24;
	buffer[buffLen++] = (appVersion & 0x00FF0000) >> 16;
	buffer[buffLen++] = (appVersion & 0x0000FF00) >> 8;
	buffer[buffLen++] = (appVersion & 0x000000FF);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_VERSION, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_sys_recvRebootCmdDeal(void)
{
	bth_hci_sendResetCmd();
	tlkmmi_sys_setPowerParam(300000, true);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_REBOOT, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, nullptr, 0);
}
static void tlkmmi_sys_recvPowerOffCmdDeal(void)
{
	bth_hci_sendResetCmd();
	tlkmmi_sys_setPowerParam(300000, false);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_POWER_OFF, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, nullptr, 0);
}
static void tlkmmi_sys_recvSetHeartCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 enable;
	uint16 timeout;
	
	enable = pData[0];
	timeout = pData[1];
	if(timeout == 0) timeout = 3;
	if(dataLen < 2 || timeout > 100){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_HEART, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	if(enable) tlkmmi_sys_setHeartParam(timeout*1000000);
	else tlkmmi_sys_setHeartParam(0);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_HEART, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, &enable, 1);
}
static void tlkmmi_sys_recvGetWorkModeCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 data[4];
	data[0] = tlkcfg_getWorkMode();
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_GET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, data, 1);
}
static void tlkmmi_sys_recvSetWorkModeCmdDeal(uint08 *pData, uint08 dataLen)
{
#if !(TLK_CFG_TEST_ENABLE)
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
#else
	if(dataLen == 0){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_WORK_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
	}else{
		tlkcfg_setWorkMode(pData[0]);
		core_reboot();
	}
#endif
}
static void tlkmmi_sys_recvFormatUDiskCmdDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_DEV_STORE_ENABLE)
	if(tlkdev_state(TLKDEV_DID_SDCARD0) == TLKDEV_STATE_CLOSED){
		tlkdev_open(TLKDEV_DID_SDCARD0, 0);
	}
	tlkdev_ioctl(TLKDEV_DID_SDCARD0, TLKDEV_OPCODE_STORE_FORMAT, 0, 0);
	tlkdev_close(TLKDEV_DID_SDCARD0);
#endif
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_FORMAT_U_DISK, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}
static void tlkmmi_sys_recvSetUSBModeCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 1){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgSetUSBModeDeal: length error - %d", dataLen);
		return;
	}
	tlkapi_array(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvDbgSetUSBModeDeal: ", pData, dataLen);
	#if (TLK_CFG_USB_ENABLE && TLK_MDI_AUDUAC_ENABLE)
	if(tlkusb_get_curMode() == TLKUSB_MODTYPE_UAC && tlkmdi_auduac_isBusy()){
		tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_UAC, 0xFFFF);
	}
	#endif	// (TLK_CFG_USB_ENABLE && TLK_MDI_AUDUAC_ENABLE)
	#if (TLK_CFG_USB_ENABLE)
	tlkdev_ioctl(TLKDEV_DID_USB, TLKDEV_OPCODE_USB_CHANGE_MODE, pData[0], 0);
	tlkcfg_setUsbMode(pData[0]);
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_USB_MODE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	#else
	tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_USB_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	#endif
}
static void tlkmmi_sys_recvSetbaudrateCmdDeal(uint08 * pData, uint08 dataLen)
{
	if(dataLen < 4){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvSetbaudrateCmdDeal: length error - %d", dataLen);
		return;
	}
	tlkapi_array(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_recvSetbaudrateCmdDeal: ", pData, dataLen);
	#if (TLK_CFG_COMM_ENABLE)
	uint32 baudrate = 0;
	baudrate |= (pData[0] & 0xFF);
	baudrate |= ((pData[1] & 0xFF) << 8);
	baudrate |= ((pData[2] & 0xFF) << 16);
	baudrate |= ((pData[3] & 0xFF) << 24);
	if(baudrate < 9600 || baudrate == tlkmdi_comm_getBaudrate()){
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_BAUDRATE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}else{
		tlkmmi_sys_sendCommRsp(TLKPRT_COMM_CMDID_SYS_SET_BAUDRATE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, pData, dataLen);
	}
	delay_ms(100);
	tlkmdi_comm_setBaudrate(baudrate);
	tlkcfg_setSerialBaudrate(baudrate);
	
	#endif
}

#endif //#if (TLKMMI_SYSTEM_ENABLE)

