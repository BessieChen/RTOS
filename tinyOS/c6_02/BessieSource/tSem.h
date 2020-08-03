//6.1 添加信号量的结构定义
#ifndef TSEM_H
#define TSEM_H

//6.1 包括tEvent的头文件
#include "tEvent.h"

typedef struct _tSem
{

	tEvent eventECB;
	uint32_t count;
	uint32_t maxCount;
}tSem;

void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount); //6.1
uint32_t tSemWait(tSem* sem, uint32_t waitTicks);//6.2 没有资源就等
uint32_t tSemNoWaitGet(tSem* sem);//6.2 没有资源就不等
void tSemNotify(tSem* sem);//6.2 我不要资源了, 谁要
#endif
