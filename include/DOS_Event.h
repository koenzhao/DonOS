#ifndef _DOS_EVENT_H_
#define _DOS_EVENT_H_

#include "DOS_Task.h"

typedef enum _DOS_Event_Return_t
{
	EVENT_FAILIRE = 0,
	EVENT_SUCCESS = 1,
	EVENT_TIMEOUT = 2,
	EVENT_ERROR   = 3,
} EventReturn_t;

typedef enum _DOS_EventType_t
{
	EVENT_TYPE_UNUSED = 0,
	EVENT_TYPE_SEM,
	EVENT_TYPE_MBOX,
	EVENT_TYPE_MEMBLOCK,
	EVENT_TYPE_FLAGGROUP,
	EVENT_TYPE_MUTEX,
	EVENT_TYPE_SUM
} EventType_t;

typedef struct _DOS_EventControlBlock_t
{	
	EventType_t event;
	TaskNode_t* eventWaitList;
} Event_t;

static void           DOS_Event_AddTaskToList(Event_t* pEvent, Task_t* pTask);

extern void           DOS_Event_Init(Event_t* event, EventType_t type);
extern void           DOS_Event_Wait(Event_t* event, void* msg, uint32_t timeout);
extern Task_t*        DOS_Event_Notify(Event_t* event, void* msg);
extern EventReturn_t  DOS_Event_Notify(Event_t* event, void* msg, Task_t* pTask);
extern void           DOS_Event_DeleteTaskFromList(Task_t* pTask);
extern void           DOS_Event_Timeout(Task_t* pTask);
extern uint32_t       DOS_Event_RemoveAll(Event_t* event);

#endif
