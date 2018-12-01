/******************************************************
 * File Name:   DOS_FlagsGroup.cpp
 * File Date:   2018.08.21
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.21 | Create   |  Koen  |
 *****************************************************/

#include "DOS_FlagsGroup.h"

void DOS_FlagGroup_Init(FlgGrp_t* flagGrp, uint32_t flags)
{
	if(flagGrp == NULL)
	{
		return ;
	}

	DOS_Event_Init(&flagGrp->event, EVENT_TYPE_FLAGGROUP);
	flagGrp->flags = flags;
}

FlgGrp_Return_t DOS_FlagGroup_CheckAndConsume(FlgGrp_t* flagGrp, uint32_t type, uint32_t* pFlag)
{
	if(flagGrp == NULL)
	{
		return FLAGGROUP_ERROR;
	}
	uint32_t srcFlag   = *pFlag;
	uint32_t isSet     = type & FLAGGROUP_SET;
	uint32_t isAll     = type & FLAGGROUP_ALL;
	uint32_t isConsume = type & FLAGGROUP_CONSUME;

	uint32_t calcFlag = isSet ? (flagGrp->flags & srcFlag):(~flagGrp->flags & srcFlag);

	if( isAll )
	{
		if(calcFlag == srcFlag)
		{
			if(isConsume)
			{
				if(isSet)
				{
					flagGrp->flags &= ~srcFlag;
				}
				else
				{
					flagGrp->flags |= srcFlag;
				}
			}
			
			*pFlag = calcFlag;
			return FLAGGROUP_SUCCESS;
		}
	}
	else
	{
		if(calcFlag)
		{
			if(isConsume)
			{
				if(isSet)
				{
					flagGrp->flags &= ~calcFlag;
				}
				else
				{
					flagGrp->flags |= calcFlag;
				}
			}
			
			*pFlag = calcFlag;
			return FLAGGROUP_SUCCESS;
		}
	}
	*pFlag = calcFlag;
	return FLAGGROUP_FAILURE;
}

FlgGrp_Return_t DOS_FlagGroup_Wait_Block(FlgGrp_t* flagGrp, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag, uint32_t waitTick)
{
	//uint32_t result = 0;
	uint32_t flag = requestFlag;
	
	DOS_DisableIrq();

	if(flagGrp == NULL)
	{
		return FLAGGROUP_ERROR;
	}

	if(FLAGGROUP_FAILURE == DOS_FlagGroup_CheckAndConsume(flagGrp, waitType, &flag))
	{
		g_currentTask->waitFlagType = waitType;
		g_currentTask->eventflags   = requestFlag;
		DOS_Event_Wait(&flagGrp->event, (void *)0, waitTick);
		DOS_EnableIrq();
		DOS_TaskSchedule();

		*resultFlag = g_currentTask->eventflags;
		return FLAGGROUP_SUCCESS;
	}
	else
	{
		*resultFlag = flag;
		DOS_EnableIrq();
		return FLAGGROUP_SUCCESS;
	}
}

FlgGrp_Return_t DOS_FlagGroup_Wait_NoBlock(FlgGrp_t* flagGrp, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag)
{
	uint32_t flag = requestFlag;

	DOS_DisableIrq();
	DOS_FlagGroup_CheckAndConsume(flagGrp, waitType, &flag);
	DOS_EnableIrq();

	*resultFlag = flag;
	return FLAGGROUP_SUCCESS;
}

FlgGrp_Return_t DOS_FlagGroup_Notify(FlgGrp_t* flagGrp, uint8_t isSet, uint32_t flag)
{
	uint8_t isSched = 0;

	DOS_DisableIrq();

	if(isSet)
	{
		flagGrp->flags |= flag;
	}
	else
	{
		flagGrp->flags &= ~flag;
	}

	TaskNode_t* pTaskNode = flagGrp->event.eventWaitList;
	while(pTaskNode)
	{
		FlgGrp_Return_t result = FLAGGROUP_SUCCESS;
		Task_t* pTask = pTaskNode->task;
		uint32_t flag = pTask->eventflags;
		result = DOS_FlagGroup_CheckAndConsume(flagGrp, pTask->waitFlagType, &flag);
		if(FLAGGROUP_SUCCESS == result)
		{
			pTask->eventflags = flag;
			DOS_Event_Notify(&flagGrp->event, (void *) 0, pTask);
			if(pTask->priority < g_currentTask->priority)
			{
				isSched = 1;
			}
		}
	}

	if(isSched)
	{
		DOS_TaskSchedule();
	}

	DOS_EnableIrq();
	
	return FLAGGROUP_SUCCESS;
}

uint32_t DOS_FlagGroup_Destory(FlgGrp_t* flagGrp)
{
	DOS_DisableIrq();

	uint32_t count = DOS_Event_RemoveAll(&flagGrp->event);

	DOS_EnableIrq();

	if(count > 0)
	{
		DOS_TaskSchedule();
	}

	return count;
}


