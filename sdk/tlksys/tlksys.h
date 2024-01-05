/********************************************************************************************************
 * @file	tlksys.h
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
#ifndef TLKSYS_H
#define TLKSYS_H

#if (TLK_CFG_SYS_ENABLE)


#define TLKSYS_DBG_FLAG       ((TLK_MAJOR_DBGID_SYS << 24) | (TLK_MINOR_DBGID_SYS_TSK << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKSYS_DBG_SIGN       "[SYS]"

#if !(TLK_CFG_OS_ENABLE)
#define TLKSYS_IRQMSG_ITEM_SIZE       16
#define TLKSYS_IRQMSG_ITEM_NUMB       16 //Must be a power of two, and less than 250
#define TLKSYS_IRQMSG_ITEM_MASK       (TLKSYS_IRQMSG_ITEM_NUMB-1)
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
int tlksys_init(void);

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
void tlksys_setIdleProc(uint08 procID);

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
int tlksys_start(uint08 procID);

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
void tlksys_pause(uint08 procID);

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
void tlksys_close(uint08 procID);

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
void tlksys_handler(void);

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
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);

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
	uint08 *pData, uint16 dataLen);

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
int tlksys_sendInnerMsg(uint16 taskID, uint16 msgID, 
	uint08 *pData, uint16 dataLen);

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
	uint08 *pData, uint16 dataLen);

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
int tlksys_sendInnerExtMsg(uint16 taskID, uint16 msgID, uint08 *pHead,
	uint16 headLen, uint08 *pData, uint16 dataLen);

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
	uint08 *pHead, uint16 headLen, uint08 *pData, uint16 dataLen);


#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_H

