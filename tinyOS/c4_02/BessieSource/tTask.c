//����ļ�, ��Ҫ�Ǹ�������ص�
#include "tinyOS.h"

//3.4 �������ȼ�
void taskInit(tTask* task, void (*func)(void*), void* param, uint32_t prio, tTaskStack* stack )
{
	//��һ������: ��Ҫ��ʼ�����Ǹ�task�ĵ�ַ
	//#2: task��Ҫִ�еĺ���func
	//#3: �ú���func�Ĳ���param
	//#4: task��Ҫ��stack�ĵ�ַ
	
	//2.2 ����: ��RunFirst()����pendSV�ж� -> ��task��stack�����Ԫ��,��ֵ��register -> �����pspָ��stack��ջ�� -> pendSV����psp�Զ���stack����ʣ���Ԫ�����δ�ŵ�register��
	//����,���Ǿ���Ҫ��task��ʼ�����������taskInit()��,�Ͱ�stack�����Ԫ�����
	
	//����Ǹ�pendsv�Զ������Ǵ�stack����ȡ��,��register��.��Ϊ�Ǻ���pop,������push
	//ע��!pendsv�Զ����register,�������Ĺ̶���˳���,����������stack��ֵ��ʱ��,��Ҫ��˳��Ҳ�ǹ̶���
	//��stack -= 1, Ҳ���ǵ�ַ-4(��Ϊstack��tTaskStack���͵�ָ��,tTaskStack������uint32_t)
	//��Ϊstack�����ֵ,���Ҫ��register,register��Ķ���unsigned long,���Լǵ�ǿ������ת��
	*(--stack) = (unsigned long) 1 << 24; //��xPSP
	*(--stack) = (unsigned long) func; //��PC,Ҳ����R15. ���ﲻд��&func,�Ҿ�������Ϊ����func����һ��ָ��(һ������ָ��),�൱��func��ֵ���ǵ�ַ,ͬʱPC��Ҫ��Ҳ�ǵ�ַ. ע��,��һ��,�Ϳ�����֮���PCָ����ں���,���Ե�����PendSV_Handler֮��,�ͻ�ִ��funcָ��ĺ���(Ҳ��������ִ�еĺ���)
	*(--stack) = (unsigned long) 0x14; //��LR,Ҳ����R14,�������ﲢû���õ�,�����⸶һ��ֵ,һ��16���Ƶ�ֵ.ע������0x14ֻ��һ���ֽ�,����ǿ������ת����. todo: �������ǽ�LR���ó���0x04��?Ϊʲô��������0x14?�᲻�ᱻ����?
	*(--stack) = (unsigned long) 0x12;
	*(--stack) = (unsigned long) 0x3;
	*(--stack) = (unsigned long) 0x2;
	*(--stack) = (unsigned long) 0x1;
	*(--stack) = (unsigned long) param; //��R0�Ĵ���,������Ĭ��:R0�Ĵ�����ľ���PCָ��ĺ�������ڲ�����ֵ,��Ȼparam��һ��void*����,��������ǿ������ת����. ��Ϊ����ֻ�ǰ�param��ֵ����ȥ,������ô�������param��ֵ,��func������������
	
	//����,��ջ���Ǹ�R4-R11��,��Ϊ���ʼpop,�����������push
	//ע��,��Ϊ����֮��д��asm������,ʹ����LDMIA R0!, {R4-R11}, ��������pop��R4,�������һ��push���Ǹ�R4��ֵ0x4
	*(--stack) = (unsigned long) 0x11;
	*(--stack) = (unsigned long) 0x10;
	*(--stack) = (unsigned long) 0x9;
	*(--stack) = (unsigned long) 0x8;  
	*(--stack) = (unsigned long) 0x7;
	*(--stack) = (unsigned long) 0x6;
	*(--stack) = (unsigned long) 0x5;
	*(--stack) = (unsigned long) 0x4;
	
	//����stack֮��,���ǾͰ�ָ��ջ����ָ��stack,��ֵ��task��stack��Ա
	task->stack = stack; //��Ϊtask->stack��tTaskStack*,����stackҲ��TaskStack*, ��Ϊ�����task��ָ��(��ַ),�����൱�ڵ�ַ->stack
	task->delayTicks = 0; //2.4, ��ΪtinyOS.h����task�ṹ������˳�ԱdelayTicks, ��������ҲҪд��
	task->prio = prio; //3.4
	
	//3.6 ��ʱ�ڵ� �� state�ĳ�ʼ��
	task->state = TINYOS_TASK_STATE_READY; // �տ�ʼ���Ǿ���״̬, û����ʱ
	tNodeInit(&(task->delayNode)); //ע��, delayNode��һ��tNode, ����tNodeInit(tNode*), ��������Ҫȡ��ַ
	
	//3.7 ��ʼ��ʱ��Ƭ
	task->slice = TINYOS_SLICE_MAX; //Ҳ����˵, �������task����ִ����ô����ʱ����
	
	//3.7 ��ʼ��linkNode
	tNodeInit(&(task->linkNode)); //��Ϊtask��tTask*, Ȼ��linkNode��tNode, ������Ҫ�Ĳ�����tNode*, ���Լǵü���&
	
	//4.1 ɾ������������:
	/*
	//3.7 ��linkNode����taskTable�Ķ�ӦԪ���� �൱��֮ǰ�� taskTable[prio] = task //3.4 ��ʼ��һ��task֮��, Ҫ��task�ŵ�taskTable����Ӧλ��
	//��ǰ��ɾ��: taskTable[prio] = task; //ע��,�����task�������һ����ַ,���Բ�Ҫд�� = &task;
	tListAddFirst(&(taskTable[task->prio]), &(task->linkNode)); //��Ϊ��һ��������tLink*, �ڶ���������tNode*, ���Զ��Ǵ����ַ
	
	//3.4 Ҫ���������е���Ӧλ�����ó�1
	tBitmapSet(&taskPrioBitmap, prio);
	*/
	
	
	//4.1 ɾ��������������, ������tTaskSchedReady()��ʵ��, �ô�: ���ǲ���Ҫ��tTask.c������taskTable, ���ǵ�����TaskSchedReady()���ǾͿ���ʹ��taskTable
	//3.7 �޸� //3.6 �����������: ��task����ʱ����ɾ��, ���ǾͰ�task�����������: Ҳ���Ǽ���taskTable, ��������tBitmap
	tTaskSchedReady(task);
	//������void tTaskSchedReady(tTask* task);�Ķ���
	/*
	{
		//3.7
		tListAddFirst(&(taskTable[task->prio]), &(task->linkNode)); //��һ��������tList*, �ڶ�����tNode*
		//3.7 ����������task��tasktable[]�еĵ�һ��task, ��Ҫ��bitmap�Ķ�Ӧλ�����ó�1
		if(tListCount(&taskTable[task->prio]) == 1) //�ҵ�bug, ��==1, ˵���ռӽ�ȥ���ǵ�һ��!
		{
			tBitmapSet(&taskPrioBitmap, task->prio);
		}
	}
	*/
	
	//4.1 ��ʼ���������
	task->suspendCount = 0; //���û��д��һ��, Ҳ�ǲ�������, ��Ϊ���ǵ�task��ȫ�ֱ���, ȫ�ֱ����ṹ����ĳ�Ա��ֵ, Ĭ����0
	
	//4.2 ��ʼ�� clean �� param
	task->clean = (void (*) (void*) ) 0; //����Ϊ��
	task->cleanParam = (void *) 0;
	task->requestDeleteFlag = 0;
	

}


