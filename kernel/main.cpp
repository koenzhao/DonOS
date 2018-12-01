//#include "DOS_CommonDefine.h"
#include "DOS_Interrupt.h"
#include "DOS_Delay.h"
#include "DOS_Task.h"
#include "DOS_Timer.h"
#include "DOS_Initial.h"
#include "test.h"
#include "DOS_Api.h"

int main()
{
	DOS_Initial();
	DOS_CreateTask(&taskIdle, Task_Idle,  NULL, TASK_IDLE_PRIO,   10, &taskStack_Idle[1024]);
	DOS_CreateTask(&task1,    Task_Test1, NULL, TASK_TEST_1_PRIO, 10, &taskStack_Test1[1024]);
	DOS_CreateTask(&task2,    Task_Test2, NULL, TASK_TEST_2_PRIO, 10, &taskStack_Test2[1024]);
	DOS_CreateTask(&task3,    Task_Test3, NULL, TASK_TEST_3_PRIO, 10, &taskStack_Test3[1024]);
	DOS_Run();
	return 0;
}
