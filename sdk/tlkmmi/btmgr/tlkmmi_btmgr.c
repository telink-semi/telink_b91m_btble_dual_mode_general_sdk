/********************************************************************************************************
 * @file	tlkmmi_btmgr.c
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
#if (TLKMMI_BTMGR_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_btmgr.h"
#include "tlkmmi_btmgrAdapt.h"
#include "tlkmmi_btmgrCtrl.h"
#include "tlkmmi_btmgrAcl.h"
#include "tlkmmi_btmgrInq.h"
#include "tlkmmi_btmgrRec.h"
#include "tlkmmi_btmgrMsgInner.h"
#include "tlkmmi_btmgrMsgOuter.h"

#include "tlkmdi/bt/tlkmdi_bt.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#if (TLK_MDI_BTIAP_ENABLE)
#include "tlkmdi/bt/tlkmdi_btiap.h"
#endif


TLKSYS_MMI_TASK_DEFINE(btmgr, Btmgr);


static int tlkmmi_btmgr_init(uint08 procID, uint16 taskID)
{
	uint08 buffLen;
	uint08 buffer[8];

	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_BT;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00) >> 8;
	tlksys_sendInnerMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_REG_COMM_TASK, buffer, buffLen);
	
	#if (TLK_STK_BT_ENABLE)
	tlkmdi_bt_init(procID);
	#endif
	
	tlkmmi_btmgr_adaptInit(procID);
	tlkmmi_btmgr_ctrlInit();
	#if (TLKMMI_BTMGR_BTACL_ENABLE)
	tlkmmi_btmgr_aclInit();
	#endif
	#if (TLK_MDI_BTINQ_ENABLE)
	tlkmmi_btmgr_inqInit();
	#endif
	#if (TLK_MDI_BTREC_ENABLE)
	tlkmmi_btmgr_recInit();
	#endif
	
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_BTMGR_DEVICE_CLASS);
    bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode
    bth_hci_sendWriteSecureConnHostSupportCmd(1);// enable SecureConnHost
	
	return TLK_ENONE;
}
static int tlkmmi_btmgr_start(void)
{
	bth_device_item_t *pDevice;
	pDevice = bth_device_getLast();
	if(pDevice != nullptr){
		#if TLKMMI_BTMGR_BTREC_ENABLE
		tlkmmi_btmgr_recStart(pDevice->devAddr, pDevice->devClass, true, true);
		#endif
	}else{
		#if TLKMMI_BTMGR_BTREC_ENABLE
		tlkmmi_btmgr_recStart(nullptr, 0, false, false);
		#endif
	}
	return TLK_ENONE;
}
static int tlkmmi_btmgr_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_btmgr_close(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_btmgr_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_btmgr_innerMsgHandler(msgID, pData, dataLen);
	}else{
		return tlkmmi_btmgr_outerMsgHandler(msgID, pData, dataLen);
	}
}
static void tlkmmi_btmgr_handler(void)
{
	tlkmdi_bt_handler();
	#if (TLK_MDI_BTIAP_ENABLE)
	tlkmdi_btiap_handler();
	#endif
}





#endif //#if (TLKMMI_BTMGR_ENABLE)

