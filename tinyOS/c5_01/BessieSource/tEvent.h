//5.1 事件控制块的结构体和函数声明
#ifndef TEVENT_H //注意不要写成
#define TEVENT_H

#include "tLib.h" //包括了tList

//5.1 每一个事件控制块里面都有一个事件类型
typedef enum _tEventType
{
	tEventTypeUnknown,
}tEventType;

//5.1 事件控制块ECB
typedef struct _tEvent
{
	tEventType type;
	tList waitList; //存的是等待事件的task的tNode
}tEvent;
	
//5.1 ECB的初始化函数
void tEventInit(tEvent* event, tEventType* type);

#endif

