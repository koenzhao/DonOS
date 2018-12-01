/******************************************************
 * File Name:   DOS_MemoryBlock.cpp
 * File Date:   2018.08.14
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.14 | Create   |  Koen  |
 *****************************************************/

#include "DOS_MemoryBlock.h"

void DOS_MemBlock_ListInit(MemBlkList_t* pList)
{
	if(pList == NULL)
	{
		return ;
	}

	pList->headNode = NULL;
	pList->nodeNum  = 0;
}

void DOS_MemBlock_NodeInit(MemBlkNode_t* pNode)
{
	pNode->pNextNode = NULL;
}

void DOS_MemBlock_AddNodeToList(MemBlkList_t* pList, MemBlkNode_t* pNode)
{
	if(pList == NULL || pNode == NULL)
	{
		return ;
	}

	MemBlkNode_t* headNode = pList->headNode;

	if(headNode == NULL)
	{
		pList->headNode = pNode;
	}
	else
	{
		while(headNode->pNextNode != NULL)
		{
			headNode = headNode->pNextNode;
		}

		headNode->pNextNode = pNode;
		pNode->pNextNode = NULL;
	}
	
	pList->nodeNum++;
}

MemBlkNode_t* DOS_MemBlock_GetFirstNode(MemBlkList_t * pList)
{
	if(pList == NULL || pList->nodeNum == 0)
	{
		return NULL;
	}

	MemBlkNode_t* firstNode = pList->headNode;

	if(firstNode)
	{
		pList->headNode = pList->headNode->pNextNode;
		firstNode->pNextNode = NULL;
		--pList->nodeNum;
		return firstNode;
	}
	else
	{
		return NULL;
	}

	
}

void DOS_MemBlock_Init(MemBlock_t* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t blockCount)
{
	uint8_t* memBlockStart = (uint8_t*)memStart;
	uint8_t* memBlockEnd   = (uint8_t*)memStart + blockSize * blockCount;

	if(blockSize < sizeof(MemBlkNode_t))
	{
		return ;
	}

	DOS_Event_Init(&memBlock->event, EVENT_TYPE_MEMBLOCK);

	memBlock->memStart  = memStart;
	memBlock->blockSize = blockSize;
	memBlock->maxCount  = blockCount;

	DOS_MemBlock_ListInit(&memBlock->blockList);

	while(memBlockStart < memBlockEnd)
	{
		DOS_MemBlock_NodeInit((MemBlkNode_t *)memBlockStart);
		DOS_MemBlock_AddNodeToList(&memBlock->blockList, (MemBlkNode_t *)memBlockStart);
		memBlockStart += blockSize;
	}
}

void DOS_MemBlock_Wait_Block(MemBlock_t* memBlk, uint8_t** mem, uint32_t waitTick)
{
	DOS_DisableIrq();

	if(memBlk == NULL)
	{
		DOS_EnableIrq();
		return ;
	}

	if(memBlk->blockList.nodeNum > 0)
	{
		*mem = (uint8_t*)DOS_MemBlock_GetFirstNode(&memBlk->blockList);
		DOS_EnableIrq();
		return ;
	}
	else
	{
		DOS_Event_Wait(&memBlk->event, (void *) 0, waitTick);
		DOS_EnableIrq();
		DOS_TaskSchedule();

		*mem = (uint8_t*)g_currentTask->eventMsg;

		return ;
	}
}

void DOS_MemBlock_Wait_Unblock(MemBlock_t* memBlk, uint8_t** mem)
{
	DOS_DisableIrq();

	if(memBlk == NULL)
	{
		DOS_EnableIrq();
		return ;
	}

	if(memBlk->blockList.nodeNum > 0)
	{
		*mem = (uint8_t*)DOS_MemBlock_GetFirstNode(&memBlk->blockList);
		DOS_EnableIrq();
		return ;
	}
	else
	{
		DOS_EnableIrq();
		return ;
	}

	//DOS_EnableIrq();
}

void DOS_MemBlock_Notify(MemBlock_t* memBlk, uint8_t* mem)
{
	DOS_DisableIrq();

	if(memBlk->event.eventWaitList)
	{
		Task_t* pTask = DOS_Event_Notify(&memBlk->event, (void*) mem);
		if(pTask->priority < g_currentTask->priority)
		{
			DOS_TaskSchedule();
		}
	}
	else
	{
		DOS_MemBlock_AddNodeToList(&memBlk->blockList, (MemBlkNode_t *) mem);
	}

	DOS_EnableIrq();
}

void DOS_MemBlock_Destory(MemBlock_t* memBlk)
{
	DOS_DisableIrq();

	if(memBlk == NULL)
	{
		return ;
	}

	uint32_t count = DOS_Event_RemoveAll(&memBlk->event);

	if(count > 0)
	{
		DOS_TaskSchedule();
	}

	DOS_EnableIrq();
}

