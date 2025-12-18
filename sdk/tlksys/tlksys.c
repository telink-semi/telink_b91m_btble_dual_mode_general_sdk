/********************************************************************************************************
 * @file	tlksys.c
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
#include "tlksys_pm.h"
#include "drivers.h"


#if (TLK_CFG_OS_ENABLE)
void tlktsk_process(void *pUsrArg);
#endif

#if (TLK_CFG_OS_ENABLE)
static uint08 sTlkSysOsIsStart = false;
static uint08 sTlkSysIdleProcID = 0;
#endif
#if !(TLK_CFG_OS_ENABLE)
static uint08 sTlkSysIrqMsgRPtr = 0;
static uint08 sTlkSysIrqMsgWPtr = 0;
static uint08 sTlkSysIrqMsgBuff[TLKSYS_IRQMSG_ITEM_NUMB][TLKSYS_IRQMSG_ITEM_SIZE];
#endif


/******************************************************************************
 * Function: tlksys_init
 * Descript: Initialize the scheduling system.
 * Params: None.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.The scheduling parameters are initialized here, and the interface
 *       needs to be called before processes and threads can be mounted.
 *     2.The system has virtual processes and virtual threads, and the virtual
 *       threads are bound to the virtual threads.
 *     3.The virtual process acts as a thread for the operating system when it
 *       is present.
 *****************************************************************************/
int tlksys_init(void)
{
	tlksys_pm_init();
	#if (TLK_CFG_OS_ENABLE)
	tlkos_init();
	#endif
	tlksys_mutex_init();
	tlksys_proc_init();
	tlksys_task_init();
	#if (TLK_CFG_OS_ENABLE)
	sTlkSysOsIsStart = false;
	sTlkSysIdleProcID = 0;
	#endif
	#if !(TLK_CFG_OS_ENABLE)
	sTlkSysIrqMsgRPtr = 0;
	sTlkSysIrqMsgWPtr = 0;
	#endif

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_setIdleProc
 * Descript: Set the idle process in the system.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: None.
 *     1.The concept of idle processes only takes effect in the case of an 
 *       operating system.
 *     2.Idle processes will be executed when the system is idle. 
 *     3.Idle processes need to have the lowest priority.
 *     4.In idle processes, scheduled tasks, queue tasks, and message queues 
 *       do not block the running of internal tasks.
 *****************************************************************************/
void tlksys_setIdleProc(uint08 procID)
{
	#if (TLK_CFG_OS_ENABLE)
	sTlkSysIdleProcID = procID;
	#endif
}

/******************************************************************************
 * Function: tlksys_start
 * Descript: Start virtual threads and virtual processes in the system.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: Negative values mean failure, others mean success.
 *     1.If procID is 0xFF or 0x00, all mounted processes and threads in the 
 *       system are started. Otherwise, only the corresponding process and all
 *       threads under the process are started.
 *     2.The start here is not really a start, but a call to the "start()"
 *       interface of the application thread. The real start is a call to
 *       "tlksys_handler()".
 *****************************************************************************/
int tlksys_start(uint08 procID)
{
	int ret;
	int index;
	tlksys_task_t *pTask;

	if(procID != 0xFF && procID != 0x00){
		return tlksys_proc_start(procID);
	}
	
	ret = TLK_ENONE;
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		pTask = tlksys_task_getItemByIndex(index);
		if(pTask != nullptr && pTask->pFuncs->Start != nullptr){
			ret = pTask->pFuncs->Start();
			if(ret != TLK_ENONE) break;
		}
	}
	return ret;
}

/******************************************************************************
 * Function: tlksys_pause
 * Descript: Pause virtual threads and virtual processes in the system.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: None
 *     1.If procID is 0xFF or 0x00, all mounted processes and threads in the 
 *       system are paused. Otherwise, only the corresponding process and all
 *       threads under the process are paused.
 *     2.The interface does not take effect in the system.
 *****************************************************************************/
void tlksys_pause(uint08 procID)
{
	int index;
	tlksys_task_t *pTask;
	
	if(procID != 0xFF && procID != 0x00){
		tlksys_proc_pause(procID);
		return;
	}
	
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		pTask = tlksys_task_getItemByIndex(index);
		if(pTask != nullptr && pTask->pFuncs->Pause != nullptr){
			pTask->pFuncs->Pause();
		}
	}
}

