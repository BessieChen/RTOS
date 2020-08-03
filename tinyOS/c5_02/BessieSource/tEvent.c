#include "tinyOS.h"

//5.2 初始化
void tEventInit(tEvent* eventECB, tEventType* type) //5.2 这里type没有用到呀..
{
	eventECB->type = tEventTypeUnknown;
	tListInit(&(eventECB->waitList));
}

//5.2 将task放入ECB的等待队列
void tEventWait(tEvent* eventECB, tTask* task, void* msg, uint32_t state, uint32_t timeout) //msg:传来的消息, state:等待状态, timeout:用户设置的超时时间
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//5.2 任务结构的参数
	task->state |= state; //这里用或|,是因为我们只是用了一个bit, 其余的bit留到以后邮箱等用
	task->waitEvent = eventECB; //所以你看, 这个像是一个双向的引用, 也就是
	task->eventMsg = msg;
	task->waitEventResult = tErrorNoError;
	
	//5.2 将task从就绪队列移除
	tTaskSchedUnReady(task);
	
	//5.2 将task插入ECB的尾部. 之所以插入到尾部, 是因为我们唤醒是唤醒第一个
	tListAddLast(&eventECB->waitList, &task->linkNode); 
	
	//5.2 检查是否有设置超时, 有的话, 还要放入延时队列中
	if(timeout)
	{
		tTimeTaskWait(task, timeout); //这个是将task放入延时队列, 延时timeout个systick
	}
	
	tTaskExitCritical(status);
}

//5.2 将task从ECB的等待队列中唤醒, 注意, 这是只是唤醒第一个task, 而不是指定某个task //疑问: 谁会调用 这个函数? 因为,等待事件如果发生,就是会唤醒指定的task,而不是唤醒第一个呀
tTask* tEventWake(tEvent* event, void* msg, uint32_t result)//msg:会传递信息进去, result: 唤醒的结果
{
	tNode* node;
	tTask* task = (tTask*) 0;
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//唤醒队列头部的task
	node = tListRemoveFirst(&event->waitList);
	if(node != (tNode*)0) //说明list有至少一个node
	{
		//找到对应的task
		task = tNodeParentAddr(tTask, linkNode, node);
		
		//将task的状态修改
		task->waitEvent = (tEvent*)0; //相当于认为task不需要等待事件了
		task->eventMsg = msg;
		task->waitEventResult = result;
		
		//将task的state修改, 也就是将所有的前面高16位都清空. 但是延时标记什么的应该还在
		task->state &= ~TINYOS_TASK_WAIT_MASK; 				//(0xFF<<16) //5.2 也就是1111 1111 0000 0000, 所以取非之后的后16位就是0000 0000 1111 1111
		
		//假设之前的task处于延时队列中, 也就是之前设置了timeOut, 那么唤醒task的同时, 就要强制的! 把task从延时队列中清除
		//如果delayticks == 0, 其他函数会把task从延时队列中删除. 总之都要删除
		if(task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);
		}
		
		//唤醒task的最后把task加入就绪队列中
		//注意这里没有调度, 因为调度的操作是由其他函数会操作的
		tTaskSchedReady(task);
	}
	
	tTaskExitCritical(status);
	
	return task;
}


//5.2 将task从ECB中强制移除, 是删除指定的task
void tEventRemoveTask(tTask* task, void* msg, uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	
	//移除特定的任务, 不像tEventWake()是移除ECB的头部任务
	tListRemove(&task->waitEvent->waitList, &task->linkNode); //这里是找到task所在的ECB:task->waitEvent->waitList, 然后从ECB中获得waitList
	
	//将task的状态修改
		task->waitEvent = (tEvent*)0; //相当于认为task不需要等待事件了
		task->eventMsg = msg;
		task->waitEventResult = result;
		task->state &= ~TINYOS_TASK_WAIT_MASK; 
	
	//这里没有做判断是否处于延时
	//因为这个函数是给sysick的handler调用的,所以当systick把延时-1之后, systick的handler会判断是否从延时队列清除
	
	tTaskExitCritical(status);

}



