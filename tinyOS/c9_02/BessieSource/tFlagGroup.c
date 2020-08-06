#include "tFlagGroup.h"
#include "tinyOS.h" //包含了tErrorNoError

//9.1
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags)
{
	tEventInit(&flagGroup->eventECB, tEventTypeFlagGroup);
	flagGroup->flag = flags;
}

//9.2 辅助函数. 如果发生了事件, 是否需要把这些事件给消耗掉 //这个是static函数, 说明只是属于这个.c文件, 所以没有在.h文件里面声明
static uint32_t tFlagGroupCheckAndConsume(tFlagGroup* flagGroup, uint32_t type, uint32_t* flag) //type:我们的设置, 见TFLAGGROUP_SET_ALL, TFLAGGROUP_CLEAR_ALL, TFLAGGROUP_CONSUME. flag:这里是传地址取值, 也就是把发生的事件的地址存到flag中
{
	//取出参数值
	uint32_t srcFlag = *flag; //取出原始的, 任务期待的标识
	
	uint32_t isSet = type & TFLAGGROUP_SET;	//请求的类型是 设置 还是 清除
	uint32_t isAll = type & TFLAGGROUP_ALL;	//是不是等待所有的标志 
	uint32_t isConsume = type & TFLAGGROUP_CONSUME; //遇见标识后, 是不是将这些标识消耗掉
	
	//计算哪些标志位发生了
	uint32_t calFlag = isSet ? (flagGroup->flag & srcFlag) : (~flagGroup->flag & srcFlag);
	
	//如果满足条件, 看是否要消耗
	if(  ((isAll != 0) && (calFlag == srcFlag))  ||  ((isAll == 0) && (calFlag != 0))  )
	{
		if(isConsume)
		{
			if(isSet)
			{
				flagGroup->flag &= ~srcFlag;
			}
			else
			{
				flagGroup->flag |= srcFlag;
			}
		}
		*flag = calFlag;
		return tErrorNoError;
	}
	
	*flag = calFlag;
	return tErrorResourceUnavailable;
}

//9.2 等待操作
uint32_t tFlagGroupWait(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag, uint32_t waitTicks)//waitType: 等待的类型, requestFlag:等待那些标识, resultFlag(注意是地址, uint32_t*)最后等到了哪些标识, waitTick:超时
{
	//检查
	uint32_t result;
	uint32_t flags = requestFlag;
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	result = tFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	//检查事件是否已经满足
	if(result != tErrorNoError)
	{
		currentTask->waitFlagsType = waitType;
		currentTask->eventFlags = requestFlag;
		tEventWait(&flagGroup->eventECB, currentTask, (void*) 0, tEventTypeFlagGroup, waitTicks);
		
		tTaskExitCritical(status);
		
		tTaskSched();
		
		*resultFlag = currentTask->eventFlags; //这个结果会从tFlagGroupNotify()中获得, task->eventFlags = flags; //这里就会返回给tFlagGroupWait()中给*resultFlag = currentTask->eventFlags;这一句
		result = currentTask->waitEventResult;
	}
	else
	{
		*resultFlag = flags;
		tTaskExitCritical(status);
	}
	return result;
	
	
}

//9.2 获取事件标识
uint32_t tFlagGroupNoWaitGet(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag)
{
	uint32_t flags = requestFlag;
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//检查: 事件标志
	uint32_t result = tFlagGroupCheckAndConsume(flagGroup ,waitType, &flags);
	tTaskExitCritical(status);
	
	*resultFlag = flags;
	return tErrorNoError;
}

//9.2 通知需要的标志位发生了
void tFlagGroupNotify(tFlagGroup* flagGroup, uint8_t isSet, uint32_t flag)
{
	//定义链表的指针 和 节点的指针
	tList* waitList;
	tNode* node;
	tNode* nextNode;
	uint32_t result;
	uint8_t sched; //是否需要调度
	
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	if(isSet)
	{
		flagGroup->flag |= flag;
	}
	else
	{
		flagGroup->flag &= ~flag;
	}
	
	//对ECB中的task进行处理:
	//01. 取出ECB中的列表
	waitList = &flagGroup->eventECB.waitList;
	//02. 遍历
	for(node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode)
	{
		//取出任务
		tTask* task = tNodeParentAddr(tTask, linkNode, node);
		
		//任务期望等待的标识
		uint32_t flags = task->eventFlags;
		
		//下一个节点
		nextNode = node->nextNode;
		
		//看task等待的事件标识是否满足
		result = tFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
		
		//如果满足
		if(result == tErrorNoError)
		{
			task->eventFlags = flags; //这里就会返回给tFlagGroupWait()中给*resultFlag = currentTask->eventFlags;这一句
			tEventWakeUpSpecificTask(&flagGroup->eventECB, task, (void*)0, tErrorNoError);//将task从ECB中唤醒
			sched = 1; 
		}
	}
	
	if(sched)
	{
		tTaskSched();
	}
	tTaskExitCritical(status);
}



