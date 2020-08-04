#include "tFlagGroup.h"

//9.1
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags)
{
	tEventInit(&flagGroup->eventECB, tEventTypeFlagGroup);
	flagGroup->flag = flags;
}
