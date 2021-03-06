#include "tinyOS.h"
#include "ARMCM3.h" //包含了SysTick

//3.4 定义: 任务就绪表, taskTable
tBitmap taskPrioBitmap;

//2.2	每个task里面都有需要反转的flag
int task1Flag;
int task2Flag;

//3.4 taskTable现在里面有TINYOS_PRIO_COUNT个元素
tTask* taskTable[TINYOS_PRIO_COUNT]; 

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

//3.4 虽然是初始化调度锁,但是这里也包括了bitmap的初始化,因为main()第一个调用的就是这个函数.  调度锁区别于临界区:临界区是使用__get_primask()来关闭中断,也就是使用了ARMCM3.h文件的函数,但是调度锁使我们自己定义的
void tTaskSchedInit(void)
{
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);//3.4
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

//3.4 现在需要修改一下我们的tTaskSched()
void tTaskSched(void)
{
	//3.4 下面需要tTask*
	tTask* highestPrioTask;
	
	//3.2 为了保险起见, 我们需要判断调度锁是不是==0,如果确定是==0,才会执行后面的语句,否则就return
	//同样, 判断调度锁的值的时候,为了保险,还是用临界区来保护
	uint32_t status = tTaskEnterCritical();
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
	
	//3.4 接下来是修改部分, 逻辑很简单, 看当前的任务是不是优先级最高的,如果不是,就切换到优先级最高的上面
	highestPrioTask = tTaskHighestTaskReady();
	if(highestPrioTask == currentTask)
	{
		tTaskExitCritical(status);
		return;
	}
	nextTask = highestPrioTask;
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


//3.4 需要修改,每次systick都需要把taskTable中的每一个任务的delayTicks减一 // 2.4 因为现在每个任务都有一个计数器delayTicks, 现在设计一个函数,能够递减delayTicks(相当于软延迟)
void tTaskSysTickHandler()
{
	uint32_t status = tTaskEnterCritical();
	int i;
	for(i = 0; i < TINYOS_PRIO_COUNT; i++) //3.4
	{
		if(taskTable[i] == (tTask*)0) //3.4 记得要强制类型转换,我发现这里不能用null或者NULL
		{
			continue; //首先,我要判断taskTable的哪个元素是不是有任务
		}
		
		if(taskTable[i]->delayTicks > 0) 
		{
			taskTable[i]->delayTicks--;
		}
		else
		{
			tBitmapSet(&taskPrioBitmap, taskTable[i]->prio); //3.4 因为如果某个task的delayTick==0, 说明这个task也要进入就绪状态了, 位图中的对应位置设置成1
		}
	}
	tTaskExitCritical(status);
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

//3.4 这里需要将修改 //2.4 实现延时函数, 因为这个函数肯定是currentTask调用的,所以里面直接写currentTask->xxx
void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	currentTask->delayTicks = delay; //也就是为currentTask设置需要延时多久
	tBitmapClear(&taskPrioBitmap, currentTask->prio); //3.4 因为延时说明currentTask不需要cpu,既然不需要,说明就绪表中对应的元素就设置成0
	//3.4 注意:即便clear()了,但是currentTask的prio还是没有变化, taskTable中也还是有这个任务
	
	tTaskExitCritical(status);
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
	tSetSysTickPeriod(1);
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



//3.4 设置优先级
void taskInit(tTask* task, void (*func)(void*), void* param, uint32_t prio, tTaskStack* stack )
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
	task->prio = prio; //3.4
	
	//3.4 初始化一个task之后, 要把task放到taskTable的相应位置
	taskTable[prio] = task; //注意,这里的task本身就是一个地址,所以不要写成 = &task;
	
	//3.4 要将就绪表中的相应位置设置成1
	tBitmapSet(&taskPrioBitmap, prio);
}

//3.4 返回当前最高优先级的任务
tTask* tTaskHighestTaskReady(void) //注意,这里不需要传入&taskPrioBitmap作为参数,因为默认是全局变量. 当然你想改成要输入参数的也可以
{
	uint32_t prio = tBitmapGetFirstSet(&taskPrioBitmap);
	return taskTable[prio];
}

int main()
{
	
	//3.4 因为shareCount是我们自己的定义的调度锁,所以需要初始化, 这个函数里面包含了 初始化就绪表, 也就是bitmap的初始化
	tTaskSchedInit();

	
	//将tTask绑定上相应的stack和func
	taskInit(&tTask1, task1Entry, (void*)0x11111111, 0, &tTask1Env[1024]); //3.4 设置成最高优先级
	taskInit(&tTask2, task2Entry, (void*)0x00000000, 1, &tTask2Env[1024]); 
	taskInit(&tTaskIdle, idleTaskEntry, (void*)0x00000001, TINYOS_PRIO_COUNT - 1, &idleTaskEnv[1024]); //3.4 设置成最低优先级,31. 这里使用的是配置文件tConfig.h中的宏定义
	
	//为我们的taskTable设置
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	idleTask = &tTaskIdle; //2.4
	
	//3.4 nextTask是最高优先级的任务
	nextTask = tTaskHighestTaskReady(); //删除: nextTask = taskTable[0]; //因为我们是刚开始,还没有执行的任务,也就是currentTask还没有被初始化,所以这里应该是nextTask被初始化
	
	//2.2 开始执行第一个任务: 和一般的任务切换不同的地方(前者:直接把stack里面的东西存到register,想象去柜台的时候没有人,第一个客人直接把东西放桌面 | 后者:当前的人先把register的东西存到stack中,新来的扔把自己stack里面的东西存到register,想象去柜台的时候有人,前面的客人:军铺盖走人,后面的客人:再放东西)
	tTaskRunFirst(); //这个函数,也要触发pendSV异常, 然后因为pendSV会执行asm代码,可以控制pc,所以可以控制下一个要去的函数
	//注意: 因为tTaskSwitch();tTaskRunFirst(); 都要用asm代码来管理硬件,所以我们就把这两个函数的定义,放到switch.c中,因为这个c文件中处理有关硬件的部分.但是这个两个函数的声明,需要放到tinyOS.h中
	
	return 0; //注意,这里不会执行到return 0,因为tTaskRunFirst()里面就会一直走到for loop
}
