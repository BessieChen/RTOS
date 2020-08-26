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

//9.2 宏定义: 希望哪种事件类型
#define TFLAGGROUP_CLEAR				(0x0 << 0) //希望某一位标识是0								00
#define TFLAGGROUP_SET					(0x1 << 0)	//设置							01
#define TFLAGGROUP_ANY					(0x0 << 1)	//等待任意的标志位		00
#define TFLAGGROUP_ALL					(0x1 << 1) //等待所有的标志位	10

//9.2 定义四种宏定义的组合
#define TFLAGGROUP_SET_ALL (TFLAGGROUP_SET | TFLAGGROUP_ALL) //设置成所有标识都出现才行 11
#define TFLAGGROUP_SET_ANY (TFLAGGROUP_SET | TFLAGGROUP_ANY) //设置成只要有一个标识都出现行 01
#define TFLAGGROUP_CLEAR_ALL (TFLAGGROUP_CLEAR | TFLAGGROUP_ALL) //清空所有标识 10
#define TFLAGGROUP_CLEAR_ANY (TFLAGGROUP_CLEAR | TFLAGGROUP_ANY) //清空任意一个标识 00

//9.2 宏定义: 任务等待的事件发生, 退出的时候, 需不需要把标志位清零
#define TFLAGGROUP_CONSUME 			(1 << 7) //1000 0000

//9.1 初始化
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags);

//9.2 等待操作
uint32_t tFlagGroupWait(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag, uint32_t waitTicks);//waitType: 等待的类型, requestFlag:等待那些标识, resultFlag(注意是地址, uint32_t*)最后等到了哪些标识, waitTick:超时
//9.2 获取事件标识
uint32_t tFlagGroupNoWaitGet(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag);
//9.2 通知需要的标志位发生了
void tFlagGroupNotify(tFlagGroup* flagGroup, uint8_t isSet, uint32_t flag);

#endif

