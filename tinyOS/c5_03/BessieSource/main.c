#include "tinyOS.h"
#include "ARMCM3.h" //������SysTick

//3.4 ����: ���������, taskTable
tBitmap taskPrioBitmap;



//3.6 taskTable����tList������, Ҳ����ÿ��Ԫ�ض���tList //3.4 taskTable����������TINYOS_PRIO_COUNT��Ԫ��: tTask* taskTable[TINYOS_PRIO_COUNT]; 
tList taskTable[TINYOS_PRIO_COUNT]; 


//2.1 ��������tinyOS.h��,���Ǿ���������stack,������������task, ����������taskִ�еĺ���
tTaskStack idleTaskEnv[TINYOS_IDELTASK_STACK_SIZE]; //2.4

tTask tTaskIdle; //2.4

//2.2 ��ʼ��������֮��,��Ȼ����Ҫ��������,������Ҫ����current��next task
tTask* currentTask;
tTask* nextTask;
tTask* idleTask; //2.4

//3.6 ������ʱ����
tList tTaskDelayedList; //ע��, ��ʱ�ڵ�delayNode��ÿ��tTask�����еĳ�Ա, ������ʱ����ȴ��main.c�����ȫ�ֱ���. //ע��, �����һ��tList, ������tList*, ��Ϊ����������Ҫ����һ���ṹ��, ������һ����ַ




//3.2 ����������ļ�����, ��uint8, ֻ�е�������==0��ʱ��,�ſ��Դ������Ⱥ���tTaskSched()
uint8_t schedLockCount;

//3.7 ��ʼ��tasktable[] 3.4 ��Ȼ�ǳ�ʼ��������,��������Ҳ������bitmap�ĳ�ʼ��,��Ϊmain()��һ�����õľ����������.  �������������ٽ���:�ٽ�����ʹ��__get_primask()���ر��ж�,Ҳ����ʹ����ARMCM3.h�ļ��ĺ���,���ǵ�����ʹ�����Լ������
void tTaskSchedInit(void)
{
	int i;
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);//3.4
	
	//3.7 ��ʼ��tasktable[]
	for(i = 0; i < TINYOS_PRIO_COUNT; i++)
	{
		tListInit(&taskTable[i]);
	}
}
//3.2 ��ʼʹ�õ�����, Ҳ�����õ��Ⱥ���tTaskSched()ʧЧ
void tTaskSchedDisable(void)//ע���������, ���������Disable,��ΪҪsched()ʧЧ
{
	//��Ϊ������Ҳ��ȫ�ֱ���,����Ϊ���tTaskSchedDisable()����Ҳ�ᱻsystick�жϻ����������(��Ϊ����Ҫʹ���������ȥ����ȫ�ֱ���xxx),����������Ҫ����������,���ٽ�������
	uint32_t status = tTaskEnterCritical();
	
	//��ʼ����������,ע��,��Ϊ��������uint8,�����255, �������==255,���ǾͲ�+1, ��ֹ���
	if(schedLockCount <= 254)
	{
		schedLockCount++;
	}
	tTaskExitCritical(status);
}

//3.4 ������Ҫ�޸�һ�����ǵ�tTaskSched()
void tTaskSched(void)
{
	//3.4 ������ҪtTask*
	tTask* highestPrioTask;
	
	//3.2 Ϊ�˱������, ������Ҫ�жϵ������ǲ���==0,���ȷ����==0,�Ż�ִ�к�������,�����return
	//ͬ��, �жϵ�������ֵ��ʱ��,Ϊ�˱���,�������ٽ���������
	uint32_t status = tTaskEnterCritical();
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
	
	//3.4 ���������޸Ĳ���, �߼��ܼ�, ����ǰ�������ǲ������ȼ���ߵ�,�������,���л������ȼ���ߵ�����
	highestPrioTask = tTaskHighestTaskReady();
	if(highestPrioTask == currentTask)
	{
		tTaskExitCritical(status);
		return;
	}
	nextTask = highestPrioTask;
	tTaskSwitch();
	tTaskExitCritical(status);
}




