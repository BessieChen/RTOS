#include "tFlagGroup.h"

//9.1
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags)
{
	tEventInit(&flagGroup->eventECB, tEventTypeFlagGroup);
	flagGroup->flag = flags;
}

//9.2 ����������¼�, �Ƿ���Ҫ����Щ�¼������ĵ�
static uint32_t tFlagGroupCheck;

//9.2 �ȴ�����
uint32_t tFlagGroupWait(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag, uint32_t waitTicks)//waitType: �ȴ�������, requestFlag:�ȴ���Щ��ʶ, resultFlag(ע���ǵ�ַ, uint32_t*)���ȵ�����Щ��ʶ, waitTick:��ʱ
{

	
}

//9.2 ��ȡ�¼���ʶ
uint32_t tFlagGroupNoWaitGet(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t resultFlag)//todo, ���������resultFlag�ֲ��ǵ�ַ��
{

}

//9.2 ֪ͨ��Ҫ�ı�־λ������
void tFlagGroupMptify(tFlagGroup* flgGroup, uint8_t isSet, uint32_t flag)
{

}



