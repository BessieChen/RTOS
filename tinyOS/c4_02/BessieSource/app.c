//��Ӧ����صĴ���
#include "tinyOS.h"

//2.2	ÿ��task���涼����Ҫ��ת��flag
int task1Flag;
int task2Flag;
int task3Flag;
//4.2 �����������4, ����Ϊ�˼�����ǵĺ���: task2 ǿ��ɾ�� task1, task4 ����ɾ�� task3, task3�Լ�ɾ���Լ�
int task4Flag; //4.2

//2.1 ��������tinyOS.h��,���Ǿ���������stack,������������task, ����������taskִ�еĺ���
tTaskStack tTask1Env[TINYOS_IDELTASK_STACK_SIZE];		//��Ϊһ��tTaskStackֻ��һ��uint32_t,ֻ�ܴ���stack�����һ��Ԫ��.�����������ó��������ʽ
tTaskStack tTask2Env[TINYOS_IDELTASK_STACK_SIZE];
tTaskStack tTask3Env[TINYOS_IDELTASK_STACK_SIZE];		//3.7 
tTaskStack tTask4Env[TINYOS_IDELTASK_STACK_SIZE];		//4.2 

tTask tTask1; //����һ������,���ǵ���,tTask��һ���ṹ��,�������һ��tTaskStack*,֮�����Ƕ����tTaskStack*��ʼ��,�Ϳ��Խ�tTask1��tTask1Env��ϵ������
tTask tTask2;
tTask tTask3; //3.7 Ϊ�˲���, ������Ҫ��task2��task3�����ȼ�����Ϊͬһ���ȼ�
tTask tTask4; //4.2 ������

//4.2 ģ�����Դ���ͷ�
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //ֻ��ģ��
}

//2.1 ����task1������Ҫִ�еĺ���
void task1Entry(void * param) 
{
	tSetSysTickPeriod(1);

	//4.2 ����������: ��һ������:Ҫ���������(currentTask), �ڶ���: �������,����ĺ���, ������: �ú����Ĳ���
	tTaskSetCleanCallFunc(currentTask, task1DestroyFunc, (void*)0); //����������д��(void*) 0, ��ʵ������û���õ�
	//֮���Լ�������, ����Ϊ
	/*
	���ֻ�����ú���:
	void tTaskSetCleanCallFunc(tTask* task, void (*clean) (void* param), void* param)
	{
		task->clean = clean;
		task->cleanParam = param;
	}	
	*/
	
	for(;;){

		task1Flag = 0;
		tTaskDelay(1); //4.2
		/*
		4.1 ɾ��
		//delay(10);
		//tTaskDelay(1);
		*/
		//4.2 ɾ��tTaskSuspend(currentTask); //4.1 task1���Լ�����
		task1Flag = 1;
		tTaskDelay(1); //4.2
		
		/*
		4.1 ɾ��
		//delay(10);
		//tTaskDelay(1);
		*/
		//4.2 ɾ�� tTaskSuspend(currentTask); //4.1 task1�ڶ��ν��Լ�����
	}
	
}
//4.2 �޸� 3.7 �޸�
void task2Entry(void * param) 
{
	//4.2 ������õ��ı�־λ
	int task1Deleted = 0;
	for(;;){
		task2Flag = 0;
		//3.7 ɾ�� tTaskDelay(1); ��ΪtTaskDelay()�������ó���Դ,��������������Ҫtask2һֱռ����Դ,Ȼ�����������ǵ�ʱ��Ƭ�Ƿ�����,�������,task2������һֱռ����ԴҲ��������
		//3.7 Ϊ����task2һֱռ����Դ,ʹ��delay()
		//4.1 ɾ��delay(0xFF);
		tTaskDelay(1); //4.1
		//4.2 ɾ��tTaskWakeUp(&tTask1); //4.1 ��task2����task1
		task2Flag = 1;
		//3.7 ɾ��: tTaskDelay(1);
		//3.7 Ϊ����task2һֱռ����Դ,ʹ��delay()
		//4.1 ɾ��delay(0xFF);
		tTaskDelay(1); //4.1
		//4.2 ɾ��tTaskWakeUp(&tTask1); //4.1 ��task2�ڶ��λ���task1
		
		//4.2 �ж�task1�Ƿ�ɾ��, ���û�о�ȥɾ��. ֮���Է���for loop����Ϊ�����ǿ���ѭ��ִ������Ĵ���, ֻ��������ô����, ֻ�����һ��if��, ����for loop����Ϊ��?
		if(!task1Deleted)
		{
			tTaskForceDelete(&tTask1); //�����Ǵ���task1�ĵ�ַ
			task1Deleted = 1;
		}
	}
}

//4.2 task3ɾ���Լ�
void task3DestroyFunc(void* param)
{
	task3Flag = 0;
	tTaskDeleteSelf();
}

//4.2 �޸� //3.7 task3������
void task3Entry(void * param) 
{
	for(;;){
		
		//4.2 �ȼ����û����Ҫǿ��ɾ���Լ�
		if(tTaskIsRequestedDeleted())
		{
			task3DestroyFunc((void*) 0);
		}

		task3Flag = 0;
		//3.7 Ϊ����task2һֱռ����Դ,ʹ��delay()
		//4.1 ɾ�� delay(0xFF);
		tTaskDelay(1);
		task3Flag = 1;
		//3.7 Ϊ����task2һֱռ����Դ,ʹ��delay()
		//4.1 ɾ�� delay(0xFF);
		tTaskDelay(1);
		
		
	}
}

//4.2 task4������
void task4Entry(void * param) 
{
	//4.2 ������õ��ı�־λ
	int task3Deleted = 0;
	
	for(;;){

		task4Flag = 0;
		//3.7 Ϊ����task2һֱռ����Դ,ʹ��delay()
		//4.1 ɾ�� delay(0xFF);
		tTaskDelay(1);
		task4Flag = 1;
		//3.7 Ϊ����task2һֱռ����Դ,ʹ��delay()
		//4.1 ɾ�� delay(0xFF);
		tTaskDelay(1);
		
		//4.2 �ж�task3�Ƿ�ɾ��, ���û�о�ȥɾ��. ֮���Է���for loop����Ϊ�����ǿ���ѭ��ִ������Ĵ���, ֻ��������ô����, ֻ�����һ��if��, ����for loop����Ϊ��?
		if(!task3Deleted)
		{
			tTaskRequestDelete(&tTask3); //�����Ǵ���task3�ĵ�ַ
			task3Deleted = 1;
		}
	}
}

//4.1 app�ĳ�ʼ������
void tAppInit(void)
{
	taskInit(&tTask1, task1Entry, (void*)0x11111111, 0, &tTask1Env[TINYOS_IDELTASK_STACK_SIZE]); //3.4 ���ó�������ȼ�
	taskInit(&tTask2, task2Entry, (void*)0x00000000, 1, &tTask2Env[TINYOS_IDELTASK_STACK_SIZE]); 
	taskInit(&tTask3, task3Entry, (void*)0x22222222, 0, &tTask3Env[TINYOS_IDELTASK_STACK_SIZE]);  //3.7 ��ʼ��task3, ע�����ȼ���task2һ��
	taskInit(&tTask4, task4Entry, (void*)0x44444444, 1, &tTask4Env[TINYOS_IDELTASK_STACK_SIZE]);
	//4.2 �����㿴����, ���ȼ���ߵ���task1,3; ֮����task2,4
	/*����������task1 -> task3 -> task2(ɾ��task1) -> task4(ɾ��task3)*/
}
