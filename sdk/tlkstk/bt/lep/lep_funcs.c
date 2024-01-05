/********************************************************************************************************
 * @file	lep_funcs.c
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
#include "lep_funcs.h"



#define LEP_FUNC_DBG_FLAG       ((TLK_MAJOR_DBGID_LEP << 24) | (TLK_MINOR_DBGID_LEP_FUNC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define LEP_FUNC_DBG_SIGN       nullptr


static uint16 sLehFuncAclHandle;
static const lep_func_item_t scLehFunSet[] = {
	//common
	{LEP_FUNCID_GAP_UPDATE_CONN_PARAM, lep_func_commUpdateConnParam},

};



int lep_func_call(uint16 funcID, uint08 *pData, uint16 dataLen)
{
	int index;
	int count;
	
	count = sizeof(scLehFunSet)/sizeof(scLehFunSet[0]);
	for(index= 0; index<count; index++){
		if(scLehFunSet[index].funID == funcID) break;
	}
	if(index == count || scLehFunSet[index].Func == nullptr){
		tlkapi_error(LEP_FUNC_DBG_FLAG, LEP_FUNC_DBG_SIGN, "Function ID is not in LEP !");
		return -TLK_EFAIL;
	}
	return scLehFunSet[index].Func(pData,dataLen);
}
void lep_func_setAclHandle(uint16 aclHandle)
{
	sLehFuncAclHandle = aclHandle;
}



static int lep_func_commUpdateConnParam(uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}




#endif //#if (TLK_STK_LE_ENABLE && TLK_CFG_TEST_ENABLE)

