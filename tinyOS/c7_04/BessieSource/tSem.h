//6.1 ����ź����Ľṹ����
#ifndef TSEM_H
#define TSEM_H

//6.1 ����tEvent��ͷ�ļ�
#include "tEvent.h"

//6.1
typedef struct _tSem
{

	tEvent eventECB;
	uint32_t count; //ʣ�����Դ����
	uint32_t maxCount;
}tSem;

//6.3 
typedef struct _tSemInfo
{
	uint32_t count;//ʣ�����Դ����
	uint32_t maxCount;//��Դһ���ж���
	uint32_t taskCount;//�ж��ٸ�task�ڵ���Դ
}tSemInfo;

void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount); //6.1
uint32_t tSemWait(tSem* sem, uint32_t waitTicks);//6.2 û����Դ�͵�
uint32_t tSemNoWaitGet(tSem* sem);//6.2 û����Դ�Ͳ���
void tSemNotify(tSem* sem);//6.2 �Ҳ�Ҫ��Դ��, ˭Ҫ
void tSemGetInfo(tSem* sem, tSemInfo* info); //6.3
uint32_t tSemDestory(tSem* sem); //6.3 ����tSem���ж��ٸ�task�ڵȴ�, ɾ����, �������ֵ˵�����ж��ٸ�task�������˾�������
#endif
