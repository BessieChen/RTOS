#include "tinyOS.h"
#include "ARMCM3.h" //������SysTick

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

//2.2 ������Ҫ��������,Ҳ����˵,����Ҫ�����ĸ���������һ�ֿ���ʹ��cpu����Դ, ��һ��������Ҫ�ŵ�task1Entry()ǰ��,��Ϊtask1Entry()��������
void tTaskSched()
{
	if(currentTask == taskTable[0])
		nextTask = taskTable[1]; //����Ϊ�˸��ٴν���pendSV()�쳣Ҳ����asm������,nextTask�ĸ������̵�
	else
		nextTask = taskTable[0];
	tTaskSwitch(); //����������һ��������ʲô֮��,��Ҫ�л���,���tTaskSwitch()������������˴���pendSV�쳣,��ΪpendSV�쳣��ʹ�û�����,���Կ���PC,���ԾͿ���ת���µĺ�����
}

//�������������ǵļ�ʱ��,����Ĳ���������ϣ���ĺ���
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

//����ʱ���ж�ʱ�Ĵ�����
void SysTick_Handler()
{
	//�жϵ�ʱ��Ӧ����ʲô? Ӧ�����л�����: ��Ҫ�ȵ�����һ������,Ҳ����ȷ����nextTask��ʲô -> ���������Ǵ���systick�쳣
	
	//1. ������һ������
	tTaskSched();
	
	//2. ���������Ǵ���systick�쳣
	
}

void task1Entry(void * param) //����task1������Ҫִ�еĺ���
{
	//����������Ϊ���ܹ�param�ܹ���ʹ��,�����Ͳ��ᱻ�Ż�(ɾ��)
	unsigned long value = *(unsigned long *)param;
	value++;
	
	//��������systick�ĳ�ʼ������,��Ϊsystick�ĳ�ʼ������tSetSysTickPeriod()��,��ʹ�ܼ��������ж�,Ϊ�˱����ж������Ļ���,�ͽ�systick�ĳ�ʼ����������
	//��ʼ��systick,����Ϊ10����(������ʵ����ִ����1.4��,������Ҳ����Ҹ�����,�о���û�б仯),��������ʾ��1s,�Ҹо����ǹ��˺ܳ�ʱ��!
	tSetSysTickPeriod(10);//��Ϊ֮��Ͷ���for loop������,�����ֻ��������һ��
	/*
	����ԭ��Ϊʲô�������ʼ��systick:
	1. ��Ϊ����������ط���ʼ��systick,��ʼ��֮��,systick��������ʼ������,���Ե�һ�η����жϵ�ʱ��,���ܾͻ����tTaskSched,��������Ҫ�ж�currentTask�ǲ���==0,��Ϊ���Ǹտ�ʼcurrentTask����
	*/
	//����:�ǵ�����ʱ��Ƶ��:
	/*
	ȥsystem_ARMCM3.c�ļ�,��41��42�иĳ�
	#define  XTAL            ( 12000000U)     // Oscillator frequency   
	#define  SYSTEM_CLOCK    (1 * XTAL)	
	�����Ǹĳ���12��Ƶ��
	Ȼ��ϵͳʱ��SYSTEM CLock����12��*1
	12��: �ִ�������:12��=12����,Ҳ����1��==1����==1 million, Ҳ����1�ĺ���6����,�������12�ĺ���6����
	���Ǽ������,һ����2^30
	*/
	
	for(;;){
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
		
		//tTaskSched();//��systick������,����Ͳ���Ҫ��
	}
}
void task2Entry(void * param) 
{
	for(;;){
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
		
		//tTaskSched();//��systick������,����Ͳ���Ҫ��
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
	task->stack = stack; //��Ϊtask->stack��tTaskStack*,����stackҲ��TaskStack*, ��Ϊ�����task��ָ��(��ַ),�����൱�ڵ�ַ->stack
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