/******************************************************************************
 * Function: tlksys_close
 * Descript: Close virtual threads and virtual processes in the system.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: None
 *     1.If procID is 0xFF or 0x00, all mounted processes and threads in the 
 *       system are closed. Otherwise, only the corresponding process and all
 *       threads under the process are closed.
 *     2.The interface does not take effect in the system.
 *****************************************************************************/
void tlksys_close(uint08 procID)
{
	int index;
	tlksys_task_t *pTask;
	
	if(procID != 0xFF && procID != 0x00){
		tlksys_proc_close(procID);
		return;
	}
	
	for(index=0; index<TLKSYS_TASK_MAX_NUMB; index++){
		pTask = tlksys_task_getItemByIndex(index);
		if(pTask != nullptr && pTask->pFuncs->Close != nullptr){
			pTask->pFuncs->Close();
		}
	}
}

/******************************************************************************
 * Function: tlksys_handler
 * Descript: Start the operating system or run processes and threads.
 * Params:
 *     @procID[IN]--ID of the process to be operated.
 * Return: None.
 * Notice:
 *     1.If an operating system is available, this interface is used to 
 *       officially start the operating system. In the absence of an operating
 *       system, the interface needs to be called cyclically to execute all 
 *       mounted processes and threads.
 *****************************************************************************/
void tlksys_handler(void)
{
#if (TLK_CFG_OS_ENABLE)
	sTlkSysOsIsStart = true;
	tlkos_start_run();
#else
	int index;
	int taskCount;
	int procCount;
	tlksys_task_t *pTask;
	tlksys_proc_t *pProc;

	if(sTlkSysIrqMsgRPtr != sTlkSysIrqMsgWPtr){
		uint16 taskID;
		uint08 mType;
		uint16 msgID;
		uint08 msgLen;
		uint08 *pData;
		while(sTlkSysIrqMsgRPtr != sTlkSysIrqMsgWPtr){
			pData = sTlkSysIrqMsgBuff[sTlkSysIrqMsgRPtr & TLKSYS_IRQMSG_ITEM_MASK];
			taskID = ((uint16)pData[1] << 8) | pData[0];
			mType  = pData[2];
			msgID  = ((uint16)pData[4] << 8) | pData[3];
			msgLen = pData[5];
			tlksys_task_input(taskID, mType, msgID, pData+6, msgLen);
			sTlkSysIrqMsgRPtr = (sTlkSysIrqMsgRPtr+1) & 0xFF;
		}
	}
	
	procCount = tlksys_proc_getItemCount();
	taskCount = tlksys_task_getItemCount();
	for(index=0; index<procCount; index++){
		pProc = tlksys_proc_getItemByIndex(index);
		if (pProc != nullptr) {
			tlksys_adapt_handler(pProc->procID);
		}
	}
	for(index=0; index<taskCount; index++){
		pTask = tlksys_task_getItemByIndex(index);
		if(pTask != nullptr && pTask->pFuncs->Handler != nullptr){
			pTask->pFuncs->Handler();
		}
	}
#endif
}


/******************************************************************************
 * Function: tlksys_sendInnerMsgFromIrq
 * Descript: Sends a inner message from the interrupt function to the specified
 *           task.
 * Params:
 *     @taskID[IN]--ID of the task to be sent.
 *     @msgID[IN]--The ID of the message. Refer to "tlksys/prt/xxx".
 *     @pData[IN]--Body information to be sent.
 *     @dataLen[IN]--The length of the body to be sent.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.The message is cached and executed in a process parallel to the thread.
 *     2.Internal messages are messages that are sent between tasks in the
 *       system or between tasks.
 *     3.In the case of non-operating systems, the system defines a cache queue
 *       with the size of "TLKSYS_IRQMSG_ITEM_SIZE" and the number of 
 *       "TLKSYS_IRQMSG_ITEM_NUMB" to cache the data. If the fifo is 
 *        insufficient, adjust the parameters
 *     4.For details, see "tlksys_sendMsg()".
 *****************************************************************************/
