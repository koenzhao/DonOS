#include "DOS_Initial.h"
#include "DOS_Task.h"
#include "DOS_Delay.h"
//#include "DOS_Event.h"
#include "DOS_Timer.h"

Event_t* eventTest;

void DOS_Initial(void)
{
	//DOS_Event_Init(&eventTest, EVENT_TYPE_UNUSED);
	DOS_Timer_ModuleInit();
}
