#ifndef _DOS_TASK_H
#define _DOS_TASK_H

#include "DOS_CommonDefine.h"
#include "DOS_List.h"

typedef uint32_t TaskStack_t;
enum _DOS_EventType_t;
struct _DOS_EventControlBlock_t;
enum _DOS_Event_Result_t;

enum TaskName
{
	IDLE = 0,
};

enum TaskPriority_t
{
	TASK_TIMER_PRIO = 1,
	TASK_TEST_1_PRIO = 2,
	TASK_TEST_2_PRIO = 10,
	TASK_TEST_3_PRIO = 10,
	TASK_IDLE_PRIO = 31,
	TASK_PRIORITY
};

enum TaskStatus_t
{
	INITIAL_STATE = 0x00,
	FREE_STATE    = 0x01,
	READY_STATE   = 0x02,
	RUNNING_STATE = 0x04,
	DELAY_STATE   = 0x08,
	SUSPEND_STATE = 0x10,
	SLEEP_STATE   = 0x20,
	DELETE_STATE  = 0x40,
	WAIT_STATE    = 0x80
};

typedef struct _TaskControlBlock_t
{
	TaskStack_t*               stack;
	uint32_t                   delay;
	TaskPriority_t             priority;
	TaskStatus_t               state;
	struct _TaskNode_t*        taskNode;
	struct _TaskNode_t*        delayNode;
	uint32_t                   slice;
	uint32_t                   sliceReload;
	_DOS_EventControlBlock_t*  waitEvent;
	void*                      eventMsg;
	uint32_t                   waitEventResult;
	uint32_t                   waitFlagType;
	uint32_t                   eventflags;
} Task_t;

typedef struct _TaskNode_t
{
	struct _TaskNode_t* preNode;
	Task_t*             task;
	struct _TaskNode_t* nextNode;
} TaskNode_t;

typedef struct _ReadyListNode_t
{
	//bool        isExist;
	TaskNode_t* headNode;
	TaskNode_t* lastNode;
} ReadyListNode_t;

extern Task_t*    g_nextTask;
extern Task_t*    g_currentTask;
extern uint32_t        taskSum;
extern List_t*     g_ReadyTaskList[TASK_PRIORITY];

static void        DOS_Ready_AddTaskToList(Task_t* pTask);
static void        DOS_Ready_DeleteTaskFromList(Task_t* pTask);


extern void        DOS_CreateTask(Task_t* task, 
									   void (*entry)(void *), 
									   void* param, 
									   TaskPriority_t priority,
									   uint32_t slice,
									   uint32_t* stack);
extern void        DOS_TaskSchedule(void);
extern void        DOS_TaskSwitch(void);
extern void        DOS_TaskStart(void);
extern void        DOS_Task_ReadyTask(Task_t* pTask);
extern void        DOS_Task_FreeTask(Task_t* pTask);
extern void        DOS_Task_DeleteTask(Task_t* task);
extern Task_t*     DOS_GetIdleTask();
extern TaskNode_t* DOS_Task_GetNode(Task_t* pTask);
extern void        DOS_Task_DelNode(Task_t* pTask);
extern TaskNode_t* DOS_Task2TaskNode(Task_t* pTask);
extern Task_t*     DOS_TaskNode2Task(TaskNode_t* pTaskNode);

extern void        DOS_TaskNode_AddNodeToList(TaskNode_t** ppHeadNode, TaskNode_t* addNode);
extern void        DOS_TaskNode_InsertNodeAfterNode(TaskNode_t* pPreNode, TaskNode_t* pInsNode);
extern void        DOS_TaskNode_InsertNodeBeforeNode(TaskNode_t* pNextNode, TaskNode_t* pInsNode);
extern void        DOS_TaskNode_CopyNode(TaskNode_t* pDstNode, TaskNode_t* pSrcNode);
extern void        DOS_TaskNode_DeleteNode(TaskNode_t* pDelNode);
extern TaskNode_t* DOS_TaskNode_PopFirstNode(TaskNode_t** ppHeadNode);
extern TaskNode_t* DOS_TaskNode_PopNode(TaskNode_t* pPopNode);




















#endif 

