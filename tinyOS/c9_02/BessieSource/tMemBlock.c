#include "tMemBlock.h"
#include "tinyOS.h" //临界区等

//8.1 
void tMemBlockInit(tMemBlock* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t maxBlockCount)
{
	//定义两个指针变量, 分别指向 存储区域 的起始地址, 终止地址
	uint8_t* memBlockStart = (uint8_t*) memStart;
	uint8_t* memBlockEnd = memBlockStart + blockSize * maxBlockCount; //我的疑问, 两个uint32_t相乘, 之后赋给uint8_t, 不会溢出吗
	
	//我们的block是要存储一个tNode的, 记得吗一个tNode里面存了两个tNode*, 如果blockSize不够大, 久存不了
	if(blockSize <sizeof(tNode))
	{
		return;
	}
	
	//初始化tList
	memBlock->memStart = memStart;
	memBlock->blockSize = blockSize;
	memBlock->maxBlockCount = maxBlockCount;
	tListInit(&memBlock->blockList);
	
	//初始化ECB
	tEventInit(&memBlock->eventECB, tEventTypeMemBlock); //tEventTypeMemBlock, //说明这个tEvent是为了存储块而建立的

	//初始化tNode, 然后加入tList的前端, 然后初始化下一个tNode
	while(memBlockStart < memBlockEnd)
	{
		tNodeInit((tNode*) memBlockStart);
		tListAddFirst(&memBlock->blockList, (tNode*)memBlockStart);
		
		//指向下一个tNode的位置
		memBlockStart += blockSize;
	}
	

}


//8.2 等待block
uint32_t tMemBlockWait(tMemBlock* memBlock, uint8_t** mem, uint32_t waitTicks)//传地址存值: 将block的地址存到参数mem中, 如果没有block就等待waitTick个systick
{
	uint32_t status = tTaskEnterCritical();
	
	if(tListCount(&memBlock->blockList) > 0) //有多余的block
	{
		*mem = (uint8_t*)tListRemoveFirst(&memBlock->blockList); //从tList的头部移除这个block, 把地址赋给mem
		
		tTaskExitCritical(status);
		return tErrorNoError;
	}

	//说明这个task需要插入ECB中等待
	tEventWait(&memBlock->eventECB, currentTask, (void*)0, tEventTypeMemBlock, waitTicks); //延时waitTicks
	
	tTaskExitCritical(status);
	
	//currentTask已经插入等待队列中, 所以切换
	tTaskSched();
	
	//以下是回到currentTask的时候执行的
	*mem = currentTask->eventMsg;
	return currentTask->waitEventResult;
}

//8.2 如果没有block就不等
uint32_t tMemBlockNoWaitGet(tMemBlock* memBlock, void** mem)
{
	uint32_t status = tTaskEnterCritical();
	
	if(tListCount(&memBlock->blockList) > 0) //有多余的block
	{
		*mem = (uint8_t*)tListRemoveFirst(&memBlock->blockList); //从tList的头部移除这个block, 把地址赋给mem
		
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	
	tTaskExitCritical(status);
	return tErrorResourceUnavailable;
}

//8.2 释放block, 也就是task用完了
void tMemBlockNotify(tMemBlock* memBlock, uint8_t* mem)
{
	uint32_t status = tTaskEnterCritical();
	
	if(tEventWaitCount(&memBlock->eventECB) > 0)//说明有task在等
	{
		tTask* task = tEventWakeUp(&memBlock->eventECB, (void*)mem, tErrorNoError); //将block的地址给mem, 这个是当做msg来传入的. task->eventMsg = msg;, 所以之后block的地址会存到task->eventMsg中
		if(task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	else
	{
		tListAddLast(&memBlock->blockList, (tNode*)mem); //将mem作为tNode的指针, 插入tList的末尾
	}
	tTaskExitCritical(status);
}

//8.3 保存状态
void tMemBlockGetInfo(tMemBlock* memBlock, tMemBlockInfo* info)
{
	uint32_t status = tTaskEnterCritical();
	
	info->count = tListCount(&memBlock->blockList);
	info->maxBlockCount = memBlock->maxBlockCount;
	info->blockSize = memBlock->blockSize;
	info->taskCount = tEventWaitCount(&memBlock->eventECB);
	
	tTaskExitCritical(status);

}

//8.3 删除存储区
uint32_t tMemBlockDestory(tMemBlock* memBlock)
{
	uint32_t status = tTaskEnterCritical();

	uint32_t count = tEventRemoveAll(&memBlock->eventECB, (void*)0, tErrorDel);
	tTaskExitCritical(status);
	
	if(count > 0)//说明有新的task加入了就绪队列
	{
		tTaskSched();
	}
	return count;
}




