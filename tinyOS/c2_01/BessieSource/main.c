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

int flag;

unsigned long stack[1024];
BlockType_t block;
BlockType_t* blockPtr;

int main()
{
	block.stackPtr = &stack[1024];
	blockPtr = &block;//��ô��Ƶ�ԭ��,��ѯ����:һ��blockptr����ָ��һ����block���׵�ַ(������stackptr��info),block.stackprt�ֿ���ָ��һ����stack���׵�ַ
	for(;;)
	{
		flag = 1;
		delay(100);
		flag = 0;
		delay(100);
		
		triggerPendSV();//todo:Ϊʲô����for���津��pendSV�쳣?
	}
}