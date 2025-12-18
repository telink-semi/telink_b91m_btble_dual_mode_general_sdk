/********************************************************************************************************
 * @file	tlksys_proc.h
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
#ifndef TLKSYS_PROC_H
#define TLKSYS_PROC_H

#if (TLK_CFG_SYS_ENABLE)


#define TLKSYS_DEBUG_ENABLE    0


typedef struct{
	uint08 resved;
	uint08 procID;
	uint16 taskID;
	#if (TLKSYS_DEBUG_ENABLE)
	const char *pTaskName;
	#endif
	tlksys_funcs_t *pFuncs;
}tlksys_task_t;

typedef struct{
	uint08 procID;
	uint08 priority;
	uint16 msgqNumb;
	tlkapi_adapt_t adapt;
	#if (TLKSYS_DEBUG_ENABLE)
	const char *pProcName;
	#endif
	#if (TLK_CFG_OS_ENABLE)
	uint16 msgqSize;
	uint16 stackSize;
	uint08 *pRecvBuff;
	tlkos_msgq_t msgq;
	tlkos_task_t osProc;
	#endif
}tlksys_proc_t;



/******************************************************************************
 * Function: tlksys_task_init
 * Descript: Initializes the system process list.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_proc_init(void);

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
	const char *pProcName, uint16 msgqNumb, uint16 msgqSize);

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
int tlksys_proc_unmount(uint08 procID);

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Start a system process.
 * Params:
 *     @procID[IN]--ID of the task to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_proc_start(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_pause
 * Descript: Pause a system process.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_proc_pause(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_close
 * Descript: Close the system process.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_proc_close(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_handler
 * Descript: Loop through the requirements in the process.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: None.
 *****************************************************************************/
int tlksys_proc_handler(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_getItemCount
 * Descript: Gets the number of processes in the process list.
 * Params: None.
 * Return: The number of processes in the process list.
 *****************************************************************************/
int tlksys_proc_getItemCount(void);

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
int tlksys_proc_deleteItem(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_getIdleItem
 * Descript: Get a idle process from the process list.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: The structure of process control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_proc_t *tlksys_proc_getIdleItem(void);

/******************************************************************************
 * Function: tlksys_proc_getUsedItem
 * Descript: Get a used process from the process list.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: The structure of process control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_proc_t *tlksys_proc_getUsedItem(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_getItemByIndex
 * Descript: The process is obtained by the offset position in the process list.
 * Params:
 *     @index[IN]--The offset position in the process list.
 * Return: The structure of process control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_proc_t *tlksys_proc_getItemByIndex(uint16 index);

/******************************************************************************
 * Function: tlksys_proc_getAdapter
 * Descript: Gets the adapter that is closely associated with the process. The
 *           adapter directly controls timing and queuing tasks.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: The adapter that is closely associated with the process.
 *****************************************************************************/
tlkapi_adapt_t *tlksys_proc_getAdapter(uint08 procID);

/******************************************************************************
 * Function: tlksys_proc_getUsedItemByTaskID
 * Descript: Based on the task number, find the process number corresponding 
 *           to the task.
 * Params:
 *     @taskID[IN]--ID of the task to be operated.
 * Return: The structure of process control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_proc_t *tlksys_proc_getUsedItemByTaskID(uint16 taskID);


#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_PROC_H

