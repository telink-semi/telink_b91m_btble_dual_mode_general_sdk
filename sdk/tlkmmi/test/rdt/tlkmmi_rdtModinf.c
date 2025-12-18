/********************************************************************************************************
 * @file	tlkmmi_rdtModinf.c
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
#if (TLK_TEST_RDT_ENABLE)
#include "tlkmmi_rdt.h"
#include "tlkmmi_rdtStd.h"
#include "tlkmmi_rdtModinf.h"


#define TLKMMI_RDT_MODULE_EXT(caseID)       \
			extern const tlkmmi_rdtModinf_t gTlkMmiRdtT##caseID##Inf
#define TLKMMI_RDT_MODULE_CALL(caseID)       \
			&gTlkMmiRdtT##caseID##Inf
#if (TLKMMI_RDT_CASE_T001_ENABLE)
TLKMMI_RDT_MODULE_EXT(001);
#endif
#if (TLKMMI_RDT_CASE_T002_ENABLE)
TLKMMI_RDT_MODULE_EXT(002);
#endif
#if (TLKMMI_RDT_CASE_T003_ENABLE)
TLKMMI_RDT_MODULE_EXT(003);
#endif
#if (TLKMMI_RDT_CASE_T005_ENABLE)
TLKMMI_RDT_MODULE_EXT(005);
#endif
static const tlkmmi_rdtModinf_t *sTlkMmiRdtTestModule[] =
{
	nullptr,
	#if (TLKMMI_RDT_CASE_T001_ENABLE)
	TLKMMI_RDT_MODULE_CALL(001),
	#endif
	#if (TLKMMI_RDT_CASE_T002_ENABLE)
	TLKMMI_RDT_MODULE_CALL(002),
	#endif
	#if (TLKMMI_RDT_CASE_T003_ENABLE)
	TLKMMI_RDT_MODULE_CALL(003),
	#endif
	#if (TLKMMI_RDT_CASE_T005_ENABLE)
	TLKMMI_RDT_MODULE_CALL(005),
	#endif
};
static const tlkmmi_rdtModinf_t *tlkmmi_rdt_getModinf(uint caseID);



int tlkmmi_rdt_modStart(uint16 caseID, uint08 role)
{
	const tlkmmi_rdtModinf_t *pModInf = tlkmmi_rdt_getModinf(caseID);
	if(pModInf == nullptr || pModInf->Start == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return pModInf->Start(role);
}
int tlkmmi_rdt_modClose(uint16 caseID)
{
	const tlkmmi_rdtModinf_t *pModInf = tlkmmi_rdt_getModinf(caseID);
	if(pModInf == nullptr || pModInf->Close == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return pModInf->Close();
}
int tlkmmi_rdt_modInput(uint16 caseID, uint08 msgID, uint08 *pData, uint16 dataLen)
{
	const tlkmmi_rdtModinf_t *pModInf = tlkmmi_rdt_getModinf(caseID);
	if(pModInf == nullptr || pModInf->Input == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return pModInf->Input(msgID, pData, dataLen);
}


static const tlkmmi_rdtModinf_t *tlkmmi_rdt_getModinf(uint caseID)
{
	uint index;
	uint count = sizeof(sTlkMmiRdtTestModule)/sizeof(sTlkMmiRdtTestModule[0]);
	for(index=0; index<count; index++){
		if(sTlkMmiRdtTestModule[index] != nullptr && sTlkMmiRdtTestModule[index]->caseID == caseID){
			break;
		}
	}
	if(index == count) return nullptr;
	return sTlkMmiRdtTestModule[index];
}



#endif //#if (TLK_TEST_RDT_ENABLE)

