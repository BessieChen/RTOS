//��Ӧ����صĴ���
#include "tinyOS.h"
#include "string.h" //8.2 ����, ���� warning:  #223-D: function "memset" declared implicitly

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

//9.2 ����, ��ʼ��
tFlagGroup flagGroup1;

//4.2 ģ�����Դ���ͷ�
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //ֻ��ģ��
}

//4.3 ����tTaskGetInfo() //2.1 ����task1������Ҫִ�еĺ���
void task1Entry(void * param) 
{	
	tSetSysTickPeriod(1);
	
	//9.2
	tFlagGroupInit(&flagGroup1, 0xFF); //16λ����1
	

	for(;;){	
		
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
		
		//9.2 ֪ͨ����: ĳЩλ����, �ڶ��͵�һλ����: ������0000 0110. ע��Ҫ�����λ��־��1
		tFlagGroupNotify(&flagGroup1, 0, 0x6); //֮��flag�ӳ�ʼ����FF�䵽F9(1111 1001)
		
	}
	
}
//4.2 �޸� 3.7 �޸�
void task2Entry(void * param) 
{

	//9.2
	uint32_t resultFlags = 0;
	for(;;){
		
		//9.2 �ȴ����¼�: �ڶ�λ����(0100), ����ȫ������, Ҳ����˵, һ��Ҫ�еڶ�λ����, ������岻������ν
		tFlagGroupWait(&flagGroup1, TFLAGGROUP_CLEAR_ALL, 0x4, &resultFlags, 3); //�ȴ��Ľ������resultFlags��
		
		//9.2 �ȴ����¼�2: ��1��0λ����(0011), ����ȫ������, Ҳ����˵, һ��Ҫ�е�1��0λ����, ������岻������ν
		tFlagGroupNoWaitGet(&flagGroup1, TFLAGGROUP_CLEAR_ALL, 0x3, &resultFlags);
		
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
	for(;;){
		
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
