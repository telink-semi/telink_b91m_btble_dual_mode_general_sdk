/********************************************************************************************************
 * @file	leh_funcs.c
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
#if (TLK_STK_LE_ENABLE && TLK_CFG_TEST_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "leh_funcs.h"



#define LEH_FUNC_DBG_FLAG       ((TLK_MAJOR_DBGID_LEH << 24) | (TLK_MINOR_DBGID_LEH_FUNC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define LEH_FUNC_DBG_SIGN       nullptr


static uint16 sLehFuncAclHandle;
static uint08 sLehFuncPeerAddr[6];
static const leh_func_item_t scLehFunSet[] = {
	//common
	{LEH_FUNCID_COMM_START_ADV, leh_func_commStartAdv},
	{LEH_FUNCID_COMM_CLOSE_ADV, leh_func_commCloseAdv},
	{LEH_FUNCID_COMM_CLS_PEER_INFO, leh_func_commClsPeerInfo},

};



int leh_func_call(uint16 funcID, uint08 *pData, uint16 dataLen)
{
	int index;
	int count;
	
	count = sizeof(scLehFunSet)/sizeof(scLehFunSet[0]);
	for(index= 0; index<count; index++){
		if(scLehFunSet[index].funID == funcID) break;
	}
	if(index == count || scLehFunSet[index].Func == nullptr){
		tlkapi_error(LEH_FUNC_DBG_FLAG, LEH_FUNC_DBG_SIGN, "Function ID is not in LEH !");
		return -TLK_EFAIL;
	}
	return scLehFunSet[index].Func(pData,dataLen);
}
void leh_func_setAclHandle(uint16 aclHandle)
{
	sLehFuncAclHandle = aclHandle;
}
void leh_func_setPeerAddr(uint08 peerAddr[6])
{
	tmemcpy(sLehFuncPeerAddr, peerAddr, 6);
}


static int leh_func_commStartAdv(uint08 *pData, uint16 dataLen)
{
	blc_ll_setAdvEnable(BLC_ADV_ENABLE);
	return TLK_ENONE;
}
static int leh_func_commCloseAdv(uint08 *pData, uint16 dataLen)
{
	blc_ll_setAdvEnable(BLC_ADV_DISABLE);
	return TLK_ENONE;
}
static int leh_func_commClsPeerInfo(uint08 *pData, uint16 dataLen)
{
	blc_smp_eraseAllBondingInfo();
	return TLK_ENONE;
}



#endif //#if (TLK_STK_LE_ENABLE && TLK_CFG_TEST_ENABLE)

