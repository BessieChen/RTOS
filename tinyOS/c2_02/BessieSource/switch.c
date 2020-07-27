#include "tinyOS.h" //��Ϊ��Ҫ��tTaskRunFirst()��switch.c�ж���
#include "ARMCM3.h" //����������֮���õ���ָ��

//����Ϊ�˴���pendSVʹ�õ�
#define NVIC_INT_CTRL				0xE000ED04 //һ��32λ�Ĵ���,���Դ���pendSV
#define NVIC_SYSPRI2				0xE000ED22 //һ��8λ�Ĵ���,����pendsv�����ȼ�

#define NVIC_PENDSVSET  		0x10000000 //����pendSV��ֵ,32bits
#define NVIC_PENDSV_PRI			0x000000FF //����pendSV�����ȼ�Ϊ���,8bits

//д�Ĵ����ĺ�
#define MEM32(addr)					*((volatile unsigned long*) addr)
#define MEM8(addr)					*((volatile unsigned char*) addr)
	

__asm void PendSV_Handler(void) //����ʹ�õ���asm,���Һ�������һ����PendSV_Handler(),pendSV�쳣������ʱ��,�ͻ���ת������
{
	//����Ϊ,Ӧ���ڸս������������ʱ��,�Ѿ��ǽ�xPSP,R11�ȼĴ��������Ԫ��,�浽PSPָ���stack����, ���������������ֶ��ӹ�PSP,������stack�д���Ĵ���R4-R11��ֵ //���PSP==0,�Ҿ��ÿ��ܾͲ����xPSP�Ķ���,�浽PSPָ���stack����
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP						//��psp��ֵ,����r0 //ע��������MRS: PSP -> register
	CBZ R0, pendSV_store_only //�ж�r0 != 0?
	
	//�������Ǿ��̸�����
	STMDB R0!, {R4-R11}		//��ΪRO��ֱ��ָ������Ҫ���̸����˵�task��stack,����ֱ�ӽ�R4-T11ѹ������, ���r0��Ϊ���µ�ջ����ַ
	
	LDR R1, =currentTask  //R1��������currenttask�ĵ�ַ
	LDR R1, [R1]					//R1�������, currenttask������(task B�ĵ�ַ)
	LDR R1, [R1]					//R1�������, task B������(stack B�ĵ�ַ)
	
	STR R0, [R1]					//�����stack B�д����µ�ַ(R0��ֵ)
  
	
pendSV_store_only				//��������ж�°���
	LDR R0, =nextTask			
	LDR R0, [R0]					//ִ����,R0�����nextTask������(task A�ĵ�ַ)
	
	LDR R1, =currentTask	//R1�����currentTask�ĵ�ַ
	STR R0, [R1]					//[R1]:ȥ�����currentTask������(task B�ĵ�ַ). ��[r1]�ĳ�task A�ĵ�ַ: ����currentTask�����ݾͳ���task A�ĵ�ַ,�൱��currentTask = nextTask	
	//������ڵ�Ӧ����֤currentTask������==nextTask������
	
	LDR R0, [R0]					//��ʱR0�����task A������,���������stack A�ĵ�ַ
	LDMIA R0!, {R4-R11}		//ȥ���stack A�ĵط�,��stack A�����Ԫ��,pop����,�ֱ��R4-R11(�ȸ�R4,����R11)

	STR R0, [R1]					//��57��,R1��Ļ���task B�ĵ�ַ,�������ǽ�R0������,Ҳ�����µ�ַ,����R1ָ��ĵط�,����֮��task B�����ݾ����µ�stack B�ĵ�ַ

	MSR PSP, R0						//ע��������MSR: register -> PSP
	ORR LR, LR, #0x04			//��һ���ʾ����,֮�����ǽ�ʹ��PSP���ջָ��, ��ΪPSPָ�����R0,����,���ǿ��Դ�R0��ʼpop,Ҳ���ǽ�stack B�е�ʣ�����ݴ浽R13,R14��
	BX LR
}

void tTaskRunFirst()
{
	//��PSP���ó�0,��Ϊ�ڵȻ�����д��asm������,���ǻ��ж����PSP==0����ת��xxx
	__set_PSP(0);//__set_PSP()����ARMCM3.h���涨���
	
	//����pendSV�쳣, ��c����->�Ĵ�������->pendSV_handler()Ҳ����asm����д��
	//����Ϊ������ȼ�,��ôpendSV�쳣�����ȼ� < �ж����ȼ� < SysTick���ȼ�
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	
	//���������´���,pendSV������
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
	
}

void tTaskSwitch()
{
	//���������´���,pendSV������
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
