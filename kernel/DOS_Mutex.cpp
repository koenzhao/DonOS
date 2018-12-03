/******************************************************
 * File Name:   DOS_Mutex.cpp
 * File Date:   2018.08.25
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.25 | Create   |  Koen  |
 *****************************************************/

#include "DOS_Mutex.h"

Mutex_Return_t DOS_Mutex_Init(Mutex_t* mutex)
{
	if(mutex == NULL)
	{
		return MUTEX_ERROR;	
	}
	
	DOS_Event_Init(&mutex->event, EVENT_TYPE_MUTEX);

	mutex->lockedCount       = 0;
	mutex->owner             = (Task_t*)0;
	mutex->ownerOriginalPrio = TASK_PRIORITY;

	return MUTEX_SUCCESS;
}

Mutex_Return_t DOS_Mutex_Wait(Mutex_t* mutex, uint32_t waitTick)
{
	DOS_DisableIrq();

	if(mutex == NULL)
	{
		return MUTEX_ERROR;
	}
	
	if(mutex->lockedCount <= 0)// No task use this mutex now
	{
		mutex->owner = g_currentTask;
		mutex->ownerOriginalPrio = g_currentTask->priority;
		mutex->lockedCount++;

		DOS_EnableIrq();
		return MUTEX_SUCCESS;
	}
	else // Some tasks use this mutex now
	{
		if(mutex->owner == g_currentTask)// The same task wait this mutex again
		{
			mutex->lockedCount++;

			DOS_EnableIrq();
			return MUTEX_SUCCESS;
		}
		else // Other task wait for this mutex 
		{
			if(g_currentTask->priority < mutex->owner->priority)// A task with higher priority
			{
				Task_t* pTask = mutex->owner;
				//Priority inherit
				if(pTask->state == READY_STATE || pTask->state == RUNNING_STATE)
				{
					DOS_Ready_DeleteTaskFromList(pTask);
					pTask->priority = g_currentTask->priority;
					DOS_Task_ReadyTask(pTask);
				}
				else
				{
					pTask->priority = g_currentTask->priority;
				}
			}
			else
			{
				//do nothing
			}

			DOS_Event_Wait(&mutex->event, (void *)0, waitTick);
			DOS_EnableIrq();
			DOS_TaskSchedule();
			return (Mutex_Return_t)g_currentTask->waitEventResult;
		}
	}
}

Mutex_Return_t DOS_Mutex_Wait_NoBlock(Mutex_t* mutex)
{
	DOS_DisableIrq();

	if(mutex == NULL)
	{
		return MUTEX_ERROR;
	}

	if(mutex->lockedCount <= 0)
	{
		mutex->owner = g_currentTask;
		mutex->ownerOriginalPrio = g_currentTask->priority;
		mutex->lockedCount++;
		DOS_EnableIrq();
		return MUTEX_SUCCESS;
		
	}
	else
	{
		if(mutex->owner == g_currentTask)
		{
			mutex->lockedCount++;
			DOS_EnableIrq();
			return MUTEX_SUCCESS;
		}

		DOS_EnableIrq();
		return MUTEX_FAILURE;
	}
}

Mutex_Return_t DOS_Mutex_Notify(Mutex_t* mutex)
{
	DOS_DisableIrq();

	if(mutex == NULL)
	{
		return MUTEX_ERROR;
	}

	if(mutex->lockedCount <= 0)// No task use this mutex
	{
		DOS_EnableIrq();
		return MUTEX_FAILURE;
	}

	if(mutex->owner != g_currentTask)// No the owner task notify! Illegal!
	{
		DOS_EnableIrq();
		return MUTEX_ERROR;
	}

	if(--mutex->lockedCount > 0)// After release, lockedcount still greater than 0!
	{
		DOS_EnableIrq();
		return MUTEX_SUCCESS;
	}

	if(mutex->owner->priority != mutex->ownerOriginalPrio)// Priority inherit happened!
	{	
		Task_t* pTask = mutex->owner;
		if(pTask->state == READY_STATE || pTask->state == RUNNING_STATE)
		{
			DOS_Ready_DeleteTaskFromList(pTask);
			pTask->priority = mutex->ownerOriginalPrio;
			DOS_Task_ReadyTask(pTask);
		}
		else
		{
			pTask->priority = mutex->ownerOriginalPrio;
		}
	}

	if(mutex->event.eventWaitList)// Mutex is 0, and there is a new owner task!
	{
		Task_t* pTask = DOS_Event_Notify(&mutex->event, (void *) 0);

		mutex->owner = pTask;
		mutex->ownerOriginalPrio = pTask->priority;
		mutex->lockedCount++;

		if(pTask->priority < g_currentTask->priority)
		{
			DOS_TaskSchedule();
		}
	}

	DOS_EnableIrq();
	return MUTEX_SUCCESS;
}

Mutex_Return_t DOS_Mutex_Destory(Mutex_t* mutex)
{
	DOS_DisableIrq();

	if(mutex->lockedCount > 0)
	{
		if(mutex->owner->priority != mutex->ownerOriginalPrio)//Proirity inherit heppened
		{
			Task_t* pTask = mutex->owner;
			if(pTask->state == READY_STATE || pTask->state == RUNNING_STATE)
			{
				DOS_Ready_DeleteTaskFromList(pTask);
				pTask->priority = mutex->ownerOriginalPrio;
				DOS_Task_ReadyTask(pTask);
			}
			else
			{
				pTask->priority = mutex->ownerOriginalPrio;
			}
		}

		uint32_t count = DOS_Event_RemoveAll(&mutex->event);
		if(count)
		{
			DOS_TaskSchedule();
		}
	}

	DOS_EnableIrq();
	return MUTEX_SUCCESS;
}



