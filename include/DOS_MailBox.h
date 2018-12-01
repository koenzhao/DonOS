/******************************************************
 * File Name:   DOS_MailBox.h
 * File Date:   2018.08.13
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.13 | Create   |  Koen  |
 *****************************************************/

#ifndef _DOS_MAILBOX_H_
#define _DOS_MAILBOX_H_

#include "DOS_Event.h"
#include "DOS_CommonDefine.h"
#include "DOS_Interrupt.h"

typedef struct _DOS_MailBox_t
{
	Event_t  event;
	uint32_t count;
	uint32_t maxCount;
	uint32_t readIdx;
	uint32_t writeIdx;
	void**   msgBuffer;
} Mbox_t;

extern void DOS_MailBox_Init(Mbox_t* mail, void** msgBuffer, uint32_t maxCount);
extern void DOS_MailBox_Wait_Block(Mbox_t* mbox, void** msg, uint32_t waitTick);
extern void DOS_MailBox_Wait_Unblock(Mbox_t* mbox, void** msg);
extern void DOS_MailBox_Notify(Mbox_t* mbox, void* msg, uint32_t notifyOption);

#endif

