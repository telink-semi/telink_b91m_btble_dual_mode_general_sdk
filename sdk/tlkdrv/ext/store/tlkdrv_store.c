/********************************************************************************************************
 * @file	tlkdrv_store.c
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
#include "tlkdrv_store.h"

static const tlkdrv_store_modinf_t *tlkdrv_store_getDev(TLKDRV_STORE_DEV_ENUM dev);

extern const tlkdrv_store_modinf_t gcTlkDrvXtsd01gInf;
static const tlkdrv_store_modinf_t *spTlkDrvStoreModInf[TLKDRV_STORE_DEV_MAX] = {
	#if (TLKDRV_STORE_XTSD01G_ENABLE)
	&gcTlkDrvXtsd01gInf,
	#else
	nullptr,
	#endif
};



bool tlkdrv_store_isOpen(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsOpen();
}

int tlkdrv_store_init(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Init();
}
int tlkdrv_store_open(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Open();
}
int tlkdrv_store_close(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close();
}
int tlkdrv_store_erase(TLKDRV_STORE_DEV_ENUM dev, uint32 addr, uint32 size)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Erase == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Erase(addr, size);
}
int tlkdrv_store_read(TLKDRV_STORE_DEV_ENUM dev, uint32 addr, uint08 *pBuff, uint16 buffLen)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Read == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Read(addr, pBuff, buffLen);
}
int tlkdrv_store_write(TLKDRV_STORE_DEV_ENUM dev, uint32 addr, uint08 *pData, uint16 dataLen)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Write == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Write(addr, pData, dataLen);
}
int tlkdrv_store_sdInit(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->SDInit== nullptr) return -TLK_ENOSUPPORT;
	return pModInf->SDInit();
}
int tlkdrv_store_format(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler== nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(TLKDRV_STORE_OPCODE_SD_FORMAT, 0, 0);
}
int tlkdrv_store_sdread(TLKDRV_STORE_DEV_ENUM dev, uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->SDRead == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->SDRead(pBuff, blkOffs, blkNumb);
}
int tlkdrv_store_sdwrite(TLKDRV_STORE_DEV_ENUM dev, uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->SDWrite == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->SDWrite(pData, blkOffs, blkNumb);
}
int tlkdrv_store_handler(TLKDRV_STORE_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(opcode, param0, param1);
}
void tlkdrv_store_shutDown(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->ShutDown == nullptr) return;
	pModInf->ShutDown();
}

int tlkdrv_store_getPageSize(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler== nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(TLKDRV_STORE_OPCODE_GET_PAGE_SIZE, 0, 0);
}
int tlkdrv_store_getPageNumb(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler== nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(TLKDRV_STORE_OPCODE_GET_PAGE_NUMB, 0, 0);
}
int tlkdrv_store_getSectorSize(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler== nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(TLKDRV_STORE_OPCODE_GET_SECTOR_SIZE, 0, 0);
}
int tlkdrv_store_getSectorNumb(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler== nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(TLKDRV_STORE_OPCODE_GET_SECTOR_NUMB, 0, 0);
}
int tlkdrv_store_getSdBlockSize(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler== nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(TLKDRV_STORE_OPCODE_GET_BLOCK_SIZE, 0, 0);
}
int tlkdrv_store_getSdBlockNumb(TLKDRV_STORE_DEV_ENUM dev)
{
	const tlkdrv_store_modinf_t *pModInf;
	pModInf = tlkdrv_store_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler== nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(TLKDRV_STORE_OPCODE_GET_BLOCK_NUMB, 0, 0);
}





static const tlkdrv_store_modinf_t *tlkdrv_store_getDev(TLKDRV_STORE_DEV_ENUM dev)
{
	if(dev >= TLKDRV_STORE_DEV_MAX) return nullptr;
	return spTlkDrvStoreModInf[dev];
}

