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

//6.2 ������
tSem sem1; //�޳�ʱ�ĵȴ�(tSemWait(&sem1, 0);)
tSem sem2; //��ʱ�ĵȴ�

//4.2 ģ�����Դ���ͷ�
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //ֻ��ģ��
}

//4.3 ����tTaskGetInfo() //2.1 ����task1������Ҫִ�еĺ���
void task1Entry(void * param) 
{
	
	tSetSysTickPeriod(1);
	
	//6.2 ����, ��ʼ��sem1
	tSemInit(&sem1, 0, 10);//˵�������Դ��10��, ����ֻ��0����ʹ��, ˵�����е���Դ����ʹ����
	
	for(;;){

		//6.2 ����: task1���Ƿ�����ʹ�õ���Դ, �еĻ�����ʹ��, û�еĻ��ȴ�0��systick, �������޳�ʱ�ĵȴ�: ֻ��ŵ��ȴ�����, ����ŵ���ʱ����, ����systick���ܴ����˶��ٴ�, task1����һֱ����sem1��Դ
		tSemWait(&sem1, 0);
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
	
}
//4.2 �޸� 3.7 �޸�
void task2Entry(void * param) 
{
	//6.2 ������ʹ��
	uint32_t error;
	
	for(;;){
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
		//6.2 task2��task1�ȴ�����Դsem1, ����֪ͨ. ���Ӧ����task1������, Ȼ���task2�л������ȼ����ߵ�task1, ����sem1����Դ�ֱ�ʹ����
		tSemNotify(&sem1);
		
		//6.2 ���Ƿ���sem1��Դ������, û�еĻ��Ͳ���
		error = tSemNoWaitGet(&sem1); //��Ϊ�ղ��Ѿ���sem1��task1��, ��������error Ӣ���ǵ��� ResourceUnavailable
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
	//6.2 ��ʼ��sem2��Դ, �������˼��, sem2�����޶����Դ(maxCount == 0), ��������ʣ��0����Դ
	tSemInit(&sem2, 0, 0);
	
	for(;;){
		
		//6.2 task3�ᳬʱ�ȴ�sem2��Դ, task3���Ƿ�����ʹ�õ���Դ, �еĻ�����ʹ��, û�еĻ��ȴ�10��systick, �����ǳ�ʱ�ĵȴ�: ��ŵ��ȴ����� and ��ʱ����, ����systick������10��, task3�Ͳ������, �ͻ�ȥ����������
		tSemWait(&sem2, 10);
		
		task3Flag = 0;
		tTaskDelay(1);
		task3Flag = 1;
		tTaskDelay(1);
		
		
	}
}

//4.2 task4������
void task4Entry(void * param) 
{
	
	
	for(;;){
		
		task4Flag = 0;
		tTaskDelay(1);
		task4Flag = 1;
		tTaskDelay(1);
	
	}
}

//4.1 app�ĳ�ʼ������
void tAppInit(void)
{
	taskInit(&tTask1, task1Entry, (void*)0x11111111, 0, &tTask1Env[TINYOS_IDELTASK_STACK_SIZE]); //3.4 ���ó�������ȼ�
	taskInit(&tTask2, task2Entry, (void*)0x00000000, 1, &tTask2Env[TINYOS_IDELTASK_STACK_SIZE]); 
	taskInit(&tTask3, task3Entry, (void*)0x22222222, 1, &tTask3Env[TINYOS_IDELTASK_STACK_SIZE]); //5.3 task1���ȼ����, task234��� //3.7 ��ʼ��task3, ע�����ȼ���task2һ��
	taskInit(&tTask4, task4Entry, (void*)0x44444444, 1, &tTask4Env[TINYOS_IDELTASK_STACK_SIZE]);
	//4.2 �����㿴����, ���ȼ���ߵ���task1,3; ֮����task2,4
	/*����������task1 -> task3 -> task2(ɾ��task1) -> task4(ɾ��task3)*/
}
