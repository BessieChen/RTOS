//6.1 定义信号量
#include "tSem.h"
#include "tinyOS.h" //包括了临界区的定义

//6.1 初始化
void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount)//maxCount==0时候说明Count没有限制, 多大都可以
{
	tEventInit(&sem->eventECB, tEventTypeSem);
	
	sem->maxCount = maxCount;
	
	if(maxCount == 0) //maxCount==0时候说明Count没有限制, 多大都可以
	{
		sem->count = startCount; //所以这里不需要判断startCount是不是合法了
	}
	else//有最大count的限制,所以要判断以下startcount是否合法
	{
		sem->count = (startCount <= maxCount) ? startCount : maxCount;
	}
}

//6.2 这个函数由currentTask调用, 判断是否能够使用sem所代表的资源, 如果能的话, 返回值是noError. 如果不能的话, 将currentTask放入等待队列中, 然后返回值是currentTask的等待结果
uint32_t tSemWait(tSem* sem, uint32_t waitTicks)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	if(sem->count > 0) //说明sem代表的资源还有剩余, 可以用 //bug!!!! 是sem->count > 0 不是&sem->count > 0
	{
		--sem->count;
		tTaskExitCritical(status);//退出临界区
		return tErrorNoError; //返回没有错误
	}
	else
	{
		tEventWait(&sem->eventECB, currentTask, (void*)0, tEventTypeSem, waitTicks); //void tEventWait(tEvent* eventECB, tTask* task, void* msg, uint32_t state, uint32_t timeout) //msg:传来的消息, state:等待状态, timeout:用户设置的超时时间
		//上面的tEventTypeSem会放入currentTask的state中, 注意tEventTypeSem只是一个bit, 说明curretTask在等待一个sem代表的资源, 同时waitTicks说明currentTask也放入了延时函数
		
		tTaskExitCritical(status);//退出临界区
		
		tTaskSched();//这一句在退出临界区之后, 因为tTaskSched()函数里面也会先进入临界区. tTaskSched()会切换到其他task
		
		return currentTask->waitEventResult; //注意, 因为tTaskSched()会到其他不知道哪里的地方, 不过最后还是会回来, 回到这一句的时候, 我们知道这个currentTask的等待结果: 要么是等到了, 要么是没等到只是掩饰结束了
	}
}

//6.2 如果有资源, 就用. 没有的话, task也不去等
uint32_t tSemNoWaitGet(tSem* sem)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	if(sem->count > 0)
	{
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else{
		tTaskExitCritical(status);
		return tErrorResourceUnavailable; //返回资源不可用, 老子不等了
		
	}
}

//6.2 通知接口, 看看还有没有task要我不用的资源
void tSemNotify(tSem* sem)
{
	uint32_t status = tTaskEnterCritical();
	
	//看有没有task在sem的等待队列里等
	if(tEventWaitCount(&sem->eventECB) > 0) //有task在等
	{
		tTask* task = tEventWakeUp(&sem->eventECB, (void*)0, tErrorNoError); //tTask* tEventWakeUp(tEvent* eventECB, void* msg, uint32_t result)//result: 保存到task->waitEventResult: 唤醒的结果
		
		//如果发现task的优先级高于我们currentTask, 切换到该task
		if(task->prio < currentTask->prio) //注意, prio越小, 优先级越高
		{
			tTaskSched();
		}
	}
	else //说明没task在等
	{
		++sem->count;
		//再判断count的合法性
		if(sem->maxCount != 0 && (sem->count > sem->maxCount))
		{
			sem->count = sem->maxCount;
		}
	}
	
	tTaskExitCritical(status);
}


