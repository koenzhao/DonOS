#ifndef _DOS_DELAY_H_
#define _DOS_DELAY_H_

#include "DOS_CommonDefine.h"
#include "DOS_Task.h"
#include "DOS_LinkedList.h"

typedef struct _DOS_DelayTimeListNode_t
{
	uint32_t delay;
	_DOS_DelayTimeListNode_t* preNode;
	_DOS_DelayTimeListNode_t* nextNode;
	TaskNode_t* delayTaskNode;
} DelayTimeListNode_t;

extern DelayTimeListNode_t* g_DelayTimeList;

static void        DOS_Task_DelayTask(Task_t* pTask);
//static void        DOS_Delay_AddTaskToList(Task_t* pTask);
//static void        DOS_Delay_DeleteTaskFromList(Task_t* pTask);
static void        DOS_Delay_AddNodeToDelayTimeList(TaskNode_t* addNode);
//static TaskNode_t* DOS_Delay_DeleteNodeFromDelayTimeList(DelayTimeListNode_t* pPopNode);
static void        DOS_Delay_AddNodeToDelayTaskList(DelayTimeListNode_t* pDelayList, TaskNode_t* pTaskNode);
static TaskNode_t* DOS_Delay_PopNodeFromDelayTaskList(TaskNode_t* popNode);
static void        DOS_DelayTimeList_InsertNewNodeBeforeNode(DelayTimeListNode_t* pNextNode, DelayTimeListNode_t* pNewNode);
static void        DOS_DleayTimeList_InsertNewNodeAfterNode(DelayTimeListNode_t* pPreNode, DelayTimeListNode_t* pNewNode);
static void        DOS_DelayTimeList_InsertNewNode(DelayTimeListNode_t* pInsListNode, DelayTimeListNode_t* pNewListNode);
static void        DOS_DelayTimeList_SetAllTaskDelayToZero(DelayTimeListNode_t* pDelayTimeNode);
static void        DOS_DelayTimeList_HeadInsertNewNode(DelayTimeListNode_t* pNewNode);


extern void        DOS_BlockDelay(int32_t count);
extern void        DOS_Delay(uint32_t ms);
extern TaskNode_t* DOS_Delay_DeleteFirstNodeOfDelayTimeList();
extern void        DOS_Delay_DeleteTaskFromList(Task_t* pTask);
extern void        DOS_Delay_AddTaskToList(Task_t* pTask);





#endif
