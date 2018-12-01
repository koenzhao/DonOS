/******************************************************
 * File Name:   DOS_Mutex.h
 * File Date:   2018.08.25
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.25 | Create   |  Koen  |
 *****************************************************/

#ifndef _DOS_MUTEX_H_
#define _DOS_MUTEX_H_

#include "DOS_CommonDefine.h"
#include "DOS_Interrupt.h"
#include "DOS_Event.h"

typedef struct _DOS_Mutex_t
{
	Event_t  event;
	uint32_t lockedCount;
	Task_t*  owner;
	uint32_t ownerOriginalPrio;
} Mutex_t;

typedef enum
{
	MUTEX_FAILURE = 0,
	MUTEX_SUCCESS = 1,
	MUTEX_ERROR   = 2,
} Mutex_Return_t;

extern Mutex_Return_t DOS_Mutex_Init(Mutex_t* mutex);
extern Mutex_Return_t DOS_Mutex_Wait(Mutex_t* mutex, uint32_t waitTick);
extern Mutex_Return_t DOS_Mutex_Wait_NoBlock(Mutex_t* mutex);
extern Mutex_Return_t DOS_Mutex_Notify(Mutex_t* mutex);
extern Mutex_Return_t DOS_Mutex_Destory(Mutex_t* mutex);

#endif

