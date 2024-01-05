/********************************************************************************************************
 * @file	tlksys_proc.c
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
#if (TLK_CFG_SYS_ENABLE)
#if (TLK_CFG_OS_ENABLE)
#include "tlkapi/tlkapi_os.h"
#endif
#include "tlksys_stdio.h"
#include "tlksys_adapt.h"
#include "tlksys_mutex.h"
#include "tlksys_proc.h"
#include "tlksys_task.h"
#include "tlksys.h"

#if (TLKSYS_DEBUG_ENABLE)
uint08 tTlkSysCurProcID = 0;
uint08 tTlkSysCurTaskID = 0;
uint08 tTlkSysProcNameLens = 0;
uint08 tTlkSysTaskNameLens = 0;
uint32 tTlkSysProcNameAddr = 0;
uint32 tTlkSysTaskNameAddr = 0;
#endif
#if !(TLK_CFG_OS_ENABLE)
static uint16 sTlkSysProcCount = 0;
#endif
static tlksys_proc_t sTlkSysProcList[TLKSYS_PROC_MAX_NUMB];

#if (TLK_CFG_OS_ENABLE)
extern void tlktsk_process(void *pUsrArg);
#endif

/******************************************************************************
 * Function: tlksys_task_init
 * Descript: Initializes the system process list.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
 *****************************************************************************/
