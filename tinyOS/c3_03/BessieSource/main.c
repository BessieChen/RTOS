#include "tinyOS.h"
#include "ARMCM3.h" //包含了SysTick

//3.3 不需要调度锁了
//int shareCount;

//2.2	每个task里面都有需要反转的flag
int task1Flag;
int task2Flag;

//2.2. 并且将两个任务都放进一个array中,易于管理
tTask* taskTable[2]; //这是一个tTask*的数组,也就是说,数组的每一个元素都是一个tTask*, 也就是每一个元素都存一个tTask类型的地址

//2.1 设置完了tinyOS.h后,我们就设置两个stack,并且设置两个task, 再设置两个task执行的函数
tTaskStack tTask1Env[1024];		//因为一个tTaskStack只是一个uint32_t,只能代表stack里面的一个元素.所以我们设置成数组的形式
tTaskStack tTask2Env[1024];
tTaskStack idleTaskEnv[1024]; //2.4

tTask tTask1; //设置一个任务,还记得吗,tTask是一个结构体,里面包含一个tTaskStack*,之后我们对这个tTaskStack*初始化,就可以将tTask1和tTask1Env联系起来了
tTask tTask2;
tTask tTaskIdle; //2.4

//2.2 初始化了任务之后,自然还需要调度任务,所以需要设置current和next task
tTask* currentTask;
tTask* nextTask;
tTask* idleTask; //2.4


//3.2 定义调度锁的计数器, 是uint8, 只有当调度锁==0的时候,才可以触发调度函数tTaskSched()
uint8_t schedLockCount;

//3.2 初始化调度锁, 区别于临界区:临界区是使用__get_primask()来关闭中断,也就是使用了ARMCM3.h文件的函数,但是调度锁使我们自己定义的
void tTaskSchedInit(void)
{
	schedLockCount = 0;
}
//3.2 开始使用调度锁, 也就是让调度函数tTaskSched()失效
void tTaskSchedDisable(void)//注意这个名字, 这个名字是Disable,因为要sched()失效
{
	//因为调度锁也是全局变量,又因为这个tTaskSchedDisable()函数也会被systick中断或者任务调用(因为他们要使用这个函数去保护全局变量xxx),所以我们需要保护调度所,用临界区保护
	uint32_t status = tTaskEnterCritical();
	
	//开始操作调度锁,注意,因为调度锁是uint8,最大是255, 所以如果==255,我们就不+1, 防止溢出
	if(schedLockCount <= 254)
	{
		schedLockCount++;
	}
	tTaskExitCritical(status);
}

//3.2 现在需要修改一下我们的tTaskSched()
void tTaskSched(void)
{
	//2.4以下的都要删掉
	/*
	if(currentTask == taskTable[0])
		nextTask = taskTable[1]; //这是为了给再次进入pendSV()异常也就是asm代码中,nextTask的更新做铺垫
	else
		nextTask = taskTable[0];
	tTaskSwitch(); //决定好了下一个任务是什么之后,就要切换了,这个tTaskSwitch()函数里面包含了触发pendSV异常,因为pendSV异常中使用汇编代码,可以控制PC,所以就可以转向新的函数了
	*/
	
	//2.4 步骤:
	/*
		1. 如果当前任务是idle
			1. 如果task1的delayTicks==0, 下一个任务就是task1
			2. task2同上
			3. 假设两个task的delayTicks != 0, 那么什么都不用做, 因为下一个任务还依旧是当前任务idelTask
			我的疑问:如果task1和task2都是delayTicks==0,但是这个函数确是先判断了task1
		2. 如果当前任务是task1
			1. 如果task2的dT==0, 下一个任务就是task2
			2. 如果task1的dT!=0, 也就是说当前task1还是不需要cpu,因为走到第二步,说明task2的dT!=0, 既然他们俩都不要cpu, 此时下一个任务是idleTask
			3. 假设task1的dT == 0, 那么什么都不用做, 因为下一个任务还依旧是当前任务task1
			我的疑问:那就是如果task1和2的dT都==0,那也依旧优先给task2,因为先判断task2
		3. 如果当前任务是task2
			1.2.3同上
	*/
	
	//3.2 为了保险起见, 我们需要判断调度锁是不是==0,如果确定是==0,才会执行后面的语句,否则就return
	//同样, 判断调度锁的值的时候,为了保险,还是用临界区来保护
	uint32_t status = tTaskEnterCritical();
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
	if(currentTask == idleTask) //注意idelTask是指针, tTask* idleTask = &tTaskIdle; //extern tTask* idleTask; //需要在tinyOS.h里写上这一句,否则main.c会报错, 要么就是全局变量都写在这个函数前面
	{
		if(taskTable[0]->delayTicks == 0)
		{
			nextTask = taskTable[0];
		}
		else if(taskTable[1]->delayTicks == 0)
		{
			nextTask = taskTable[1];
		}
		else //也就是两个task的dT != 0, 他俩都不要cpu
		{
			tTaskExitCritical(status);
			return; //所以nextTask依旧是currentTask,和之前一样
		}
	}
	else if(currentTask == taskTable[0])
	{
		if(taskTable[1]->delayTicks == 0)
		{
			nextTask = taskTable[1]; //如果task1和2的dT都==0,那也依旧优先给task2,因为先判断task2
		}
		else if(taskTable[0]->delayTicks != 0)
		{
			nextTask = idleTask; //说明也就是两个task的dT != 0, 他俩都不要cpu, 所以给idle
		}
		else //说明task2的dT != 0, 但是task1的dT == 0, 所以nextTask依旧是task1
		{
			tTaskExitCritical(status);
			return;
		}
	}
	else //currentTask == taskTable[1]
	{
		if(taskTable[0]->delayTicks == 0)
		{
			nextTask = taskTable[0]; //如果task1和2的dT都==0,那也依旧优先给task1,因为先判断task1
		}
		else if(taskTable[1]->delayTicks != 0)
		{
			nextTask = idleTask; //说明也就是两个task的dT != 0, 他俩都不要cpu, 所以给idle
		}
		else //说明task1的dT != 0, 但是task2的dT == 0, 所以nextTask依旧是task2
		{
			tTaskExitCritical(status);
			return;
		}
	}
	tTaskSwitch();
	tTaskExitCritical(status);
}




