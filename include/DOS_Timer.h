#ifndef _DOS_TIMER_H_
#define _DOS_TIMER_H_

#include "DOS_CommonDefine.h"
#include "DOS_Semaphore.h"
#include "DOS_Interrupt.h"
#include "DOS_List.h"

#define TIMER_TYPE_HARD (1 << 0)
#define TIMER_TYPE_SOFT (0 << 0)

struct _DOS_TimerNode_t;
struct _DOS_Timer_t;

typedef enum _DOS_Timer_Return_t
{
	TIMER_FAILURE,
	TIMER_SUCCESS,
	TIMER_ERROR,
} Timer_Return_t;

typedef enum _DOS_Timer_State_t
{
	TIMER_CREATED,
	TIMER_STARTED,
	TIMER_RUNNING,
	TIMER_STOPPED,
	TIMER_DESTORYED,
} Timer_State_t;

typedef struct _DOS_Timer_t
{
	List_t*  timerNode;
	uint32_t startDelayTicks;
	uint32_t durationTicks;
	uint32_t delayTicks;
	void (*callback) (void* arg);
	void* arg;
	uint32_t config;
	Timer_State_t state;
} Timer_t;

typedef struct _DOS_TimerNode_t
{
	_DOS_TimerNode_t* preNode;
	_DOS_Timer_t*     timer;
	_DOS_TimerNode_t* nextNode;
} TimerNode_t;

extern void           DOS_SetSysTickPeriod(uint32_t ms);
extern Timer_Return_t DOS_Timer_Init(Timer_t* timer, uint32_t delayTicks, uint32_t durationTicks,
                                         void (*callback)(void * arg), void* arg, uint32_t config);
extern Timer_Return_t DOS_Timer_Start(Timer_t* timer);
extern Timer_Return_t DOS_Timer_Stop(Timer_t* timer);
extern Timer_Return_t DOS_Timer_TimerListProcess(TimerNode_t** pTimerList);
extern void           DOS_Timer_TickNotify(void);
extern void           DOS_Timer_SoftTask(void* arg);                          
extern void           DOS_Timer_ModuleInit(void);
extern void           DOS_Timer_Destory(Timer_t* timer);

#endif
