#include "tinyOS.h"

#define NVIC_INT_CTRL				0xE000ED04 //һ��32λ�Ĵ���,���Դ���pendSV
#define NVIC_SYSPRI2				0xE000ED22 //һ��8λ�Ĵ���,����pendsv�����ȼ�

#define NVIC_PENDSVSET  		0x10000000 //����pendSV��ֵ,32bits
#define NVIC_PENDSV_PRI			0x000000FF //����pendSV�����ȼ�Ϊ���,8bits

//д�Ĵ����ĺ�
#define MEM32(addr)					*((volatile unsigned long*) addr)
#define MEM8(addr)					*((volatile unsigned char*) addr)

//_BlockType_t,���Դ洢�ܶ���Ϣ,Ŀǰֻ��stackPtr
typedef struct _BlockType_t
{
	unsigned long* stackPtr;
}BlockType_t;

//��δ���pendSV�쳣,���Ǵ�c����->�Ĵ�������->pendSV_handler()Ҳ����asm����д��
void triggerPendSV(void)
{
		//����Ϊ������ȼ�,��ôpendSV�쳣�����ȼ� < �ж����ȼ� < SysTick���ȼ�
		MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	
		//���������´���,pendSV������
		MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

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


int main()
{
	//��tTask������Ӧ��stack��func
	taskInit(&tTask1, task1Entry, (void*)0x11111111, &tTask1Env[1024]); //ע��tTask1Env��һ��uint32_t������,���Բ���д��&tTaskEnv,��Ҫд��&tTaskEnv[xx]
	taskInit(&tTask2, task2Entry, (void*)0x00000000, &tTask2Env[1024]); 
	
	for(;;)
	{
		flag = 1;
		delay(100);
		flag = 0;
		delay(100);
		
		triggerPendSV();//todo:Ϊʲô����for���津��pendSV�쳣?
	}
}