//3.2 退出调度锁, 也就是让调度函数tTaskSched()生效 : 当然,只有当调度锁==0的时候,才会让tTaskSched()生效, 生效的意思是:立即执行tTaskSched(), 但是执行tTaskSched()不代表就去真的调度,因为是否应该调度要需要看我自己的delay是不是==0,别人的delay是不是==0
//注意, 这个函数需要放到tTaskSched()下面,因为需要使用到tTaskSched()
void tTaskSchedEnable(void)	
{
	//同理,需要用临界区来保护调度所
	uint32_t status = tTaskEnterCritical();
	
	//安全起见,先判断调度锁是不是已经==0了
	if(schedLockCount > 0)
	{
		schedLockCount--;
	}
	//只有当调度锁==0的时候,才会让tTaskSched()生效, 生效的意思是:立即执行tTaskSched(), 但是执行tTaskSched()不代表就去真的调度,因为是否应该调度要需要看我自己的delay是不是==0,别人的delay是不是==0
	if(schedLockCount == 0)
	{
		tTaskSched();
	}
	tTaskExitCritical(status);
}

//3.1 设置临界区
uint32_t tTaskEnterCritical(void)
{
	uint32_t primask = __get_PRIMASK(); //这是"ARMCM3.h"的
	__disable_irq(); //"ARMCM3.h"的,是关闭中断
	return primask; //返回中断使能
}

//3.1 退出临界区
void tTaskExitCritical(uint32_t primask)
{
	__set_PRIMASK(primask);
}

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


//2.4 因为现在每个任务都有一个计数器delayTicks, 现在设计一个函数,能够递减delayTicks(相当于软延迟)
void tTaskSysTickHandler()
{
	int i;
	for(i = 0; i < 2; i++)
	{
		if(taskTable[i]->delayTicks > 0) //如果进入if,说明要需要递减
		{
			taskTable[i]->delayTicks--;
		}
	}
	
	//因为上面递减了delayTicks,所以有可能有一个以上的任务延迟完毕,可以被开启了. 以下的tTaskSched()里面就可以判断是否delayTicks==0,是否可以被开启
	tTaskSched();
}

//2.4 发生时钟中断时的处理函数, 就像是pendSV一样,是系统会自动调用到这里的,注意函数名不要写错
void SysTick_Handler()
{
	//中断的时候应该做什么? 应该是切换任务: 需要先调度下一个任务,也就是确定好nextTask是什么 -> 接下来就是触发systick异常
	
	//1. 调度下一个任务
	//tTaskSched(); //2.4中就不能直接调用这个函数
	
	//2.4, 需要先递减delayTicks,然后调用tTaskSched()
	tTaskSysTickHandler(); //这个函数里面会调用tTasksched()
	
}

//2.4 实现延时函数, 因为这个函数肯定是currentTask调用的,所以里面直接写currentTask->xxx
void tTaskDelay(uint32_t delay)
{
	currentTask->delayTicks = delay; //也就是为currentTask设置需要延时多久
	tTaskSched();//所以现在currentTask是放弃了cpu,现在由tTaskSched()来判断还有谁是需要cpu的
}

//2.4 这里是配置我们的计时器,传入的参数是我们希望的毫秒
void tSetSysTickPeriod(uint32_t ms)
{
	//重置计数器的时候的值:也就是说,每次计数器中断的时候,会将(递减计数器)的值-1,当递减计数器的值==0的时候,会将递减计数器的值设置成这个重置计数器的值
	SysTick->LOAD = ms / 1000 * SystemCoreClock - 1; //其实我不是很清楚为什么-1, 可能是因为觉得设置成重置计数器的时候,也需要花费时间,所以-1相当于用于这个花费时间?
	
	//见下补充
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	
	//递减计数器
	SysTick->VAL = 0;
	
	//其他配置
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
									SysTick_CTRL_TICKINT_Msk | //产生中断使能的标志位
									SysTick_CTRL_ENABLE_Msk;	//使能计数器的标志位
	
	//补充:
	/*
	1. SysTick_IRQn是指systick的基地�
	2. __NVIC_PRIO_BITS是4的宏定义,这里指的是占先优先级为4,因为m3内核中占先优先级和响应由县级一共是4位,这里占先优先级已经有4位,所以响应优先级只有0位
	这里代表的是最低优先级: 1<<4 -1, 也就是16-1=15,也就是1111, 注意优先级是0的时候是最高优先级
	*/
}