//3.2 �˳�������, Ҳ�����õ��Ⱥ���tTaskSched()��Ч : ��Ȼ,ֻ�е�������==0��ʱ��,�Ż���tTaskSched()��Ч, ��Ч����˼��:����ִ��tTaskSched(), ����ִ��tTaskSched()�������ȥ��ĵ���,��Ϊ�Ƿ�Ӧ�õ���Ҫ��Ҫ�����Լ���delay�ǲ���==0,���˵�delay�ǲ���==0
//ע��, ���������Ҫ�ŵ�tTaskSched()����,��Ϊ��Ҫʹ�õ�tTaskSched()
void tTaskSchedEnable(void)	
{
	//ͬ��,��Ҫ���ٽ���������������
	uint32_t status = tTaskEnterCritical();
	
	//��ȫ���,���жϵ������ǲ����Ѿ�==0��
	if(schedLockCount > 0)
	{
		schedLockCount--;
	}
	//ֻ�е�������==0��ʱ��,�Ż���tTaskSched()��Ч, ��Ч����˼��:����ִ��tTaskSched(), ����ִ��tTaskSched()�������ȥ��ĵ���,��Ϊ�Ƿ�Ӧ�õ���Ҫ��Ҫ�����Լ���delay�ǲ���==0,���˵�delay�ǲ���==0
	if(schedLockCount == 0)
	{
		tTaskSched();
	}
	tTaskExitCritical(status);
}

//3.1 �����ٽ���
uint32_t tTaskEnterCritical(void)
{
	uint32_t primask = __get_PRIMASK(); //����"ARMCM3.h"��
	__disable_irq(); //"ARMCM3.h"��,�ǹر��ж�
	return primask; //�����ж�ʹ��
}

//3.1 �˳��ٽ���
void tTaskExitCritical(uint32_t primask)
{
	__set_PRIMASK(primask);
}

//_BlockType_t,���Դ洢�ܶ���Ϣ,Ŀǰֻ��stackPtr
typedef struct _BlockType_t
{
	unsigned long* stackPtr;
}BlockType_t;


void delay(int count)
{
	while(count)
	{
		count--;
	}
}



//3.6 ��ʱ����tTaskDelayedList�ĳ�ʼ��, ע������Ĳ�������Ҫ��tList*, ��ΪtTaskDelayedList����ȫ�ֱ���
void tTaskDelayedInit(void)
{
	tListInit(&tTaskDelayedList);
}

//3.6 �����������ʱ����, ������������Ҫ��ʱ���
void tTimeTaskWait(tTask* task, uint32_t ticks)
{
	//task���޸�:
	task->delayTicks = ticks;
	task->state |= TINYOS_TASK_STATE_DELAYED;
	
	//task������ʱ����
	tListAddLast(&tTaskDelayedList, &(task->delayNode)); //ע���Ǽ���task��delayNode��ַ,Ҳ����tNodeָ��
}

//3.6 ��delayTicks == 0֮��, ���������ʱ������ɾ��
void tTimeTaskWakeUp(tTask* task)
{
	//task���޸�
	task->state &= ~(TINYOS_TASK_STATE_DELAYED); //ע������ֻ�Ǹı�ڶ�λ, ���ڶ�λ���ó�0
	
	//task����ʱ����ɾ��
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}

//3.7 �޸� //3.6 �����������: ��task����ʱ����ɾ��, ���ǾͰ�task�����������: Ҳ���Ǽ���taskTable, ��������tBitmap
void tTaskSchedReady(tTask* task)
{
	//3.7
	tListAddFirst(&(taskTable[task->prio]), &(task->linkNode)); //��һ��������tList*, �ڶ�����tNode*
	//3.7 ����������task��tasktable[]�еĵ�һ��task, ��Ҫ��bitmap�Ķ�Ӧλ�����ó�1
	if(tListCount(&taskTable[task->prio]) == 1) //�ҵ�bug, ��==1, ˵���ռӽ�ȥ���ǵ�һ��!
	{
		tBitmapSet(&taskPrioBitmap, task->prio);
	}
}

