//6.1 �����ź���
#include "tSem.h"

//6.1 ��ʼ��
void tSemInit(tSem* sem, uint32_t startCount, uint32_t maxCount)//maxCount==0ʱ��˵��Countû������, ��󶼿���
{
	tEventInit(&sem->eventECB, tEventTypeSem);
	
	sem->maxCount = maxCount;
	
	if(maxCount == 0) //maxCount==0ʱ��˵��Countû������, ��󶼿���
	{
		sem->count = startCount; //�������ﲻ��Ҫ�ж�startCount�ǲ��ǺϷ���
	}
	else//�����count������,����Ҫ�ж�����startcount�Ƿ�Ϸ�
	{
		sem->count = (startCount <= maxCount) ? startCount : maxCount;
	}
}
