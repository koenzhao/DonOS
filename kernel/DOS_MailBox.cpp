/******************************************************
 * File Name:   DOS_MailBox.cpp
 * File Date:   2018.08.13
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.13 | Create   |  Koen  |
 *****************************************************/

#include "DOS_MailBox.h"


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_MailBox_Init(Mbox_t* mbox, void** msgBuffer, uint32_t maxCount)
{
	if(mbox == NULL)
	{
		return ;
	}

	DOS_Event_Init(&mbox->event, EVENT_TYPE_MBOX);

	mbox->count     = 0;
	mbox->maxCount  = maxCount;
	mbox->readIdx   = 0;
	mbox->writeIdx  = 0;
	mbox->msgBuffer = msgBuffer;
}


/********************************************
 * Function: 
 * Description: 
 * Input: 
 * Output: 
 * Return: 
 * Author: Koen
 *******************************************/
void DOS_MailBox_Wait_Block(Mbox_t* mbox, void** msg, uint32_t waitTick)
{
	DOS_DisableIrq();

	if(mbox == NULL)
	{
		return ;
	}

	if(mbox->count > 0) // There are msgs in MailBox
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->readIdx++];

		if(mbox->readIdx >= mbox->maxCount)
		{
			mbox->readIdx = 0;
		}

		DOS_EnableIrq();
		return ;
	}
	else // There are no msgs in MailBox
	{
		DOS_Event_Wait(&mbox->event, (void *)0, waitTick);
		DOS_EnableIrq();

		DOS_TaskSchedule();

		*msg = g_currentTask->eventMsg;

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
void DOS_MailBox_Wait_Unblock(Mbox_t* mbox, void** msg)
{
	DOS_DisableIrq();

	if(mbox == NULL)
	{
		return ;
	}

	if(mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->readIdx++];

		if(mbox->readIdx >= mbox->maxCount)
		{
			mbox->readIdx = 0;
		}

		DOS_EnableIrq();
		return ;
	}
	else
	{
		DOS_EnableIrq();
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
void DOS_MailBox_Notify(Mbox_t* mbox, void* msg, uint32_t notifyOption)
{
	DOS_DisableIrq();

	if(mbox->event.eventWaitList != NULL) // Tasks waitting for msg
	{
		Task_t* pTask = DOS_Event_Notify(&mbox->event, (void *)msg);

		if(pTask->priority < g_currentTask->priority)
		{
			DOS_TaskSchedule();
		}
	}
	else // No task in EventWaitList
	{
		if(mbox->count >= mbox->maxCount)// msgBuffer is full
		{
			DOS_EnableIrq();
			return ;
		}

		if(notifyOption)
		{
			if(mbox->readIdx <= 0)
			{
				mbox->readIdx = mbox->maxCount - 1;
			}
			else
			{
				--mbox->readIdx;
			}

			mbox->msgBuffer[mbox->readIdx] = msg;
		}
		else
		{
			mbox->msgBuffer[mbox->writeIdx++] = msg;
			if(mbox->writeIdx >= mbox->maxCount)
			{
				mbox->writeIdx = 0;
			}
		}

		mbox->count++;
	}

	DOS_EnableIrq();
	return ;
}


void DOS_MailBox_ClearMsgs(Mbox_t* mbox)
{
	DOS_DisableIrq();
	
	if(mbox == NULL)
	{
		return ;
	}

	if(mbox->event.eventWaitList == NULL)
	{
		mbox->readIdx  = 0;
		mbox->writeIdx = 0;
		mbox->count    = 0;
	}
	else
	{
		//MsgBuffer is ampty
	}

	DOS_EnableIrq();
}

void DOS_MailBox_Delete(Mbox_t* mbox)
{
	DOS_DisableIrq();

	uint32_t count = DOS_Event_RemoveAll(&mbox->event);

	if(count > 0)
	{
		DOS_TaskSchedule();
	}

	DOS_EnableIrq();
}

