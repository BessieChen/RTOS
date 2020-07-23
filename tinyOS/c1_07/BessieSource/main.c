#define NVIC_INT_CTRL				0xE000ED04 //一个32位寄存器,可以触发pendSV
#define NVIC_SYSPRI2				0xE000ED22 //一个8位寄存器,设置pendsv的优先级

#define NVIC_PENDSVSET  		0x10000000 //触发pendSV的值,32bits
#define NVIC_PENDSV_PRI			0x000000FF //设置pendSV的优先级为最低,8bits

//写寄存器的宏
#define MEM32(addr)					*((volatile unsigned long*) addr)
#define MEM8(addr)					*((volatile unsigned char*) addr)

//_BlockType_t,可以存储很多信息,目前只有stackPtr
typedef struct _BlockType_t
{
	unsigned long* stackPtr;
}BlockType_t;

//如何触发pendSV异常,就是从c语言->寄存器触发->pendSV_handler()也就是asm语言写的
void triggerPendSV(void)
{
		//设置为最低优先级,那么pendSV异常的优先级 < 中断优先级 < SysTick优先级
		MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	
		//运行完以下代码,pendSV被触发
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
	blockPtr = &block;//这么设计的原因,查询方便:一个blockptr可以指向一大块的block的首地址(包含了stackptr等info),block.stackprt又可以指向一大块的stack的首地址
	for(;;)
	{
		flag = 1;
		delay(100);
		flag = 0;
		delay(100);
		
		triggerPendSV();//todo:为什么是在for里面触发pendSV异常?
	}
}