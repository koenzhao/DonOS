/******************************************************
 * File Name:   DOS_Task.cpp
 * File Date:   2018.06.07
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.06.07 | Create   |  Koen  |
 *****************************************************/


#include "DOS_Cpu.h"
#include "DOS_Task.h"
#include "DOS_Commondefine.h"
#include "DOS_Interrupt.h"
#include "DOS_Timer.h"
#include "DOS_LinkedList.h"
#include "DOS_Bitmap.h"
#include "DOS_Delay.h"
#include "DOS_Event.h"

Task_t* taskList[10] = {0};
Task_t* g_currentTask = 0;
Task_t* g_nextTask = 0;
uint32_t taskIndex = 0;
uint32_t taskSum   = 0;

uint32_t g_readyListState = 0x00000000;
//ReadyListNode_t g_ReadyTaskList[TASK_PRIORITY] = {0};
//TaskNode_t* g_ReadyTaskList[TASK_PRIORITY] = {0};
List_t* g_ReadyTaskList[TASK_PRIORITY] = {0};

/********************************************
 * Function: DOS_CreateTask
 * Description: An user interface for create a task
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_CreateTask(Task_t* task, 
						void (*entry)(void *), 
						void* param, 
						TaskPriority_t priority,
						uint32_t slice,
						uint32_t* stack)
{
	*(--stack) = (uint32_t)(1<<24);
	*(--stack) = (uint32_t)entry;
	*(--stack) = (unsigned long)0x14;                   
    *(--stack) = (unsigned long)0x12;                   
    *(--stack) = (unsigned long)0x3;                    
    *(--stack) = (unsigned long)0x2;                   
    *(--stack) = (unsigned long)0x1;                    
    *(--stack) = (unsigned long)param;                  
    *(--stack) = (unsigned long)0x11;                   
    *(--stack) = (unsigned long)0x10;                   
    *(--stack) = (unsigned long)0x9;                    
    *(--stack) = (unsigned long)0x8;                    
    *(--stack) = (unsigned long)0x7;                    
    *(--stack) = (unsigned long)0x6;                    
    *(--stack) = (unsigned long)0x5;                    
    *(--stack) = (unsigned long)0x4;
	
	task->stack = stack;
	task->priority = priority;
	task->slice = slice;
	task->sliceReload = slice;
	task->state = INITIAL_STATE;
	task->waitEvent = NULL;
	task->eventMsg  = NULL;
	task->waitEventResult = NULL;
	
	//task->taskNode = new TaskNode_t;
	//task->taskNode->task = task;
	
	DOS_Task_ReadyTask(task);
	
	taskSum++;
}
						
/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_TaskSchedule(void)
{
	int16_t i = DOS_32Bitmap_GetFirstSetBit(&g_readyListState);
	if(i == DOS_ERROR)
	{
		printf("[ERROR] Fail to get the first set bit!");
		return ;
	}
	
	if(g_currentTask->state == RUNNING_STATE)
	{
		g_currentTask->state = READY_STATE;
	}
#if 0
	TaskNode_t* readyTask = g_ReadyTaskList[i];
	g_nextTask = readyTask->task;
#else
	g_nextTask = (Task_t*)g_ReadyTaskList[i]->data;
#endif
	g_nextTask->state = RUNNING_STATE;

	DOS_TaskSwitch();
}

/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_TaskSwitch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_TaskStart(void)
{
	__set_PSP(0);
	
	//DOS_SetSysTickPeriod(10);

    MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
    
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

/********************************************
 * Function: DOS_Task_ReadyTask
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Task_ReadyTask(Task_t* pTask)
{
	if(pTask == NULL)
	{
		printf("[ERROR] Pointer is NULL!");
		return ;
	}
	if(pTask->state == READY_STATE)
	{
		printf("[ERROR] The task is already ready!");
		return ;
	}

	if(pTask->state != FREE_STATE || pTask->taskNode != NULL)
	{
		DOS_Task_FreeTask(pTask);
	}

	DOS_Ready_AddTaskToList(pTask);
	
	pTask->state = READY_STATE;
}


/********************************************
 * Function: DOS_Task_FreeTask
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Task_FreeTask(Task_t* pTask)
{
	if(pTask == NULL)
	{
		printf("[ERROR] Pointer is NULL!");
		return ;
	}
	if(pTask->state == FREE_STATE)
	{
		printf("[ERROR] The task is already free!");
		return ;
	}

	//if(pTask->taskNode != NULL)
	{	
		switch(pTask->state)
		{
			case RUNNING_STATE:
			case READY_STATE:
				DOS_Ready_DeleteTaskFromList(pTask);
				break;
			case DELAY_STATE:
				DOS_Delay_DeleteTaskFromList(pTask);
				break;
			case WAIT_STATE:
				DOS_Event_DeleteTaskFromList(pTask);
				break;
			default:
				//DOS_PopTaskNode(freeNode);
				break;
		}
		pTask->state = FREE_STATE;
	}
	//else
	{
		//pTask->state = FREE_STATE;
	}
}


/********************************************
 * Function: DOS_Task_DeleteTask
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Task_DeleteTask(Task_t* task)
{
	if(task == NULL)
	{
		return ;
	}

	task->state = DELETE_STATE;
	delete task->taskNode;
}


/********************************************
 * Function: DOS_GetIdleTask
 * Description: User interface for get Idle Task
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
Task_t* DOS_GetIdleTask()
{
	return (Task_t*)g_ReadyTaskList[TASK_PRIORITY - 1]->data;
	
}


/********************************************
 * Function: DOS_Ready_AddTaskToList
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
static void DOS_Ready_AddTaskToList(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return ;
	}

#if 0
	TaskNode_t* pAddNode  = DOS_Task_GetNode(pTask);
	TaskNode_t* pListNode = g_ReadyTaskList[pTask->priority];
	pTask->taskNode = pAddNode;

	if(pListNode == NULL)
	{
		g_ReadyTaskList[pTask->priority] = pAddNode;
	}
	else
	{
		while(pListNode->nextNode)
		{
			pListNode = pListNode->nextNode;
		}
		pListNode->nextNode = pAddNode;
		pAddNode->preNode   = pListNode;
	}
#else
	g_ReadyTaskList[pTask->priority] = DOS_List_Append(g_ReadyTaskList[pTask->priority], (void *)pTask);
#endif
	DOS_32Bitmap_SetBit(&g_readyListState, pTask->priority);
}


/********************************************
 * Function: DOS_Ready_DeleteTaskFromList
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Ready_DeleteTaskFromList(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return ;
	}

#if 0
	TaskNode_t* pDelNode = pTask->taskNode;

	if(pDelNode->preNode != NULL && pDelNode->nextNode != NULL)
	{
		pDelNode->preNode->nextNode = pDelNode->nextNode;
		pDelNode->nextNode->preNode = pDelNode->preNode;
	}
	else if(pDelNode->preNode == NULL && pDelNode->nextNode != NULL)
	{
		g_ReadyTaskList[pTask->priority] = pDelNode->nextNode;
		g_ReadyTaskList[pTask->priority]->preNode = NULL;
	}
	else if(pDelNode->preNode != NULL && pDelNode->nextNode == NULL)
	{
		pDelNode->preNode->nextNode = pDelNode->nextNode;
	}
	else if(pDelNode->preNode == NULL && pDelNode->nextNode == NULL)
	{
		g_ReadyTaskList[pTask->priority] = pDelNode->nextNode;
	}

	pDelNode->nextNode = NULL;
	pDelNode->preNode  = NULL;

	DOS_Task_DelNode(pTask);
#else
	g_ReadyTaskList[pTask->priority] = DOS_List_Remove(g_ReadyTaskList[pTask->priority], (void *)pTask);
#endif
	if(g_ReadyTaskList[pTask->priority] == NULL)
	{
		DOS_32Bitmap_ClearBit(&g_readyListState, pTask->priority);
	}
}


/********************************************
 * Function: DOS_Task_GetNode
 * Description: External interface for get a new "TaskNode_t" for task.
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
TaskNode_t* DOS_Task_GetNode(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return NULL;
	}

#if 0
	if(pTask->taskNode != NULL)
	{
		return pTask->taskNode;
	}
	else
	{
		TaskNode_t* pTaskNode = new TaskNode_t;
		pTaskNode->task       = pTask;
		pTaskNode->nextNode   = NULL;
		pTaskNode->preNode    = NULL;
		pTask->taskNode       = pTaskNode;
		return pTaskNode;
	}
#else
	TaskNode_t* pTaskNode = new TaskNode_t;
	pTaskNode->task       = pTask;
	pTaskNode->nextNode   = NULL;
	pTaskNode->preNode    = NULL;

	return pTaskNode;
#endif
}

/********************************************
 * Function: DOS_Task_DelNode
 * Description: External interface for delete a "TaskNode_t" for task
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Task_DelNode(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return ;
	}

#if 0
	if(pTask->taskNode == NULL)
	{
		return ;
	}
	else
	{
		delete pTask->taskNode;
		pTask->taskNode = NULL;
	}
#else
	if(pTask->taskNode != NULL || pTask->delayNode != NULL)
	{
		TaskNode_t* pTaskNode;
		if(pTask->state == DELAY_STATE)
		{
			pTaskNode = pTask->delayNode;
		}
		else
		{
			pTaskNode = pTask->taskNode;
		}
		delete pTaskNode;
	}
#endif
}

/********************************************
 * Function: DOS_Task2TaskNode
 * Description: External interface for
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
TaskNode_t* DOS_Task2TaskNode(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return NULL;
	}

	if(pTask->taskNode)
	{
		return pTask->taskNode;
	}
	else
	{
		TaskNode_t* retNode = new TaskNode_t;
		retNode->task = pTask;
		pTask->taskNode = retNode;
		return retNode;
	}
}

/********************************************
 * Function: DOS_TaskNode2Task
 * Description: External interface for
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
Task_t* DOS_TaskNode2Task(TaskNode_t* pTaskNode)
{
	if(pTaskNode == NULL)
	{
		return NULL;
	}

	if(pTaskNode->task)
	{
		return pTaskNode->task;
	}
	else
	{
		Task_t* retTask = pTaskNode->task;
		pTaskNode->nextNode = NULL;
		pTaskNode->preNode = NULL;
		delete pTaskNode;
		return retTask;
	}
}


#if 0
/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
static TaskNode_t* DOS_Task_PopNodeFromReadyList(TaskNode_t* popNode)
{
#if 0
	DOS_PopTaskNode(popNode);
	if(g_ReadyTaskList[popNode->task->priority].headNode->nextNode == NULL)
	{
		DOS_32Bitmap_ClearBit(&g_readyListState, popNode->task->priority);
	}
	return popNode;
#else
	if(popNode == NULL)
	{
		return NULL;
	}

	if(popNode->preNode != NULL && popNode->nextNode != NULL)
	{
		popNode->preNode->nextNode = popNode->nextNode;
		popNode->nextNode->preNode = popNode->preNode;
	}
	else if(popNode->preNode == NULL && popNode->nextNode != NULL)
	{
		g_ReadyTaskList[popNode->task->priority] = popNode->nextNode;
		g_ReadyTaskList[popNode->task->priority]->preNode = NULL;
	}
	else if(popNode->preNode != NULL && popNode->nextNode == NULL)
	{
		popNode->preNode->nextNode = popNode->nextNode;
	}
	else if(popNode->preNode == NULL && popNode->nextNode == NULL)
	{
		g_ReadyTaskList[popNode->task->priority] = popNode->nextNode;
	}

	popNode->nextNode = NULL;
	popNode->preNode  = NULL;

	if(g_ReadyTaskList[popNode->task->priority] == NULL)
	{
		DOS_32Bitmap_ClearBit(&g_readyListState, popNode->task->priority);
	}

	return popNode;
#endif	
}
#endif



#if 0
/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
static void DOS_Task_AddNodeToReadyList(TaskNode_t* addNode)
{
#if 0
	return DOS_TailAddTaskNode(g_ReadyTaskList[addNode->task->priority].headNode, addNode);
#else
	if(addNode == NULL)
	{
		return ;
	}

	TaskNode_t* pListNode = g_ReadyTaskList[addNode->task->priority];
	if(pListNode == NULL)
	{
		g_ReadyTaskList[addNode->task->priority] = addNode;
	}
	else
	{
		while(pListNode->nextNode)
		{
			pListNode = pListNode->nextNode;
		}
		pListNode->nextNode = addNode;
		addNode->preNode = pListNode;
	}
#endif
}
#endif


#if 0
/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
static void DOS_Task_FreeNode(TaskNode_t* freeNode)
{
	if(freeNode == NULL)
	{
		printf("[ERROR] Pointer is NULL!");
		return ;
	}

	if(freeNode->task->state != FREE_STATE)
	{
		switch(freeNode->task->state)
		{
			case RUNNING_STATE:
			case READY_STATE:
				//DOS_Task_PopNodeFromReadyList(freeNode);
				//DOS_Ready_DeleteTaskFromList(Task_t * pTask)
				break;
			case DELAY_STATE:
				DOS_Delay_PopNodeFromDelayList(freeNode);
				break;
			case WAIT_STATE:
				break;
			default:
				//DOS_PopTaskNode(freeNode);
				break;
		}
		freeNode->task->state = FREE_STATE;
	}
}
#endif


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_TaskNode_AddNodeToList(TaskNode_t** ppHeadNode, TaskNode_t* addNode)
{
	if(ppHeadNode == NULL || addNode == NULL)
	{
		return ;
	}

	TaskNode_t* pHeadNode = *ppHeadNode;
	if(pHeadNode == NULL)
	{
		*ppHeadNode = addNode;
		return ;
	}
	else
	{
		while(pHeadNode->nextNode)
		{
			pHeadNode = pHeadNode->nextNode;
		}

		pHeadNode->nextNode = addNode;
		addNode->preNode = pHeadNode;
		addNode->nextNode = NULL;
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
void DOS_TaskNode_InsertNodeAfterNode(TaskNode_t* pPreNode, TaskNode_t* pInsNode)
{
	if(pPreNode == NULL || pInsNode == NULL)
	{
		return ;
	}

	pInsNode->nextNode = pPreNode->nextNode;
	pInsNode->preNode = pPreNode;
	if(pInsNode->nextNode != NULL)
	{
		pInsNode->nextNode->preNode = pInsNode;
	}
	pInsNode->preNode->nextNode = pInsNode;
}

/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_TaskNode_InsertNodeBeforeNode(TaskNode_t* pNextNode, TaskNode_t* pInsNode)
{
	if(pNextNode == NULL || pInsNode == NULL)
	{
		return ;
	}

	pInsNode->preNode = pNextNode->preNode;
	pInsNode->nextNode = pNextNode;
	if(pInsNode->preNode != NULL)
	{
		pInsNode->preNode->nextNode = pInsNode;
	}
	pInsNode->nextNode->preNode = pInsNode;
}

/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_TaskNode_CopyNode(TaskNode_t* pDstNode, TaskNode_t* pSrcNode)
{
	if(pDstNode == NULL || pSrcNode == NULL)
	{
		return ;
	}

	pDstNode = new TaskNode_t;
	pDstNode->task = pSrcNode->task;
}

/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_TaskNode_DeleteNode(TaskNode_t* pDelNode)
{
	if(pDelNode == NULL)
	{
		return ;
	}

	delete pDelNode;
}

/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
TaskNode_t* DOS_TaskNode_PopFirstNode(TaskNode_t** ppHeadNode)
{
	if(ppHeadNode == NULL || *ppHeadNode == NULL)
	{
		return NULL;
	}

	TaskNode_t* pHeadNode = *ppHeadNode;
	*ppHeadNode = pHeadNode->nextNode;
	if(pHeadNode->nextNode != NULL)
	{
		pHeadNode->nextNode->preNode = NULL;
	}

	return pHeadNode;
}

/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
TaskNode_t* DOS_TaskNode_PopNode(TaskNode_t* pPopNode)
{
	if(pPopNode == NULL)
	{
		return NULL;
	}

	if(pPopNode->nextNode == NULL && pPopNode->preNode == NULL)
	{
		return pPopNode;
	}
	else if(pPopNode->preNode == NULL && pPopNode->nextNode != NULL)
	{
		return pPopNode;
	}
	else if(pPopNode->preNode != NULL && pPopNode->nextNode == NULL)
	{
		return pPopNode;
	}
	else if(pPopNode->preNode != NULL && pPopNode->nextNode != NULL)
	{
		return pPopNode;
	}
	return pPopNode;
}

/**********************END**********************/

