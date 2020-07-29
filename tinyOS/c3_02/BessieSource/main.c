#include "tinyOS.h"
#include "ARMCM3.h" //������SysTick

//3.2 ���������ж϶��ܷ��ʵ�ȫ�ֱ���
int shareCount;

//2.2	ÿ��task���涼����Ҫ��ת��flag
int task1Flag;
int task2Flag;

//2.2. ���ҽ��������񶼷Ž�һ��array��,���ڹ���
tTask* taskTable[2]; //����һ��tTask*������,Ҳ����˵,�����ÿһ��Ԫ�ض���һ��tTask*, Ҳ����ÿһ��Ԫ�ض���һ��tTask���͵ĵ�ַ

//2.1 ��������tinyOS.h��,���Ǿ���������stack,������������task, ����������taskִ�еĺ���
tTaskStack tTask1Env[1024];		//��Ϊһ��tTaskStackֻ��һ��uint32_t,ֻ�ܴ���stack�����һ��Ԫ��.�����������ó��������ʽ
tTaskStack tTask2Env[1024];
tTaskStack idleTaskEnv[1024]; //2.4

tTask tTask1; //����һ������,���ǵ���,tTask��һ���ṹ��,�������һ��tTaskStack*,֮�����Ƕ����tTaskStack*��ʼ��,�Ϳ��Խ�tTask1��tTask1Env��ϵ������
tTask tTask2;
tTask tTaskIdle; //2.4

//2.2 ��ʼ��������֮��,��Ȼ����Ҫ��������,������Ҫ����current��next task
tTask* currentTask;
tTask* nextTask;
tTask* idleTask; //2.4


//3.2 ����������ļ�����, ��uint8, ֻ�е�������==0��ʱ��,�ſ��Դ������Ⱥ���tTaskSched()
uint8_t schedLockCount;

