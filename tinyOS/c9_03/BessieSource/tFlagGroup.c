#include "tFlagGroup.h"
#include "tinyOS.h" //������tErrorNoError

//9.1
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags)
{
	tEventInit(&flagGroup->eventECB, tEventTypeFlagGroup);
	flagGroup->flag = flags;
}

//9.2 ��������. ����������¼�, �Ƿ���Ҫ����Щ�¼������ĵ� //�����static����, ˵��ֻ���������.c�ļ�, ����û����.h�ļ���������
static uint32_t tFlagGroupCheckAndConsume(tFlagGroup* flagGroup, uint32_t type, uint32_t* flag) //type:���ǵ�����, ��TFLAGGROUP_SET_ALL, TFLAGGROUP_CLEAR_ALL, TFLAGGROUP_CONSUME. flag:�����Ǵ���ַȡֵ, Ҳ���ǰѷ������¼��ĵ�ַ�浽flag��
{
	//ȡ������ֵ
	uint32_t srcFlag = *flag; //ȡ��ԭʼ��, �����ڴ��ı�ʶ
	
	uint32_t isSet = type & TFLAGGROUP_SET;	//����������� ���� ���� ���
	uint32_t isAll = type & TFLAGGROUP_ALL;	//�ǲ��ǵȴ����еı�־ 
	uint32_t isConsume = type & TFLAGGROUP_CONSUME; //������ʶ��, �ǲ��ǽ���Щ��ʶ���ĵ�
	
	//������Щ��־λ������
	uint32_t calFlag = isSet ? (flagGroup->flag & srcFlag) : (~flagGroup->flag & srcFlag);
	
	//�����������, ���Ƿ�Ҫ����
	if(  ((isAll != 0) && (calFlag == srcFlag))  ||  ((isAll == 0) && (calFlag != 0))  )
	{
		if(isConsume)
		{
			if(isSet)
			{
				flagGroup->flag &= ~srcFlag;
			}
			else
			{
				flagGroup->flag |= srcFlag;
			}
		}
		*flag = calFlag;
		return tErrorNoError;
	}
	
	*flag = calFlag;
	return tErrorResourceUnavailable;
}

//9.2 �ȴ�����
uint32_t tFlagGroupWait(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag, uint32_t waitTicks)//waitType: �ȴ�������, requestFlag:�ȴ���Щ��ʶ, resultFlag(ע���ǵ�ַ, uint32_t*)���ȵ�����Щ��ʶ, waitTick:��ʱ
{
	//���
	uint32_t result;
	uint32_t flags = requestFlag;
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	result = tFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	//����¼��Ƿ��Ѿ�����
	if(result != tErrorNoError)
	{
		currentTask->waitFlagsType = waitType;
		currentTask->eventFlags = requestFlag;
		tEventWait(&flagGroup->eventECB, currentTask, (void*) 0, tEventTypeFlagGroup, waitTicks);
		
		tTaskExitCritical(status);
		
		tTaskSched();
		
		*resultFlag = currentTask->eventFlags; //���������tFlagGroupNotify()�л��, task->eventFlags = flags; //����ͻ᷵�ظ�tFlagGroupWait()�и�*resultFlag = currentTask->eventFlags;��һ��
		result = currentTask->waitEventResult;
	}
	else
	{
		*resultFlag = flags;
		tTaskExitCritical(status);
	}
	return result;
	
	
}

//9.2 ��ȡ�¼���ʶ
uint32_t tFlagGroupNoWaitGet(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag)
{
	uint32_t flags = requestFlag;
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//���: �¼���־
	uint32_t result = tFlagGroupCheckAndConsume(flagGroup ,waitType, &flags);
	tTaskExitCritical(status);
	
	*resultFlag = flags;
	return tErrorNoError;
}

//9.2 ֪ͨ��Ҫ�ı�־λ������
void tFlagGroupNotify(tFlagGroup* flagGroup, uint8_t isSet, uint32_t flag)
{
	//���������ָ�� �� �ڵ��ָ��
	tList* waitList;
	tNode* node;
	tNode* nextNode;
	uint32_t result;
	uint8_t sched; //�Ƿ���Ҫ����
	
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	if(isSet)
	{
		flagGroup->flag |= flag;
	}
	else
	{
		flagGroup->flag &= ~flag;
	}
	
	//��ECB�е�task���д���:
	//01. ȡ��ECB�е��б�
	waitList = &flagGroup->eventECB.waitList;
	//02. ����
	for(node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode)
	{
		//ȡ������
		tTask* task = tNodeParentAddr(tTask, linkNode, node);
		
		//���������ȴ��ı�ʶ
		uint32_t flags = task->eventFlags;
		
		//��һ���ڵ�
		nextNode = node->nextNode;
		
		//��task�ȴ����¼���ʶ�Ƿ�����
		result = tFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
		
		//�������
		if(result == tErrorNoError)
		{
			task->eventFlags = flags; //����ͻ᷵�ظ�tFlagGroupWait()�и�*resultFlag = currentTask->eventFlags;��һ��
			tEventWakeUpSpecificTask(&flagGroup->eventECB, task, (void*)0, tErrorNoError);//��task��ECB�л���
			sched = 1; 
		}
	}
	
	if(sched)
	{
		tTaskSched();
	}
	tTaskExitCritical(status);
}