//3.7 �޸� //3.6 ��task��Ҫ��ʱ, �����������Ҫ��task�Ӿ���������ɾ��
void tTaskSchedUnReady(tTask* task)
{
	//3.7
	tListRemove(&(taskTable[task->prio]), &(task->linkNode)); //��һ��������tList*, �ڶ�����tNode*
	//ע��, ���ﲻҪ��linkNode���뵽taskTable[prio]��ĩβ, ��Ϊ��������ǰ�task�Ӿ�������ɾ��, ������ʲôʱ��Ƭ����ת
	//3.7 �������tasktable����, ��Ҫ��bitmap�Ķ�Ӧλ�����ó�0
	if(tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}
//3.7 ����ʱ��Ƭ��ת�Ĵ���, ��Ϊÿ��systick, ���е�ͬһ�����ȼ������������ʣ��ʱ��Ƭ��Ҫ��һ. 3.6 ��Ҫ�޸�, ��Ϊ������Ҫɨ�������ʱ������, Ҳ����tList tTaskDelayedList. 3.4 ��Ҫ�޸�,ÿ��systick����Ҫ��taskTable�е�ÿһ�������delayTicks��һ // 2.4 ��Ϊ����ÿ��������һ��������delayTicks, �������һ������,�ܹ��ݼ�delayTicks(�൱�����ӳ�)
//����systick����,������������: 1. ��ʱ���е�task����ʱʱ�䶼��һ, 2. �������е�task��ʱ��Ƭ����һ
void tTaskSysTickHandler()
{
	//3.6 ����֮��for loop��Ҫ�õ���:
	tNode* node;
	uint32_t status = tTaskEnterCritical();
	//3.6 ɾ��3.4����:
	/*
	int i;
	for(i = 0; i < TINYOS_PRIO_COUNT; i++) //3.4
	{
		if(taskTable[i] == (tTask*)0) //3.4 �ǵ�Ҫǿ������ת��,�ҷ������ﲻ����null����NULL
		{
			continue; //����,��Ҫ�ж�taskTable���ĸ�Ԫ���ǲ���������
		}
		
		if(taskTable[i]->delayTicks > 0) 
		{
			taskTable[i]->delayTicks--;
		}
		else
		{
			tBitmapSet(&taskPrioBitmap, taskTable[i]->prio); //3.4 ��Ϊ���ĳ��task��delayTick==0, ˵�����taskҲҪ�������״̬��, λͼ�еĶ�Ӧλ�����ó�1
		}
	}
	*/
	
	//3.6 ��ΪtTaskDelayList�����Ķ���������ʱ��task�ĳ�ԱtNode delayNode, �������ǾͿ��Ժ������ҵ�������ʱ��task, �Ͳ�����3.4�����, ȫ��task������һ��
	// �ҵ�������ʱ��task֮��,�Ϳ��Զ����ǵ�delayTicks - 1, �������tick == 0, �Ϳ��԰��Ǹ�task ����ʱ����ɾ��, ���Ҽ����������
	// ������Ҫע���, ��ȷ���׳���: �Ǿ���tTaskDelayedList��һ��tList, ������tList*, ����������Ҫ��. : tTaskDelayedList.headNode, ��������-> : ����tTaskDelayedList->headNode�Ǵ��
	for(node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		//����Ǹ�task�ĵ�ַ
		tTask* task = (tTask*)tNodeParentAddr(tTask, delayNode, node); //��һ������: �ṹ��, �ڶ���: �ṹ��ĳ�Ա, ������: �ó�Ա��ʵ�ʵ�ַ. ��: �ṹ���ʵ���׵�ַ

		//��task��delayTIck�ݼ�
		//���tick == 0, ���ӳٶ����Ƴ�(tTaskDelayedList, task->state), �����������(bitmap, taskTable, task->state)
		if(--task->delayTicks == 0)
		{
			//5.2 ���task���ڵȴ��¼�, ��Ҫ������ECB��ɾ��, ��������:��ʱ��tErrorTimeout (����: �߼���, task���ܻ�ͬʱ�ŵ� �ȴ����� �� ��ʱ������, ����ֻҪ��ʱ����, �ȴ��ͽ������ҵȴ��д�����)
			//�߼���: ����һ��task�����˵ȴ����к���ʱ����, ֻҪ����һ�����ƻ�(�����ѻ���delay==0), �ʹ�����������ɾ��
			if(task->waitEvent)
			{
				tEventRemoveTask(task, (void*)0, tErrorTimeout);
			}
			tTimeTaskWakeUp(task); //������, nextTask��������task
			tTaskSchedReady(task);
		}
	}
	
	//3.7 �������е�task��ʱ��Ƭ����һ
	if(--currentTask->slice == 0)
	{
		//��linkNode��tList�ĵ�һλɾ��
		tListRemoveFirst(&(taskTable[currentTask->prio]));
		//��linkNode�ŵ�ĩβ
		tListAddLast(&(taskTable[currentTask->prio]), &(currentTask->linkNode));
		//����currentTask��ʱ��Ƭ
		currentTask->slice = TINYOS_SLICE_MAX;
		
	}
	tTaskExitCritical(status);
	//��Ϊ����ݼ���delayTicks,�����п�����һ�����ϵ������ӳ����,���Ա�������. ���µ�tTaskSched()����Ϳ����ж��Ƿ�delayTicks==0,�Ƿ���Ա�����
	tTaskSched();
}

//3.3 ���Լ���Ƶ�, ��ⱨ��ĺ���
void ifError(void)
{
	
}

//2.4 ���һ����������, ����ʲô������, ������������Ľ����ӳ�
void idleTaskEntry(void* param)
{
	for(;;)
	{}
}





//3.7 ��Ҫ�޸��� //3.4 ���ص�ǰ������ȼ�������
tTask* tTaskHighestTaskReady(void) //ע��,���ﲻ��Ҫ����&taskPrioBitmap��Ϊ����,��ΪĬ����ȫ�ֱ���. ��Ȼ����ĳ�Ҫ���������Ҳ����
{
	//3.7 ��һ������, ����ͨ��bitmap֪��������ȼ��ǵڼ������ȼ�
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	
	//3.7 Ȼ���taskTable���ҵ���Ӧ�����ȼ�, Ҳ����һ��tList, ������Ҫ�����tList���ҳ���һ��tNode,(ע�ⲻ��tNode*) ���tNode��Ȼ��������tNode*, ����tNode����Ҳ���Լ��ĵ�ַ(��ʵtList��headNode.nextNode��ľ������tNode�Լ��ĵ�ַ), ����ͨ�������ַ, �ҵ����tNode������tTask, �����tTask���׵�ַ����
	tNode* linkNodeAddr = tListFirst(&(taskTable[highestPrio])); //ע�ⷵ�صľ���tNode*
	
	//3.7 ����֪��linkNode��ʵ�ʵ�ַ, �����tTask��ʵ���׵�ַ
	tTask* taskAddr = tNodeParentAddr(tTask, linkNode, linkNodeAddr); 
	return taskAddr;
}

int main()
{
	
	//3.4 ��ΪshareCount�������Լ��Ķ���ĵ�����,������Ҫ��ʼ��, ���������������� ��ʼ��������, Ҳ����bitmap�ĳ�ʼ��
	tTaskSchedInit();
	
	//3.6 ��ʼ����ʱ����
	tTaskDelayedInit();
	
	tAppInit();

	
	//��tTask������Ӧ��stack��func
	
	taskInit(&tTaskIdle, idleTaskEntry, (void*)0x00000001, TINYOS_PRIO_COUNT - 1, &idleTaskEnv[TINYOS_IDELTASK_STACK_SIZE]); //3.4 ���ó�������ȼ�,31. ����ʹ�õ��������ļ�tConfig.h�еĺ궨��
	
	//Ϊ���ǵ�taskTable����
	//3.7 ɾ�� taskTable[0] = &tTask1;
	//3.7 ɾ�� taskTable[1] = &tTask2;
	idleTask = &tTaskIdle; //2.4
	
	//3.4 nextTask��������ȼ�������
	nextTask = tTaskHighestTaskReady(); //ɾ��: nextTask = taskTable[0]; //��Ϊ�����Ǹտ�ʼ,��û��ִ�е�����,Ҳ����currentTask��û�б���ʼ��,��������Ӧ����nextTask����ʼ��
	
	//2.2 ��ʼִ�е�һ������: ��һ��������л���ͬ�ĵط�(ǰ��:ֱ�Ӱ�stack����Ķ����浽register,����ȥ��̨��ʱ��û����,��һ������ֱ�ӰѶ��������� | ����:��ǰ�����Ȱ�register�Ķ����浽stack��,�������Ӱ��Լ�stack����Ķ����浽register,����ȥ��̨��ʱ������,ǰ��Ŀ���:���̸�����,����Ŀ���:�ٷŶ���)
	tTaskRunFirst(); //�������,ҲҪ����pendSV�쳣, Ȼ����ΪpendSV��ִ��asm����,���Կ���pc,���Կ��Կ�����һ��Ҫȥ�ĺ���
	//ע��: ��ΪtTaskSwitch();tTaskRunFirst(); ��Ҫ��asm����������Ӳ��,�������ǾͰ������������Ķ���,�ŵ�switch.c��,��Ϊ���c�ļ��д����й�Ӳ���Ĳ���.���������������������,��Ҫ�ŵ�tinyOS.h��
	
	return 0; //ע��,���ﲻ��ִ�е�return 0,��ΪtTaskRunFirst()����ͻ�һֱ�ߵ�for loop
}


//4.2 ����������ȼ�����(��������)�Ƴ�, ���������Ȼ�������ݺ�tTaskSchedUnReady()һ��, �����Ժ�᲻һ����
void tTaskSchedRemove(tTask* task)
{
	
	//�Ƴ�
	tListRemove(&taskTable[task->prio], &(task->linkNode));
	
	//�Ƴ���֮��,���==0,�Ͱ�bitmapҲ���
	if(tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}	

//4.2 ���������ʱ����ɾ��
void tTimeTaskRemove(tTask* task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}







