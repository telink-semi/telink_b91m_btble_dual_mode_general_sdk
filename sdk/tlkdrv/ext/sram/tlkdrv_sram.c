/********************************************************************************************************
 * @file	tlkdrv_sram.c
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
#include "tlkdrv_sram.h"


static const tlkdrv_sram_modinf_t *tlkdrv_sram_getDev(TLKDRV_SRAM_DEV_ENUM dev);

extern const tlkdrv_sram_modinf_t gcTlkDrvAsp1604Infs;
static const tlkdrv_sram_modinf_t *spTlkDrvSramModInfs[TLKDRV_SRAM_DEV_MAX] = {
	#if (TLKDRV_SRAM_ASP1604_ENABLE)
	&gcTlkDrvAsp1604Infs,
	#else
	nullptr,
	#endif
};



bool tlkdrv_sram_isOpen(TLKDRV_SRAM_DEV_ENUM dev)
{
	const tlkdrv_sram_modinf_t *pModInf;
	pModInf = tlkdrv_sram_getDev(dev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsOpen();
}

int tlkdrv_sram_init(TLKDRV_SRAM_DEV_ENUM dev)
{
	const tlkdrv_sram_modinf_t *pModInf;
	pModInf = tlkdrv_sram_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Init();
}
int tlkdrv_sram_open(TLKDRV_SRAM_DEV_ENUM dev)
{
	const tlkdrv_sram_modinf_t *pModInf;
	pModInf = tlkdrv_sram_getDev(dev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Open();
}
int tlkdrv_sram_close(TLKDRV_SRAM_DEV_ENUM dev)
{
	const tlkdrv_sram_modinf_t *pModInf;
	pModInf = tlkdrv_sram_getDev(dev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close();
}
int tlkdrv_sram_read(TLKDRV_SRAM_DEV_ENUM dev, uint32 addr, uint08 *pBuff, uint16 buffLen)
{
	const tlkdrv_sram_modinf_t *pModInf;
	pModInf = tlkdrv_sram_getDev(dev);
	if(pModInf == nullptr || pModInf->Read == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Read(addr, pBuff, buffLen);
}
int tlkdrv_sram_write(TLKDRV_SRAM_DEV_ENUM dev, uint32 addr, uint08 *pData, uint16 dataLen)
{
	const tlkdrv_sram_modinf_t *pModInf;
	pModInf = tlkdrv_sram_getDev(dev);
	if(pModInf == nullptr || pModInf->Write == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Write(addr, pData, dataLen);
}
int tlkdrv_sram_handler(TLKDRV_SRAM_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_sram_modinf_t *pModInf;
	pModInf = tlkdrv_sram_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(opcode, param0, param1);
}




static const tlkdrv_sram_modinf_t *tlkdrv_sram_getDev(TLKDRV_SRAM_DEV_ENUM dev)
{
	if(dev >= TLKDRV_SRAM_DEV_MAX) return nullptr;
	return spTlkDrvSramModInfs[dev];
}


