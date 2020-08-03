//6.1 定义信号量
#include "tSem.h"

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
