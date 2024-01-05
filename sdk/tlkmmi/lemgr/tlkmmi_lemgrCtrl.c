/********************************************************************************************************
 * @file	tlkmmi_lemgrCtrl.c
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
#if (TLKMMI_LEMGR_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"
#include "tlkmdi/le/tlkmdi_le_device_manage.h"
#include "tlkmdi/le/tlkmdi_le_simple_sdp.h"
#include "tlkmdi/le/tlkmdi_le_custom_pair.h"
#include "tlkmmi/lemgr/tlkmmi_lemgr.h"
//#include "tlkmmi/lemgr/tlkmmi_lemgrComm.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrCtrl.h"
#include "tlkmdi/le/tlkmdi_lemgrAtt.h"
#include "tlkmdi/le/tlkmdi_leAcl.h"
#include "tlkmdi/le/tlkmdi_le_command.h"
#include "tlkmmi/lemgr/tlkmmi_lemgrAcl.h"

#define TLKMMI_LEMGR_NAME_DEF     "TLK-BLE"


static tlkmmi_lemgr_ctrl_t gTlkMmiLemgrCtrl = {0};


int tlkmmi_lemgr_ctrlInit(void)
{
	uint08 index;
	unsigned int leNameAddr;
	leNameAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_NAME_ADDR);
	if(leNameAddr == 0){
		return -TLK_EFAIL;
	}
		
	// Read Local Name
	tlkapi_flash_read(leNameAddr, gTlkMmiLemgrCtrl.lename, TLK_CFG_FLASH_LE_NAME_LENS-1);
	for(index=0; index<TLK_CFG_FLASH_LE_NAME_LENS-1; index++){
		if(gTlkMmiLemgrCtrl.lename[index] == 0xFF || gTlkMmiLemgrCtrl.lename[index] == 0x00) break;
	}
	if(index == 0){
		index = strlen(TLKMMI_LEMGR_NAME_DEF);
		tmemcpy(gTlkMmiLemgrCtrl.lename, TLKMMI_LEMGR_NAME_DEF, index);
	}
	gTlkMmiLemgrCtrl.nameLen = index;
	gTlkMmiLemgrCtrl.lename[index] = 0x00;

	tlkmdi_lemgr_attSetDevName(gTlkMmiLemgrCtrl.lename, index);
	
	return TLK_ENONE;
}



bool tlkmmi_lemgr_volumeInc(void)
{
	return tlkmdi_le_aclVolumeInc(tlkmmi_lemgr_aclGetSlaveHandle());
}
bool tlkmmi_lemgr_volumeDec(void)
{
	return tlkmdi_le_aclVolumeDec(tlkmmi_lemgr_aclGetSlaveHandle());
}

uint08 tlkmmi_lemgr_getNameLen(void)
{
	return gTlkMmiLemgrCtrl.nameLen;
}

/******************************************************************************
 * Function: tlkmmi_lemgr_getName
 * Descript: Get BT Name.
 * Params:
 * Return: Return Bt name is success.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_lemgr_getName(void)
{
	return gTlkMmiLemgrCtrl.lename;
}

/******************************************************************************
 * Function: tlkmmi_lemgr_getAddr
 * Descript: Get the Bt address. 
 * Params:
 * Return: Return Bt Address.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_lemgr_getAddr(void)
{
	return gTlkMmiLemgrCtrl.leaddr;
}


int tlkmmi_lemgr_setName(uint08 *pName, uint08 nameLen)
{
	uint08 btBuffer[TLK_CFG_FLASH_BT_NAME_LENS];
	uint08 leBuffer[TLK_CFG_FLASH_LE_NAME_LENS];
	unsigned int btNameAddr, leNameAddr;

	btNameAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_NAME_ADDR);
	leNameAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_NAME_ADDR);
	if(btNameAddr == 0 || leNameAddr == 0){
		return -TLK_EFAIL;
	}
	
	if(pName == nullptr || nameLen == 0) return -TLK_EPARAM;
	if(nameLen > TLK_CFG_FLASH_LE_NAME_LENS-1) nameLen = TLK_CFG_FLASH_LE_NAME_LENS-1;
	
	tlkapi_flash_read(btNameAddr, btBuffer, TLK_CFG_FLASH_BT_NAME_LENS);
	tlkapi_flash_read(leNameAddr, leBuffer, TLK_CFG_FLASH_LE_NAME_LENS);
	
	tmemset(leBuffer, 0xFF, TLK_CFG_FLASH_LE_NAME_LENS);
	tmemcpy(leBuffer, pName, nameLen);
	tlkapi_flash_eraseSector(leNameAddr & 0xFFFFF000);
	tlkapi_flash_write(btNameAddr, btBuffer, TLK_CFG_FLASH_BT_NAME_LENS);
	tlkapi_flash_write(leNameAddr, leBuffer, TLK_CFG_FLASH_LE_NAME_LENS);
	
	tmemcpy(gTlkMmiLemgrCtrl.lename, pName, nameLen);
	gTlkMmiLemgrCtrl.nameLen = nameLen;
	gTlkMmiLemgrCtrl.lename[nameLen] = 0x00;
	
	tlkmdi_lemgr_attSetDevName(gTlkMmiLemgrCtrl.lename, nameLen);
	
	return tlkmdi_le_setAclName(pName, nameLen);
}
int tlkmmi_lemgr_setAddr(uint08 *pAddr)
{
	uint08 btBuffer[6];
	uint08 leBuffer[8];
	unsigned int btAddr, leAddr;
	btAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_ADDR_ADDR);
	leAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_ADDR_ADDR);
	if(btAddr == 0 || leAddr == 0){
		return -TLK_EFAIL;
	}
	
	if(pAddr == nullptr) return -TLK_EPARAM;
	
	tlkapi_flash_read(btAddr, btBuffer, 6);
	tlkapi_flash_read(leAddr, leBuffer, 8);
	
	tmemcpy(leBuffer, pAddr, 6);
	tlkapi_flash_eraseSector(leAddr & 0xFFFFF000);
	tlkapi_flash_write(btAddr, btBuffer, 6);
	tlkapi_flash_write(leAddr, leBuffer, 8);
	
	tmemcpy(gTlkMmiLemgrCtrl.leaddr, pAddr, 6);
	return tlkmdi_le_setAclName(gTlkMmiLemgrCtrl.leaddr, 6);
}

int tlkmmi_lemgr_setAddr1(uint08 *pPubAddr, uint08 *pRndAddr)
{
	tmemcpy(gTlkMmiLemgrCtrl.leaddr,   pPubAddr, 6);
	tmemcpy(gTlkMmiLemgrCtrl.leaddr+6, pRndAddr, 6);
	tlkmdi_le_setAddr1(pPubAddr,pRndAddr);
	return TLK_ENONE;
}



#endif //TLKMMI_LEMGR_ENABLE

