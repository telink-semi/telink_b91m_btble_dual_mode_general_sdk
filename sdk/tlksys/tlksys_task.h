/********************************************************************************************************
 * @file	tlksys_task.h
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
#ifndef TLKSYS_TASK_H
#define TLKSYS_TASK_H

#if (TLK_CFG_SYS_ENABLE)


/******************************************************************************
 * Function: tlksys_task_init
 * Descript: Initializes the system task list.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_task_init(void);

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
	const char *pTaskName);

/******************************************************************************
 * Function: tlksys_task_unmount
 * Descript: Unmount the task from the task list.
 * Params:
 *     @taskID[IN]--ID of the task to be uninstalled.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_task_unmount(uint16 taskID);

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Start a system task.
 * Params:
 *     @taskID[IN]--Id of the task to be started.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_task_start(tlksys_task_t *pTask);

/******************************************************************************
 * Function: tlksys_task_start
 * Descript: Pause a system task.
 * Params:
 *     @taskID[IN]--ID of the task to be paused.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_task_pause(tlksys_task_t *pTask);

/******************************************************************************
 * Function: tlksys_task_close
 * Descript: Close the system task.
 * Params:
 *     @taskID[IN]--ID of the task to be closed.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlksys_task_close(tlksys_task_t *pTask);

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
	uint08 *pData, uint16 dataLen);

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
void tlksys_task_handler(uint16 taskID);

/******************************************************************************
 * Function: tlksys_task_getItemCount
 * Descript: Gets the number of existing tasks in the task list.
 * Params: None.
 * Return: The number of existing tasks in the task list.
 *****************************************************************************/
int tlksys_task_getItemCount(void);

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
int tlksys_task_deleteItem(uint16 taskID);

/******************************************************************************
 * Function: tlksys_task_getIdleItem
 * Descript: Get a idle task from the task list.
 * Params: None.
 * Return: The structure of task control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_task_t *tlksys_task_getIdleItem(void);

/******************************************************************************
 * Function: tlksys_task_getUsedItem
 * Descript: Get a used task from the task list.
 * Params:
 *     @taskID[IN]--ID of the task to be obtained.
 * Return: The structure of task control, and "nullptr" means failure.
 *****************************************************************************/
tlksys_task_t *tlksys_task_getUsedItem(uint16 taskID);

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
tlksys_task_t *tlksys_task_getItemByIndex(uint16 index);

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
 *     1.The offset of the output is relative to the entire task list.
 *****************************************************************************/
tlksys_task_t *tlksys_task_getItemByProcID(uint08 procID, uint16 offset, uint16 *pIndex);




#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_TASK_H

