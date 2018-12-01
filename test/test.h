#ifndef _TEST_H_
#define _TEST_H_

#include "DOS_Task.h"
#include "DOS_Event.h"
#include "DOS_Semaphore.h"
#include "DOS_MailBox.h"
#include "DOS_MemoryBlock.h"
#include "DOS_CommonDefine.h"
#include "DOS_FlagsGroup.h"
#include "DOS_Timer.h"

extern Task_t task1;
extern Task_t task2;
extern Task_t task3;
extern Task_t taskIdle;

extern uint32_t taskStack_Idle[1024];
extern uint32_t taskStack_Test1[1024];
extern uint32_t taskStack_Test2[1024];
extern uint32_t taskStack_Test3[1024];
extern void Task_Test1(void* param);
extern void Task_Test2(void* param);
extern void Task_Test3(void* param);
extern void Task_Idle(void* param);

#endif
