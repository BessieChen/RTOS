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

//7.3 ����
tMbox mbox1;
void* mbox1MsgBuffer[20];
uint32_t msg1[20];

tMbox mbox2;
void* mbox2MsgBuffer[20];
uint32_t msg2[20];

//4.2 ģ�����Դ���ͷ�
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //ֻ��ģ��
}

//4.3 ����tTaskGetInfo() //2.1 ����task1������Ҫִ�еĺ���
void task1Entry(void * param) 
{
	void* msgTask1;
	
	tSetSysTickPeriod(1);

	//7.3 ����mbox�ĳ�ʼ��
	tMboxInit(&mbox1, mbox1MsgBuffer, 20);
	tMboxInit(&mbox2, mbox2MsgBuffer, 20);

	//7.3 ��task1��mbox1�еȴ�
	tMboxWait(&mbox1, (void*)msgTask1, 0);
	for(;;){	
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
	
}
//4.2 �޸� 3.7 �޸�
void task2Entry(void * param) 
{
	//7.3 �Ƿ�ɾ��
	int destory = 0;
	for(;;){
		
		
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
		//7.3 ɾ��mbox1
		if(!destory)
		{
			tMboxDestory(&mbox1);
			destory = 1;
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
		
		//7.3 ��mbox2������Ϣ
		int i = 0;
		for(i = 0; i < 20; i++)
		{
			msg2[i] = i + 7;
			tMboxNotify(&mbox2, &msg2[i], tMBOXSendNormal);
		
		}
		
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
		//7.3 ��ȡmbox2�е���Ϣ
		
		uint32_t* msgTask4;
		tMboxWait(&mbox2, (void*)msgTask4, 0);//�о���ʦд���˰�? tMboxWait(&mbox2, (void*)&msgTask4, 0); //�����Ǵ�mbox�ж�ȡһ����Ϣ, ����Ϣ�ĵ�ַ�浽msgTask
		task4Flag = *msgTask4; //������ַ��ŵ�ֵ, Ҳ�������ǲ����Msg[i] = i, ��Ϊ�ǵ�һ������msg[0] = 7, ���task4Flag��7
		
		tMboxFlush(&mbox2); //��һ��Ͷ�ȡ��һ��msg, ������һ�����, ����msg���ᱻɾ��
		/*
		task4Flag = 0;
		tTaskDelay(1);
		task4Flag = 1;
		tTaskDelay(1);
		*/
	
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
