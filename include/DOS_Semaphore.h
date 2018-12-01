/******************************************************
 * File Name:   DOS_Semaphore.cpp
 * File Date:   2018.08.06
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.06 | Create   |  Koen  |
 *****************************************************/

#ifndef _DOS_SEMAPHORE_H_
#define _DOS_SEMAPHORE_H_

#include "DOS_Event.h"
#include "DOS_CommonDefine.h"

typedef struct _DOS_Semaphore_t
{
	Event_t  event;
	uint32_t count;
	uint32_t maxCount;
} Sem_t;

extern void DOS_Sem_Init(Sem_t* sem, uint32_t initCount, uint32_t maxCount);
extern void DOS_Sem_Wait_Block(Sem_t* sem, uint32_t timeout);
extern void DOS_Sem_Wait_Unblock(Sem_t* sem);
extern void DOS_Sem_Notify(Sem_t* sem);
extern void DOS_Sem_Destory(Sem_t* sem);



#endif

