#include "DOS_Cpu.h"
#include "DOS_Timer.h"

static List_t*      s_HardTimerList;
static List_t*      s_SoftTimerList;
static Sem_t        s_TimerProtectSem;
static Sem_t        s_TimerTickSem;
static Task_t       timerTask;
static uint32_t     taskStack_Timer[1024];

void DOS_SetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1; 
  	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
  	SysTick->VAL   = 0;                           
  	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk; 
}

Timer_Return_t DOS_Timer_Init(Timer_t* timer, uint32_t delayTicks, uint32_t durationTicks,
                                  void (*callback)(void * arg), void* arg, uint32_t config)
{
	timer->timerNode     = NULL;
	timer->startDelayTicks    = delayTicks/10;
	timer->durationTicks = durationTicks/10;
	timer->callback      = callback;
	timer->arg           = arg;
	timer->config        = config;

	if(delayTicks == 0)
	{
		timer->delayTicks = durationTicks/10;
	}
	else
	{
		timer->delayTicks = timer->startDelayTicks;
	}
	
	timer->state = TIMER_CREATED;
	return TIMER_SUCCESS;
}




Timer_Return_t DOS_Timer_Start(Timer_t* timer)
{
	if(timer == NULL)
	{
		return TIMER_ERROR;
	}

	switch(timer->state)
	{
		case TIMER_CREATED:
		case TIMER_STOPPED:
			timer->delayTicks = timer->startDelayTicks ? timer->startDelayTicks : timer->durationTicks;
			timer->state = TIMER_STARTED;

			if(timer->config & TIMER_TYPE_HARD) // Hard timer
			{
				DOS_DisableIrq();
				s_HardTimerList = DOS_List_Append(s_HardTimerList, (void *)timer);
				DOS_EnableIrq();
			}
			else // Soft timer
			{
				DOS_Sem_Wait_Block(&s_TimerProtectSem, 0);
				s_SoftTimerList = DOS_List_Append(s_SoftTimerList, (void *)timer);
				DOS_Sem_Notify(&s_TimerProtectSem);
			}
			break;
		default:
			break;
	}
	return TIMER_SUCCESS;
}


Timer_Return_t DOS_Timer_Stop(Timer_t* timer)
{
	if(timer == NULL)
	{
		return TIMER_ERROR;
	}

	switch(timer->state)
	{
		case TIMER_STARTED:
		case TIMER_RUNNING:
			if(timer->config & TIMER_TYPE_HARD)
			{
				DOS_DisableIrq();
				s_HardTimerList = DOS_List_Remove(s_HardTimerList, (void *)timer);
				DOS_EnableIrq();
			}
			else
			{
				DOS_Sem_Wait_Block(&s_TimerProtectSem, 0);
				s_SoftTimerList = DOS_List_Remove(s_SoftTimerList, (void *)timer);
				DOS_Sem_Notify(&s_TimerProtectSem);
			}
			timer->state = TIMER_STOPPED;
			break;
		default:
			break;
	}
	return TIMER_SUCCESS;
}


Timer_Return_t DOS_Timer_TimerListProcess(List_t** ppTimerList)
{
	if(*ppTimerList == NULL || (*ppTimerList)->data == NULL)
	{
		return TIMER_ERROR;
	}

	for(List_t* node = *ppTimerList; node; node = node->next)
	{
		Timer_t* timer = (Timer_t*)node->data;
		if((timer->delayTicks == 0) || (--timer->delayTicks == 0))
		{
			timer->state = TIMER_RUNNING;
			timer->callback(timer->arg);
			timer->state = TIMER_STARTED;
			if(timer->durationTicks > 0)
			{
				timer->delayTicks = timer->durationTicks;
			}
			else
			{
				*ppTimerList = DOS_List_Remove(*ppTimerList, (void *)timer);
				
				timer->state = TIMER_STOPPED;
			}
		}
	}
	return TIMER_SUCCESS;
}

void DOS_Timer_TickNotify(void)
{
	DOS_DisableIrq();

	DOS_Timer_TimerListProcess(&s_HardTimerList);
	
	DOS_EnableIrq();

	DOS_Sem_Notify(&s_TimerTickSem);
}

void DOS_Timer_SoftTask(void* arg)
{
	for(;;)
	{
		DOS_Sem_Wait_Block(&s_TimerTickSem, 0);
		
		DOS_Sem_Wait_Block(&s_TimerProtectSem, 0);
		DOS_Timer_TimerListProcess(&s_SoftTimerList);
		DOS_Sem_Notify(&s_TimerProtectSem);
	}
}
                               
void DOS_Timer_ModuleInit(void)
{
	s_HardTimerList = NULL;
	s_SoftTimerList = NULL;

	DOS_Sem_Init(&s_TimerProtectSem, 1, 1);
	DOS_Sem_Init(&s_TimerTickSem, 0, 0);
	DOS_CreateTask(&timerTask, DOS_Timer_SoftTask, NULL, TASK_TIMER_PRIO, 10, &taskStack_Timer[1024]);
}

void DOS_Timer_Destory(Timer_t* timer)
{
	DOS_Timer_Stop(timer);
	timer->state = TIMER_DESTORYED;
}

