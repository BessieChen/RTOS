#include "tFlagGroup.h"

//9.1
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags)
{
	tEventInit(&flagGroup->eventECB, tEventTypeFlagGroup);
	flagGroup->flag = flags;
}

//9.2 如果发生了事件, 是否需要把这些事件给消耗掉
static uint32_t tFlagGroupCheck;

//9.2 等待操作
uint32_t tFlagGroupWait(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag, uint32_t waitTicks)//waitType: 等待的类型, requestFlag:等待那些标识, resultFlag(注意是地址, uint32_t*)最后等到了哪些标识, waitTick:超时
{

	
}

//9.2 获取事件标识
uint32_t tFlagGroupNoWaitGet(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t resultFlag)//todo, 疑问这里的resultFlag又不是地址了
{

}

//9.2 通知需要的标志位发生了
void tFlagGroupMptify(tFlagGroup* flgGroup, uint8_t isSet, uint32_t flag)
{

}



