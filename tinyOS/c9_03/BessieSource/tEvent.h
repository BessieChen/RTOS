//5.1 事件控制块的结构体和函数声明
#ifndef TEVENT_H //注意不要写成
#define TEVENT_H

#include "tLib.h" //包括了tList
#include "tTask.h" //包括了tTask

//5.1 每一个事件控制块里面都有一个事件类型
typedef enum _tEventType
{
	tEventTypeUnknown,
	tEventTypeSem, //信号量中的等待队列
	tEventTypeMbox, //邮箱中的等待队列
	tEventTypeMemBlock, //说明这个tEvent是为了存储块而建立的
	tEventTypeFlagGroup, //说明这个tEvent是为了事件标志组而建立的
}tEventType;

//5.1 事件控制块ECB
typedef struct _tEvent
{
	tEventType type;
	tList waitList; //存的是等待事件的task的tNode
}tEvent;
	
//5.1 ECB的初始化函数
void tEventInit(tEvent* event, tEventType type); //5.2 这里type没有用到呀..
void tEventWait(tEvent* event, tTask* task, void* msg, uint32_t state, uint32_t timeout);//5.2 将task放入ECB的等待队列
tTask* tEventWakeUp(tEvent* event, void* msg, uint32_t result);//5.2 将task从ECB的等待队列中唤醒
void tEventRemoveTask(tTask* task, void* msg, uint32_t result);//5.2 将task从ECB中强制移除
uint32_t tEventRemoveAll(tEvent* eventECB, void* msg, uint32_t result);//5.3
uint32_t tEventWaitCount(tEvent* eventECB);//5.3
tTask* tEventWakeUpSpecificTask(tEvent* eventECB, tTask* task, void* msg, uint32_t result); //9.2



#endif

