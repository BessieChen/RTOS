#include "tinyOS.h" //因为需要给tTaskRunFirst()在switch.c中定义
#include "ARMCM3.h" //包含了我们之后用到的指令

//这是为了触发pendSV使用的
#define NVIC_INT_CTRL				0xE000ED04 //一个32位寄存器,可以触发pendSV
#define NVIC_SYSPRI2				0xE000ED22 //一个8位寄存器,设置pendsv的优先级

#define NVIC_PENDSVSET  		0x10000000 //触发pendSV的值,32bits
#define NVIC_PENDSV_PRI			0x000000FF //设置pendSV的优先级为最低,8bits

//写寄存器的宏
#define MEM32(addr)					*((volatile unsigned long*) addr)
#define MEM8(addr)					*((volatile unsigned char*) addr)
	

__asm void PendSV_Handler(void) //这里使用的是asm,并且函数名字一定是PendSV_Handler(),pendSV异常发生的时候,就会跳转到这里
{
	//我认为,应该在刚进入这个函数的时候,已经是将xPSP,R11等寄存器里面的元素,存到PSP指向的stack中了, 接下来就是我们手动接管PSP,并且往stack中存入寄存器R4-R11的值 //如果PSP==0,我觉得可能就不会把xPSP的东西,存到PSP指向的stack中了
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP						//将psp的值,存入r0 //注意这里是MRS: PSP -> register
	CBZ R0, pendSV_store_only //判断r0 != 0?
	
	//接下来是军铺盖走人
	STMDB R0!, {R4-R11}		//因为RO就直接指向了需要管铺盖走人的task的stack,所以直接将R4-T11压入其中, 最后r0成为了新的栈顶地址
	
	LDR R1, =currentTask  //R1存的最后是currenttask的地址
	LDR R1, [R1]					//R1最后存的是, currenttask的内容(task B的地址)
	LDR R1, [R1]					//R1最后存的是, task B的内容(stack B的地址)
	
	STR R0, [R1]					//最后网stack B中存入新地址(R0的值)
  
	
pendSV_store_only				//接下来是卸下包裹
	LDR R0, =nextTask			
	LDR R0, [R0]					//执行完,R0存的是nextTask的内容(task A的地址)
	
	LDR R1, =currentTask	//R1存的是currentTask的地址
	STR R0, [R1]					//[R1]:去到这个currentTask的内容(task B的地址). 将[r1]改成task A的地址: 于是currentTask的内容就成了task A的地址,相当于currentTask = nextTask	
	//在这个节点应该验证currentTask的内容==nextTask的内容
	
	LDR R0, [R0]					//此时R0存的是task A的内容,这个内容是stack A的地址
	LDMIA R0!, {R4-R11}		//去这个stack A的地方,将stack A里面的元素,pop出来,分别给R4-R11(先给R4,最后给R11)

	STR R0, [R1]					//见57行,R1存的还是task B的地址,所以我们将R0的内容,也就是新地址,放入R1指向的地方,所以之后task B的内容就是新的stack B的地址

	MSR PSP, R0						//注意这里是MSR: register -> PSP
	ORR LR, LR, #0x04			//这一句表示的是,之后我们将使用PSP这个栈指针, 因为PSP指向的是R0,所以,我们可以从R0开始pop,也就是将stack B中的剩余数据存到R13,R14等
	BX LR
}

void tTaskRunFirst()
{
	//将PSP设置成0,因为在等会我们写的asm代码中,我们会判断如果PSP==0就跳转到xxx
	__set_PSP(0);//__set_PSP()就是ARMCM3.h里面定义的
	
	//触发pendSV异常, 从c语言->寄存器触发->pendSV_handler()也就是asm语言写的
	//设置为最低优先级,那么pendSV异常的优先级 < 中断优先级 < SysTick优先级
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	
	//运行完以下代码,pendSV被触发
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
	
}

void tTaskSwitch()
{
	//运行完以下代码,pendSV被触发
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
