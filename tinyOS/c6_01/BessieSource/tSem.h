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

void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount);

#endif
