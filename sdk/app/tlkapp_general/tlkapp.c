/********************************************************************************************************
 * @file	tlkapp.c
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
#include "tlkstk/tlkstk_stdio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlksys/tlksys_stdio.h"

#include "tlkapp_config.h"
#include "tlkapp_irq.h"
#include "tlkapp.h"
#include "tlkapp_pm.h"
#include "drivers.h"



extern void flash_plic_preempt_config(unsigned char preempt_en,unsigned char threshold);
extern void trng_init(void);
#if (TLKMMI_TEST_ENABLE)
extern uint tlkcfg_getWorkMode(void);
#endif
extern void tlkbt_setWorkMode(u8 workMode);
extern void tlkbt_setCtlSecConFeat(uint8_t support);
extern int tlkdev_init(void);
extern int tlksys_init(void);
extern int tlkcfg_load(void);

static int tlkapp_sysProcInit(void);
static int tlkapp_sysTaskInit(void);


static uint32 sTlkAppTimer; 


/******************************************************************************
 * Function: tlkapp_init
 * Descript: user initialization when MCU power on or wake_up from deepSleep mode.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/  
int tlkapp_init(void)
{
	int ret;
	
  	g_plic_preempt_en = 1; //Interrupt nesting is mandatory, tlksys_enter_critical/tlksys_leave_critical
	flash_plic_preempt_config(1, 1);
	trng_init();
	tlkbt_setWorkMode(1);
	tlkbt_setCtlSecConFeat(1);

	tlkapp_irq_init();

	tlk_dbgio_init();
	tlkcfg_load();
	#if (TLK_CFG_SYS_ENABLE)
	ret = tlksys_init();
	if(ret != TLK_ENONE) return ret;
	#endif
	ret = tlkdev_init();
	if(ret != TLK_ENONE) return ret;
				
	#if(TLK_CFG_PM_ENABLE)
	tlkapp_pm_init();
	#endif
	#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_appendBusyCheckCB(tlkapp_pmIsBusy, "tlkapp");
	#endif
	
	sTlkAppTimer = clock_time()|1;

	ret = tlkapp_sysProcInit();
	if(ret != TLK_ENONE) return ret;
	ret = tlkapp_sysTaskInit();
	if(ret != TLK_ENONE) return ret;
	#if (TLK_CFG_SYS_ENABLE)
	ret = tlksys_start(0xFF);
	if(ret != TLK_ENONE) return ret;
	#endif
	return TLK_ENONE;
}



/******************************************************************************
 * Function: tlkapp_deepInit
 * Descript: user initialization when MCU wake_up from deepSleep_retention mode.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
void tlkapp_deepInit(void)
{
	
}

/******************************************************************************
 * Function: tlkapp_pmIsBusy
 * Descript: user check is step into deepSleep_retention Mode or not.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
bool tlkapp_pmIsBusy(void)
{
	if(sTlkAppTimer != 0){
		if(!clock_time_exceed(sTlkAppTimer, 5000000)) return true;
		sTlkAppTimer = 0;
	}

	return false;
}


/******************************************************************************
 * Function: tlkapp_handler
 * Descript: BTBLE SDK main loop.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_handler(void)
{
	#if (TLK_CFG_SYS_ENABLE)
	tlksys_handler();
	#endif
	/*PM entry*/
	#if(TLK_CFG_PM_ENABLE)
	tlkapp_pm_handler();
	#endif
}

