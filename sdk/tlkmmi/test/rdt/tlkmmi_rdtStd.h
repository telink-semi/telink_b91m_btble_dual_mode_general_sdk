/********************************************************************************************************
 * @file	tlkmmi_rdtStd.h
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
#ifndef TLKMMI_RDT_STD_H
#define TLKMMI_RDT_STD_H

#if (TLK_TEST_RDT_ENABLE)



#include "tlkmmi_rdtCfg.h"
#include "tlkmmi_rdtDef.h"

#define TLKMMI_RDT_DBG_FLAG       ((TLK_MAJOR_DBGID_MMI_TEST << 24) | (TLK_MINOR_DBGID_MMI_TEST_RDT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_RDT_DBG_SIGN       nullptr

typedef struct{
	uint caseID;
	int(*Start)(uint08 role);
	int(*Close)(void);
	int(*Input)(uint08 msgID, uint08 *pData, uint16 dataLen);
}tlkmmi_rdtModinf_t;


#define TLKMMI_RDT_MODINF_DEFINE(caseID)       \
	static int tlkmmi_rdt_t##caseID##Start(uint08 role);\
	static int tlkmmi_rdt_t##caseID##Close(void);\
	static int tlkmmi_rdt_t##caseID##Input(uint08 msgID, uint08 *pData, uint16 dataLen);\
	const tlkmmi_rdtModinf_t gTlkMmiRdtT##caseID##Inf = {\
		caseID,\
		tlkmmi_rdt_t##caseID##Start,\
		tlkmmi_rdt_t##caseID##Close,\
		tlkmmi_rdt_t##caseID##Input\
	}\




#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_STD_H

