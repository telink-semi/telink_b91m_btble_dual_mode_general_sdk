/********************************************************************************************************
 * @file	tlkmmi_testModinf.c
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
#if (TLKMMI_TEST_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_testStdio.h"
#include "tlkmmi_test.h"
#include "tlkmmi_testModinf.h"

static const tlkmmi_testModinf_t *tlkmmi_test_getModinf(TLKMMI_TEST_MODTYPE_ENUM type);

#if (TLK_TEST_FAT_ENABLE)
extern const tlkmmi_testModinf_t gTlkMmiFatModinf;
#endif
#if (TLK_TEST_RDT_ENABLE)
extern const tlkmmi_testModinf_t gTlkMmiRatModinf;
#endif
#if (TLK_TEST_PTS_ENABLE)
extern const tlkmmi_testModinf_t gTlkMmiPtsModinf;
#endif
#if (TLK_TEST_EMI_ENABLE)
extern const tlkmmi_testModinf_t gTlkMmiEmiModinf;
#endif
#if (TLK_TEST_USR_ENABLE)
extern const tlkmmi_testModinf_t gTlkMmiUsrModinf;
#endif
#if (TLK_TEST_BQB_ENABLE)
extern const tlkmmi_testModinf_t gTlkMmiBqbModinf;
#endif
#if (TLK_TEST_ATS_ENABLE)
extern const tlkmmi_testModinf_t gTlkMmiAtsModinf;
#endif
static const tlkmmi_testModinf_t *sTlkMmiTestModule[TLKMMI_TEST_MODTYPE_MAX] = 
{
	nullptr,
#if (TLK_TEST_PTS_ENABLE)
	&gTlkMmiPtsModinf,
#else
	nullptr,
#endif
#if (TLK_TEST_FAT_ENABLE)
	&gTlkMmiFatModinf,
#else
	nullptr,
#endif
#if (TLK_TEST_RDT_ENABLE)
	&gTlkMmiRatModinf,
#else
	nullptr,
#endif
#if (TLK_TEST_EMI_ENABLE)
	&gTlkMmiEmiModinf,
#else
	nullptr,
#endif
#if (TLK_TEST_USR_ENABLE)
	&gTlkMmiUsrModinf,
#else
	nullptr,
#endif
#if (TLK_TEST_BQB_ENABLE)
	&gTlkMmiBqbModinf,
#else
	nullptr,
#endif
#if (TLK_TEST_ATS_ENABLE)
	&gTlkMmiAtsModinf,
#else
	nullptr,
#endif
};


int tlkmmi_test_modInit(TLKMMI_TEST_MODTYPE_ENUM type, uint08 procID, uint16 taskID)
{
	const tlkmmi_testModinf_t *pModinf = tlkmmi_test_getModinf(type);
	if(pModinf == nullptr || pModinf->Init == nullptr) return -TLK_ENOSUPPORT;
	return pModinf->Init(procID, taskID);
}
int tlkmmi_test_modStart(TLKMMI_TEST_MODTYPE_ENUM type)
{
	const tlkmmi_testModinf_t *pModinf = tlkmmi_test_getModinf(type);
	if(pModinf == nullptr || pModinf->Start == nullptr) return -TLK_ENOSUPPORT;
	return pModinf->Start();
}
int tlkmmi_test_modPause(TLKMMI_TEST_MODTYPE_ENUM type)
{
	const tlkmmi_testModinf_t *pModinf = tlkmmi_test_getModinf(type);
	if(pModinf == nullptr || pModinf->Pause == nullptr) return -TLK_ENOSUPPORT;
	return pModinf->Pause();
}
int tlkmmi_test_modClose(TLKMMI_TEST_MODTYPE_ENUM type)
{
	const tlkmmi_testModinf_t *pModinf = tlkmmi_test_getModinf(type);
	if(pModinf == nullptr || pModinf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModinf->Close();
}
int tlkmmi_test_modInput(TLKMMI_TEST_MODTYPE_ENUM type, uint32 msgID, uint08 *pData, uint16 dataLen)
{
	const tlkmmi_testModinf_t *pModinf = tlkmmi_test_getModinf(type);
	if(pModinf == nullptr || pModinf->Input == nullptr) return -TLK_ENOSUPPORT;
	return pModinf->Input(msgID, pData, dataLen);
}
int tlkmmi_test_modHandler(TLKMMI_TEST_MODTYPE_ENUM type)
{
	const tlkmmi_testModinf_t *pModinf = tlkmmi_test_getModinf(type);
	if(pModinf == nullptr || pModinf->Handler == nullptr) return -TLK_ENOSUPPORT;
	pModinf->Handler();
	return TLK_ENONE;
}

static const tlkmmi_testModinf_t *tlkmmi_test_getModinf(TLKMMI_TEST_MODTYPE_ENUM type)
{
	if(type >= TLKMMI_TEST_MODTYPE_MAX) return nullptr;
	return sTlkMmiTestModule[type];
}


#endif //#if (TLKMMI_TEST_ENABLE)