//3.3 我自己设计的, 检测报错的函数
void ifError(void)
{
	
}

//2.1 设置task1任务需要执行的函数
void task1Entry(void * param) 
{
	//3.3 我们来检测一下我们的bitmap的函数是否运行正确: 
	//首先定义:
	tBitmap bitmap;
	int i; //注意, 容易错的地方: 我们要将i设置成int,而不是uint32_t(unsigned int的意思), 不要因为tBitmapPosCount()返回的是uint32_t,就将i设置成uint32_t
					//uint32_t i, 如果在递增的时候可以,但是在递减的时候, i--, 永远都是i > 0 , 就是会无限循环!
	uint32_t flag; 
	
	tSetSysTickPeriod(1);
	
	//3.3 我们来检测一下我们的bitmap的函数是否运行正确: 
	//3.3 首先将bitmap初始化
	tBitmapInit(&bitmap); //注意,因为需要bitmap指针,所有这里直接传入了bitmap的地址
	//3.3 从最高bit开始设置成1, 如果报错, 会走到ifError()
	for(i = tBitmapPosCount() - 1; i >= 0; i --) //注意: 首先设置的是第31位 //注意!是tBitmapPosCOunt(), 是函数啊!! 不是tBitmapPosCOunt啊!!
	{
		tBitmapSet(&bitmap, i);
		flag = tBitmapGetFirstSet(&bitmap);
		if(flag != i )
		{
			ifError();
		}
	}
	//3.3 从最低bit开始设置成0, 如果报错, 会走到ifError()
	for(i = 0; i < tBitmapPosCount(); i++)
	{
		tBitmapClear(&bitmap, i);
		flag = tBitmapGetFirstSet(&bitmap);
		if(flag != (i + 1) ) //注意, 这里是i+1, 因为是删除了第i位的元素,所以flag是i+1. 如果删除的是第31位元素, 变成bitmap全0, 那么刚好也是返回32 == 31+1
		{
			ifError();
		}
	}
	
	for(;;){
		
	
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);

	}
}
void task2Entry(void * param) 
{
	for(;;){

		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
	}
}


//2.4 添加一个空闲任务, 里面什么都不做, 等着其他任务的结束延迟
void idleTaskEntry(void* param)
{
	for(;;)
	{}
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
	task->stack = stack; //因为task->stack是tTaskStack*,而且stack也是TaskStack*, 因为这里的task是指针(地址),所以相当于地址->stack
	task->delayTicks = 0; //2.4, 因为tinyOS.h中往task结构体添加了成员delayTicks, 所以这里也要写上
}


int main()
{
	
	//3.2 因为shareCount是我们自己的定义的调度锁,所以需要初始化
	tTaskSchedInit();
	
	//将tTask绑定上相应的stack和func
	taskInit(&tTask1, task1Entry, (void*)0x11111111, &tTask1Env[1024]); //注意tTask1Env是一个uint32_t的数组,所以不能写成&tTaskEnv,而要写成&tTaskEnv[xx]
	taskInit(&tTask2, task2Entry, (void*)0x00000000, &tTask2Env[1024]); 
	taskInit(&tTaskIdle, idleTaskEntry, (void*)0x00000001, &idleTaskEnv[1024]); //2.4, idleTask的初始化
	
	//为我们的taskTable设置
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	idleTask = &tTaskIdle; //2.4
	
	nextTask = taskTable[0]; //因为我们是刚开始,还没有执行的任务,也就是currentTask还没有被初始化,所以这里应该是nextTask被初始化
	
	//2.2 开始执行第一个任务: 和一般的任务切换不同的地方(前者:直接把stack里面的东西存到register,想象去柜台的时候没有人,第一个客人直接把东西放桌面 | 后者:当前的人先把register的东西存到stack中,新来的扔把自己stack里面的东西存到register,想象去柜台的时候有人,前面的客人:军铺盖走人,后面的客人:再放东西)
	tTaskRunFirst(); //这个函数,也要触发pendSV异常, 然后因为pendSV会执行asm代码,可以控制pc,所以可以控制下一个要去的函数
	//注意: 因为tTaskSwitch();tTaskRunFirst(); 都要用asm代码来管理硬件,所以我们就把这两个函数的定义,放到switch.c中,因为这个c文件中处理有关硬件的部分.但是这个两个函数的声明,需要放到tinyOS.h中
	
	return 0; //注意,这里不会执行到return 0,因为tTaskRunFirst()里面就会一直走到for loop
}
