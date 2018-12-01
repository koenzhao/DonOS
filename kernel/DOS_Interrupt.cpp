#include "DOS_Cpu.h"
#include "DOS_Interrupt.h"
#include "DOS_CommonDefine.h"
#include "DOS_Task.h"
#include "DOS_Delay.h"
#include "DOS_Event.h"
#include "DOS_Timer.h"

static void DOS_SystemTicksHandler(void)
{
	DOS_DisableIrq();

#if 0
	if(g_DelayTaskList->nextNode != NULL)
	{
		TaskNode_t* delayNode = g_DelayTaskList->nextNode;
		while(delayNode != NULL)
		{
			TaskNode_t* tempNode = 0;
			(delayNode->task->delay)--;
			tempNode = delayNode->nextNode;
			if(delayNode->task->delay == 0)
			{
				DOS_Task_ReadyTask(delayNode->task);
			}
			delayNode = tempNode;
		}
	}
#else
	if((g_DelayTimeList->delayTaskNode != NULL))//&& (g_DelayTimeList->delay >= 0))
	{
		if( (g_DelayTimeList->delay == 0) || (--g_DelayTimeList->delay == 0) )
		{
			TaskNode_t* pTaskToReady = DOS_Delay_DeleteFirstNodeOfDelayTimeList();
			while(pTaskToReady)
			{
				TaskNode_t* pNextTask = pTaskToReady->nextNode;
				Task_t*     pTask = pTaskToReady->task;
				DOS_Task_ReadyTask(pTask);
				if(pTask->waitEvent != NULL)
				{
					DOS_Event_Timeout(pTask);
				}
				pTaskToReady = pNextTask;
			}
		}
	}
#endif

	if((--g_currentTask->slice) == 0)
	{
		if(g_currentTask->taskNode->nextNode != NULL)
		{
			DOS_Task_ReadyTask(g_currentTask);
		}
		g_currentTask->slice = g_currentTask->sliceReload;
	}

	DOS_EnableIrq();

	DOS_Timer_TickNotify();
	
	DOS_TaskSchedule();
}

#ifdef __cplusplus
extern "C"
{
#endif
void SysTick_Handler () 
{
	DOS_SystemTicksHandler();
	//DOS_TaskSchedule();
}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif
__asm void PendSV_Handler ()
{
    IMPORT  g_currentTask               
    IMPORT  g_nextTask                  
    
    MRS     R0, PSP                  
    CBZ     R0, PendSVHandler_nosave  
                                      
    STMDB   R0!, {R4-R11}             
                                     
    LDR     R1, =g_currentTask         
    LDR     R1, [R1]                
    STR     R0, [R1]                 

PendSVHandler_nosave                  
                                     
    LDR     R0, =g_currentTask          
    LDR     R1, =g_nextTask             
    LDR     R2, [R1]  
    STR     R2, [R0]                 
 
    LDR     R0, [R2]                  
    LDMIA   R0!, {R4-R11}            

    MSR     PSP, R0                 
    ORR     LR, LR, #0x04          
    BX      LR                     
}
#ifdef __cplusplus
}
#endif

static int32_t disIrqCnt = 0;
void DOS_DisableIrq(void)
{
	if(0 == disIrqCnt)
	{
		__disable_irq();
	}
	disIrqCnt++;
}

void DOS_EnableIrq(void)
{
	disIrqCnt--;
	if(0 <= disIrqCnt)
	{
		__enable_irq();
		disIrqCnt = 0;
	}
}

void triggerPendSVC (void) 
{
    MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;   // 向NVIC_SYSPRI2写NVIC_PENDSV_PRI，设置其为最低优先级
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;    // 向NVIC_INT_CTRL写NVIC_PENDSVSET，用于PendSV
}