//4.1 ���ù�����
void tTaskSuspend(tTask* task) //��Ҫ���������
{
	//�����ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//�����������ʱ״̬, ���ܹ���. ����: ֻ���ھ��������е�ʱ��, ���ܹ���
	if(!(task->state & TINYOS_TASK_STATE_DELAYED))
	{
		//����ǵ�һ�α�����, �ͽ�״̬λ���óɹ���. (����ǵڶ��ε����α�����, ״̬λ�Ѿ���������, �Ͳ���Ҫ�ظ�����)
		if(++task->suspendCount <= 1) //������һ�����ص�д��, Ҳ�������Ǳ�������˵���� == 1
		{
			//����
			task->state |= TINYOS_TASK_STATE_SUSPEND;
			
			//��task�Ӿ����б����Ƴ�, ע��: �Ƴ�֮��, task�Ȳ������, Ҳ��������(����������Ҫ�ھ��������Ŷ�), task���ھ���һ������״̬��, ��ʦ˵��, ���������һ���������tList������еĹ����task, ������ʦ����û��ʵ��
			tTaskSchedUnReady(task);
			
			//�����������ǵ�ǰ����, ���л�����������
			if(task == currentTask)
			{
				tTaskSched();
			}
		}
		
	}
	
	tTaskExitCritical(status);
}

