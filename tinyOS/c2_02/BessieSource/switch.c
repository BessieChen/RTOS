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
	//���ǵ�Ŀ����:��register�������stack��ֵ
	//����,���ҵ�stack�ĵ�ַ.
	//����: һ��blockptr����ָ��һ����block���׵�ַ(������stackptr��info),block.stackprt�ֿ���ָ��һ����stack���׵�ַ
	
	IMPORT blockPtr
	LDR R0, =blockPtr	//��blockPtr�ĵ�ַ,��ֵ��r0.���ں��൱��&
	LDR R0, [R0] 			//[r0]����blockPtr���������,���ֵ����block�ĵ�ַ
	LDR R0, [R0]			//[r0]������block���������,��Ϊblock��һ���ṹ��,����װ��block.stackPtr,����block��ֵ���൱����block.stackPtr��ֵ, ��stackPtr��ֵ��ʵ��stack�ĵ�ַ(����block.stackPtr = &stack[1024]), ����[r0]����stack�ĵ�1024��Ԫ�صĵ�ַ
	
	STMDB R0!, {R4-R11}		//��r4-r11��ֵ,����r0ָ���λ��, STMDB��store��mem����˼, ���Ȱ�R0��ֵ��R11,֮���R10,... ��������ջ����R4
												//����֮��,stack������register���������
												//ע��:R0Ӧ���ǵ�ַ, ����,�ҷ����µ�R0�Ⱦɵ�R0С��32,R4-R11һ��8���Ĵ���,�պ�һ���Ĵ�������4���ֽ�
	
												//��Ϊstack�����µ�Ԫ��,����stack��ջ����ַ��Ҫ�ı�(�µĵ�ַ�Ǹ�С��),r0���������ַ,���Խ�������r0��ֵ,����block.stackPtr
	LDR R1, =blockPtr			//�����line7��˼·һ��,��blockPtr�ĵ�ַ,��ֵ��r1
	LDR R1, [R1]					//[R1]����blockPtr���������,���ֵ����block�ĵ�ַ
	STR R0, [R1]					//[R1]����block���������,��Ϊblock��һ���ṹ��,����װ��block.stackPtr,���ڰ�r0��ֵ,����block.stackPtr

	ADD R4, R4, #1				//�����register�����ֵ�޸�
	ADD R5, R5, #1
	
	LDMIA R0!, {R4-R11}		//�����ǽ�stack������r4-r11��ֵ,�и�ֵ��r4-r11,����line21-22��register���޸ľ���Ч��
	
	BX LR 								//�����Ƿ��ص��������ĸ��׺���,Ҳ���Ƿ��ص�triggerPendSV()
	
	IMPORT currentTask
	IMPORT nextTask
	
	
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