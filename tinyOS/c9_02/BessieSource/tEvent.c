#include "tinyOS.h"

//5.2 ��ʼ��
void tEventInit(tEvent* eventECB, tEventType type) //5.2 ����typeû���õ�ѽ..
{
	eventECB->type = tEventTypeUnknown;
	tListInit(&(eventECB->waitList));
}

//5.2 ��task����ECB�ĵȴ�����
void tEventWait(tEvent* eventECB, tTask* task, void* msg, uint32_t state, uint32_t timeout) //msg:��������Ϣ, state:�ȴ�״̬, timeout:�û����õĳ�ʱʱ��
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//5.2 ����ṹ�Ĳ���
	task->state |= state; //�����û�|,����Ϊ����ֻ������һ��bit, �����bit�����Ժ��������
	task->waitEvent = eventECB; //�����㿴, �������һ��˫�������, Ҳ����
	task->eventMsg = msg;
	task->waitEventResult = tErrorNoError;
	
	//5.2 ��task�Ӿ��������Ƴ�
	tTaskSchedUnReady(task);
	
	//5.2 ��task����ECB��β��. ֮���Բ��뵽β��, ����Ϊ���ǻ����ǻ��ѵ�һ��
	tListAddLast(&eventECB->waitList, &task->linkNode); 
	
	//5.2 ����Ƿ������ó�ʱ, �еĻ�, ��Ҫ������ʱ������
	if(timeout)
	{
		tTimeTaskWait(task, timeout); //����ǽ�task������ʱ����, ��ʱtimeout��systick
	}
	
	tTaskExitCritical(status);
}

//5.2 ��task��ECB�ĵȴ������л���, ע��, ����ֻ�ǻ��ѵ�һ��task, ������ָ��ĳ��task //����: ˭����� �������? ��Ϊ,�ȴ��¼��������,���ǻỽ��ָ����task,�����ǻ��ѵ�һ��ѽ
tTask* tEventWakeUp(tEvent* eventECB, void* msg, uint32_t result)//msg:�ᴫ����Ϣ��ȥ, result: �����������Ǹ���task���Ǵ�eventECB���Ƴ���ԭ����ʲô
{
	tNode* node;
	tTask* task = (tTask*) 0;
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//���Ѷ���ͷ����task
	node = tListRemoveFirst(&eventECB->waitList);
	if(node != (tNode*)0) //˵��list������һ��node
	{
		//�ҵ���Ӧ��task
		task = tNodeParentAddr(tTask, linkNode, node);
		
		//��task��״̬�޸�
		task->waitEvent = (tEvent*)0; //�൱����Ϊtask����Ҫ�ȴ��¼���
		task->eventMsg = msg;
		task->waitEventResult = result;
		
		//��task��state�޸�, Ҳ���ǽ����е�ǰ���16λ�����. ������ʱ���ʲô��Ӧ�û���
		task->state &= ~TINYOS_TASK_WAIT_MASK; 				//(0xFF<<16) //5.2 Ҳ����1111 1111 0000 0000, ����ȡ��֮��ĺ�16λ����0000 0000 1111 1111
		
		//����֮ǰ��task������ʱ������, Ҳ����֮ǰ������timeOut, ��ô����task��ͬʱ, ��Ҫǿ�Ƶ�! ��task����ʱ���������
		//���delayticks == 0, �����������task����ʱ������ɾ��. ��֮��Ҫɾ��
		//���delayticks == 0, ��һ�����ܾ;���, ������û�м������ʱ����
		if(task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);
		}
		
		//����task������task�������������
		//ע������û�е���, ��Ϊ���ȵĲ����������������������
		tTaskSchedReady(task);
	}
	
	tTaskExitCritical(status);
	
	return task;
}


