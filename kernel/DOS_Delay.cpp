/******************************************************
 * File Name:   DOS_Delay.cpp
 * File Date:   2018.06.07
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.06.07 | Create   |  Koen  |
 *****************************************************/
#include "DOS_Delay.h"
#include "DOS_LinkedList.h"

DelayTimeListNode_t * g_DelayTimeList = new DelayTimeListNode_t;

/********************************************
 * Function: DOS_BlockDelay
 * Description: An user interface to delay, but it block.
 * Input: count
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
void DOS_BlockDelay(int32_t count)
{
	while(--count);
}


/********************************************
 * Function: DOS_Delay
 * Description: An user interface to delay, noblock.
 * Input: ms
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
void DOS_Delay(uint32_t ms)
{
	g_currentTask->delay = ms/10;
	DOS_Task_DelayTask(g_currentTask);	
	DOS_TaskSchedule();
}


/********************************************
 * Function: DOS_Task_DelayTask
 * Description: An internal interface
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
void DOS_Task_DelayTask(Task_t* pTask)
{
	if(pTask == NULL)
	{
		printf("[ERROR] Pointer is NULL!");
		return ;
	}
	if(pTask->state == DELAY_STATE)
	{
		printf("[ERROR] The task is already delay!");
		return ;
	}

	if(pTask->state != FREE_STATE)
	{
		DOS_Task_FreeTask(pTask);
	}
	
	DOS_Delay_AddTaskToList(pTask);
	pTask->state = DELAY_STATE;
}


/********************************************
 * Function: DOS_Delay_AddTaskToList
 * Description: 
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
void DOS_Delay_AddTaskToList(Task_t* pTask)
{
	if(pTask == NULL)
	{
		return ;
	}

	TaskNode_t* pAddNode = DOS_Task_GetNode(pTask);
	pTask->delayNode = pAddNode;
	DOS_Delay_AddNodeToDelayTimeList(pAddNode);
}


/********************************************
 * Function: DOS_Delay_DeleteTaskFromList
 * Description: 
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
void DOS_Delay_DeleteTaskFromList(Task_t* pTask)
{
	if(pTask == NULL || pTask->taskNode == NULL)
	{
		return ;
	}
	
	TaskNode_t* pDelNode = pTask->delayNode;
	//DOS_Delay_DeleteNodeFromDelayTimeList(pTask->taskNode);
	DOS_Delay_PopNodeFromDelayTaskList(pDelNode);
	DOS_Task_DelNode(pTask);
}

/********************************************
 * Function: DOS_Delay_AddNodeToDelayTimeList
 * Description: 
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
static void DOS_Delay_AddNodeToDelayTimeList(TaskNode_t* addNode)
{
	//if(addNode->task->delay <= 0)
	if(addNode == NULL)
	{
		//printf("[ERROR] Delay node's delay is <= 0");
		return ;
	}

#if 0
	DOS_TailAddTaskNode(g_DelayTaskList, addNode);
#elif 0
	if(g_DelayTimeList->delayTaskNode == NULL && g_DelayTimeList->delay == 0)
	{
		//The DelayTimeList is NULL, addNode will be the first TaskNode.
		g_DelayTimeList->delay = addNode->task->delay;
		g_DelayTimeList->delayTaskNode = addNode;
	}
	else
	{
		uint32_t delay = addNode->task->delay;
		DelayTimeListNode_t* tmp_delayTimeListNode = g_DelayTimeList;
		while(delay > tmp_delayTimeListNode->delay)
		{
			if(tmp_delayTimeListNode->nextNode != NULL)
			{
				tmp_delayTimeListNode = tmp_delayTimeListNode->nextNode;
			}
			else//tmp_delayTimeListNode reach the last node.
			{
				DelayTimeListNode_t* newDelayListNode = new DelayTimeListNode_t;
				newDelayListNode->delay = (addNode->task->delay) - (tmp_delayTimeListNode->delay);
				newDelayListNode->delayTaskNode = addNode;
				DOS_DelayTimeList_InsertNewNode(tmp_delayTimeListNode, newDelayListNode);
				return ;
			}
		}
		
		if(delay == tmp_delayTimeListNode->delay)//add to current list
		{
			DOS_DelayTaskList_AddTaskNode(tmp_delayTimeListNode, addNode);
		}
		else if(delay < tmp_delayTimeListNode->delay)
		{
			if(tmp_delayTimeListNode == g_DelayTimeList)
			{
				DelayTimeListNode_t* newDelayListNode = new DelayTimeListNode_t;
				newDelayListNode->delay = (addNode->task->delay);
				newDelayListNode->delayTaskNode = addNode;	
				DOS_DelayTimeList_HeadInsertNewNode(newDelayListNode);
			}
			else
			{
				DelayTimeListNode_t* newDelayListNode = new DelayTimeListNode_t;
				newDelayListNode->delay = (addNode->task->delay) - (tmp_delayTimeListNode->preNode->delay);
				newDelayListNode->delayTaskNode = addNode;
				DOS_DelayTimeList_InsertNewNode(tmp_delayTimeListNode->preNode, newDelayListNode);
			}
		}
	}
#else
	//int32_t delay = addNode->task->delay;
	if(g_DelayTimeList->delayTaskNode == NULL && g_DelayTimeList->delay == 0)
	{
		//The DelayTimeList is NULL, addNode will be the first TaskNode.

		g_DelayTimeList->delay = addNode->task->delay;
		g_DelayTimeList->delayTaskNode = addNode;
	}
	else// The DelayTimeList is no NULL
	{
		int32_t delay = (int32_t)addNode->task->delay;
		int32_t accumulateDelay = g_DelayTimeList->delay;
		//int32_t nextDelay = 0;
		DelayTimeListNode_t* tmp_delayTimeListNode = g_DelayTimeList;

		if(delay < accumulateDelay)
		{
			//The new node should be insert in the head of g_DelayTimeList.
			DelayTimeListNode_t* newDelayListNode = new DelayTimeListNode_t;
			newDelayListNode->delay = (addNode->task->delay);
			newDelayListNode->delayTaskNode = addNode;	
			DOS_DelayTimeList_HeadInsertNewNode(newDelayListNode);
		}
		else if(tmp_delayTimeListNode->nextNode == NULL)
		{
			//The g_DelayTimeList only have one node.
			if(delay != accumulateDelay)
			{
				DelayTimeListNode_t* newDelayListNode = new DelayTimeListNode_t;
				newDelayListNode->delay = delay-accumulateDelay;
				newDelayListNode->delayTaskNode = addNode;
				DOS_DelayTimeList_InsertNewNode(tmp_delayTimeListNode, newDelayListNode);
			}
			else if(delay == accumulateDelay)
			{
				DOS_Delay_AddNodeToDelayTaskList(g_DelayTimeList, addNode);
			}
		}
		else
		{
			DelayTimeListNode_t* next_delayTimeListNode = tmp_delayTimeListNode->nextNode;
			int32_t nextDelay = tmp_delayTimeListNode->nextNode->delay;
			
//			while((delay - accumulateDelay) > nextDelay)
//			{
//				accumulateDelay += nextDelay;
//				if(next_delayTimeListNode->nextNode != NULL)
//				{
//					next_delayTimeListNode = next_delayTimeListNode->nextNode;
//					nextDelay = next_delayTimeListNode->delay;
//				}
//				else
//				{
//					//nextDelay = 0;
//					break;
//				}
//			}

			for(; next_delayTimeListNode; next_delayTimeListNode = next_delayTimeListNode->nextNode)
			{
				int32_t diff = delay - accumulateDelay;
				if(diff == 0)
				{
					DOS_Delay_AddNodeToDelayTaskList(next_delayTimeListNode->preNode, addNode);
					break;
				}
				else if(diff == nextDelay)
				{
					DOS_Delay_AddNodeToDelayTaskList(next_delayTimeListNode, addNode);
					break;
				}
				else if(diff < nextDelay)
				{
					if(diff > 0)
					{
						DelayTimeListNode_t* newDelayListNode = new DelayTimeListNode_t;
						newDelayListNode->delay = diff;
						newDelayListNode->delayTaskNode = addNode;
						DOS_DelayTimeList_InsertNewNode(next_delayTimeListNode->preNode, newDelayListNode);
						break;
					}
					else if((delay - (accumulateDelay+nextDelay)) < 0)
					{
						DelayTimeListNode_t* newDelayListNode = new DelayTimeListNode_t;
						newDelayListNode->delay = diff;
						newDelayListNode->delayTaskNode = addNode;
						DOS_DelayTimeList_InsertNewNode(next_delayTimeListNode->preNode, newDelayListNode);
						break;
					}
					break;
				}
				else if(diff > nextDelay)
				{
					accumulateDelay += nextDelay;
					if(next_delayTimeListNode->nextNode != NULL)
					{
						nextDelay = next_delayTimeListNode->nextNode->delay;
						continue;
					}
					else// Reach the last node of the DelayTimeList
					{
						DelayTimeListNode_t* newDelayListNode = new DelayTimeListNode_t;
						newDelayListNode->delay = diff - nextDelay;
						newDelayListNode->delayTaskNode = addNode;
						DOS_DelayTimeList_InsertNewNode(next_delayTimeListNode, newDelayListNode);
						break;
					}
				}
			}
		}
	}

#endif
}


/********************************************
 * Function: DOS_Delay_DeleteNodeFromDelayTimeList
 * Description: No used
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
#if 0
static TaskNode_t* DOS_Delay_DeleteNodeFromDelayTimeList(DelayTimeListNode_t* pPopNode)
{
	if(pPopNode == NULL)
	{
		return NULL;
	}

	TaskNode_t* pTaskListHeadNode = pPopNode->delayTaskNode;

	if(pPopNode->preNode == NULL && pPopNode->nextNode == NULL)
	{
		// The DelayTimeList only have one node
		g_DelayTimeList->delayTaskNode = NULL;
		g_DelayTimeList->delay = 0;
	}
	else if(pPopNode->preNode == NULL && pPopNode->nextNode != NULL)
	{
		// The node is the first node of the DelayTimeList
		g_DelayTimeList->nextNode = pPopNode->nextNode;
		pPopNode->nextNode->preNode = g_DelayTimeList;
		delete pPopNode;
	}
	else if(pPopNode->preNode != NULL && pPopNode->nextNode == NULL)
	{
		// The node is the last node of the DelayTimeList
		pPopNode->preNode->nextNode = pPopNode->nextNode;
		pPopNode->nextNode = NULL;
		pPopNode->preNode = NULL;
		delete pPopNode;
	}
	else if(pPopNode->preNode != NULL && pPopNode->nextNode != NULL)
	{
		// The node is the middle one of the DelayTimeList
		pPopNode->preNode->nextNode = pPopNode->nextNode;
		pPopNode->nextNode->preNode = pPopNode->preNode;
		pPopNode->preNode = NULL;
		pPopNode->nextNode = NULL;
		delete pPopNode;
	}

	return pTaskListHeadNode;
}
#endif

/********************************************
 * Function: DOS_Delay_AddNodeToDelayTaskList
 * Description: 
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
static void DOS_Delay_AddNodeToDelayTaskList(DelayTimeListNode_t* pDelayList, TaskNode_t* pTaskNode)
{
	if(pDelayList == NULL || pTaskNode == NULL)
	{
		return ;
	}

	TaskNode_t* pTailNode = pDelayList->delayTaskNode;

	while(pTailNode->nextNode)
	{
		pTailNode = pTailNode->nextNode;
	}

	pTaskNode->nextNode = pTailNode->nextNode;
	pTaskNode->preNode  = pTailNode;
	pTailNode->nextNode = pTaskNode;
}

/********************************************
 * Function: DOS_Delay_PopNodeFromDelayTaskList
 * Description: 
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
static TaskNode_t* DOS_Delay_PopNodeFromDelayTaskList(TaskNode_t* popNode)
{
	if(popNode->task->delay > 0)
	{
		printf("[ERROR] Can not pop node whose delay is > 0");
		return NULL;
	}

#if 0
	return DOS_PopTaskNode(popNode);
#else
	if(popNode->preNode != NULL && popNode->nextNode != NULL)
	{
		popNode->preNode->nextNode = popNode->nextNode;
		popNode->nextNode->preNode = popNode->preNode;
		popNode->nextNode = NULL;
		popNode->preNode  = NULL;
	}
	else if(popNode->preNode != NULL && popNode->nextNode == NULL)
	{
		popNode->preNode->nextNode = popNode->nextNode;
		popNode->nextNode = NULL;
		popNode->preNode  = NULL;
	}
	else if(popNode->preNode == NULL && popNode->nextNode != NULL)
	{
		popNode->nextNode->preNode = popNode->preNode;
		popNode->nextNode = NULL;
		popNode->preNode  = NULL;
	}
	else
	{
		//popNode is free already.
	}
	return popNode;
#endif
}

/********************************************
 * Function: DOS_Delay_DeleteFirstNodeOfDelayTimeList
 * Description: An user interface for delete a timeout delay task
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
TaskNode_t* DOS_Delay_DeleteFirstNodeOfDelayTimeList()
{
	TaskNode_t* pFirstTaskNode = g_DelayTimeList->delayTaskNode;
	DelayTimeListNode_t* pDelNode = g_DelayTimeList;
	DOS_DelayTimeList_SetAllTaskDelayToZero(pDelNode);
	if(g_DelayTimeList->nextNode != NULL)
	{
		g_DelayTimeList = g_DelayTimeList->nextNode;
		delete pDelNode;
	}
	else// The DelayTimeList only have one node.
	{
		g_DelayTimeList->delayTaskNode = NULL;
		g_DelayTimeList->delay = 0;
	}
	
	return pFirstTaskNode;
}


/********************************************
 * Function: DOS_DelayTimeList_InsertNewNodeBeforeNode
 * Description: An interal interface for
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
static void DOS_DelayTimeList_InsertNewNodeBeforeNode(DelayTimeListNode_t* pNextNode, DelayTimeListNode_t* pNewNode)
{
	if(pNextNode == NULL || pNewNode == NULL)
	{
		return ;
	}
	if(pNewNode->delay > pNextNode->delay)
	{
		return ;
	}

	pNewNode->preNode = pNextNode->preNode;
	pNewNode->nextNode = pNextNode;
	pNewNode->preNode->nextNode = pNewNode;
	pNewNode->nextNode->preNode = pNewNode;
	pNextNode->delay -= pNewNode->delay; 
}


/********************************************
 * Function: DOS_DleayTimeList_InsertNewNodeAfterNode
 * Description: An interal interface for
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
static void DOS_DleayTimeList_InsertNewNodeAfterNode(DelayTimeListNode_t* pPreNode, DelayTimeListNode_t* pNewNode)
{
	if(pPreNode == NULL || pNewNode == NULL)
	{
		return ;
	}
	if(pPreNode->nextNode == NULL)
	{
		pNewNode->nextNode = pPreNode->nextNode;
		pNewNode->preNode  = pPreNode;
		pNewNode->preNode->nextNode = pNewNode;
	}
	else
	{
		DOS_DelayTimeList_InsertNewNodeBeforeNode(pPreNode->nextNode, pNewNode);
	}
}


/********************************************
 * Function: DOS_DelayTimeList_InsertNewNode
 * Description: An interal interface for
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
static void DOS_DelayTimeList_InsertNewNode(DelayTimeListNode_t* pInsListNode, DelayTimeListNode_t* pNewListNode)
{
	if(pInsListNode == NULL || pNewListNode == NULL)
	{
		return ;
	}

	DOS_DleayTimeList_InsertNewNodeAfterNode(pInsListNode, pNewListNode);
}


/********************************************
 * Function: 
 * Description: An interal interface for
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
static void DOS_DelayTimeList_SetAllTaskDelayToZero(DelayTimeListNode_t* pDelayTimeNode)
{
	if(pDelayTimeNode == NULL)
	{
		return ;
	}

	TaskNode_t* pTask = pDelayTimeNode->delayTaskNode;

	while(pTask)
	{
		pTask->task->delay = 0;
		pTask = pTask->nextNode;
	}
}


/********************************************
 * Function: DOS_DelayTimeList_HeadInsertNewNode
 * Description: An interal interface for
 * Input: 
 * Output:
 * Return:
 * Author: Koen
 *******************************************/
static void DOS_DelayTimeList_HeadInsertNewNode(DelayTimeListNode_t* pNewNode)
{
	if(pNewNode == NULL)
	{
		return ;
	}
	g_DelayTimeList->delay -= pNewNode->delay;
	pNewNode->nextNode = g_DelayTimeList;
	g_DelayTimeList = pNewNode;
}

/**********************END**********************/

