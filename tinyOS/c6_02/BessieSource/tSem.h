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

void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount); //6.1
uint32_t tSemWait(tSem* sem, uint32_t waitTicks);//6.2 û����Դ�͵�
uint32_t tSemNoWaitGet(tSem* sem);//6.2 û����Դ�Ͳ���
void tSemNotify(tSem* sem);//6.2 �Ҳ�Ҫ��Դ��, ˭Ҫ
#endif
