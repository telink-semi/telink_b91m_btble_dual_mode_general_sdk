/********************************************************************************************************
 * @file	tlkmmi_btmgrCtrl.c
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
#if (TLKMMI_BTMGR_ENABLE)
#include "tlkmdi/bt/tlkmdi_btacl.h"
#include "tlkmdi/bt/tlkmdi_btinq.h"
#include "tlkmdi/bt/tlkmdi_bthid.h"
#include "tlkmdi/bt/tlkmdi_btiap.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/hid/btp_hid.h"
#include "tlkmmi/btmgr/tlkmmi_btmgr.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrCtrl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrAcl.h"
#include "tlkmmi/btmgr/tlkmmi_btmgrInq.h"

extern int tlkcfg_getFlashAddr(unsigned int offset);


#define TLKMMI_BTMGR_NAME_DEF     "Telink-BT"


tlkmmi_btmgr_ctrl_t gTlkMmiBtmgrCtrl;



/******************************************************************************
 * Function: tlkmmi_btmgr_ctrlInit
 * Descript: Initial the Bt manager ctrl block, and read local name and 
 *           Bt address. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_ctrlInit(void)
{
	uint16 index;
	uint08 bttemp[16];
	unsigned int nameAddr, addrAddr;

	nameAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_NAME_ADDR);
	addrAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_ADDR_ADDR);

	if(nameAddr == 0 || addrAddr == 0){
		return -TLK_EFAIL;
	}

	tmemset(&gTlkMmiBtmgrCtrl, 0, sizeof(tlkmmi_btmgr_ctrl_t));
	
	// Read Local Name
	tlkapi_flash_read(nameAddr, gTlkMmiBtmgrCtrl.btname, TLK_CFG_FLASH_BT_NAME_LENS-1);
	for(index=0; index<TLK_CFG_FLASH_BT_NAME_LENS-1; index++){
		if(gTlkMmiBtmgrCtrl.btname[index] == 0xFF || gTlkMmiBtmgrCtrl.btname[index] == 0x00) break;
	}
	if(index == 0){
		index = strlen(TLKMMI_BTMGR_NAME_DEF);
		tmemcpy(gTlkMmiBtmgrCtrl.btname, TLKMMI_BTMGR_NAME_DEF, index);
	}
	gTlkMmiBtmgrCtrl.btname[index] = 0x00;
	// Read Local Address
	
	
	tlkapi_flash_read(addrAddr, bttemp, 6+2+4+1);
	if(bttemp[0] == 0xFF && bttemp[1] == 0xFF){
	    uint08 randValue[6];
	    tlkapi_random(randValue, 6);
	    bttemp[0] = randValue[5];
	    bttemp[1] = randValue[4];
	    bttemp[2] = randValue[3];
	    bttemp[3] = randValue[2];
	    bttemp[4] = randValue[1];
	    bttemp[5] = randValue[0];
	    tlkapi_flash_write(addrAddr, bttemp, 6);
	}
	tmemcpy(gTlkMmiBtmgrCtrl.btaddr, bttemp, 6);

	bth_hci_sendSetBtAddrCmd(gTlkMmiBtmgrCtrl.btaddr);
	bth_hci_sendWriteLocalNameCmd(gTlkMmiBtmgrCtrl.btname);
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();

	#if (TLK_MDI_BTIAP_ENABLE)
	tlkmdi_btiap_setAddr(gTlkMmiBtmgrCtrl.btaddr);
	tlkmdi_btiap_setName(gTlkMmiBtmgrCtrl.btname, strlen((char*)gTlkMmiBtmgrCtrl.btname));
	#endif
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_getName
 * Descript: Get BT Name.
 * Params:
 * Return: Return Bt name is success.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_btmgr_getName(void)
{
	return gTlkMmiBtmgrCtrl.btname;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_getAddr
 * Descript: Get the Bt address. 
 * Params:
 * Return: Return Bt Address.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_btmgr_getAddr(void)
{
	return gTlkMmiBtmgrCtrl.btaddr;
}

int tlkmmi_btmgr_setName(uint08 *pName, uint08 nameLen)
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
	if(nameLen > TLK_CFG_FLASH_BT_NAME_LENS-1) nameLen = TLK_CFG_FLASH_BT_NAME_LENS-1;
	
	tlkapi_flash_read(btNameAddr, btBuffer, TLK_CFG_FLASH_BT_NAME_LENS);
	tlkapi_flash_read(leNameAddr, leBuffer, TLK_CFG_FLASH_LE_NAME_LENS);
	
	tmemset(btBuffer, 0xFF, TLK_CFG_FLASH_BT_NAME_LENS);
	tmemcpy(btBuffer, pName, nameLen);
	tlkapi_flash_eraseSector(btNameAddr & 0xFFFFF000);
	tlkapi_flash_write(btNameAddr, btBuffer, TLK_CFG_FLASH_BT_NAME_LENS);
	tlkapi_flash_write(leNameAddr, leBuffer, TLK_CFG_FLASH_LE_NAME_LENS);
	
	tmemcpy(gTlkMmiBtmgrCtrl.btname, pName, nameLen);
	gTlkMmiBtmgrCtrl.btname[nameLen] = 0x00;
	bth_hci_sendWriteLocalNameCmd(gTlkMmiBtmgrCtrl.btname);

	#if (TLK_MDI_BTIAP_ENABLE)
	tlkmdi_btiap_setName(gTlkMmiBtmgrCtrl.btname, strlen((char*)gTlkMmiBtmgrCtrl.btname));
	#endif
	
	return TLK_ENONE;
}
int tlkmmi_btmgr_setAddr(uint08 *pAddr)
{
	uint08 btBuffer[6];
	uint08 leBuffer[12];
	unsigned int btAddr, leAddr;

	btAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_ADDR_ADDR);
	leAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_ADDR_ADDR);
	if(btAddr == 0 || leAddr == 0){
		return -TLK_EFAIL;
	}
	
	
	if(pAddr == nullptr) return -TLK_EPARAM;

	tlkapi_flash_read(btAddr, btBuffer, 6);
	tlkapi_flash_read(leAddr, leBuffer, 12);

	tmemcpy(btBuffer, pAddr, 6);
	tlkapi_flash_eraseSector(btAddr & 0xFFFFF000);
	tlkapi_flash_write(btAddr, btBuffer, 6);
	tlkapi_flash_write(leAddr, leBuffer, 12);
	
	tmemcpy(gTlkMmiBtmgrCtrl.btaddr, pAddr, 6);
	bth_hci_sendSetBtAddrCmd(gTlkMmiBtmgrCtrl.btaddr);

	#if (TLK_MDI_BTIAP_ENABLE)
	tlkmdi_btiap_setAddr(gTlkMmiBtmgrCtrl.btaddr);
	#endif
	
	return TLK_ENONE;
}


int tlkmmi_btmgr_ctrlVolInc(void)
{
	#if (TLK_MDI_BTHID_ENABLE)
	int ret;
	uint16 aclHandle;
	uint16 consumeKey;
	aclHandle = btp_hidd_getAnyConnHandle();
	if(aclHandle == 0) return -TLK_ENOREADY;
	consumeKey = 0x00e9;
	ret = tlkmdi_bthid_sendData(aclHandle, TLKMDI_BTHID_REPORT_ID_CONSUMER_INPUT, BTP_HID_DATA_RTYPE_INPUT, (uint08*)&consumeKey, 2);
	consumeKey = 0x0000;
	if(ret == TLK_ENONE) ret = tlkmdi_bthid_sendData(aclHandle, TLKMDI_BTHID_REPORT_ID_CONSUMER_INPUT, BTP_HID_DATA_RTYPE_INPUT, (uint08*)&consumeKey, 2);
	return ret;
	#else
	return -TLK_ENOSUPPORT;
	#endif
}
int tlkmmi_btmgr_ctrlVolDec(void)
{
	#if (TLK_MDI_BTHID_ENABLE)
	int ret;
	uint16 aclHandle;
	uint16 consumeKey;
	aclHandle = btp_hidd_getAnyConnHandle();
	if(aclHandle == 0) return -TLK_ENOREADY;
	consumeKey = 0x00ea;
	ret = tlkmdi_bthid_sendData(aclHandle, TLKMDI_BTHID_REPORT_ID_CONSUMER_INPUT, BTP_HID_DATA_RTYPE_INPUT, (uint08*)&consumeKey, 2);
	consumeKey = 0x0000;
	if(ret == TLK_ENONE) ret = tlkmdi_bthid_sendData(aclHandle, TLKMDI_BTHID_REPORT_ID_CONSUMER_INPUT, BTP_HID_DATA_RTYPE_INPUT, (uint08*)&consumeKey, 2);
	return ret;
	#else
	return -TLK_ENOSUPPORT;
	#endif
}



#endif //#if (TLKMMI_BTMGR_ENABLE)

