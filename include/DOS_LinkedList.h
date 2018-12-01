#ifndef _DOS_LINKED_H_
#define _DOS_LINKED_H_

#include "DOS_Task.h"

extern void        DOS_InsertTaskNode(TaskNode_t* currNode, TaskNode_t* newNode);
extern void        DOS_DeleteTaskNode(TaskNode_t* delNode);
extern void        DOS_HeadAddTaskNode(TaskNode_t* headNode, TaskNode_t* newNode);
extern void        DOS_TailAddTaskNode(TaskNode_t* headNode, TaskNode_t* newNode);
extern TaskNode_t* DOS_ReplaceTaskNode(TaskNode_t* oriNode, TaskNode_t* newNode);
extern TaskNode_t* DOS_PopTaskNode(TaskNode_t* popNode);
extern int32_t     DOS_GetListLength(TaskNode_t* headNode);


#endif
