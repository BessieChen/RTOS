//6.1 ����ź����Ľṹ����
#ifndef TSEM_H
#define TSEM_H

//6.1 ����tEvent��ͷ�ļ�
#include "tEvent.h"

typedef struct _tSem
{

	tEvent eventECB;
	uint32_t count;
	uint32_t maxCount;
}tSem;

void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount);

#endif
