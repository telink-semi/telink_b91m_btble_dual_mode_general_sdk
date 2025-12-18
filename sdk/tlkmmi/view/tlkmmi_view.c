/********************************************************************************************************
 * @file	tlkmmi_view.c
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
#if (TLKMMI_VIEW_ENABLE)
#include "tlkmmi/view/tlkmmi_view.h"
#include "tlksys/tlksys_stdio.h"
#if (TLK_MDI_GUI_ENABLE)
#include "tlkmdi/gui/tlkmdi_gui.h"
#endif



TLKSYS_MMI_TASK_DEFINE(view, View);


static int tlkmmi_view_init(uint08 procID, uint16 taskID)
{
	uint08 buffLen;
	uint08 buffer[8];

	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_VIEW;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00) >> 8;
	tlksys_sendInnerMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_REG_COMM_TASK, buffer, buffLen);
	
	tlkmmi_view_adaptInit(procID);
	#if (TLK_MDI_GUI_ENABLE)
	tlkmdi_gui_init();
	#endif
	
	return TLK_ENONE;
}
static int tlkmmi_view_start(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_view_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_view_close(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_view_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	
	return -TLK_ENOSUPPORT;
}
static void tlkmmi_view_handler(void)
{
	tlkmdi_gui_handler();
}



#endif //#if (TLKMMI_VIEW_ENABLE)

