//#include "DOS_CommonDefine.h"
//#include "DOS_Task.h"
#include "DOS_Delay.h"
#include "test.h"
#include "DOS_Initial.h"

Task_t task1;
Task_t task2;
Task_t task3;
Task_t taskIdle;

uint32_t taskStack_Idle[1024]  = {0};
uint32_t taskStack_Test1[1024] = {0};
uint32_t taskStack_Test2[1024] = {0};
uint32_t taskStack_Test3[1024] = {0};

Timer_t timer1;
Timer_t timer2;
Timer_t timer3;
uint32_t bit1;
uint32_t bit2;
uint32_t bit3;

void timerFunc (void * arg)
{
    uint32_t * ptrBit = (uint32_t *)arg;
    *ptrBit ^= 0x1;
}


bool flag1 = FALSE;
void Task_Test1(void* param)
{
	uint32_t destory = 0;

	DOS_Timer_Init(&timer1, 100, 10, timerFunc, (void *)&bit1, TIMER_TYPE_HARD);
	DOS_Timer_Start(&timer1);
	
	DOS_Timer_Init(&timer2, 200, 20, timerFunc, (void *)&bit2, TIMER_TYPE_HARD);
	DOS_Timer_Start(&timer2);
	
	DOS_Timer_Init(&timer3, 300, 0, timerFunc, (void *)&bit3, TIMER_TYPE_SOFT);
	DOS_Timer_Start(&timer3);
	
	for(;;)
	{
		DOS_Delay(100);
		flag1 = TRUE;
		DOS_Delay(100);
		flag1 = FALSE;

		if(destory == 0)
		{
			DOS_Delay(200);
			DOS_Timer_Destory(&timer1);
			destory = 1;
		}
	}
}

bool flag2 = FALSE;
void Task_Test2(void* param)
{
	for(;;)
	{
		DOS_Delay(200);
		flag2 = TRUE;
		DOS_Delay(200);
		flag2 = FALSE;
		
	}
}

bool flag3 = FALSE;
void Task_Test3(void* param)
{
	for(;;)
	{
		DOS_Delay(400);
		flag3 = TRUE;
		DOS_Delay(400);
		flag3 = FALSE;
	}
}

void Task_Idle(void* param)
{
	for(;;)
	{
		asm("NOP");
	}
}

