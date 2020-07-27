#include "tinyOS.h"

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

//2.2	ÿ��task���涼����Ҫ��ת��flag
int task1Flag;
int task2Flag;

//2.1 ��������tinyOS.h��,���Ǿ���������stack,������������task, ����������taskִ�еĺ���
tTaskStack tTask1Env[1024];		//��Ϊһ��tTaskStackֻ��һ��uint32_t,ֻ�ܴ���stack�����һ��Ԫ��.�����������ó��������ʽ
tTaskStack tTask2Env[1024];

tTask tTask1; //����һ������,���ǵ���,tTask��һ���ṹ��,�������һ��tTaskStack*,֮�����Ƕ����tTaskStack*��ʼ��,�Ϳ��Խ�tTask1��tTask1Env��ϵ������
tTask tTask2;

//2.2 ��ʼ��������֮��,��Ȼ����Ҫ��������,������Ҫ����current��next task
tTask* currentTask;
tTask* nextTask;

//2.2. ���ҽ��������񶼷Ž�һ��array��,���ڹ���
tTask* taskTable[2]; //����һ��tTask*������,Ҳ����˵,�����ÿһ��Ԫ�ض���һ��tTask*, Ҳ����ÿһ��Ԫ�ض���һ��tTask���͵ĵ�ַ

void task1Entry(void * param) //����task1������Ҫִ�еĺ���
{
	for(;;){
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
	}
}
void task2Entry(void * param) 
{
	for(;;){
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
	}
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
	task->stack = stack; //��Ϊtask->stack��tTaskStack*,����stackҲ��TaskStack*
}

//2.2 ������Ҫ��������,Ҳ����˵,����Ҫ�����ĸ���������һ�ֿ���ʹ��cpu����Դ
void tTaskSched()
{
	if(currentTask == taskTable[0])
		nextTask = taskTable[1];
	else
		nextTask = taskTable[0];
	tTaskSwitch(); //����������һ��������ʲô֮��,��Ҫ�л���,���tTaskSwitch()������������˴���pendSV�쳣,��ΪpendSV�쳣��ʹ�û�����,���Կ���PC,���ԾͿ���ת���µĺ�����
}


int main()
{
	//��tTask������Ӧ��stack��func
	taskInit(&tTask1, task1Entry, (void*)0x11111111, &tTask1Env[1024]); //ע��tTask1Env��һ��uint32_t������,���Բ���д��&tTaskEnv,��Ҫд��&tTaskEnv[xx]
	taskInit(&tTask2, task2Entry, (void*)0x00000000, &tTask2Env[1024]); 
	
	//Ϊ���ǵ�taskTable����
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0]; //��Ϊ�����Ǹտ�ʼ,��û��ִ�е�����,Ҳ����currentTask��û�б���ʼ��,��������Ӧ����nextTask����ʼ��
	
	//2.2 ��ʼִ�е�һ������: ��һ��������л���ͬ�ĵط�(ǰ��:ֱ�Ӱ�stack����Ķ����浽register,����ȥ��̨��ʱ��û����,��һ������ֱ�ӰѶ��������� | ����:��ǰ�����Ȱ�register�Ķ����浽stack��,�������Ӱ��Լ�stack����Ķ����浽register,����ȥ��̨��ʱ������,ǰ��Ŀ���:���̸�����,����Ŀ���:�ٷŶ���)
	tTaskRunFirst(); //�������,ҲҪ����pendSV�쳣, Ȼ����ΪpendSV��ִ��asm����,���Կ���pc,���Կ��Կ�����һ��Ҫȥ�ĺ���
	//ע��: ��ΪtTaskSwitch();tTaskRunFirst(); ��Ҫ��asm����������Ӳ��,�������ǾͰ������������Ķ���,�ŵ�switch.c��,��Ϊ���c�ļ��д����й�Ӳ���Ĳ���.���������������������,��Ҫ�ŵ�tinyOS.h��
	
	return 0; //ע��,���ﲻ��ִ�е�return 0,��ΪtTaskRunFirst()����ͻ�һֱ�ߵ�for loop
}