int tlksys_sendInnerMsgFromIrq(uint16 taskID, uint16 msgID, 
	uint08 *pData, uint16 dataLen)
{
	int ret;
#if !(TLK_CFG_OS_ENABLE)
	if(dataLen > 255 || dataLen+6 > TLKSYS_IRQMSG_ITEM_SIZE){
		ret = TLK_EOVERFLOW;
	}else if(((sTlkSysIrqMsgWPtr+1) & 0xFF) == sTlkSysIrqMsgRPtr){
		ret = TLK_EQUOTA;
	}else{
		uint08 buffLen = 0;
		uint08 *pBuffer = sTlkSysIrqMsgBuff[sTlkSysIrqMsgWPtr & TLKSYS_IRQMSG_ITEM_MASK];
		pBuffer[buffLen++] = taskID & 0xFF;
		pBuffer[buffLen++] = (taskID & 0xFF00)>>8;
		pBuffer[buffLen++] = TLKPRT_COMM_MTYPE_NONE;
		pBuffer[buffLen++] = (msgID & 0xFF);
		pBuffer[buffLen++] = (msgID & 0xFF00) >> 8;
		pBuffer[buffLen++] = (dataLen & 0xFF);
		tmemcpy(pBuffer+buffLen, pData, dataLen);
		sTlkSysIrqMsgWPtr = (sTlkSysIrqMsgWPtr+1) & 0xFF;
		ret = TLK_ENONE;
	}
#else
	uint08 buffLen;
	uint08 buffer[8];
	tlksys_proc_t *pProc;
	buffLen = 0;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00)>>8;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_NONE;
	buffer[buffLen++] = (msgID & 0xFF);
	buffer[buffLen++] = (msgID & 0xFF00) >> 8;
	buffer[buffLen++] = (dataLen & 0xFF);
	buffer[buffLen++] = (dataLen & 0xFF00) >> 8;
	pProc = tlksys_proc_getUsedItemByTaskID(taskID);
	if(pProc == nullptr) return -TLK_ENOSUPPORT;
	if(!sTlkSysOsIsStart || pProc->osProc.handle == tlkos_task_getCurHandle()){
		ret = tlksys_task_input(taskID, TLKPRT_COMM_MTYPE_NONE, msgID, pData, dataLen);
	}else{
		ret = tlkos_msgq_extSendFromIrq(&pProc->msgq, buffer, buffLen, nullptr, 0, pData, dataLen);
	}	
#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_sendMsg
 * Descript: Sends a message to a specified task.
 * Params:
 *     @taskID[IN]--ID of the task to be sent.
 *     @mType[IN]--The type of the message. Refer to TLKPRT_COMM_MTYPE_ENUM.
 *     @msgID[IN]--The ID of the message. Refer to "tlksys/prt/xxx".
 *     @pHead[IN]--Header data information to be sent.
 *     @headLen[IN]--The length of the header data to be sent.
 *     @pData[IN]--Body information to be sent.
 *     @dataLen[IN]--The length of the body to be sent.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.In the case of non-operating systems, if the header data is carried,
 *       the system will occupy a stack space of the size of "headLen+dataLen".
 *     2.Currently, messages with "mType=TLKPRT_COMM_MTYPE_NONE" are regarded
 *       as internal messages by default.
 *     3.In the case of non-operating systems, all messages (In addition to 
 *       the interrupt message) are processed immediately, rather than being 
 *       cached for later execution.
 *     4.In the case of an operating system, all messages are cached and 
 *       executed after subsequent tasks are activated.
 *     5.Disable calling this interface in interrupt; If used in interrupt,
 *       you can call "tlksys_sendInnerMsgFromIrq()".
 *****************************************************************************/