#if (TLKMMI_SYSTEM_ENABLE)
extern const tlksys_funcs_t gTlkMmiSystemTask;
#endif
#if (TLKMMI_DEVICE_ENABLE)
extern const tlksys_funcs_t gTlkMmiDeviceTask;
#endif
#if (TLKMMI_DEBUG_ENABLE)
extern const tlksys_funcs_t gTlkMmiDebugTask;
#endif
#if (TLKMMI_STACK_ENABLE)
extern const tlksys_funcs_t gTlkMmiStackTask;
#endif
#if (TLKMMI_AUDIO_ENABLE)
extern const tlksys_funcs_t gTlkMmiAudioTask;
#endif
#if (TLKMMI_BTMGR_ENABLE)
extern const tlksys_funcs_t gTlkMmiBtmgrTask;
#endif
#if (TLKMMI_LEMGR_ENABLE)
extern const tlksys_funcs_t gTlkMmiLemgrTask;
#endif
#if (TLKMMI_LEMST_ENABLE)
extern const tlksys_funcs_t gTlkMmiLemstTask;
#endif
#if (TLKMMI_PHONE_ENABLE)
extern const tlksys_funcs_t gTlkMmiPhoneTask;
#endif
#if (TLKMMI_FILE_ENABLE)
extern const tlksys_funcs_t gTlkMmiFileTask;
#endif
#if (TLKMMI_TEST_ENABLE)
extern const tlksys_funcs_t gTlkMmiTestTask;
#endif
#if (TLKMMI_VIEW_ENABLE)
extern const tlksys_funcs_t gTlkMmiViewTask;
#endif
#if (TLKMMI_SENSOR_ENABLE)
extern const tlksys_funcs_t gTlkMmiSensorTask;
#endif
static int tlkapp_sysProcInit(void)
{
	int ret = TLK_ENONE;

	tlksys_setIdleProc(TLKSYS_PROCID_SYSTEM);
	if(ret == TLK_ENONE) ret = tlksys_proc_mount(TLKSYS_PROCID_SYSTEM, 1, 12*1024, "system", 8, 128); //System processes must come first
	#if (TLKMMI_AUDIO_ENABLE)
//	if(ret == TLK_ENONE) ret = tlksys_proc_mount(TLKSYS_PROCID_AUDIO,  3, 5*1024, "audio", 8, 64);
	#endif
	return ret;
}

static int tlkapp_sysTaskInit(void)
{
	int ret = TLK_ENONE;

	#if (TLKMMI_SYSTEM_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM, TLKSYS_TASKID_SYSTEM, &gTlkMmiSystemTask, "system"); //This should be first in stack process.
	#endif
	#if (TLKMMI_DEVICE_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_DEVICE,  &gTlkMmiDeviceTask, "device");
	#endif
	#if (TLKMMI_DEBUG_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_DEBUG,  &gTlkMmiDebugTask, "debug");
	#endif
	#if (TLKMMI_STACK_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_STACK,  &gTlkMmiStackTask, "stack");
	#endif
	#if (TLKMMI_TEST_ENABLE)
	if(tlkcfg_getWorkMode() != TLK_WORK_MODE_NORMAL){
		if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM, TLKSYS_TASKID_TEST, &gTlkMmiTestTask, "test");
		if(tlkcfg_getWorkMode() != TLK_WORK_MODE_TEST_ATS){
			return ret;
		} 
	}
	#endif
	#if (TLKMMI_FILE_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_FILE,   &gTlkMmiFileTask, "file");
	#endif
	#if (TLKMMI_AUDIO_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_AUDIO,  &gTlkMmiAudioTask, "audio");
	#endif
	#if (TLKMMI_BTMGR_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_BTMGR,  &gTlkMmiBtmgrTask, "btmgr");
	#endif
	#if (TLKMMI_LEMGR_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_LEMGR,  &gTlkMmiLemgrTask, "lemgr");
	#endif
	#if (TLKMMI_LEMST_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_LEMST,  &gTlkMmiLemstTask, "lemst");
	#endif
	#if (TLKMMI_PHONE_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,  TLKSYS_TASKID_PHONE,  &gTlkMmiPhoneTask, "phone");
	#endif
	#if (TLKMMI_VIEW_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM,   TLKSYS_TASKID_VIEW,  &gTlkMmiViewTask, "view");
	#endif
	#if (TLKMMI_SENSOR_ENABLE)
	if(ret == TLK_ENONE) ret = tlksys_task_mount(TLKSYS_PROCID_SYSTEM, TLKSYS_TASKID_SENSOR, &gTlkMmiSensorTask, "sensor");
	#endif
	return ret;
}


