#include "DOS_Event.h"
#include "DOS_Interrupt.h"
#include "DOS_Delay.h"

Event_t* g_EventList[EVENT_TYPE_SUM] = {0};

/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
static void DOS_Event_AddTaskToList(Event_t* pEvent, Task_t* pTask)
{
	if(pEvent == NULL || pTask == NULL)
	{
		return ;
	}

	if(pTask->state != FREE_STATE)
	{
		DOS_Task_FreeTask(pTask);
	}

	TaskNode_t* pAddNode = DOS_Task_GetNode(pTask);
	pTask->taskNode = pAddNode;

	DOS_TaskNode_AddNodeToList(&pEvent->eventWaitList, pAddNode);

	pTask->state = WAIT_STATE;
	pTask->waitEvent = pEvent;
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Event_RemoveTaskFromList(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return ;
	}
	else if(pTask->state != WAIT_STATE || pTask->waitEvent == NULL)
	{
		return ;
	}
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
TaskNode_t* DOS_Event_GetFirstTaskNodeOfList(Event_t* pEvent)
{
	if(pEvent == NULL || pEvent->eventWaitList == NULL)
	{
		return NULL;
	}

#if 0
	return DOS_TaskNode_PopFirstNode(&(pEvent->eventWaitList));
#else
	TaskNode_t* pPopNode = pEvent->eventWaitList;
	return pPopNode;
#endif
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Event_Init(Event_t* event, EventType_t type)
{
	//*event = new Event_t;	
	(event)->event = type;
	(event)->eventWaitList = NULL;
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Event_Wait(Event_t* event, void* msg, uint32_t timeout)
{
	DOS_DisableIrq();

	Task_t* pTask = g_currentTask;

	DOS_Event_AddTaskToList(event, pTask);

	pTask->eventMsg = msg;

	if(timeout)
	{
		pTask->delay = timeout/10;
		DOS_Delay_AddTaskToList(pTask);
	}

	DOS_EnableIrq();

	//DOS_TaskSchedule();
}


/********************************************
 * Function: DOS_Event_Notify
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
Task_t* DOS_Event_Notify(Event_t* event, void* msg)
{
	DOS_DisableIrq();
	Task_t* reTask;

	if(event == NULL)
	{
		return NULL;
	}

	if(event->eventWaitList != NULL)
	{
		TaskNode_t* toRdyTask = DOS_Event_GetFirstTaskNodeOfList(event);
		Task_t* pTask = toRdyTask->task;
		pTask->eventMsg = msg;
		reTask = pTask;
		DOS_Task_ReadyTask(pTask);
		if(pTask->delay > 0)
		{
			DOS_Delay_DeleteTaskFromList(pTask);
		}
	}

	DOS_EnableIrq();
	return reTask;
}


/********************************************
 * Function: DOS_Event_Notify_Task
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
EventReturn_t DOS_Event_Notify(Event_t* event, void* msg, Task_t* pTask)
{
	DOS_DisableIrq();

	if(event == NULL || pTask == NULL || pTask->state != WAIT_STATE)
	{
		return EVENT_ERROR;
	}

	pTask->eventMsg = msg;
	DOS_Task_ReadyTask(pTask);
	if(pTask->delay > 0)
	{
		DOS_Delay_DeleteTaskFromList(pTask);
	}

	DOS_EnableIrq();
	return EVENT_SUCCESS;
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Event_DeleteTaskFromList(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return ;
	}

	TaskNode_t* pDelNode = pTask->taskNode;
	//TaskNode_t* pWaitListHead = pTask->waitEvent->eventWaitList;
	//TaskNode_t* pWaitListNode = pWaitListHead;

	if(pDelNode->preNode != NULL && pDelNode->nextNode != NULL)//Middle Node
	{
		pDelNode->preNode->nextNode = pDelNode->nextNode;
		pDelNode->nextNode->preNode = pDelNode->preNode;
	}
	else if(pDelNode->preNode == NULL && pDelNode->nextNode != NULL)//First Node
	{
		pTask->waitEvent->eventWaitList = pDelNode->nextNode;
		pTask->waitEvent->eventWaitList->preNode = NULL;
	}
	else if(pDelNode->preNode != NULL && pDelNode->nextNode == NULL)//Last Node
	{
		pDelNode->preNode->nextNode = pDelNode->nextNode;
	}
	else if(pDelNode->preNode == NULL && pDelNode->nextNode == NULL)//Only One Node
	{
		pTask->waitEvent->eventWaitList = pDelNode->nextNode;
	}

	pDelNode->nextNode = NULL;
	pDelNode->preNode  = NULL;

	DOS_Task_DelNode(pTask);

	pTask->waitEvent = NULL;
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Event_Timeout(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return;
	}

	pTask->waitEventResult = EVENT_TIMEOUT;
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
uint32_t DOS_Event_RemoveAll(Event_t* event)
{
	DOS_DisableIrq();
	uint32_t count = 0;

	if(event == NULL || event->eventWaitList == NULL)
	{
		return 0;
	}

	while(event->eventWaitList)
	{
		TaskNode_t* toRdyTask = DOS_Event_GetFirstTaskNodeOfList(event);
		Task_t* pTask = toRdyTask->task;
		DOS_Task_ReadyTask(pTask);
		if(pTask->delay > 0)
		{
			DOS_Delay_DeleteTaskFromList(pTask);
		}
		++count;
	}

	DOS_EnableIrq();
	return count;
}

