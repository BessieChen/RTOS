#include "tinyOS.h"

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

//2.1 设置完了tinyOS.h后,我们就设置两个stack,并且设置两个task, 再设置两个task执行的函数
tTaskStack tTask1Env[1024];		//因为一个tTaskStack只是一个uint32_t,只能代表stack里面的一个元素.所以我们设置成数组的形式
tTaskStack tTask2Env[1024];

tTask tTask1; //设置一个任务,还记得吗,tTask是一个结构体,里面包含一个tTaskStack*,之后我们对这个tTaskStack*初始化,就可以将tTask1和tTask1Env联系起来了
tTask tTask2;

void task1(void * param) //设置task1任务需要执行的函数
{
	for(;;){}
}
void task2(void * param) 
{
	for(;;){}
}

//设置初始化任务的函数: 初始化stack: 将tTask1和tTask1Env联系起来, 初始化task函数
void taskInit(tTask* task, void (*func)(void*), void* param, tTaskStack* env )
{
	//第一个参数: 需要初始化的那个task的地址
	//#2: task需要执行的函数func
	//#3: 该函数func的参数param
	//#4: task需要的stack的地址
	
	task->stack = env; //因为stack是tTaskStack*,而且env也是TaskStack*
}


int main()
{
	//将tTask绑定上相应的stack和func
	taskInit(&tTask1, task1, (void*)0x11111111, &tTask1Env[1024]); //注意tTask1Env是一个uint32_t的数组,所以不能写成&tTaskEnv,而要写成&tTaskEnv[xx]
	taskInit(&tTask2, task2, (void*)0x00000000, &tTask2Env[1024]); 
	
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