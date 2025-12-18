/********************************************************************************************************
 * @file	tlkapi_adapt.h
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
#ifndef TLKAPI_ADAPT_H
#define TLKAPI_ADAPT_H



#define TLKAPI_TIMEOUT_MAX      (0x03FFFFFF)// BIT(31) >> 4
#define TLKAPI_TIMEOUT_MIN      50  //unit: us



typedef struct tlkapi_timer_s  tlkapi_timer_t;
typedef struct tlkapi_queue_s  tlkapi_queue_t;


/******************************************************************************
 * Object: TlkApiQueueCB
 * Brief : Defines the format of the user callback interface.
 * Params:
 *     @pQueue[IN]--The control block of the queue task.
 *     @pUsrArg[IN]--The parameters entered by the user when 
 *         "tlkapi_adapt_initQueue()" is called.
 * Notice: 
 *     1.When the callback completes execution and returns true, the adapter
 *       will push the queue task into the queue task list again.
 *****************************************************************************/
typedef bool(*TlkApiQueueCB)(tlkapi_queue_t *pQueue, uint32 userArg);

/******************************************************************************
 * Object: TlkApiTimerCB
 * Brief : Defines the format of the user callback interface.
 * Params:
 *     @pTimer[IN]--The control block of the timer task.
 *     @pUsrArg[IN]--The parameters entered by the user when 
 *         "tlkapi_adapt_initTimer()" is called.
 * Notice: 
 *     1.When the callback completes execution and returns true, the adapter
 *       will push the timer task into the timer task list again.
 *****************************************************************************/
typedef bool(*TlkApiTimerCB)(tlkapi_timer_t *pTimer, uint32 userArg);


/******************************************************************************
 * Object: tlkapi_timer_t, tlkapi_timer_s
 * Brief : Timer task control block, used to control the arrival time of timer
 *         tasks.
 * Params:
 *     @arrival--Time for the next task execution, which is obtained from the
 *         system tick value.
 *     @timeout--The preset timeout period of a timer task.
 *     @pUsrArg--The parameters entered by the user are returned to the user
 *         with timerCB.
 *     @timerCB--Interface for executing the callback of the timer task.
 *     @pNext--The next node of the timer task control block. It is maintained
 *         by the system and cannot be modified by users
 * Notice: 
 *     1.Some parameters in the control block cannot be modified by users. For
 *       example, if the "pNext" field is modified unexpectedly, a crash occurs.
 *     2.When the "timerCB" completes execution and returns true, the adapter
 *       will push the timer task into the timer task list again.
 *     3.The reference time of the timer task that reinserts the task list will
 *       depend on "pAdapt->isUpdate": If "isUpdate" is set to true, the 
 *       reference time is the current time. Otherwise, the reference time is 
 *       the last end time of the timer task.
 *****************************************************************************/
struct tlkapi_timer_s{
	uint32 arrival;
	uint32 timeout;
	uint32 userArg;
	TlkApiTimerCB timerCB;
	struct tlkapi_timer_s *pNext;
};

/******************************************************************************
 * Object: tlkapi_queue_t, tlkapi_queue_s
 * Brief : Queue task control block, used to control the execution of queue 
 *         tasks.
 * Params:
 *     @pUsrArg--The parameters entered by the user are returned to the user
 *         with queueCB.
 *     @timerCB--Interface for executing the callback of the queue task.
 *     @pNext--The next node of the queue task control block. It is maintained
 *         by the system and cannot be modified by users
 * Notice: 
 *     1.Some parameters in the control block cannot be modified by users. For
 *       example, if the "pNext" field is modified unexpectedly, a crash occurs.
 *     2.When the "queueCB" completes execution and returns true, the adapter
 *       will push the queue task into the queue task list again.
 *****************************************************************************/
struct tlkapi_queue_s{
	uint32 userArg;
	TlkApiQueueCB queueCB;
	struct tlkapi_queue_s *pNext;
};

