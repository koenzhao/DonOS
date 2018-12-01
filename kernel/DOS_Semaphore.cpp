/******************************************************
 * File Name:   DOS_Semaphore.cpp
 * File Date:   2018.08.06
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.06 | Create   |  Koen  |
 *****************************************************/

#include "DOS_Semaphore.h"
#include "DOS_Interrupt.h"


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Sem_Init(Sem_t* sem, uint32_t initCount, uint32_t maxCount)
{
	if(sem == NULL)
	{
		return ;
	}

	DOS_Event_Init(&sem->event, EVENT_TYPE_SEM);

	sem->maxCount = maxCount;

	if(maxCount == 0)//No Limit
	{
		sem->count = initCount;
	}
	else
	{
		sem->count = (initCount > maxCount) ? maxCount : initCount;
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
void DOS_Sem_Wait_Block(Sem_t* sem, uint32_t timeout)
{
	DOS_DisableIrq();

	if(sem == NULL)
	{
		DOS_EnableIrq();
		return ;
	}

	//Task_t* pTask = g_currentTask;

	if(sem->count > 0)
	{
		--sem->count;
		DOS_EnableIrq();
		return ;
	}
	else
	{
		DOS_Event_Wait(&sem->event, (void *)0, timeout);
		DOS_EnableIrq();
		DOS_TaskSchedule();
		return ;
	}

	//DOS_EnableIrq();
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Sem_Wait_Unblock(Sem_t* sem)
{
	DOS_DisableIrq();

	if(sem == NULL)
	{
		DOS_EnableIrq();
		return ;
	}

	//Task_t* pTask = g_currentTask;

	if(sem->count > 0)
	{
		--sem->count;
		DOS_EnableIrq();
		return ;
	}
	else
	{
		//DOS_Event_Wait(sem->event, (void *)0, timeout);
		DOS_EnableIrq();
		//DOS_TaskSchedule();
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
void DOS_Sem_Notify(Sem_t* sem)
{
	DOS_DisableIrq();

	if(sem->event.eventWaitList != NULL)
	{
		Task_t* pTask = DOS_Event_Notify(&sem->event, (void *)NULL);

		if(pTask->priority < g_currentTask->priority)
		{
			DOS_TaskSchedule();
		}
	}
	else
	{
		++sem->count;
		if((sem->count != 0) && (sem->count > sem->maxCount))
		{
			sem->count = sem->maxCount;
		}
	}

	DOS_EnableIrq();
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_Sem_Destory(Sem_t* sem)
{
	DOS_DisableIrq();
	uint32_t count = DOS_Event_RemoveAll(&sem->event);
	DOS_EnableIrq();

	if(count > 0)
	{
		DOS_TaskSchedule();
	}
}

