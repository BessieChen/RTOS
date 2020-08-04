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

//7.1 ����: ����ĳ�ʼ
tMbox mbox1;
tMbox mbox2; //7.2 ����, ���ڳ�ʱ�ȴ�

void* mbox1MsgBuffer[20]; //��������20��Ԫ�ص�void����, Ȼ��������ָ��
void* mbox2MsgBuffer[20]; //��������20��Ԫ�ص�void����, Ȼ��������ָ��

//7.2 ������Ϣ������
uint32_t msg[20];

//4.2 ģ�����Դ���ͷ�
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //ֻ��ģ��
}

//4.3 ����tTaskGetInfo() //2.1 ����task1������Ҫִ�еĺ���
void task1Entry(void * param) 
{
	
	tSetSysTickPeriod(1);
	
	//7.1 ����:
	tMboxInit(&mbox1, (void*)mbox1MsgBuffer, 20); //������20��


	for(;;){	
		//7.2 ͨ����ͨ�ķ�ʽ, ����msg��mbox1
		uint32_t i = 0;
		for(i = 0; i < 20; i++)
		{
			msg[i] = i+3;
			tMboxNotify(&mbox1, &msg[i], tMBOXSendNormal); //��ͨ���͸�mbox1
		}
		
		//7.2 ��һ����һ�����ʱ, ��Ϊ�����for loop�Ѿ���mbox1��װ����, ������Ҫ�ӳپ�һ����task2ȥ��ȡmbox1
		tTaskDelay(100);
		
		//7.2 �������ȼ���ʽ, �ӵ���������
		for(i = 0; i < 20 ;i++)
		{
			msg[i] = i+7;
			tMboxNotify(&mbox1, &msg[i], tMBOXSendFront);
		}
		
		//7.2 ��һ����һ�����ʱ, ��Ϊ�����for loop�Ѿ���mbox1��װ����, ������Ҫ�ӳپ�һ����task2ȥ��ȡmbox1
		tTaskDelay(100);
		//tTaskDelay(100);
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
	
}
//4.2 �޸� 3.7 �޸�
void task2Entry(void * param) 
{
	for(;;){
		
		//7.2
		void* msg2; 
		
		//7.2 �ȴ�mbox1
		uint32_t err = tMboxWait(&mbox1, &msg2, 0);
		
		//7.2
		if(err == tErrorNoError)//˵����Ϣ�ȵ���
		{
			//��Ϣ��ת��
			uint32_t value = *(uint32_t*)msg2;
			task2Flag = value;
			tTaskDelay(1);
		}
		
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
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
	//7.2 ��������ȴ��ĳ�ʱ
	//tMboxInit(&mbox2, mbox2MsgBuffer, 20); //��ʼ��mbox2
	
	for(;;){
		
		//7.2  ��ʱ�ȴ�		
		//void* msg;
		//tMboxWait(&mbox2, &msg, 100); //��ʱʱ����100
		
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