//5.2 ��task��ECB��ǿ���Ƴ�, ��ɾ��ָ����task
void tEventRemoveTask(tTask* task, void* msg, uint32_t result) //�����������Ǹ���task���Ǵ�eventECB���Ƴ���ԭ����ʲô
{
	uint32_t status = tTaskEnterCritical();
	
	//�Ƴ��ض�������, ����tEventWake()���Ƴ�ECB��ͷ������
	tListRemove(&task->waitEvent->waitList, &task->linkNode); //�������ҵ�task���ڵ�ECB:task->waitEvent->waitList, Ȼ���ECB�л��waitList
	
	//��task��״̬�޸�
		task->waitEvent = (tEvent*)0; //�൱����Ϊtask����Ҫ�ȴ��¼���
		task->eventMsg = msg;
		task->waitEventResult = result;
		task->state &= ~TINYOS_TASK_WAIT_MASK; 
	
	//����û�����ж��Ƿ�����ʱ
	//��Ϊ��������Ǹ�sysick��handler���õ�,���Ե�systick����ʱ-1֮��, systick��handler���ж��Ƿ����ʱ�������
	
	tTaskExitCritical(status);

}


//5.3 ���eventECB�е�����task,(������task������ʱ����, Ҳ����ʱ������ɾ��) �����������·������������. ����ֵ: eventECB�ڱ����֮ǰ�ж���task. Ҳ����˵����ֵ��: �ж���task������(���뵽�˾���������)
uint32_t tEventRemoveAll(tEvent* eventECB, void* msg, uint32_t result) //�����������Ǹ���task���Ǵ�eventECB���Ƴ���ԭ����ʲô
{
	uint32_t ret = 0;
	tNode* node = (tNode*) 0;
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	ret = tListCount(&eventECB->waitList);
	
	//�Ƴ�ͷ����task
	node = tListRemoveFirst(&eventECB->waitList);
	while(node != (void*)0)
	{
		tTask* task = tNodeParentAddr(tTask, linkNode, node);
		
		task->waitEvent = (tEvent*)0;
		task->eventMsg = msg;
		task->waitEventResult = result;
		task->state &= ~TINYOS_TASK_WAIT_MASK;

		//����ʱ������ɾ��
		if(task->delayTicks != 0)
		{
			tTimeTaskWakeUp(task);
		}			
		
		//�����������
		tTaskSchedReady(task);
		
		//��һ��һ��while��׼��
		node = tListRemoveFirst(&eventECB->waitList);
	}
	
	tTaskExitCritical(status);

	return ret;
	
}	

//5.3 ��ȡECB�е�task����
uint32_t tEventWaitCount(tEvent* eventECB)
{
	uint32_t ret = 0;
	
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	ret = tListCount(&eventECB->waitList);
	
	tTaskExitCritical(status);
	
	return ret;
}

//9.2 ��task��ECB�ĵȴ������л���, ע��, ����ָ��ĳ��task //����: ˭����� �������? ��Ϊ,�ȴ��¼��������,���ǻỽ��ָ����task,�����ǻ��ѵ�һ��ѽ
tTask* tEventWakeUpSpecificTask(tEvent* eventECB, tTask* task, void* msg, uint32_t result)//msg:�ᴫ����Ϣ��ȥ, result: �����������Ǹ���task���Ǵ�eventECB���Ƴ���ԭ����ʲô
{
	tNode* node;
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	//9.2 ɾ��ָ����task
	tListRemove(&eventECB->waitList, &task->linkNode);
	
	//�ҵ���Ӧ��task
	task = tNodeParentAddr(tTask, linkNode, node);
	
	//��task��״̬�޸�
	task->waitEvent = (tEvent*)0; //�൱����Ϊtask����Ҫ�ȴ��¼���
	task->eventMsg = msg;
	task->waitEventResult = result;
	
	//��task��state�޸�, Ҳ���ǽ����е�ǰ���16λ�����. ������ʱ���ʲô��Ӧ�û���
	task->state &= ~TINYOS_TASK_WAIT_MASK; 				//(0xFF<<16) //5.2 Ҳ����1111 1111 0000 0000, ����ȡ��֮��ĺ�16λ����0000 0000 1111 1111
	
	//����֮ǰ��task������ʱ������, Ҳ����֮ǰ������timeOut, ��ô����task��ͬʱ, ��Ҫǿ�Ƶ�! ��task����ʱ���������
	//���delayticks == 0, �����������task����ʱ������ɾ��. ��֮��Ҫɾ��
	//���delayticks == 0, ��һ�����ܾ;���, ������û�м������ʱ����
	if(task->delayTicks != 0)
	{
		tTimeTaskWakeUp(task);
	}
	
	//����task������task�������������
	//ע������û�е���, ��Ϊ���ȵĲ����������������������
	tTaskSchedReady(task);
	
	tTaskExitCritical(status);
	
	return task;
}


