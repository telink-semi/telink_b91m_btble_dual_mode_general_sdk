/********************************************************************************************************
 * @file	tlkmmi_sysCtrl.c
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
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlksys/prt/tlkpto_stdio.h"
#include "tlkmmi_sys.h"
#include "tlkmmi_sysCtrl.h"
#include "tlkmmi_sysMsgInner.h"
#include "drivers.h"

extern unsigned int tlkcfg_getFlashAddr(unsigned int offset);

static tlkmmi_sys_ctrl_t sTlkMmiSysCtrl;



/******************************************************************************
 * Function: tlkmmi_sys_ctrlInit
 * Descript: This function is used to initialize system parameters and configuration.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
int tlkmmi_sys_ctrlInit(void)
{
	tmemset(&sTlkMmiSysCtrl, 0, sizeof(tlkmmi_sys_ctrl_t));
		
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_sys_commHandler
 * Descript: This function to control the machine power off or restart 
 *           and register the heart beat.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
void tlkmmi_sys_ctrlHandler(void)
{
	if(sTlkMmiSysCtrl.powerTimer != 0 && clock_time_exceed(sTlkMmiSysCtrl.powerTimer, sTlkMmiSysCtrl.powerIntv)){
		if(sTlkMmiSysCtrl.isBoot) core_reboot();
		else tlkmmi_sys_poweroff();
		sTlkMmiSysCtrl.powerTimer = 0;
	}
	if(sTlkMmiSysCtrl.heartTimer != 0 && clock_time_exceed(sTlkMmiSysCtrl.heartTimer, sTlkMmiSysCtrl.heartIntv)){
		sTlkMmiSysCtrl.heartTimer = clock_time()|1;
		tlkmmi_sys_sendCommEvt(TLKPRT_COMM_EVTID_SYS_HEART_BEAT, (uint08*)&sTlkMmiSysCtrl.heartNumb, 2);
		sTlkMmiSysCtrl.heartNumb ++;
	}	
}

void tlkmmi_sys_setPowerParam(uint32 offTimeUs, bool isBoot)
{
	if(sTlkMmiSysCtrl.powerTimer == 0 || sTlkMmiSysCtrl.isBoot != isBoot){
		sTlkMmiSysCtrl.powerTimer = clock_time()|1;
		sTlkMmiSysCtrl.powerIntv = offTimeUs;
		sTlkMmiSysCtrl.isBoot = isBoot;
	}
}
void tlkmmi_sys_setHeartParam(uint32 intervalUs)
{
	if(intervalUs != 0 && (sTlkMmiSysCtrl.heartTimer == 0 || sTlkMmiSysCtrl.heartIntv != intervalUs)){
		sTlkMmiSysCtrl.heartTimer = clock_time()|1;
		sTlkMmiSysCtrl.heartIntv = intervalUs;
	}else if(intervalUs == 0 && sTlkMmiSysCtrl.heartTimer != 0){
		sTlkMmiSysCtrl.heartTimer = 0;
	}
}

/******************************************************************************
 * Function: tlkmmi_sys_poweroff
 * Descript: This Function for power off the meachine.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
void tlkmmi_sys_poweroff(void)
{
	usb_dp_pullup_en(0);

	tlksys_set_mcuMode(DEEPSLEEP_MODE, 0);

//	gpio_input_dis(TLKAPP_WAKEUP_PIN);
//	gpio_output_dis(TLKAPP_WAKEUP_PIN);
//	gpio_set_up_down_res(TLKAPP_WAKEUP_PIN,GPIO_PIN_PULLDOWN_100K);
//	pm_set_gpio_wakeup(TLKAPP_WAKEUP_PIN, WAKEUP_LEVEL_HIGH, 0);
}


int tlkmmi_sys_restoreFactorySettings(void)
{
	uint08 i;
	unsigned int addr;
	
	//play list
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PLAY_LIST_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	for( i = 0; i < TLK_CFG_FLASH_PLAY_LIST_LENS/0x1000; i++ )
	{
		tlkapi_flash_eraseSector(addr + i * 0x1000);
	}

	//play info
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PLAY_INFO_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	tlkapi_flash_eraseSector(addr);

	

	//ota
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_OTA_PARAM_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	tlkapi_flash_eraseSector(addr);

	//sys
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_SYS_CONFIG_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	tlkapi_flash_eraseSector(addr);


	//pbap
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PBAP_LIST_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	for( i = 0; i < TLK_CFG_FLASH_PBAP_LIST_LENS/0x1000; i++ )
	{
		tlkapi_flash_eraseSector(addr + i * 0x1000);
	}

	//factory param
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_FACTORY_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	tlkapi_flash_eraseSector(addr);

	//volume
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_VOLUME_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	tlkapi_flash_eraseSector(addr);

	//pair addr
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_PAIR_ADDR0);
	if(addr == 0) return -TLK_EFAIL;
	tlkapi_flash_eraseSector(addr);
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_PAIR_ADDR1);
	if(addr == 0) return -TLK_EFAIL;
	tlkapi_flash_eraseSector(addr);

	//le smp
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_SMP_PAIRING_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	for( i = 0; i < (TLK_CFG_FLASH_LE_SMP_PAIRING_SIZE*2)/0x1000; i++ )
	{
		tlkapi_flash_eraseSector(addr + i * 0x1000);
	}

	//le pair: slave info
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_ADR_CUSTOM_PAIRING_ADDR);
	if(addr == 0) return -TLK_EFAIL;
	for( i = 0; i < TLK_CFG_FLASH_LE_ADR_CUSTOM_PAIRING_SIZE/0x1000; i++ )
	{
		tlkapi_flash_eraseSector(addr + i * 0x1000);
	}

	//peer device att handle
	addr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_LE_SDP_ATT_ADRR);
	if(addr == 0) return -TLK_EFAIL;
	for( i = 0; i < TLK_CFG_FLASH_LE_SDP_ATT_SIZE/0x1000; i++ )
	{
		tlkapi_flash_eraseSector(addr + i * 0x1000);
	}
	return TLK_ENONE;
}


#endif //#if (TLKMMI_SYSTEM_ENABLE)