int tlksys_sendMsg(uint16 taskID, uint08 mType, uint16 msgID,
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	int ret;
#if !(TLK_CFG_OS_ENABLE)
	if(pHead == nullptr || headLen == 0){
		ret = tlksys_task_input(taskID, mType, msgID, pData, dataLen);
	}else if(headLen+dataLen > 1024){
		ret = -TLK_EOVERFLOW;
	}else{
		uint16 buffLen;
		uint08 buffer[headLen+dataLen];

		buffLen = 0;
		if(headLen != 0){
			tmemcpy(buffer+buffLen, pHead, headLen);
			buffLen += headLen;
		}
		if(headLen != 0){
			tmemcpy(buffer+buffLen, pData, dataLen);
			buffLen += dataLen;
		}
		ret = tlksys_task_input(taskID, mType, msgID, buffer, buffLen);
	}
#else
	uint08 buffLen;
	uint08 buffer[8];
	tlksys_proc_t *pProc;
	buffLen = 0;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00)>>8;
	buffer[buffLen++] = mType;
	buffer[buffLen++] = (msgID & 0xFF);
	buffer[buffLen++] = (msgID & 0xFF00) >> 8;
	buffer[buffLen++] = ((headLen+dataLen) & 0xFF);
	buffer[buffLen++] = ((headLen+dataLen) & 0xFF00) >> 8;
	pProc = tlksys_proc_getUsedItemByTaskID(taskID);
	if(pProc == nullptr) return -TLK_ENOSUPPORT;
	if(!sTlkSysOsIsStart || pProc->osProc.handle == tlkos_task_getCurHandle()){
		if(headLen+dataLen > 1024){
			ret = -TLK_EOVERFLOW;
		}else{
			uint16 buffLen = 0;
			uint08 buffer[headLen+dataLen];
			if(headLen != 0){
				tmemcpy(buffer+buffLen, pHead, headLen);
				buffLen += headLen;
			}
			if(headLen != 0){
				tmemcpy(buffer+buffLen, pData, dataLen);
				buffLen += dataLen;
			}
			ret = tlksys_task_input(taskID, mType, msgID, buffer, buffLen);
		}
	}else{
		ret = tlkos_msgq_extSend(&pProc->msgq, buffer, buffLen, pHead, headLen, pData, dataLen, 0);
		tlkapi_trace(TLKSYS_DBG_FLAG, TLKSYS_DBG_SIGN,
				"sendMsg: ProcID[%d],TaskID[%d],mType[%d],msgID[%d],MsgLen[%d],ret[%d]",
				pProc->procID, taskID, mType, msgID, dataLen, ret);
	}
#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_sendInnerMsgFromIrq
 * Descript: Sends a inner message to the specified task. 
 * Params:
 *     @taskID[IN]--ID of the task to be sent.
 *     @msgID[IN]--The ID of the message. Refer to "tlksys/prt/xxx".
 *     @pData[IN]--Body information to be sent.
 *     @dataLen[IN]--The length of the body to be sent.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.The message is cached and executed in a process parallel to the thread.
 *     2.Internal messages are messages that are sent between tasks in the
 *       system or between tasks.
 *     3.For details, see "tlksys_sendMsg()".
 *****************************************************************************/
int tlksys_sendInnerMsg(uint16 taskID, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	return tlksys_sendMsg(taskID, TLKPRT_COMM_MTYPE_NONE, msgID, nullptr, 0, pData, dataLen);
}

/******************************************************************************
 * Function: tlksys_sendInnerExtMsg
 * Descript: Send an extended internal message to the specified task. 
 * Params:
 *     @taskID[IN]--ID of the task to be sent.
 *     @msgID[IN]--The ID of the message. Refer to "tlksys/prt/xxx".
 *     @pHead[IN]--Header data information to be sent.
 *     @headLen[IN]--The length of the header data to be sent.
 *     @pData[IN]--Body information to be sent.
 *     @dataLen[IN]--The length of the body to be sent.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.The message is cached and executed in a process parallel to the thread.
 *     2.Internal messages are messages that are sent between tasks in the
 *       system or between tasks.
 *     3.For details, see "tlksys_sendMsg()".
 *****************************************************************************/
int tlksys_sendInnerExtMsg(uint16 taskID, uint16 msgID, 
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return tlksys_sendMsg(taskID, TLKPRT_COMM_MTYPE_NONE, msgID, pHead, headLen, pData, dataLen);
}

/******************************************************************************
 * Function: tlksys_sendOuterMsg
 * Descript: Send an external message to the specified task. 
 * Params:
 *     @taskID[IN]--ID of the task to be sent.
 *     @msgID[IN]--The ID of the message. Refer to "TLKPRT_COMM_CMDID_ENUM".
 *     @pData[IN]--Body information to be sent.
 *     @dataLen[IN]--The length of the body to be sent.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.The message is cached and executed in a process parallel to the thread.
 *     2.External messages are messages sent from outside the system to inside
 *       the system. At present, the external messages of the system refer only
 *       to the messages sent to the system through serial communication.
 *     3.For details, see "tlksys_sendMsg()".
 *****************************************************************************/
