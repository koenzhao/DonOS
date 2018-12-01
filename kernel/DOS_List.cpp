/******************************************************
 * File Name:   DOS_List.cpp
 * File Date:   2018.08.27
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.27 | Create   |  Koen  |
 *****************************************************/

#include "DOS_List.h"

List_t* DOS_List_New(void* data)
{
	List_t* newList = (List_t*)new List_t;
	newList->data = data;
	newList->next = NULL;
	newList->prev = NULL;

	return newList;
}

List_t* DOS_List_Delete(List_t* node)
{
	node->data = NULL;
	node->prev = NULL;
	node->next = NULL;
	delete node;
	return NULL;
}


List_t* DOS_List_GetLast(List_t* list)
{
	if(list)
	{
		while(list->next)
		{
			list = list->next;
		}
	}
	return list;
}

List_t* DOS_List_Append(List_t* list, void* data)
{
	List_t* newList;
	List_t* last;

	newList = DOS_List_New(data);

	if(list)
	{
		last = DOS_List_GetLast(list);
		last->next = newList;
		newList->prev = last;

		return list;
	}
	else
	{
		newList->prev = NULL;
		return newList;
	}
	
}

List_t* DOS_List_Prepend(List_t* list, void* data)
{
	List_t* newList;

	newList = DOS_List_New(data);
	newList->next = list;

	if(list)
	{
		newList->prev = list->prev;
		if(list->prev)
		{
			list->prev->next = newList;
		}
		list->prev = newList;
	}
	else
	{
		newList->prev = NULL;
	}

	return newList;
}

List_t* DOS_List_Insert_After(List_t* list, List_t* node, void* data)
{
	if(!list)
	{
		return NULL;
	}

	if(node->next == NULL)
	{
		DOS_List_Append(list, data);
	}
	else
	{
		List_t* newList = DOS_List_New(data);
		List_t* nextList = node->next;
		node->next = newList;
		nextList->prev = newList;
		newList->prev = node;
		newList->next = nextList;
	}

	return list;
}

List_t* DOS_List_Insert_Before(List_t* list, List_t* node, void* data)
{
	if(!list)
	{
		return list;
	}

	if(node->prev == NULL && list == node)
	{
		DOS_List_Prepend(list, data);
	}
	else
	{
		List_t* newList = DOS_List_New(data);
		List_t* preList = node->prev;
		preList->next = newList;
		node->prev = newList;
		newList->prev = preList;
		newList->next = node;
	}

	return list;
}

List_t* DPS_List_RemoveNode(List_t* list, List_t* node)
{
	if(list == NULL)
	{
		return list;
	}

	if(node->prev)
	{
		if(node->prev->next == node)
		{
			node->prev->next = node->next;
		}
	}

	if(node->next)
	{
		if(node->next->prev == node)
		{
			node->next->prev = node->prev;
		}
	}

	if(list == node)
	{
		list = list->next;
	}

	node->next = NULL;
	node->prev = NULL;

	return list;
}


List_t* DOS_List_Remove(List_t* list, void* data)
{
	List_t* tmp;

	tmp = list;
	while(tmp)
	{
		if(tmp->data != data)
		{
			tmp = tmp->next;
		}
		else
		{
			list = DPS_List_RemoveNode(list, tmp);
			DOS_List_Delete(tmp);
			break;
		}
	}
	
	return list;
}


