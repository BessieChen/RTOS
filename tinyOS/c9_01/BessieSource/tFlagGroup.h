//9.1 添加: 事件标志组的结构定义
#ifndef TFLAGGROUP_H
#define TFLAGGROUP_H

#include "tEvent.h"

//9.1
typedef struct _tFlagGroup
{
	tEvent eventECB;
	uint32_t flag; //可以包含32个事件标识
}tFlagGroup;

//9.1 初始化
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags);

#endif

