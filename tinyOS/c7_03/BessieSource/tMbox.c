//7.1 邮箱的定义
#include "tMbox.h"
//7.2 tTask等的操作:
#include "tinyOS.h"

//7.1
void tMboxInit(tMbox* mbox, void** msgBuffer, uint32_t maxCount)
{
	tEventInit(&mbox->eventECB, tEventTypeMbox); //所以这个ECB是邮箱类型的
	mbox->maxCount = maxCount; //容量
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;  //实际存储
}

//7.2 等待操作(这个是task调用的)
uint32_t tMboxWait(tMbox* mbox, void** msg, uint32_t waitTicks) //void** msg: 用于存放消息的指针, uint32_t waitTicks: 等待的时间
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//检查有没有消息
	if(mbox->count >0 )//有消息
	{
		--mbox->count; //消息-1
		*msg = mbox->msgBuffer[mbox->read++]; //从中取出消息,通过read索引来取出, 然后read指针+1
		if(mbox->read >= mbox->maxCount) //这里其实是想写成如果read == maxCount, 所以maxCount其实就是capacity, 而不是索引. 所以一旦read == maxCount, read就是非法的
		{
			mbox->read = 0; 
		}
		tTaskExitCritical(status);
		return tErrorNoError; //没有错误, 说明的确有消息, 然后任务获得msg
	}
	else//没有消息, task需要等待
	{
		tEventWait(&mbox->eventECB, currentTask, (void*)0, tEventTypeMbox, waitTicks); //task的等待队列是mbox类型, 并且如果超过waitTicks个systick之后还没有等到,就不等了
		tTaskExitCritical(status);//退出临界区, 因为接下来的TaskSched()也要进入临界区
		
		//此时task已经进入等待队列, 需要切换到其他task
		tTaskSched();
		
		//走到这一句的时候, 是又回到了这个currentTask, 回到的原因: msg到了, 或者延时结束了
		*msg = currentTask->eventMsg; //获得消息
		return currentTask->waitEventResult; //具体到底是: msg到了, 或者延时结束了, 是看我们中途经过了什么
	}
}

//7.2 实现: 如果没有消息就不等了
uint32_t tMboxNoWaitGet(tMbox* mbox, void** msg)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	if(mbox->count > 0)//有消息
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else //没消息
	{ 
		tTaskExitCritical(status);
		return tErrorResourceUnavailable;
	}
}

//7.2 消息的通知, 也就是现在来了一个消息msg, 告诉那些在等待队列中的task们
uint32_t tMboxNotify(tMbox* mbox, void* msg, uint32_t notifyOption) // uint32_t notifyOption 指明了如果插入的msg是高优先级的, 会被优先读取
{
	//临界区
	uint32_t status = tTaskEnterCritical();

	//判断有没有任务在等待
	if(tEventWaitCount(&mbox->eventECB) > 0) //说明有task在等, 那就直接将msg给这个task, msg也没必要往buffer里面加了
	{
		//唤醒一个task
		tTask* task = tEventWakeUp(&mbox->eventECB, (void*)msg, tErrorNoError); //msg会传入task->eventMsg中, 然后tErrorNoError会传入task->eventMsgResult中, 之后会返回到tMboxWait()中的*msg = currentTask->eventMsg; currentTask->waitEventResult; 
		
		//既然一个task被唤醒了, 这个task可能优先级更高, 如果更高, 就要切换到他
		if(task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	else //说明没有task在等, msg需要加入buffer中, 但是我们要看看, 是不是邮箱已经满了
	{
		if(mbox->count >= mbox->maxCount) //满了就要立即退出
		{
			tTaskExitCritical(status);
			return tErrorResourceFull; //邮箱已满
		}
		
		//走到这一步, 说明没有满, msg可以加入buffer中
		// tMBOXSendNormal 			0x00 //这个是按普通方法来缓存msg, 也就是在write+1的地方加入msg
		// tMBOXSendFront				0x01 //这个说明加入的msg的优先级比较高, 所以在read-1的地方加入msg, 这样task第一个读的就是这个msg
		if(notifyOption & tMBOXSendFront) //说明notifyOption的末位 == 1
		{
			if(mbox->read <= 0) //这里是read == 0
			{
				mbox->read = mbox->maxCount - 1; 
			}
			else
			{
				--mbox->read;
			}
			
			//所以read是先-1, 后放入, 最后read指向有元素的位置
			mbox->msgBuffer[mbox->read] = msg;
		}
		else //普通的方式写入
		{	
			//这里就比较神奇了, 首先是write指向的就是一个没有放元素的位置, 然后放置完msg后, write在+1, 最后判断write的合法性
			mbox->msgBuffer[mbox->write++] = msg;
			if(mbox->write >= mbox->maxCount)
			{
				mbox->write = 0;
			}
		}
		
		//对消息的数量+1
		mbox->count++;
	}
	
	tTaskExitCritical(status);
	return tErrorNoError;
}


//7.3 清空邮箱中的所有msg
void tMboxFlush(tMbox* mbox)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//判断有没有任务在等待, 如果有的话, 说明邮箱是空的, 不需要flush. 如果没有任务, 说明邮箱可能有东西
	if(tEventWaitCount(&mbox->eventECB) == 0)//说明邮箱可能有东西
	{
		mbox->read = 0;
		mbox->write = 0; //直接通过index指向就可以清空
		mbox->count = 0;
	}
	tTaskExitCritical(status);
}

//7.3 删除邮箱, 包括里面的msg和ECB等待队列, 返回值: 加入到就需队列的task个数
uint32_t tMboxDestory(tMbox* mbox)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//移除ECB中的所有任务, 错误码是删除: 也就是说task从从ECB移除的原因是被删除
	uint32_t count = tEventRemoveAll(&mbox->eventECB, (void*)0, tErrorDel);
	
	tTaskExitCritical(status);
	
	//看是否有>0的task加入了就绪队列
	if(count > 0)
	{
		tTaskSched();
	}
	
	return count;
}



