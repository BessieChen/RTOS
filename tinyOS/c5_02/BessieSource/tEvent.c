#include "tinyOS.h"

//5.2 ��ʼ��
void tEventInit(tEvent* eventECB, tEventType* type) //5.2 ����typeû���õ�ѽ..
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
tTask* tEventWake(tEvent* event, void* msg, uint32_t result)//msg:�ᴫ����Ϣ��ȥ, result: ���ѵĽ��
{
	tNode* node;
	tTask* task = (tTask*) 0;
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//���Ѷ���ͷ����task
	node = tListRemoveFirst(&event->waitList);
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
void tEventRemoveTask(tTask* task, void* msg, uint32_t result)
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



