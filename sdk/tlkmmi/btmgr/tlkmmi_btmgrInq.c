/********************************************************************************************************
 * @file	tlkmmi_btmgrInq.c
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
#include "tlkmmi_btmgr.h"
#if (TLKMMI_BTMGR_BTINQ_ENABLE)
#include "tlkmmi_btmgrCtrl.h"
#include "tlkmmi_btmgrAcl.h"
#include "tlkmmi_btmgrInq.h"
#include "tlkmmi_btmgrRec.h"
#include "tlkmmi_btmgrMsgOuter.h"
#include "tlkmmi_btmgrMsgInner.h"

#include "tlksys/prt/tlkpto_stdio.h"
#include "tlkmdi/bt/tlkmdi_btacl.h"
#include "tlkmdi/bt/tlkmdi_btinq.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"



static int  tlkmmi_btmgr_inquiryReportCB(uint32 devClass, uint08 rssi, uint08 nameLen, uint08 *pBtaddr, uint08 *pBtName);
static void tlkmmi_btmgr_inquiryCompleteCB(void);


/******************************************************************************
 * Function: tlkmmi_btmgr_inqInit
 * Descript: Trigger the BT Inquiry initial. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_inqInit(void)
{
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_startInquiry
 * Descript: Trigger the BT start to Inquiry. 
 * Params:
 *        @inqType[IN]--The inquiry type.
 *        @rssiThd[IN]--The rssi of signal.
 *        @maxNumb[IN]--How many devices should bt update.
 *        @timeout[IN]--The inquiry timeout.
 *        @isGetName[IN]--whether get the device name or not.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_startInquiry(uint08 inqType, uint08 rssiThd, uint08 maxNumb, uint08 inqWind, uint08 inqNumb, bool isGetName)
{
	int ret;

	#if (TLKMMI_BTMGR_BTACL_ENABLE)
	if(tlkmmi_btmgr_aclIsBusy()) return -TLK_EBUSY;
	#endif
	
	ret = tlkmdi_btinq_start(inqType, rssiThd, maxNumb, inqWind, inqNumb, true, true);
	if(ret != TLK_ENONE) return ret;
	
	tlkmdi_btinq_regCallback(tlkmmi_btmgr_inquiryReportCB, tlkmmi_btmgr_inquiryCompleteCB);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_closeInquiry
 * Descript: Trigger the BT stop to Inquiry. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_closeInquiry(void)
{
//	tlkmdi_btinq_regCallback(nullptr, nullptr);
	if(!tlkmdi_btinq_isBusy()) return TLK_ENONE;
	tlkmdi_btinq_close();
	if(tlkmdi_btinq_isBusy()) return -TLK_EBUSY;
	return TLK_ENONE;
}

static int tlkmmi_btmgr_inquiryReportCB(uint32 devClass, uint08 rssi, uint08 nameLen, uint08 *pBtaddr, uint08 *pBtName)
{
	uint08 buffLen;
	uint08 buffer[72];
	
	buffLen = 0;
	buffer[buffLen++] = bth_devClassToDevType(devClass);
	buffer[buffLen++] = rssi;
	tmemcpy(buffer+buffLen, pBtaddr, 6);
	buffLen += 6;
	if(nameLen > 64) nameLen = 64;
	buffer[buffLen++] = nameLen;
	if(nameLen != 0 && pBtName != nullptr){
		tmemcpy(buffer+buffLen, pBtName, nameLen);
		buffLen += nameLen;
	}
	return tlkmmi_btmgr_sendCommEvt(TLKPRT_COMM_EVTID_BT_INQUIRY, buffer, buffLen);
}
static void tlkmmi_btmgr_inquiryCompleteCB(void)
{
	tlkmdi_btinq_regCallback(nullptr, nullptr);
	tlkmmi_btmgr_sendCommEvt(TLKPRT_COMM_EVTID_BT_INQUIRY_COMPLETE, nullptr, 0);
}



#endif //TLKMMI_BTMGR_BTINQ_ENABLE