/******************************************************************************
 * Object: tlkapi_adapt_t
 * Brief : An integrated control block of queue tasks and timer tasks, used to
 *         maintain the execution of multiple tasks in the system.
 * Params:
 *     @isUpdate--Whether to forcibly align the timer with the current time 
 *         after the timer task expires. After alignment, the timing time starts
 *         from the current time.
 *     @reserve0--Reserve Parameter, not used.
 *     @reserve1--Reserve Parameter, not used.
 *     @pQueueList--The list of queue tasks in the adapter.
 *     @pTimerList--The list of timer tasks in the adapter.
 * Notice:
 *     1.When "isUpdate" is set to true, the system time error is gradually 
 *       accumulated.
 *     2."isUpdate" defaults to false. Users can assign this value to the 
 *       control block if they want to change it.
 *****************************************************************************/
typedef struct{
	uint08 isUpdate;
	uint08 reserve0;
	uint16 reserve1;
	tlkapi_queue_t *pQueueList; //Circulation list
	tlkapi_timer_t *pTimerList; //Singly linked list
}tlkapi_adapt_t;



/******************************************************************************
 * Function: tlkapi_adapt_init
 * Descript: Initializes the adapter control parameters.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1."pAdapt->isUpdate" defaults to false. Users can assign this value to  
 *       the control block if they want to change it.
 *****************************************************************************/
