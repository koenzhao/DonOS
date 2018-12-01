#include "DOS_LinkedList.h"

void DOS_InsertTaskNode(TaskNode_t* currNode, TaskNode_t* newNode)
{
	if(currNode == NULL || newNode == NULL)
	{
		return ;
	}
	
	newNode->nextNode = currNode->nextNode;
	newNode->preNode = currNode;
	currNode->nextNode = newNode;
	newNode->nextNode->preNode = newNode;
}

void DOS_DeleteTaskNode(TaskNode_t* delNode)
{
	if(delNode == NULL)
	{
		return ;
	}
	
	delNode->preNode->nextNode = delNode->nextNode;
	delNode->nextNode->preNode = delNode->preNode;
	delNode->nextNode = NULL;
	delNode->preNode  = NULL;
	
	delete delNode;
}

TaskNode_t* DOS_PopTaskNode(TaskNode_t* popNode)
{
	if(popNode == NULL)
	{
		return NULL;
	}

#if 1	
	popNode->preNode->nextNode = popNode->nextNode;
	popNode->nextNode->preNode = popNode->preNode;
	popNode->nextNode = NULL;
	popNode->preNode  = NULL;
#else
	if(popNode->preNode != NULL)
	{
		popNode->preNode->nextNode = popNode->nextNode;
	}
	else if(popNode->nextNode != NULL)
	{
		popNode->nextNode->preNode = popNode->preNode;
	}
	popNode->nextNode = NULL;
	popNode->preNode  = NULL;
#endif	
	
	return popNode;
}

void DOS_HeadAddTaskNode(TaskNode_t* headNode, TaskNode_t* newNode)
{
	if(headNode == NULL || newNode == NULL)
	{
		return ;
	}
	
	newNode->nextNode = headNode->nextNode;
	newNode->preNode = headNode;
	headNode->nextNode = newNode;
	newNode->nextNode->preNode = newNode;
}

void DOS_TailAddTaskNode(TaskNode_t* headNode, TaskNode_t* newNode)
{
	if(headNode == NULL || newNode == NULL)
	{
		return ;
	}
	
	TaskNode_t* lastNode = headNode;
	for(; lastNode->nextNode; lastNode = lastNode->nextNode)
	{
		;
	}
	newNode->nextNode = lastNode->nextNode;
	newNode->preNode = lastNode;
	lastNode->nextNode = newNode;
}

TaskNode_t* DOS_ReplaceTaskNode(TaskNode_t* oriNode, TaskNode_t* newNode)
{
	if(oriNode == NULL || newNode == NULL)
	{
		return NULL;
	}

	newNode->preNode  = oriNode->preNode;
	newNode->nextNode = oriNode->nextNode;
	oriNode->preNode  = NULL;
	oriNode->nextNode = NULL;

	return oriNode;
}

int32_t DOS_GetListLength(TaskNode_t* headNode)
{
	if(headNode != NULL)
	{
		return -1;
	}

	int32_t length = 0;
	TaskNode_t* node = headNode->nextNode;
	while(node)
	{
		length++;
	}

	return length;
}

