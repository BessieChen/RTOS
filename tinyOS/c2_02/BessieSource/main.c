#include "tinyOS.h"

//_BlockType_t,可以存储很多信息,目前只有stackPtr
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

//2.2	每个task里面都有需要反转的flag
int task1Flag;
int task2Flag;

//2.1 设置完了tinyOS.h后,我们就设置两个stack,并且设置两个task, 再设置两个task执行的函数
tTaskStack tTask1Env[1024];		//因为一个tTaskStack只是一个uint32_t,只能代表stack里面的一个元素.所以我们设置成数组的形式
tTaskStack tTask2Env[1024];

tTask tTask1; //设置一个任务,还记得吗,tTask是一个结构体,里面包含一个tTaskStack*,之后我们对这个tTaskStack*初始化,就可以将tTask1和tTask1Env联系起来了
tTask tTask2;

//2.2 初始化了任务之后,自然还需要调度任务,所以需要设置current和next task
tTask* currentTask;
tTask* nextTask;

//2.2. 并且将两个任务都放进一个array中,易于管理
tTask* taskTable[2]; //这是一个tTask*的数组,也就是说,数组的每一个元素都是一个tTask*, 也就是每一个元素都存一个tTask类型的地址

void task1Entry(void * param) //设置task1任务需要执行的函数
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

//设置初始化任务的函数: 初始化stack: 将tTask1和tTask1Env联系起来, 初始化task函数
void taskInit(tTask* task, void (*func)(void*), void* param, tTaskStack* stack )
{
	//第一个参数: 需要初始化的那个task的地址
	//#2: task需要执行的函数func
	//#3: 该函数func的参数param
	//#4: task需要的stack的地址
	
	//2.2 过程: 用RunFirst()触发pendSV中断 -> 将task的stack里面的元素,赋值给register -> 最后让psp指向stack的栈顶 -> pendSV利用psp自动将stack里面剩余的元素依次存放到register中
	//所以,我们就需要在task初始化的这个函数taskInit()中,就把stack里面的元素填好
	
	//最后是给pendsv自动帮我们从stack里面取出,给register的.因为是后面pop,所以先push
	//注意!pendsv自动存给register,是有它的固定的顺序的,所以我们向stack存值的时候,需要的顺序也是固定的
	//先stack -= 1, 也就是地址-4(因为stack是tTaskStack类型的指针,tTaskStack类型是uint32_t)
	//因为stack里面的值,最后都要给register,register存的都是unsigned long,所以记得强制类型转换
	*(--stack) = (unsigned long) 1 << 24; //给xPSP
	*(--stack) = (unsigned long) func; //给PC,也就是R15. 这里不写成&func,我觉得是因为本来func就是一个指针(一个函数指针),相当于func的值就是地址,同时PC需要的也是地址. 注意,这一句,就可以让之后的PC指向入口函数,所以当出了PendSV_Handler之后,就会执行func指向的函数(也就是任务执行的函数)
	*(--stack) = (unsigned long) 0x14; //给LR,也就是R14,但是这里并没有用到,就随意付一个值,一个16进制的值.注意这里0x14只是一个字节,但是强制类型转换了. todo: 不是我们将LR设置成了0x04吗?为什么这里又是0x14?会不会被覆盖?
	*(--stack) = (unsigned long) 0x12;
	*(--stack) = (unsigned long) 0x3;
	*(--stack) = (unsigned long) 0x2;
	*(--stack) = (unsigned long) 0x1;
	*(--stack) = (unsigned long) param; //给R0寄存器,编译器默认:R0寄存器存的就是PC指向的函数的入口参数的值,虽然param是一个void*类型,但是这里强制类型转换了. 因为这里只是把param的值传进去,具体怎么处理这个param的值,是func里面做的事情
	
	//首先,出栈的是给R4-R11的,因为是最开始pop,所以在这里后push
	//注意,因为我们之后写的asm代码中,使用了LDMIA R0!, {R4-R11}, 所以是先pop给R4,所以最后一个push的是给R4的值0x4
	*(--stack) = (unsigned long) 0x11;
	*(--stack) = (unsigned long) 0x10;
	*(--stack) = (unsigned long) 0x9;
	*(--stack) = (unsigned long) 0x8;
	*(--stack) = (unsigned long) 0x7;
	*(--stack) = (unsigned long) 0x6;
	*(--stack) = (unsigned long) 0x5;
	*(--stack) = (unsigned long) 0x4;
	
	//填满stack之后,我们就把指向栈顶的指针stack,赋值给task的stack成员
	task->stack = stack; //因为task->stack是tTaskStack*,而且stack也是TaskStack*
}

//2.2 我们需要调度任务,也就是说,我们要决定哪个任务在下一轮可以使用cpu和资源
void tTaskSched()
{
	if(currentTask == taskTable[0])
		nextTask = taskTable[1];
	else
		nextTask = taskTable[0];
	tTaskSwitch(); //决定好了下一个任务是什么之后,就要切换了,这个tTaskSwitch()函数里面包含了触发pendSV异常,因为pendSV异常中使用汇编代码,可以控制PC,所以就可以转向新的函数了
}


int main()
{
	//将tTask绑定上相应的stack和func
	taskInit(&tTask1, task1Entry, (void*)0x11111111, &tTask1Env[1024]); //注意tTask1Env是一个uint32_t的数组,所以不能写成&tTaskEnv,而要写成&tTaskEnv[xx]
	taskInit(&tTask2, task2Entry, (void*)0x00000000, &tTask2Env[1024]); 
	
	//为我们的taskTable设置
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0]; //因为我们是刚开始,还没有执行的任务,也就是currentTask还没有被初始化,所以这里应该是nextTask被初始化
	
	//2.2 开始执行第一个任务: 和一般的任务切换不同的地方(前者:直接把stack里面的东西存到register,想象去柜台的时候没有人,第一个客人直接把东西放桌面 | 后者:当前的人先把register的东西存到stack中,新来的扔把自己stack里面的东西存到register,想象去柜台的时候有人,前面的客人:军铺盖走人,后面的客人:再放东西)
	tTaskRunFirst(); //这个函数,也要触发pendSV异常, 然后因为pendSV会执行asm代码,可以控制pc,所以可以控制下一个要去的函数
	//注意: 因为tTaskSwitch();tTaskRunFirst(); 都要用asm代码来管理硬件,所以我们就把这两个函数的定义,放到switch.c中,因为这个c文件中处理有关硬件的部分.但是这个两个函数的声明,需要放到tinyOS.h中
	
	return 0; //注意,这里不会执行到return 0,因为tTaskRunFirst()里面就会一直走到for loop
}