//3.2 ��ʼ��������, �������ٽ���:�ٽ�����ʹ��__get_primask()���ر��ж�,Ҳ����ʹ����ARMCM3.h�ļ��ĺ���,���ǵ�����ʹ�����Լ������
void tTaskSchedInit(void)
{
	schedLockCount = 0;
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

//3.2 ������Ҫ�޸�һ�����ǵ�tTaskSched()
void tTaskSched(void)
{
	//2.4���µĶ�Ҫɾ��
	/*
	if(currentTask == taskTable[0])
		nextTask = taskTable[1]; //����Ϊ�˸��ٴν���pendSV()�쳣Ҳ����asm������,nextTask�ĸ������̵�
	else
		nextTask = taskTable[0];
	tTaskSwitch(); //����������һ��������ʲô֮��,��Ҫ�л���,���tTaskSwitch()������������˴���pendSV�쳣,��ΪpendSV�쳣��ʹ�û�����,���Կ���PC,���ԾͿ���ת���µĺ�����
	*/
	
	//2.4 ����:
	/*
		1. �����ǰ������idle
			1. ���task1��delayTicks==0, ��һ���������task1
			2. task2ͬ��
			3. ��������task��delayTicks != 0, ��ôʲô��������, ��Ϊ��һ�����������ǵ�ǰ����idelTask
			�ҵ�����:���task1��task2����delayTicks==0,�����������ȷ�����ж���task1
		2. �����ǰ������task1
			1. ���task2��dT==0, ��һ���������task2
			2. ���task1��dT!=0, Ҳ����˵��ǰtask1���ǲ���Ҫcpu,��Ϊ�ߵ��ڶ���,˵��task2��dT!=0, ��Ȼ����������Ҫcpu, ��ʱ��һ��������idleTask
			3. ����task1��dT == 0, ��ôʲô��������, ��Ϊ��һ�����������ǵ�ǰ����task1
			�ҵ�����:�Ǿ������task1��2��dT��==0,��Ҳ�������ȸ�task2,��Ϊ���ж�task2
		3. �����ǰ������task2
			1.2.3ͬ��
	*/
	
	//3.2 Ϊ�˱������, ������Ҫ�жϵ������ǲ���==0,���ȷ����==0,�Ż�ִ�к�������,�����return
	//ͬ��, �жϵ�������ֵ��ʱ��,Ϊ�˱���,�������ٽ���������
	uint32_t status = tTaskEnterCritical();
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
	if(currentTask == idleTask) //ע��idelTask��ָ��, tTask* idleTask = &tTaskIdle; //extern tTask* idleTask; //��Ҫ��tinyOS.h��д����һ��,����main.c�ᱨ��, Ҫô����ȫ�ֱ�����д���������ǰ��
	{
		if(taskTable[0]->delayTicks == 0)
		{
			nextTask = taskTable[0];
		}
		else if(taskTable[1]->delayTicks == 0)
		{
			nextTask = taskTable[1];
		}
		else //Ҳ��������task��dT != 0, ��������Ҫcpu
		{
			tTaskExitCritical(status);
			return; //����nextTask������currentTask,��֮ǰһ��
		}
	}
	else if(currentTask == taskTable[0])
	{
		if(taskTable[1]->delayTicks == 0)
		{
			nextTask = taskTable[1]; //���task1��2��dT��==0,��Ҳ�������ȸ�task2,��Ϊ���ж�task2
		}
		else if(taskTable[0]->delayTicks != 0)
		{
			nextTask = idleTask; //˵��Ҳ��������task��dT != 0, ��������Ҫcpu, ���Ը�idle
		}
		else //˵��task2��dT != 0, ����task1��dT == 0, ����nextTask������task1
		{
			tTaskExitCritical(status);
			return;
		}
	}
	else //currentTask == taskTable[1]
	{
		if(taskTable[0]->delayTicks == 0)
		{
			nextTask = taskTable[0]; //���task1��2��dT��==0,��Ҳ�������ȸ�task1,��Ϊ���ж�task1
		}
		else if(taskTable[1]->delayTicks != 0)
		{
			nextTask = idleTask; //˵��Ҳ��������task��dT != 0, ��������Ҫcpu, ���Ը�idle
		}
		else //˵��task1��dT != 0, ����task2��dT == 0, ����nextTask������task2
		{
			tTaskExitCritical(status);
			return;
		}
	}
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


//2.4 ��Ϊ����ÿ��������һ��������delayTicks, �������һ������,�ܹ��ݼ�delayTicks(�൱�����ӳ�)
void tTaskSysTickHandler()
{
	int i;
	for(i = 0; i < 2; i++)
	{
		if(taskTable[i]->delayTicks > 0) //�������if,˵��Ҫ��Ҫ�ݼ�
		{
			taskTable[i]->delayTicks--;
		}
	}
	
	//��Ϊ����ݼ���delayTicks,�����п�����һ�����ϵ������ӳ����,���Ա�������. ���µ�tTaskSched()����Ϳ����ж��Ƿ�delayTicks==0,�Ƿ���Ա�����
	tTaskSched();
}

//2.4 ����ʱ���ж�ʱ�Ĵ�����, ������pendSVһ��,��ϵͳ���Զ����õ������,ע�⺯������Ҫд��
void SysTick_Handler()
{
	//�жϵ�ʱ��Ӧ����ʲô? Ӧ�����л�����: ��Ҫ�ȵ�����һ������,Ҳ����ȷ����nextTask��ʲô -> ���������Ǵ���systick�쳣
	
	//1. ������һ������
	//tTaskSched(); //2.4�оͲ���ֱ�ӵ����������
	
	//2.4, ��Ҫ�ȵݼ�delayTicks,Ȼ�����tTaskSched()
	tTaskSysTickHandler(); //���������������tTasksched()
	
}

//2.4 ʵ����ʱ����, ��Ϊ��������϶���currentTask���õ�,��������ֱ��дcurrentTask->xxx
void tTaskDelay(uint32_t delay)
{
	currentTask->delayTicks = delay; //Ҳ����ΪcurrentTask������Ҫ��ʱ���
	tTaskSched();//��������currentTask�Ƿ�����cpu,������tTaskSched()���жϻ���˭����Ҫcpu��
}

//2.4 �������������ǵļ�ʱ��,����Ĳ���������ϣ���ĺ���
void tSetSysTickPeriod(uint32_t ms)
{
	//���ü�������ʱ���ֵ:Ҳ����˵,ÿ�μ������жϵ�ʱ��,�Ὣ(�ݼ�������)��ֵ-1,���ݼ���������ֵ==0��ʱ��,�Ὣ�ݼ���������ֵ���ó�������ü�������ֵ
	SysTick->LOAD = ms / 1000 * SystemCoreClock - 1; //��ʵ�Ҳ��Ǻ����Ϊʲô-1, ��������Ϊ�������ó����ü�������ʱ��,Ҳ��Ҫ����ʱ��,����-1�൱�������������ʱ��?
	
	//���²���
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	
	//�ݼ�������
	SysTick->VAL = 0;
	
	//��������
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
									SysTick_CTRL_TICKINT_Msk | //�����ж�ʹ�ܵı�־λ
									SysTick_CTRL_ENABLE_Msk;	//ʹ�ܼ������ı�־λ
	
	//����:
	/*
	1. SysTick_IRQn��ָsystick�Ļ����
	2. __NVIC_PRIO_BITS��4�ĺ궨��,����ָ����ռ�����ȼ�Ϊ4,��Ϊm3�ں���ռ�����ȼ�����Ӧ���ؼ�һ����4λ,����ռ�����ȼ��Ѿ���4λ,������Ӧ���ȼ�ֻ��0λ
	����������������ȼ�: 1<<4 -1, Ҳ����16-1=15,Ҳ����1111, ע�����ȼ���0��ʱ����������ȼ�
	*/
}




//����task1������Ҫִ�еĺ���
void task1Entry(void * param) 
{
	
	tSetSysTickPeriod(1);
	
	for(;;){
		
		int var;
		//int i; //ע��,����һ����Ҫ���ڿ�ִ�����ǰ��(����:int i; int var)(��ִ�����: var = xxx;) ,����ᱨ��:error:  #268: declaration may not appear after executable statement in block
		
		tTaskSchedDisable();
		var = shareCount;
		//for(i = 0; i < 0xf; i++)
	
		task1Flag = 0;
		tTaskDelay(1);
		
		//��Ϊ��һ��ᴥ����ʱ,�����ߵ���һ��֮ǰ,�Ѿ�ȥ��һ��task2��
		var++;
		shareCount = var;
		tTaskSchedEnable();
		
		task1Flag = 1;
		tTaskDelay(1);
		
		//tTaskSched();//��systick������,����Ͳ���Ҫ��
	}
}
void task2Entry(void * param) 
{
	for(;;){
		
		tTaskSchedDisable();
		shareCount++;
		tTaskSchedEnable();
		
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
		//tTaskSched();//��systick������,����Ͳ���Ҫ��
	}
}


//2.4 ���һ����������, ����ʲô������, ������������Ľ����ӳ�
void idleTaskEntry(void* param)
{
	for(;;)
	{}
}



//���ó�ʼ������ĺ���: ��ʼ��stack: ��tTask1��tTask1Env��ϵ����, ��ʼ��task����
void taskInit(tTask* task, void (*func)(void*), void* param, tTaskStack* stack )
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
}


int main()
{
	
	//3.2 ��ΪshareCount�������Լ��Ķ���ĵ�����,������Ҫ��ʼ��
	tTaskSchedInit();
	
	//��tTask������Ӧ��stack��func
	taskInit(&tTask1, task1Entry, (void*)0x11111111, &tTask1Env[1024]); //ע��tTask1Env��һ��uint32_t������,���Բ���д��&tTaskEnv,��Ҫд��&tTaskEnv[xx]
	taskInit(&tTask2, task2Entry, (void*)0x00000000, &tTask2Env[1024]); 
	taskInit(&tTaskIdle, idleTaskEntry, (void*)0x00000001, &idleTaskEnv[1024]); //2.4, idleTask�ĳ�ʼ��
	
	//Ϊ���ǵ�taskTable����
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	idleTask = &tTaskIdle; //2.4
	
	nextTask = taskTable[0]; //��Ϊ�����Ǹտ�ʼ,��û��ִ�е�����,Ҳ����currentTask��û�б���ʼ��,��������Ӧ����nextTask����ʼ��
	
	//2.2 ��ʼִ�е�һ������: ��һ��������л���ͬ�ĵط�(ǰ��:ֱ�Ӱ�stack����Ķ����浽register,����ȥ��̨��ʱ��û����,��һ������ֱ�ӰѶ��������� | ����:��ǰ�����Ȱ�register�Ķ����浽stack��,�������Ӱ��Լ�stack����Ķ����浽register,����ȥ��̨��ʱ������,ǰ��Ŀ���:���̸�����,����Ŀ���:�ٷŶ���)
	tTaskRunFirst(); //�������,ҲҪ����pendSV�쳣, Ȼ����ΪpendSV��ִ��asm����,���Կ���pc,���Կ��Կ�����һ��Ҫȥ�ĺ���
	//ע��: ��ΪtTaskSwitch();tTaskRunFirst(); ��Ҫ��asm����������Ӳ��,�������ǾͰ������������Ķ���,�ŵ�switch.c��,��Ϊ���c�ļ��д����й�Ӳ���Ĳ���.���������������������,��Ҫ�ŵ�tinyOS.h��
	
	return 0; //ע��,���ﲻ��ִ�е�return 0,��ΪtTaskRunFirst()����ͻ�һֱ�ߵ�for loop
}
