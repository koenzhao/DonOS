/******************************************************
 * File Name:   DOS_List.cpp
 * File Date:   2018.08.27
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.27 | Create   |  Koen  |
 *****************************************************/

#ifndef _DOS_LIST_H_
#define _DOS_LIST_H_

#include "DOS_CommonDefine.h"

typedef struct _DOS_List_t
{
	void* data;
	struct _DOS_List_t* next;
	struct _DOS_List_t* prev;
} List_t;

List_t* DOS_List_New(void* data);
List_t* DOS_List_Delete(List_t* node);
List_t* DOS_List_GetLast(List_t* list);
List_t* DOS_List_Append(List_t* list, void* data);
List_t* DOS_List_Prepend(List_t* list, void* data);
List_t* DOS_List_Insert_After(List_t* list, List_t* node, void* data);
List_t* DOS_List_Insert_Before(List_t* list, List_t node, void* data);
List_t* DPS_List_RemoveNode(List_t* list, List_t* node);
List_t* DOS_List_Remove(List_t* list, void* data);

#endif