//4.1 �ӹ��𵽻ָ��ĺ���, ���Իָ������л��߾���
void tTaskWakeUp(tTask* task)
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//���ж�task�Ƿ��ǹ���״̬
	if(task->state & TINYOS_TASK_STATE_SUSPEND)
	{
		//����-1, ���-1֮��, ��������� == 0, �ָ�֮ǰ��״̬: ����
		if(--task->suspendCount == 0)
		{
			task->state &= ~(TINYOS_TASK_STATE_SUSPEND);
			
			//�����������
			tTaskSchedReady(task); //�Ա�: tTaskSchedUnReady(task);�ǽ�task�Ӿ����б����Ƴ�
			
			//����: Ҳ����˵������task��������ȼ���ͷ��task, �����task�ͻ�������, ������ǵĻ�, ���task�����Ǿ���״̬
			tTaskSched();
		}
	}
	tTaskExitCritical(status);
}	

//4.2 ����Ļص�����
void tTaskSetCleanCallFunc(tTask* task, void (*clean) (void* param), void* param)
{
	task->clean = clean;
	task->cleanParam = param;
}	

//4.2 ǿ��ɾ������
void tTaskForceDelete(tTask* task)
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//�����Ƿ�����ʱ������, Ȼ���ж��ǲ����ڹ���״̬, ���������, task���Ǿ���. ��֮task��Ҫ�������ܴ��ڵ��κ�һ��(����/��ʱ)������ɾ��. ����ǹ���״̬, ��ֱ��ʲô������
	if(task->state &= TINYOS_TASK_STATE_DELAYED) //˵����ʱ������
	{
		tTimeTaskRemove(task);
	}
	else if( !(task->state &= TINYOS_TASK_STATE_SUSPEND) ) //˵�����ǹ���, �Ǿ�ֻ�����ھ���������
	{
		tTaskSchedRemove(task); //ע��, ���ﲻ��tTaskSchedUnReady(), ��Ȼ������ʵ�ֶ�һ��
	}
	
	//����task��clean����, Ϊ���Ͻ�, ���жϴ治�������clean����
	if(task->clean) //����clean����
	{
		task->clean(task->cleanParam); //���øú���
	}
	
	//���task���ǵ�ǰ���е�����, ���л�����������: �Ҿ����������������if, ��������, �Լ�ɾ���Լ���
	if(task == currentTask)
	{
		tTaskSched();
	}
	
	tTaskExitCritical(status);
}

//4.2 ����ɾ���Ľӿ�, ����task������ɾ�����
void tTaskRequestDelete(tTask* task)
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//���ñ��
	task->requestDeleteFlag = 1; //˵��Ҫ��ɾ��
	
	tTaskExitCritical(status);
}

//4.2 ���task�Ƿ���ɾ������, ���ҷ��ؽ����������(������Ӧ�þ���task�Լ�, ��Ϊ��������д����currentTask->xxx)
uint8_t tTaskIsRequestedDeleted(void)
{
	uint8_t result; 
	
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	result = currentTask->requestDeleteFlag;
	
	tTaskExitCritical(status);

	return result;
}

//4.2 ɾ��task�Լ��ĺ���(������Ӧ�þ���task�Լ�, ��Ϊ��������д����currentTask->xxx)
void tTaskDeleteSelf(void)
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//��Ϊ��ɾ���Լ�, ˵����currentTask�����Լ�(��Ϊֻ��currentTask���ܹ�����, �Ż��ߵ������������)
	tTaskSchedRemove(currentTask); //��ΪcurrentTask���Լ�, ����˵���Լ����ھ���������
	
	//curerntTask�����ܴ�����ʱ״̬, ����������ʱ������
	
	//����������
	if(currentTask->clean)
	{
		currentTask->clean(currentTask->cleanParam);
	}
	
	//����һ��Ҫ�ǵ�, �л�����������
	tTaskSched();
	
	tTaskExitCritical(status);
}	