int tlksys_proc_init(void)
{
	#if !(TLK_CFG_OS_ENABLE)
	sTlkSysProcCount = 0;
	#endif
	tmemset(sTlkSysProcList, 0, TLKSYS_PROC_MAX_NUMB*sizeof(tlksys_proc_t));
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_mount
 * Descript: Mount the process to the system.
 * Params:
 *     @procID[IN]--Number of the process to be mounted.
 *     @priority[IN]--The execution priority of the process.
 *     @stackSize[IN]--The size of the stack occupied by process execution.
 *     @pProcName[IN]--The name of the virtual process to be mount.
 * Return: The positive value is the proc ID, otherwise the mount fails.
 * Notice:
 *     1.If procID==0, it is allocated by the system;
 *     2.If procID!=0, and the proc ID is occupied, a failure is returned.
 *     3.After each process is mounted, it is assigned a message queue and 
 *       adapter that is used for calling scheduled and queue tasks.
 *****************************************************************************/
int tlksys_proc_mount(uint08 procID, uint08 priority, uint16 stackSize,
	const char *pProcName, uint16 msgqNumb, uint16 msgqSize)
{
	#if (TLK_CFG_OS_ENABLE)
	int ret;
	#endif
	tlksys_proc_t *pProc;

	if(procID == 0) return -TLK_EPARAM;
	#if (TLK_CFG_OS_ENABLE)
	if(msgqNumb == 0 && msgqSize != 0) return -TLK_EPARAM;
	#endif

	pProc = tlksys_proc_getUsedItem(procID);
	if(pProc != nullptr) return -TLK_EREPEAT;

	pProc = tlksys_proc_getIdleItem();
	if(pProc == nullptr) return -TLK_EQUOTA;
	
	pProc->procID    = procID;
	pProc->priority  = priority;
	#if (TLK_CFG_OS_ENABLE)
	pProc->msgqNumb  = msgqNumb;
	pProc->msgqSize  = msgqSize;
	pProc->pRecvBuff = nullptr;
	pProc->stackSize = stackSize;
	#endif
	#if (TLKSYS_DEBUG_ENABLE)
	pProc->pProcName = pProcName;
	#endif
	tlksys_adapt_init(procID);

	#if (TLK_CFG_OS_ENABLE)
	if(msgqNumb != 0 && msgqSize != 0){
		pProc->pRecvBuff = tlkos_malloc(msgqSize);
		if(pProc->pRecvBuff == nullptr){
			tlkapi_trace(TLKSYS_DBG_FLAG, TLKSYS_DBG_SIGN, "tlksys_proc_mount: failure - tlkos_malloc");
			tlksys_proc_deleteItem(procID);
			return -TLK_EFAIL;
		}
	}
	if(msgqNumb != 0){
		msgqSize = (msgqSize + 3) & 0xFFFC;
		ret = tlkos_msgq_create(&pProc->msgq, pProc->msgqNumb, pProc->msgqSize);
		if(ret != TLK_ENONE){
			tlkapi_trace(TLKSYS_DBG_FLAG, TLKSYS_DBG_SIGN, "tlksys_proc_mount: failure - tlkos_msgq_create");
			tlksys_proc_deleteItem(procID);
			return -TLK_EFAIL;
		}
	}
	ret = tlkos_task_create(&pProc->osProc, "SystemTask", priority, stackSize, tlktsk_process, pProc);
	if(ret != TLK_ENONE){
		tlkapi_trace(TLKSYS_DBG_FLAG, TLKSYS_DBG_SIGN, "tlksys_proc_mount: failure - tlkos_task_create");
		if(msgqNumb != 0) tlkos_msgq_destroy(&pProc->msgq);
		tlksys_proc_deleteItem(procID);
		return -TLK_EFAIL;
	}
	#endif
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_unmount
 * Descript: Unmount the process from the proc list.
 * Params:
 *     @procID[IN]--ID of the process to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.If you unmount a process with an operating system (OS), the tasks  
 *       mounted under it will not be executed.
 *     2.If there is no operating system (OS), queue tasks and scheduled tasks 
 *       in a deleted process will not be executed.
 *****************************************************************************/
int tlksys_proc_unmount(uint08 procID)
{
	uint16 offset;
	tlksys_proc_t *pProc;
	tlksys_task_t *pTask;

	pProc = tlksys_proc_getUsedItem(procID);
	if(pProc == nullptr) return -TLK_EQUOTA;

	offset = 0;
	while(true){
		pTask = tlksys_task_getItemByProcID(procID, offset, &offset);
		if(pTask == nullptr) break;
		tlksys_task_unmount(pTask->taskID);
	}
		
	#if (TLK_CFG_OS_ENABLE)
	tlkos_task_destroy(&pProc->osProc);
	#endif
	tlksys_proc_deleteItem(procID);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Start a system process.
 * Params:
 *     @procID[IN]--ID of the task to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_proc_start(uint08 procID)
{
	uint16 offset;
	tlksys_task_t *pTask;
	
	offset = 0;
	pTask = tlksys_task_getItemByProcID(procID, offset, &offset);
	while(pTask != nullptr){
		if(pTask->pFuncs->Start != nullptr) pTask->pFuncs->Start();
		pTask = tlksys_task_getItemByProcID(procID, offset+1, &offset);
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_pause
 * Descript: Pause a system process.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_proc_pause(uint08 procID)
{
	uint16 offset;
	tlksys_task_t *pTask;
	
	offset = 0;
	pTask = tlksys_task_getItemByProcID(procID, offset, &offset);
	while(pTask != nullptr){
		if(pTask->pFuncs->Pause != nullptr) pTask->pFuncs->Pause();
		pTask = tlksys_task_getItemByProcID(procID, offset+1, &offset);
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_close
 * Descript: Close the system process.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_proc_close(uint08 procID)
{
	uint16 offset;
	tlksys_task_t *pTask;
	
	offset = 0;
	pTask = tlksys_task_getItemByProcID(procID, offset, &offset);
	while(pTask != nullptr){
		if(pTask->pFuncs->Close != nullptr) pTask->pFuncs->Close();
		pTask = tlksys_task_getItemByProcID(procID, offset+1, &offset);
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_handler
 * Descript: Loop through the requirements in the process.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: None.
 *****************************************************************************/
int tlksys_proc_handler(uint08 procID)
{
	uint16 offset;
	tlksys_task_t *pTask;
	#if (TLKSYS_DEBUG_ENABLE)
	tlksys_proc_t *pProc;
	#endif

	#if (TLKSYS_DEBUG_ENABLE)
	pProc = tlksys_proc_getUsedItem(procID);
	if(pProc != nullptr){
		tTlkSysCurProcID = procID;
		if(pProc->pProcName == nullptr) tTlkSysProcNameLens = 0;
		else tTlkSysProcNameLens = tstrlen(pProc->pProcName);
		tTlkSysProcNameAddr = (uint32)pProc->pProcName;
	}
	#endif
	
	tlksys_adapt_handler(procID);

	offset = 0;
	pTask = tlksys_task_getItemByProcID(procID, offset, &offset);
	while(pTask != nullptr){
		if(pTask->pFuncs->Handler != nullptr) pTask->pFuncs->Handler();
		pTask = tlksys_task_getItemByProcID(procID, offset+1, &offset);
	}
	
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlksys_proc_getItemCount
 * Descript: Gets the number of processes in the process list.
 * Params: None.
 * Return: The number of processes in the process list.
 *****************************************************************************/
int tlksys_proc_getItemCount(void)
{
#if (TLK_CFG_OS_ENABLE)
	int index;
	int count;
	for(index=0; index<TLKSYS_PROC_MAX_NUMB; index++){
		if(sTlkSysProcList[index].procID != 0) count ++;
	}
	return count;
#else
	return sTlkSysProcCount;
#endif
}

/******************************************************************************
 * Function: tlksys_proc_deleteItem
 * Descript: Delete the specified process.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.Invoke the interface with caution. The process can be deleted by 
 *       calling "tlksys_proc_unmount()".
 *     2.If you delete a process with an operating system (OS), the tasks  
 *       mounted under it will not be executed.
 *     3.If there is no operating system (OS), queue tasks and scheduled tasks 
 *       in a deleted process will not be executed.
 *****************************************************************************/
int tlksys_proc_deleteItem(uint08 procID)
{
	int index;
	if(procID == 0) return -TLK_EPARAM;

#if (TLK_CFG_OS_ENABLE)
	for(index=0; index<TLKSYS_PROC_MAX_NUMB; index++){
		if(sTlkSysProcList[index].procID == procID) break;
	}
	if(index == TLKSYS_PROC_MAX_NUMB) return -TLK_ENOOBJECT;
	sTlkSysProcList[index].procID = 0;
	if(sTlkSysProcList[index].pRecvBuff != nullptr){
		tlkos_free(sTlkSysProcList[index].pRecvBuff);
		sTlkSysProcList[index].pRecvBuff = nullptr;
	}
	tlkos_msgq_destroy(&sTlkSysProcList[index].msgq);
#else
	for(index=0; index<sTlkSysProcCount; index++){
		if(sTlkSysProcList[index].procID == procID) break;
	}
	if(index == sTlkSysProcCount) return -TLK_ENOOBJECT;
	sTlkSysProcCount --;
	if(index != sTlkSysProcCount){
		tmemcpy(&sTlkSysProcList[index], &sTlkSysProcList[index+1], (sTlkSysProcCount-index)*sizeof(tlksys_proc_t));
	}
#endif
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_proc_getIdleItem
 * Descript: Get a idle process from the process list.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: The structure of process control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_proc_t *tlksys_proc_getIdleItem(void)
{
#if (TLK_CFG_OS_ENABLE)
	uint08 index;
	for(index=0; index<TLKSYS_PROC_MAX_NUMB; index++){
		if(sTlkSysProcList[index].procID == 0) break;
	}
	if(index == TLKSYS_PROC_MAX_NUMB) return nullptr;
	return &sTlkSysProcList[index];
#else
	if(sTlkSysProcCount >= TLKSYS_PROC_MAX_NUMB) return nullptr;
	return &sTlkSysProcList[sTlkSysProcCount++];
#endif
}

/******************************************************************************
 * Function: tlksys_proc_getUsedItem
 * Descript: Get a used process from the process list.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: The structure of process control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_proc_t *tlksys_proc_getUsedItem(uint08 procID)
{
	int index;
	if(procID == 0) return nullptr;
#if (TLK_CFG_OS_ENABLE)
	for(index=0; index<TLKSYS_PROC_MAX_NUMB; index++){
		if(sTlkSysProcList[index].procID == procID) break;
	}
	if(index == TLKSYS_PROC_MAX_NUMB) return nullptr;
#else
	for(index=0; index<sTlkSysProcCount; index++){
		if(sTlkSysProcList[index].procID == procID) break;
	}
	if(index == sTlkSysProcCount) return nullptr;
#endif
	return &sTlkSysProcList[index];
}

/******************************************************************************
 * Function: tlksys_proc_getUsedItemByTaskID
 * Descript: Based on the task number, find the process number corresponding 
 *           to the task.
 * Params:
 *     @taskID[IN]--ID of the task to be operated.
 * Return: The structure of process control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_proc_t *tlksys_proc_getUsedItemByTaskID(uint16 taskID)
{
	tlksys_task_t *pTask;
	pTask = tlksys_task_getUsedItem(taskID);
	if(pTask == nullptr) return nullptr;
	return tlksys_proc_getUsedItem(pTask->procID);
}

/******************************************************************************
 * Function: tlksys_proc_getItemByIndex
 * Descript: The process is obtained by the offset position in the process list.
 * Params:
 *     @index[IN]--The offset position in the process list.
 * Return: The structure of process control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_proc_t *tlksys_proc_getItemByIndex(uint16 index)
{
#if (TLK_CFG_OS_ENABLE)
	if(index >= TLKSYS_PROC_MAX_NUMB || sTlkSysProcList[index].procID == 0) return nullptr;
	return &sTlkSysProcList[index];
#else
	if(index >= sTlkSysProcCount) return nullptr;
	return &sTlkSysProcList[index];
#endif
}

/******************************************************************************
 * Function: tlksys_proc_getAdapter
 * Descript: Gets the adapter that is closely associated with the process. The
 *           adapter directly controls timing and queuing tasks.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: The adapter that is closely associated with the process.
 *****************************************************************************/
tlkapi_adapt_t *tlksys_proc_getAdapter(uint08 procID)
{
	tlksys_proc_t *pProc = tlksys_proc_getUsedItem(procID);
	if(pProc != nullptr) return &pProc->adapt;
	else return nullptr;
}




#endif //#if (TLK_CFG_SYS_ENABLE)

