/********************************************************************************************************
 * @file	tlksys_stdio.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKSYS_STDIO_H
#define TLKSYS_STDIO_H

#if (TLK_CFG_SYS_ENABLE)


#define TLKSYS_PROC_MAX_NUMB        8
#define TLKSYS_TASK_MAX_NUMB        16


typedef enum{
	TLKSYS_PROCID_NONE = 0,
	TLKSYS_PROCID_SYSTEM, //System processes must come first
	TLKSYS_PROCID_DEVICE,
	TLKSYS_PROCID_AUDIO,
	TLKSYS_PROCID_FILEM,
	TLKSYS_PROCID_STACK,
	TLKSYS_PROCID_TEST,
	TLKSYS_PROCID_VIEW,
}TLKSYS_PROCID_ENUM;

typedef enum{
	TLKSYS_TASKID_NONE = 0,
	TLKSYS_TASKID_SYSTEM, //System task must come first
	TLKSYS_TASKID_SENSOR,
	TLKSYS_TASKID_DEVICE,
	TLKSYS_TASKID_DEBUG,
	TLKSYS_TASKID_STACK,
	TLKSYS_TASKID_AUDIO,
	TLKSYS_TASKID_BTMGR,
	TLKSYS_TASKID_LEMGR,
	TLKSYS_TASKID_LEMST,
	TLKSYS_TASKID_PHONE,
	TLKSYS_TASKID_FILE,
	TLKSYS_TASKID_TEST,
	TLKSYS_TASKID_VIEW,
}TLKSYS_TASKID_ENUM;


typedef struct{
	int(*Init)(uint08 procID, uint16 taskID);
	int(*Start)(void);
	int(*Pause)(void);
	int(*Close)(void);
	int(*Input)(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen); //mtype=0:Inner message; Other for outer message
	void(*Handler)(void);
}tlksys_funcs_t;


#include "tlksys/prt/tlkpti_stdio.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "tlksys/tlksys_adapt.h"
#include "tlksys/tlksys_pm.h"


#define TLKSYS_MMI_TASK_DEFINE(name0, name1)       \
	static int tlkmmi_##name0##_init(uint08 procID, uint16 taskID);\
	static int tlkmmi_##name0##_start(void);\
	static int tlkmmi_##name0##_pause(void);\
	static int tlkmmi_##name0##_close(void);\
	static int tlkmmi_##name0##_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen);\
	static void tlkmmi_##name0##_handler(void);\
	const tlksys_funcs_t gTlkMmi##name1##Task = {\
		tlkmmi_##name0##_init,\
		tlkmmi_##name0##_start,\
		tlkmmi_##name0##_pause,\
		tlkmmi_##name0##_close,\
		tlkmmi_##name0##_input,\
		tlkmmi_##name0##_handler\
	}


extern int  tlksys_init(void);
extern int  tlksys_start(uint08 procID);
extern void tlksys_pause(uint08 procID);
extern void tlksys_close(uint08 procID);
extern void tlksys_handler(void);
extern void tlksys_setIdleProc(uint08 procID);

extern int tlksys_sendInnerMsgFromIrq(uint16 taskID, uint16 msgID, uint08 *pData, uint16 dataLen);
extern int tlksys_sendInnerMsg(uint16 taskID, uint16 msgID, uint08 *pData, uint16 dataLen);
extern int tlksys_sendOuterMsg(uint16 taskID, uint08 mType, uint16 msgID, uint08 *pData, uint16 dataLen);
extern int tlksys_sendInnerExtMsg(uint16 taskID, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);
extern int tlksys_sendOuterExtMsg(uint16 taskID, uint08 mType, uint16 msgID, uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * Function: tlksys_proc_mount
 * Descript: Mount the process to the system.
 * Params:
 *     @procID--Number of the process to be mounted.
 *     @priority--The execution priority of the process.
 *     @stackSize--The size of the stack occupied by process execution.
 *     @pProcName--The name of the virtual process to be mount.
 * Return: The positive value is the proc ID, otherwise the mount fails.
 * Others:
 *     1.If procID==0, it is allocated by the system;
 *     2.If procID!=0, and the proc ID is occupied, a failure is returned.
*******************************************************************************/
extern int tlksys_proc_mount(uint08 procID, uint08 priority, uint16 stackSize,
	const char *pProcName, uint16 msgqNumb, uint16 msgqSize);

/******************************************************************************
 * Function: tlksys_proc_unmount
 * Descript: Unmount the process from the proc list.
 * Params:
 *     @procID--ID of the process to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
extern int tlksys_proc_unmount(uint08 procID);

/******************************************************************************
 * Function: tlksys_task_mount
 * Descript: Mount the specified task to the task list and obtain the task ID.
 * Params:
 *     @procID--Number of the process to be mounted.
 *     @taskID--The desired task ID of the user.
 *     @pFuncs--Interface for mounting the task.
 *     @pTaskName--The name of the virtual task to be mount.
 * Return: The positive value is the task ID, otherwise the mount fails.
 * Others:
 *     1.If taskID==0, it is allocated by the system;
 *     2.If taskID! =0, and the change ID is occupied, a failure is returned.
*******************************************************************************/
extern int tlksys_task_mount(uint08 procID, uint16 taskID, const tlksys_funcs_t *pFuncs,
	const char *pTaskName);

/******************************************************************************
 * Function: tlksys_task_unmount
 * Descript: Unmount the task from the task list.
 * Params:
 *     @taskID--ID of the task to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
extern int tlksys_task_unmount(uint16 taskID);



#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_STDIO_H

