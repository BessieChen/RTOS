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

//8.2 ������, ��ʼ���洢����
typedef uint8_t (*tBlock)[100]; //����Ƕ���һ������ uint8_t[100]. ���tBlock block; �൱�ڶ�����uint8_t block[100].
uint8_t mem1[20][100]; //�洢������, һ��20���洢��, һ���洢��һ��100�ֽ�
tMemBlock memBlock1;

//4.2 ģ�����Դ���ͷ�
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //ֻ��ģ��
}

//4.3 ����tTaskGetInfo() //2.1 ����task1������Ҫִ�еĺ���
void task1Entry(void * param) 
{	
	//8.2
	uint8_t i;
	tBlock block[20]; //��ΪtBlock block; �൱�ڶ�����uint8_t block[100]. ��������tBlock block[20];�൱��uint8_t block[20][100]; Ҳ��  һ��20���洢��, һ���洢��һ��100�ֽ�
	tSetSysTickPeriod(1);
	
	//8.2 ����
	tMemBlockInit(&memBlock1, (uint8_t*)mem1, 100, 20); // һ��20���洢��, һ���洢��һ��100�ֽ�
	for(i = 0; i < 20; i++) //8.2 �����еĴ洢�鶼ȡ����
	{
		//8.2 �ù����еĴ洢��, ����Щ�洢���ĵ�ַ����block[i]. ��tMemBlock�еĴ洢��mem1�ĵ�һ�����ֵĵ�ַ, ��ֵ��&block[i], ע�����ﴫ��ַȡֵ, ������һ�仰������, �ҷ���memBlock�е�countһֱ����, ����block[0],[1]..���ֵ��mem1[0],[1]�ĵ�ַ
		tMemBlockWait(&memBlock1, (uint8_t**)&block[i], 0); //uint32_t tMemBlockWait(tMemBlock* memBlock, uint8_t** mem, uint32_t waitTicks)//����ַ��ֵ: ��block�ĵ�ַ�浽mem��, ���û��block�͵ȴ�waitTick��systick
	}
	
	//8.2 ��ʱ
	tTaskDelay(2);
	
	//8.2 ��֮ǰblock��õĴ洢��, �ͷŻ�ȥ, ���ǻ���memBlock1
	for(i = 0; i < 20; i++)
	{
		memset(block[i], i+7, 100); //8.2 �ڶ�������: ��block[i]���涼��ֵΪi, ����������: block[i]�ĳ���
		tMemBlockNotify(&memBlock1, (uint8_t*)block[i]); //8.2 �ͷŻ�ȥ, ���ǻ���memBlock1, ��ʱ��Ϊtask2�Ѿ��ڵȴ���, �����л���task2
		tTaskDelay(2); //8.2 ��ʱ
	}
	
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
	for(;;){
		
		//8.2 ��ȡ
		tBlock block;
		tMemBlockWait(&memBlock1, (uint8_t**)&block, 0); //�ߵ�����, �Ǵ�task1����, task1��ʱ�Ѿ�ִ������ʹ�����еĴ洢��, ����������Ҫ�ȴ�, ����task2����ȴ�����, ��һ�����л���task1
		
		//8.2 ȡ����һ���ֽڵ�ֵ
		//��block��ȡ����һ���ֽ�
		task2Flag = *(uint8_t*)block; //��Ϊmemset(block[i], i+7, 100);, ���Ե�һ���ֽ���0+7=7
		
		/*task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);*/
		
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
