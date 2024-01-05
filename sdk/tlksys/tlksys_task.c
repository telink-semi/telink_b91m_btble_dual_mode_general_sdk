/********************************************************************************************************
 * @file	tlksys_task.c
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
#include "tlksys.h"
#include "tlksys_stdio.h"
#include "tlksys_adapt.h"
#include "tlksys_mutex.h"
#include "tlksys_proc.h"
#include "tlksys_task.h"


#if !(TLK_CFG_OS_ENABLE)
static uint16 sTlkSysTaskCount = 0;
#endif
static tlksys_task_t sTlkSysTaskList[TLKSYS_TASK_MAX_NUMB] = {0};


/******************************************************************************
 * Function: tlksys_task_init
 * Descript: Initializes the system task list.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: None.
 *****************************************************************************/
int tlksys_task_init(void)
{
	#if !(TLK_CFG_OS_ENABLE)
	sTlkSysTaskCount = 0;
	#endif
	tmemset(sTlkSysTaskList, 0, TLKSYS_TASK_MAX_NUMB*sizeof(tlksys_task_t));
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_task_mount
 * Descript: Mount the specified task to the task list and obtain the task ID.
 * Params:
 *     @procID[IN]--Number of the process to be mounted.
 *     @taskID[IN]--The desired task ID of the user.
 *     @pFuncs[IN]--Interface for mounting the task.
 *     @pTaskName[IN]--The name of the virtual task to be mount.
 * Return: The positive value is the task ID, otherwise the mount fails.
 * Notice:
 *     1.If taskID==0, it is allocated by the system;
 *     2.If taskID! =0, and the change ID is occupied, a failure is returned.
 *****************************************************************************/
int tlksys_task_mount(uint08 procID, uint16 taskID, const tlksys_funcs_t *pFuncs, 
	const char *pTaskName)
{
	tlksys_task_t *pTask;

	if(pFuncs == nullptr || taskID == 0) return -TLK_EPARAM;

	if(tlksys_proc_getUsedItem(procID) == nullptr){
		return -TLK_ENOREADY;
	}
	
	pTask = tlksys_task_getUsedItem(taskID);
	if(pTask != nullptr) return -TLK_EREPEAT;
	
	pTask = tlksys_task_getIdleItem();
	if(pTask == nullptr) return -TLK_EQUOTA;

	pTask->resved = 0;
	pTask->procID = procID;
	pTask->taskID = taskID;
	pTask->pFuncs = (tlksys_funcs_t*)pFuncs;
	#if (TLKSYS_DEBUG_ENABLE)
	pTask->pTaskName = pTaskName;
	#endif
	if(pFuncs->Init != nullptr && pFuncs->Init(procID, taskID) != TLK_ENONE){
		tlksys_task_deleteItem(taskID);
		return -TLK_EINIT;
	}
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_task_unmount
 * Descript: Unmount the task from the task list.
 * Params:
 *     @taskID[IN]--ID of the task to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: None.
 *****************************************************************************/
int tlksys_task_unmount(uint16 taskID)
{
	tlksys_task_t *pTask;
	
	pTask = tlksys_task_getUsedItem(taskID);
	if(pTask == nullptr) return -TLK_ENOOBJECT;

	tlksys_task_deleteItem(taskID);
	if(pTask->pFuncs->Close != nullptr){
		pTask->pFuncs->Close();
	}
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Start a system task.
 * Params:
 *     @taskID[IN]--Id of the task to be started.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: None.
 *****************************************************************************/
int tlksys_task_start(tlksys_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
	if(pTask->pFuncs->Start == nullptr) return -TLK_ENOSUPPORT;
	return pTask->pFuncs->Start();
}

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Pause a system task.
 * Params:
 *     @taskID[IN]--ID of the task to be paused.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: None.
 *****************************************************************************/
int tlksys_task_pause(tlksys_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
	if(pTask->pFuncs->Pause == nullptr) return -TLK_ENOSUPPORT;
	return pTask->pFuncs->Pause();
}

/******************************************************************************
 * Function: tlksys_task_close
 * Descript: Close the system task.
 * Params:
 *     @taskID[IN]--ID of the task to be closed.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: None.
 *****************************************************************************/
int tlksys_task_close(tlksys_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
	if(pTask->pFuncs->Close == nullptr) return -TLK_ENOSUPPORT;
	return pTask->pFuncs->Close();
}

/******************************************************************************
 * Function: tlksys_task_input
 * Descript: Send a message to a task.
 * Params:
 *     @taskID[IN]--ID of the task to be entered.
 *     @mtype[IN]--The type of the message. 0-Inner msg, others for outer message.
 *     @msgID[IN]--The ID of the message under the message type.
 *     @pData[IN]--Message body data.
 *     @dataLen[IN]--Message body length.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.For message interactions that are not in the same process, the "pHead"
 *       and HeadLen fields must be set to NULL.
 *****************************************************************************/
int tlksys_task_input(uint16 taskID, uint08 mtype, uint16 msgID, 
	uint08 *pData, uint16 dataLen) 
{
	tlksys_task_t *pTask = tlksys_task_getUsedItem(taskID);
	if(pTask == nullptr) return -TLK_EPARAM;
	if(pTask->pFuncs->Input == nullptr) return -TLK_ENOSUPPORT;
	return pTask->pFuncs->Input(mtype, msgID, pData, dataLen);
}

/******************************************************************************
 * Function: tlksys_task_handler
 * Descript: Loop through the requirements in the task.
 * Params:
 *     @taskID[IN]--ID of the task to be run.
 * Return: None.
 * Notice: 
 *     1.Do not use the modified interface unless necessary. The modified 
 *       interface will be discarded later.
 *     2.Users can replace the interface with scheduled and queued tasks in 
 *       the adapter.
 *****************************************************************************/
void tlksys_task_handler(uint16 taskID)
{
	tlksys_task_t *pTask = tlksys_task_getUsedItem(taskID);
	if(pTask == nullptr || pTask->pFuncs->Handler == nullptr) return;
	pTask->pFuncs->Handler();
}

/******************************************************************************
 * Function: tlksys_task_getItemCount
 * Descript: Gets the number of existing tasks in the task list.
 * Params: None.
 * Return: The number of existing tasks in the task list.
 *****************************************************************************/
int tlksys_task_getItemCount(void)
{
#if (TLK_CFG_OS_ENABLE)
	int index;
	int count;
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		if(sTlkSysTaskList[index].procID != 0) count ++;
	}
	return count;
#else
	return sTlkSysTaskCount;
#endif
}

/******************************************************************************
 * Function: tlksys_task_deleteItem
 * Descript: Deletes the specified task.
 * Params:
 *     @taskID[IN]--ID of the task to be deleted.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.Invoke the interface with caution. The task can be deleted by calling
 *       "tlksys_task_unmount()".
 *****************************************************************************/
int tlksys_task_deleteItem(uint16 taskID)
{
	int index;
	if(taskID == 0) return -TLK_EPARAM;

#if (TLK_CFG_OS_ENABLE)
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		if(sTlkSysTaskList[index].taskID == taskID) break;
	}
	if(index == TLKSYS_TASK_MAX_NUMB) return -TLK_ENOOBJECT;
	sTlkSysTaskList[index].procID = 0;
	sTlkSysTaskList[index].pFuncs = nullptr;
	sTlkSysTaskList[index].taskID = 0; //Must be here for avoid fault.
#else
	for(index=0; index<sTlkSysTaskCount; index++){
		if(sTlkSysTaskList[index].taskID == taskID) break;
	}
	if(index == sTlkSysTaskCount) return -TLK_ENOOBJECT;
	sTlkSysTaskCount --;
	if(index != sTlkSysTaskCount){
		tmemcpy(&sTlkSysTaskList[index], &sTlkSysTaskList[index+1], (sTlkSysTaskCount-index)*sizeof(tlksys_task_t));
	}
#endif
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_task_getIdleItem
 * Descript: Get a idle task from the task list.
 * Params: None.
 * Return: The structure of task control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_task_t *tlksys_task_getIdleItem(void)
{
#if (TLK_CFG_OS_ENABLE)
	uint08 index;
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		if(sTlkSysTaskList[index].procID == 0 && sTlkSysTaskList[index].taskID == 0) break;
	}
	if(index == TLKSYS_TASK_MAX_NUMB) return nullptr;
	return &sTlkSysTaskList[index];
#else
	if(sTlkSysTaskCount >= TLKSYS_TASK_MAX_NUMB) return nullptr;
	return &sTlkSysTaskList[sTlkSysTaskCount++];
#endif
}

/******************************************************************************
 * Function: tlksys_task_getUsedItem
 * Descript: Get a used task from the task list.
 * Params:
 *     @taskID[IN]--ID of the task to be obtained.
 * Return: The structure of task control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_task_t *tlksys_task_getUsedItem(uint16 taskID)
{
	int index;
	if(taskID == 0) return nullptr;
#if (TLK_CFG_OS_ENABLE)
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		if(sTlkSysTaskList[index].taskID == taskID) break;
	}
	if(index == TLKSYS_TASK_MAX_NUMB) return nullptr;
#else
	for(index=0; index<sTlkSysTaskCount; index++){
		if(sTlkSysTaskList[index].taskID == taskID) break;
	}
	if(index == sTlkSysTaskCount) return nullptr;
#endif
	return &sTlkSysTaskList[index];
}

/******************************************************************************
 * Function: tlksys_task_getItemByIndex
 * Descript: The task is obtained by the offset position in the task list.
 * Params:
 *     @index[IN]--Offset position in the task list.
 * Return: The structure of task control, and "nullptr" means failure.
 * Notice:
 *     1.Use this interface with caution. If the task at the specified offset
 *       does not exist, nullptr is returned.
 *****************************************************************************/
tlksys_task_t *tlksys_task_getItemByIndex(uint16 index)
{
#if (TLK_CFG_OS_ENABLE)
	if(index >= TLKSYS_TASK_MAX_NUMB || sTlkSysTaskList[index].taskID == 0) return nullptr;
	return &sTlkSysTaskList[index];
#else
	if(index >= sTlkSysTaskCount) return nullptr;
	return &sTlkSysTaskList[index];
#endif
}

/******************************************************************************
 * Function: tlksys_task_getItemByProcID
 * Descript: Gets the task of the offset position in the specified process 
 *           relative to the process.
 * Params:
 *     @taskID[IN]--ID of the task to be obtained.
 *     @offset[IN]--Offset position in the thread list for Search.
 *     @pIndex[OUT]--The offset position of the searched task in the task list.
 * Return: The structure of task control, and "nullptr" means failure.
 * Notice:
 *     1.The offset of the input is relative to the order in which the process
 *       was inserted.
 *     2.The offset of the output is relative to the entire task list.
 *****************************************************************************/
tlksys_task_t *tlksys_task_getItemByProcID(uint08 procID, uint16 offset, uint16 *pIndex)
{
	int index;
#if (TLK_CFG_OS_ENABLE)
	if(offset >= TLKSYS_TASK_MAX_NUMB) return nullptr;
	for(index=offset; index<TLKSYS_TASK_MAX_NUMB; index++){
		if(sTlkSysTaskList[index].procID == procID) break;
	}
	if(index == TLKSYS_TASK_MAX_NUMB) return nullptr;
	if(pIndex != nullptr) *pIndex = index;
#else
	if(offset >= sTlkSysTaskCount) return nullptr;
	for(index=offset; index<sTlkSysTaskCount; index++){
		if(sTlkSysTaskList[index].procID == procID) break;
	}
	if(index == sTlkSysTaskCount) return nullptr;
	if(pIndex != nullptr) *pIndex = index;
#endif
	return &sTlkSysTaskList[index];
}


#endif //#if (TLK_CFG_SYS_ENABLE)