int tlksys_sendOuterMsg(uint16 taskID, uint08 mType, uint16 msgID,
	uint08 *pData, uint16 dataLen)
{
	return tlksys_sendMsg(taskID, mType, msgID, nullptr, 0, pData, dataLen);
}

/******************************************************************************
 * Function: tlksys_sendOuterExtMsg
 * Descript: Send an extended external message to the specified task. 
 * Params:
 *     @taskID[IN]--ID of the task to be sent.
 *     @msgID[IN]--The ID of the message. Refer to "TLKPRT_COMM_CMDID_ENUM".
 *     @pHead[IN]--Header data information to be sent.
 *     @headLen[IN]--The length of the header data to be sent.
 *     @pData[IN]--Body information to be sent.
 *     @dataLen[IN]--The length of the body to be sent.
 * Return: Negative values mean failure, others mean success.
 * Notice:
 *     1.The message is cached and executed in a process parallel to the thread.
 *     2.External messages are messages sent from outside the system to inside
 *       the system. At present, the external messages of the system refer only
 *       to the messages sent to the system through serial communication.
 *     3.For details, see "tlksys_sendMsg()".
 *****************************************************************************/
int tlksys_sendOuterExtMsg(uint16 taskID, uint08 mType, uint16 msgID,
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen)
{
	return tlksys_sendMsg(taskID, mType, msgID, pHead, headLen, pData, dataLen);
}


#if (TLK_CFG_OS_ENABLE)
#include "drivers.h"
/******************************************************************************
 * Function: tlksys_sendOuterExtMsg
 * Descript: Generic handler for the process. 
 * Params:
 *     @pUsrArg[IN]--User parameter, which is entered by the user when the
 *         process is mounted.
 * Return: None.
 * Notice:
 *     1.This interface is available only when the operating system is available.
 *     2.In addition to idle processes, the process will select the activation
 *       wait interval according to the adapter, and then enter the message 
 *       queue, and this interval is used as the waiting time. The current
 *       maximum blocking time is 1s.
 *****************************************************************************/
void tlktsk_process(void *pUsrArg)
{
	uint32 gTimer;
	uint32 waitTime;
	tlksys_proc_t *pProc;
	
	pProc = pUsrArg;
	gTimer = clock_time();
	while(true){
		if(sTlkSysIdleProcID == pProc->procID){
			waitTime = 0;
		}else{
			waitTime = tlksys_adapt_interval(pProc->procID);
			if(waitTime == 0xFFFFFFFF){
				waitTime = 1000000;
			}
		}

		if(pProc->msgqSize != 0 && tlkos_msgq_wait(&pProc->msgq, pProc->pRecvBuff, pProc->msgqSize, waitTime) == TLK_ENONE){
			uint16 taskID  = ((uint16)pProc->pRecvBuff[1] << 8) | pProc->pRecvBuff[0];
			uint08 mType   = pProc->pRecvBuff[2];
			uint16 msgID   = ((uint16)pProc->pRecvBuff[4] << 8) | pProc->pRecvBuff[3];
			uint16 dataLen = ((uint16)pProc->pRecvBuff[6] << 8) | pProc->pRecvBuff[5];
			tlkapi_trace(TLKSYS_DBG_FLAG, TLKSYS_DBG_SIGN,
				"RecvMsg: ProcID[%d],TaskID[%d],mType[%d],msgID[%d],MsgLen[%d]",
				pProc->procID, taskID, mType, msgID, dataLen);
			tlksys_task_input(taskID, mType, msgID, pProc->pRecvBuff+6, dataLen);
		}

		tlksys_proc_handler(pProc->procID);

		if(clock_time_exceed(gTimer, 3000000)){
			gTimer = clock_time();
			tlkapi_trace(TLKSYS_DBG_FLAG, TLKSYS_DBG_SIGN,
				"RecvMsg: ProcID[%d],ValidStack[%d]",
				pProc->procID, tlkos_task_getStackValid(&pProc->osProc));
		}
	}
}

#endif //#if (TLK_CFG_OS_ENABLE)

#endif //#if (TLK_CFG_SYS_ENABLE)