int tlkapi_adapt_init(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_clear
 * Descript: Clear all queue tasks and timer tasks in the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 * Return: None.
 * Notice:
 *     1.This interface removes all queue and timer tasks from the adapter and
 *       sets the "pNext" field in the task control block to nullptr.
 *     2.A removed task does not notify the task initiator through a callback.
 *****************************************************************************/
void tlkapi_adapt_clear(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_handler
 * Descript: Run all queue tasks and timer tasks that reach the execution time.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 * Return: None.
 * Notice:
 *     1.If the task initiator returns true in the task callback function, the
 *       system automatically inserts the task into the task list. Otherwise,
 *       the system removes the executed task.
 *****************************************************************************/
void tlkapi_adapt_handler(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_timerInterval
 * Descript: Gets the time interval between the start of the latest task and 
 *           the current time for all tasks (including queue tasks and timer 
 *           tasks) in the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 * Return: The interval between the execution time of the latest task and the
 *     current time in the adapter. Unit: us.
 * Notice: 
 *     1.If the return value is zero, it means that the adapter is busy and 
 *       needs to be executed immediately.
 *****************************************************************************/
uint tlkapi_adapt_interval(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_intervalTicks
 * Descript: Gets the time interval between the start of the latest task and 
 *           the current time for all tasks (including queue tasks and timer 
 *           tasks) in the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 * Return: The interval between the execution time of the latest task and the
 *     current time in the adapter. Unit: ticks.
 * Notice: 
 *     1.The transformation relationship between tick and time is as follows:
 *       time(us)=ticks/SYSTEM_TIMER_TICK_1US.
 *****************************************************************************/
uint tlkapi_adapt_intervalTicks(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_initTimer
 * Descript: Initialize the timer task.
 * Params:
 *     @pTimer[IN]--The control block of the timer task.
 *     @timerCB[IN]--The callback interface after the time has arrived.
 *     @timeout[IN]--The preset timeout period of a timer task. Unit:us.
 *     @userArg[IN]--The parameters entered by the user are returned to the user
 *         with "queueCB".
 * Return: Operating results. "TLK_ENONE" means success, others means failure.
 * Notice: 
 *     1.The timer task is maintained by the system tick. Due to the tick 
 *       length (32 bits), the value of "timeout" must be between 
 *       "TLKAPI_TIMEOUT_MIN" and "TLKAPI_TIMEOUT_MAX".
 *     2.The timer task that exists in the adapter cannot be re-initialized, 
 *       otherwise it will cause a crash. The user can call the interface 
 *       "tlkapi_adapt_isHaveTimer()" to detect if the timer task exists in
 *       the adapter.
 *     3.When the "timerCB" completes execution and returns true, the adapter
 *       will push the timer into the timer task list again. 
 *     4.The reference time of the timer task that reinserts the task list will
 *       depend on "pAdapt->isUpdate": If "isUpdate" is set to true, the 
 *       reference time is the current time. Otherwise, the reference time is 
 *       the last end time of the timer task.
 *****************************************************************************/
int tlkapi_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB,
	uint32 userArg, uint32 timeout);

/******************************************************************************
 * Function: tlkapi_adapt_initQueue
 * Descript: Initializes the queue task.
 * Params:
 *     @pQueue[IN]--The control block of the queue task.
 *     @queueCB[IN]--The callback interface for the work to be executed.
 *     @userArg[IN]--The parameters entered by the user are returned to the user
 *         with "queueCB".
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.The queue task that exists in the adapter cannot be re-initialized, 
 *       otherwise it will cause a crash. The user can call the interface 
 *       "tlkapi_adapt_isHaveQueue()" to detect if the queue task exists in
 *       the adapter.
 *****************************************************************************/
int tlkapi_adapt_initQueue(tlkapi_queue_t *pQueue, TlkApiQueueCB queueCB,
	uint32 userArg);

/******************************************************************************
 * Function: tlkapi_adapt_deinitTimer
 * Descript: Deinitializes the timer task.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pTimer[IN]--The control block of the timer task.
 * Return: None.
 * Notice: 
 *     1.After deinitialization, the timer task is removed from the adapter.
 *     2.After deinitialization, the data in the timer task control block will
 *       be reset.
 *****************************************************************************/
void tlkapi_adapt_deinitTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkapi_adapt_deinitQueue
 * Descript: Deinitializes the queue task.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pQueue[IN]--The control block of the queue task.
 * Return: None.
 * Notice: 
 *     1.After deinitialization, the queue task is removed from the adapter.
 *     2.After deinitialization, the data in the queue task control block will
 *       be reset.
 *****************************************************************************/
void tlkapi_adapt_deinitQueue(tlkapi_adapt_t *pAdapt, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlkapi_adapt_isHaveTimer
 * Descript: Check whether timer tasks exist in the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pTimer[IN]--The control block of the timer task.
 * Return: "true" means that the timer task exists in the adapter, otherwise
 *    it does not exist in the adapter.
 *****************************************************************************/
bool tlkapi_adapt_isHaveTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkapi_adapt_isHaveQueue
 * Descript: Check whether queue tasks exist in the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pQueue[IN]--The control block of the queue task.
 * Return: "true" means that the queue task exists in the adapter, otherwise
 *    it does not exist in the adapter.
 *****************************************************************************/
bool tlkapi_adapt_isHaveQueue(tlkapi_adapt_t *pAdapt, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlkapi_adapt_appendQueue
 * Descript: Pushes the queue task into the queue task list in the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pQueue[IN]--The control block of the queue task.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.Before invoking this interface, "tlkapi_adapt_deinitQueue()" needs to
 *       be called to initialize the queue task.
 *     2.After the interface is called, the system executes the queue task when
 *       it is idle (Next loop). 
 *     3.Queue tasks are executed in the order in which they were inserted.
 *****************************************************************************/
int tlkapi_adapt_appendQueue(tlkapi_adapt_t *pAdapt, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlkapi_adapt_removeQueue
 * Descript: Removes the queue task from the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pQueue[IN]--The control block of the queue task.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkapi_adapt_removeQueue(tlkapi_adapt_t *pAdapt, tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: tlkapi_adapt_updateTimer
 * Descript: Update the timeout period of the timer task.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pTimer[IN]--The control block of the timer task.
 *     @timeout[IN]--The preset timeout period of the timer task. Unit:us.
 *     @isInsert[IN]--Whether to insert the timer task that updates the timeout
 *         period into the adapter.
 * Notice: 
 *     1.The timer task is maintained by the system tick. Due to the tick 
 *       length (32 bits), the value of "timeout" must be between 
 *       "TLKAPI_TIMEOUT_MIN" and "TLKAPI_TIMEOUT_MAX".
 *     2.In the case of "isInsert" being true, if the task to be updated 
 *       already exists in the adaptation, it is removed from the adaptation 
 *       and then reinserted.
 *     3.If "isInsert" is not true, the timeout will take effect after the 
 *       next task execution if the task exists in the adaptation.
 *****************************************************************************/
int tlkapi_adapt_updateTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer,
	uint32 timeout, bool isInsert);

/******************************************************************************
 * Function: tlkapi_adapt_insertTimer
 * Descript: Insert the timer task into the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pTimer[IN]--The control block of the timer task.
 *     @isUpdate[IN]--Whether to update the reference time. After the reference
 *         time is updated, it is the current time.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.The reference time of the timer task that reinserts the task list will
 *       depend on "pAdapt->isUpdate" and "isUpdate": If "pAdapt->isUpdate" or  
 *       "isUpdate" is set to true, the reference time is the current time.
 *       Otherwise, the reference time is the last end time of the timer task.
 *****************************************************************************/
int tlkapi_adapt_insertTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer,
	bool isUpdate);

/******************************************************************************
 * Function: tlkapi_adapt_removeTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 *     @pTimer[IN]--The control block of the timer task.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkapi_adapt_removeTimer(tlkapi_adapt_t *pAdapt, tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkapi_adapt_timerInterval
 * Descript: Obtain the interval between the execution time of the latest timer
 *           task and the current time in the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 * Return: The interval between the execution time of the latest timer task and
 *     the current time in the adapter. Unit: us.
 * Notice: 
 *     1.If the return value is zero, it means that the adapter is busy and 
 *       needs to be executed immediately.
 *     2.Unlike "tlkapi_adapt_interval()", here only the idle interval of the
 *       timer task list in the adapter is obtained.
 *****************************************************************************/
uint tlkapi_adapt_timerInterval(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_timerIntervalTicks
 * Descript: Obtain the interval between the execution time of the latest timer
 *           task and the current time in the adapter.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 * Return: The interval between the execution time of the latest timer task and
 *     the current time in the adapter. Unit: ticks.
 * Notice: 
 *     1.If the return value is zero, it means that the adapter is busy and 
 *       needs to be executed immediately.
 *     2.The transformation relationship between tick and time is as follows:
 *       time(us)=ticks/SYSTEM_TIMER_TICK_1US.
 *     3.Unlike "tlkapi_adapt_intervalTicks()", here only the idle interval of
 *       the timer task list in the adapter is obtained.
 *****************************************************************************/
uint tlkapi_adapt_timerIntervalTicks(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_takeFirstTimer
 * Descript: Gets the first timer task in the adapter that reaches the
 *           execution time.
 * Params:
 *     @pAdapt[IN]--The control block of the adapter.
 * Return: The first timer task in the adapter that reaches the execution time.
 * Notice: 
 *     1.After calling this interface, the timer task is removed from the 
 *       adapter. If it needs to be reinserted, the caller needs to call 
 *       "tlkapi_adapt_insertTimer()".
 *     2.The interface returns nullptr, which does not mean that there are no 
 *       timer tasks in the adapter, only that there are currently no timer 
 *       tasks that need to be executed.
 *****************************************************************************/
tlkapi_timer_t *tlkapi_adapt_takeFirstTimer(tlkapi_adapt_t *pAdapt);

/******************************************************************************
 * Function: tlkapi_adapt_timerCompare
 * Descript: Detects the state of current time relative to refer time.
 * Params:
 *     @curTicks[IN]--The current time.
 *     @refTicks[IN]--The refer time.
 * Return: The state of current time relative to refer time:
 *         0 means that curTicks have arrived; 
 *         -1 means that curTicks have expired;
 *         1 means the curTicks have not arrived.
 *****************************************************************************/
int tlkapi_adapt_timerCompare(uint32 curTicks, uint32 refTicks);


#endif //TLKAPI_ADAPT_H

