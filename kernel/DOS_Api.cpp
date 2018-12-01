#include "DOS_Task.h"
#include "DOS_Timer.h"

void DOS_Run(void)
{
	//currentTask = DOS_GetIdleTask();
	g_nextTask = DOS_GetIdleTask();
	DOS_SetSysTickPeriod(10);
	DOS_TaskStart();
}

