//6.1 添加信号量的结构定义
#ifndef TSEM_H
#define TSEM_H

//6.1 包括tEvent的头文件
#include "tEvent.h"

//6.1
typedef struct _tSem
{

	tEvent eventECB;
	uint32_t count; //剩余的资源个数
	uint32_t maxCount;
}tSem;

//6.3 
typedef struct _tSemInfo
{
	uint32_t count;//剩余的资源个数
	uint32_t maxCount;//资源一共有多少
	uint32_t taskCount;//有多少个task在等资源
}tSemInfo;

void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount); //6.1
uint32_t tSemWait(tSem* sem, uint32_t waitTicks);//6.2 没有资源就等
uint32_t tSemNoWaitGet(tSem* sem);//6.2 没有资源就不等
void tSemNotify(tSem* sem);//6.2 我不要资源了, 谁要
void tSemGetInfo(tSem* sem, tSemInfo* info); //6.3
uint32_t tSemDestory(tSem* sem); //6.3 返回tSem中有多少个task在等待, 删除后, 这个返回值说明了有多少个task被加入了就绪队列
#endif
