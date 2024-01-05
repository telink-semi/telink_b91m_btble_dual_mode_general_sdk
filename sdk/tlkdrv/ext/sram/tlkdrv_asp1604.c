/********************************************************************************************************
 * @file	tlkdrv_asp1604.c
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
#if (TLKDRV_SRAM_ASP1604_ENABLE)
#include "tlkdrv_asp1604.h"
#include "drivers.h"


#define TLKDRV_ASP1604_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_ASP1604_DBG_SIGN     "[DRV]"


static bool tlkdrv_asp1604_isOpen(void);
static int tlkdrv_asp1604_init(void);
static int tlkdrv_asp1604_open(void);
static int tlkdrv_asp1604_close(void);
static int tlkdrv_asp1604_read(uint32 addr, uint08 *pBuff, uint16 buffLen);
static int tlkdrv_asp1604_write(uint32 addr, uint08 *pData, uint16 dataLen);
static int tlkdrv_asp1604_handler(uint16 opcode, uint32 param0, uint32 param1);
const tlkdrv_sram_modinf_t gcTlkDrvAsp1604Infs = {
	tlkdrv_asp1604_isOpen, //IsOpen
	tlkdrv_asp1604_init,  //Init
	tlkdrv_asp1604_open,  //Open
	tlkdrv_asp1604_close, //Close
	tlkdrv_asp1604_read,  //Read
	tlkdrv_asp1604_write, //Write
	tlkdrv_asp1604_handler, //Handler
};
static tlkdrv_asp1604_t sTlkDrvAsp1604Ctrl = {0};




static bool tlkdrv_asp1604_isOpen(void)
{
	return sTlkDrvAsp1604Ctrl.isOpen;
}
static int tlkdrv_asp1604_init(void)
{
	sTlkDrvAsp1604Ctrl.isInit = true;
	sTlkDrvAsp1604Ctrl.isOpen = false;
	
	
	return TLK_ENONE;
}
static int tlkdrv_asp1604_open(void)
{
	if(!sTlkDrvAsp1604Ctrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvAsp1604Ctrl.isOpen) return -TLK_EREPEAT;

	sTlkDrvAsp1604Ctrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_asp1604_close(void)
{
	if(!sTlkDrvAsp1604Ctrl.isOpen) return TLK_ENONE;

	return TLK_ENONE;
}
static int tlkdrv_asp1604_read(uint32 addr, uint08 *pBuff, uint16 buffLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_asp1604_write(uint32 addr, uint08 *pData, uint16 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_asp1604_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}



#endif //#if (TLKDRV_SRAM_ASP1604_ENABLE)

