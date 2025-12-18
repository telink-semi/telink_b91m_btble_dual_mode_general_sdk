/********************************************************************************************************
 * @file	tlkmmi_stack.c
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
#if (TLKMMI_STACK_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_stack.h"
#include "tlkmmi_stackAdapt.h"
#include "tlkstk/tlkstk_stdio.h"

extern void btble_pm_initPowerManagement_module(void);

TLKSYS_MMI_TASK_DEFINE(stack, Stack);


static int tlkmmi_stack_init(uint08 procID, uint16 taskID)
{
	tlkmmi_stack_adaptInit(procID);
	tlkstk_init();
	
	#if (TLK_CFG_PM_ENABLE)
	btble_pm_initPowerManagement_module();
	#endif
	
	return TLK_ENONE;
}
static int tlkmmi_stack_start(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_stack_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_stack_close(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_stack_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	
	return -TLK_ENOSUPPORT;
}
static void tlkmmi_stack_handler(void)
{
	tlkstk_handler();
}




#endif //#if (TLKMMI_STACK_ENABLE)

