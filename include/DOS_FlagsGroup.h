/******************************************************
 * File Name:   DOS_FlagsGroup.h
 * File Date:   2018.08.21
 * File Author: Koen Chiu
 * Description: 
 * History:
 * |    Date    |  Commit  |  Name  |
 * | 2018.08.21 | Create   |  Koen  |
 *****************************************************/

#ifndef _DOS_FLAGSGROUP_H_
#define _DOS_FLAGSGROUP_H_

#include "DOS_CommonDefine.h"
#include "DOS_Event.h"
#include "DOS_Interrupt.h"

#define FLAGGROUP_CLR     (0x0 << 0)
#define FLAGGROUP_SET     (0x1 << 0)
#define FLAGGROUP_ANY     (0x0 << 1)
#define FLAGGROUP_ALL     (0x1 << 1)
#define FLAGGROUP_CONSUME (0x1 << 7)

#define FLAGGROUP_SET_ALL (0 | FLAGGROUP_SET | FLAGGROUP_ALL)
#define FLAGGROUP_SET_ANY (0 | FLAGGROUP_SET | FLAGGROUP_ANY)
#define FLAGGROUP_CLR_ALL (0 | FLAGGROUP_CLR | FLAGGROUP_ALL)
#define FLAGGROUP_CLR_ANY (0 | FLAGGROUP_CLR | FLAGGROUP_ANY)


typedef enum
{
	FLAGGROUP_FAILURE = 0,
	FLAGGROUP_SUCCESS = 1,
	FLAGGROUP_ERROR   = 2,
	
} FlgGrp_Return_t;

typedef struct _DOS_FlagGroup_t
{
	Event_t event;
	uint32_t flags;
} FlgGrp_t;


extern void            DOS_FlagGroup_Init(FlgGrp_t* flagGrp, uint32_t flags);
extern FlgGrp_Return_t DOS_FlagGroup_CheckAndConsume(FlgGrp_t* flagGrp, uint32_t type, uint32_t* pFlag);
extern FlgGrp_Return_t DOS_FlagGroup_Wait_Block(FlgGrp_t* flagGrp, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag, uint32_t waitTick);
extern FlgGrp_Return_t DOS_FlagGroup_Wait_NoBlock(FlgGrp_t* flagGrp, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag);
extern FlgGrp_Return_t DOS_FlagGroup_Notify(FlgGrp_t* flagGrp, uint8_t isSet, uint32_t flag);
extern uint32_t        DOS_FlagGroup_Destory(FlgGrp_t* flagGrp);

#endif

