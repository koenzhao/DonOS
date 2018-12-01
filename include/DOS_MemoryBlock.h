/******************************************************
 * File Name:   DOS_MemoryBlock.h
 * File Date:   2018.08.14
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.14 | Create   |  Koen  |
 *****************************************************/

#ifndef _DOS_MEMORY_BLOCK_H_
#define _DOS_MEMORY_BLOCK_H_

#include "DOS_CommonDefine.h"
#include "DOS_Event.h"
#include "DOS_Interrupt.h"

typedef struct _DOS_MemBlkNode_t
{
	//uint8_t* MemAdr;
	_DOS_MemBlkNode_t* pNextNode;
} MemBlkNode_t;

typedef struct _DOS_MemBlkList_t
{
	_DOS_MemBlkNode_t* headNode;
	uint32_t           nodeNum;
} MemBlkList_t;

typedef struct _DOS_MemBlock_t
{
	Event_t   event;
	void*     memStart;
	uint32_t  blockSize;
	uint32_t  maxCount;
	MemBlkList_t blockList;
} MemBlock_t;

void DOS_MemBlock_ListInit(MemBlkList_t* pList);
void DOS_MemBlock_NodeInit(MemBlkNode_t* pNode);
void DOS_MemBlock_AddNodeToList(MemBlkList_t* pList, MemBlkNode_t* pNode);
void DOS_MemBlock_Init(MemBlock_t* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t blockCount);
void DOS_MemBlock_Wait_Block(MemBlock_t* memBlk, uint8_t** mem, uint32_t waitTick);
void DOS_MemBlock_Wait_Unblock(MemBlock_t* memBlk, uint8_t** mem);
void DOS_MemBlock_Notify(MemBlock_t* memBlk, uint8_t* mem);
void DOS_MemBlock_Destory(MemBlock_t* memBlk);

#endif

