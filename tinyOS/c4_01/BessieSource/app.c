//��Ӧ����صĴ���
#include "tinyOS.h"

//2.2	ÿ��task���涼����Ҫ��ת��flag
int task1Flag;
int task2Flag;
int task3Flag;

//2.1 ��������tinyOS.h��,���Ǿ���������stack,������������task, ����������taskִ�еĺ���
tTaskStack tTask1Env[1024];		//��Ϊһ��tTaskStackֻ��һ��uint32_t,ֻ�ܴ���stack�����һ��Ԫ��.�����������ó��������ʽ
tTaskStack tTask2Env[1024];
tTaskStack tTask3Env[1024];		//3.7 

tTask tTask1; //����һ������,���ǵ���,tTask��һ���ṹ��,�������һ��tTaskStack*,֮�����Ƕ����tTaskStack*��ʼ��,�Ϳ��Խ�tTask1��tTask1Env��ϵ������
tTask tTask2;
tTask tTask3; //3.7 Ϊ�˲���, ������Ҫ��task2��task3�����ȼ�����Ϊͬһ���ȼ�

//2.1 ����task1������Ҫִ�еĺ���
void task1Entry(void * param) 
{
	tSetSysTickPeriod(1);

	for(;;){

		task1Flag = 0;
		/*
		4.1 ɾ��
		//delay(10);
		//tTaskDelay(1);
		*/
		tTaskSuspend(currentTask); //4.1 task1���Լ�����
		task1Flag = 1;
		/*
		4.1 ɾ��
		//delay(10);
		//tTaskDelay(1);
		*/
		tTaskSuspend(currentTask); //4.1 task1�ڶ��ν��Լ�����
	}
	
}
//3.7 �޸�
void task2Entry(void * param) 
{
	for(;;){
		task2Flag = 0;
		//3.7 ɾ�� tTaskDelay(1); ��ΪtTaskDelay()�������ó���Դ,��������������Ҫtask2һֱռ����Դ,Ȼ�����������ǵ�ʱ��Ƭ�Ƿ�����,�������,task2������һֱռ����ԴҲ��������
		//3.7 Ϊ����task2һֱռ����Դ,ʹ��delay()
		//4.1 ɾ��delay(0xFF);
		tTaskDelay(1); //4.1
		tTaskWakeUp(&tTask1); //4.1 ��task2����task1
		task2Flag = 1;
		//3.7 ɾ��: tTaskDelay(1);
		//3.7 Ϊ����task2һֱռ����Դ,ʹ��delay()
		//4.1 ɾ��delay(0xFF);
		tTaskDelay(1); //4.1
		tTaskWakeUp(&tTask1); //4.1 ��task2�ڶ��λ���task1

	}
}

//3.7 task3������
void task3Entry(void * param) 
{
	for(;;){

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

//4.1 app�ĳ�ʼ������
void tAppInit(void)
{
	taskInit(&tTask1, task1Entry, (void*)0x11111111, 0, &tTask1Env[TINYOS_IDELTASK_STACK_SIZE]); //3.4 ���ó�������ȼ�
	taskInit(&tTask2, task2Entry, (void*)0x00000000, 1, &tTask2Env[TINYOS_IDELTASK_STACK_SIZE]); 
	taskInit(&tTask3, task3Entry, (void*)0x00000000, 1, &tTask3Env[TINYOS_IDELTASK_STACK_SIZE]);  //3.7 ��ʼ��task3, ע�����ȼ���task2һ��
}
