//6.1 �����ź���
#include "tSem.h"
#include "tinyOS.h" //�������ٽ����Ķ���

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

//6.2 ���������currentTask����, �ж��Ƿ��ܹ�ʹ��sem���������Դ, ����ܵĻ�, ����ֵ��noError. ������ܵĻ�, ��currentTask����ȴ�������, Ȼ�󷵻�ֵ��currentTask�ĵȴ����
uint32_t tSemWait(tSem* sem, uint32_t waitTicks)
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	if(sem->count > 0) //˵��sem�������Դ����ʣ��, ������ //bug!!!! ��sem->count > 0 ����&sem->count > 0
	{
		--sem->count;
		tTaskExitCritical(status);//�˳��ٽ���
		return tErrorNoError; //����û�д���
	}
	else
	{
		tEventWait(&sem->eventECB, currentTask, (void*)0, tEventTypeSem, waitTicks); //void tEventWait(tEvent* eventECB, tTask* task, void* msg, uint32_t state, uint32_t timeout) //msg:��������Ϣ, state:�ȴ�״̬, timeout:�û����õĳ�ʱʱ��
		//�����tEventTypeSem�����currentTask��state��, ע��tEventTypeSemֻ��һ��bit, ˵��curretTask�ڵȴ�һ��sem�������Դ, ͬʱwaitTicks˵��currentTaskҲ��������ʱ����
		
		tTaskExitCritical(status);//�˳��ٽ���
		
		tTaskSched();//��һ�����˳��ٽ���֮��, ��ΪtTaskSched()��������Ҳ���Ƚ����ٽ���. tTaskSched()���л�������task
		
		return currentTask->waitEventResult; //ע��, ��ΪtTaskSched()�ᵽ������֪������ĵط�, ��������ǻ����, �ص���һ���ʱ��, ����֪�����currentTask�ĵȴ����: Ҫô�ǵȵ���, Ҫô��û�ȵ�ֻ�����ν�����
	}
}

//6.2 �������Դ, ����. û�еĻ�, taskҲ��ȥ��
uint32_t tSemNoWaitGet(tSem* sem)
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	if(sem->count > 0)
	{
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else{
		tTaskExitCritical(status);
		return tErrorResourceUnavailable; //������Դ������, ���Ӳ�����
		
	}
}

//6.2 ֪ͨ�ӿ�, ��������û��taskҪ�Ҳ��õ���Դ
void tSemNotify(tSem* sem)
{
	uint32_t status = tTaskEnterCritical();
	
	//����û��task��sem�ĵȴ��������
	if(tEventWaitCount(&sem->eventECB) > 0) //��task�ڵ�
	{
		tTask* task = tEventWakeUp(&sem->eventECB, (void*)0, tErrorNoError); //tTask* tEventWakeUp(tEvent* eventECB, void* msg, uint32_t result)//result: ���浽task->waitEventResult: ���ѵĽ��
		
		//�������task�����ȼ���������currentTask, �л�����task
		if(task->prio < currentTask->prio) //ע��, prioԽС, ���ȼ�Խ��
		{
			tTaskSched();
		}
	}
	else //˵��ûtask�ڵ�
	{
		++sem->count;
		//���ж�count�ĺϷ���
		if(sem->maxCount != 0 && (sem->count > sem->maxCount))
		{
			sem->count = sem->maxCount;
		}
	}
	
	tTaskExitCritical(status);
}


