//这个文件, 主要是跟任务相关的
#include "tinyOS.h"

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
	
	//3.6 延时节点 和 state的初始化
	task->state = TINYOS_TASK_STATE_READY; // 刚开始就是就绪状态, 没有延时
	tNodeInit(&(task->delayNode)); //注意, delayNode是一个tNode, 但是tNodeInit(tNode*), 所以我们要取地址
	
	//3.7 初始化时间片
	task->slice = TINYOS_SLICE_MAX; //也就是说, 现在这个task可以执行这么长的时间了
	
	//3.7 初始化linkNode
	tNodeInit(&(task->linkNode)); //因为task是tTask*, 然后linkNode是tNode, 但是需要的参数是tNode*, 所以记得加上&
	
	//4.1 删除了下面两个:
	/*
	//3.7 将linkNode加入taskTable的对应元素中 相当于之前的 taskTable[prio] = task //3.4 初始化一个task之后, 要把task放到taskTable的相应位置
	//以前的删除: taskTable[prio] = task; //注意,这里的task本身就是一个地址,所以不要写成 = &task;
	tListAddFirst(&(taskTable[task->prio]), &(task->linkNode)); //因为第一个参数是tLink*, 第二个参数是tNode*, 所以都是传入地址
	
	//3.4 要将就绪表中的相应位置设置成1
	tBitmapSet(&taskPrioBitmap, prio);
	*/
	
	
	//4.1 删除的那两个函数, 可以用tTaskSchedReady()来实现, 好处: 我们不需要再tTask.c中声明taskTable, 但是调用体TaskSchedReady()我们就可以使用taskTable
	//3.7 修改 //3.6 加入就绪队列: 当task从延时队列删除, 我们就把task加入就绪队列: 也就是加入taskTable, 并且设置tBitmap
	tTaskSchedReady(task);
	//以下是void tTaskSchedReady(tTask* task);的定义
	/*
	{
		//3.7
		tListAddFirst(&(taskTable[task->prio]), &(task->linkNode)); //第一个参数是tList*, 第二个是tNode*
		//3.7 如果发现这个task是tasktable[]中的第一个task, 就要将bitmap的对应位置设置成1
		if(tListCount(&taskTable[task->prio]) == 1) //我的bug, 是==1, 说明刚加进去的是第一个!
		{
			tBitmapSet(&taskPrioBitmap, task->prio);
		}
	}
	*/
	
	//4.1 初始挂起计数器
	task->suspendCount = 0; //如果没有写着一句, 也是不会出错的, 因为我们的task是全局变量, 全局变量结构里面的成员的值, 默认是0
	
	//4.2 初始化 clean 和 param
	task->clean = (void (*) (void*) ) 0; //参数为空
	task->cleanParam = (void *) 0;
	task->requestDeleteFlag = 0;
	

}


//4.1 设置挂起函数
void tTaskSuspend(tTask* task) //需要挂起的任务
{
	//设置临界区
	uint32_t status = tTaskEnterCritical();
	
	//如果任务在延时状态, 不能挂起. 回忆: 只有在就绪和运行的时候, 才能挂起
	if(!(task->state & TINYOS_TASK_STATE_DELAYED))
	{
		//如果是第一次被挂起, 就将状态位设置成挂起. (如果是第二次第三次被挂起, 状态位已经被设置了, 就不需要重复设置)
		if(++task->suspendCount <= 1) //这里是一个保守的写法, 也就是我们本来是想说明是 == 1
		{
			//设置
			task->state |= TINYOS_TASK_STATE_SUSPEND;
			
			//将task从就绪列表中移除, 注意: 移除之后, task既不会就绪, 也不会运行(运行首先需要在就绪里面排队), task现在就在一个挂起状态中, 老师说了, 你可以设置一个挂起队列tList存放所有的挂起的task, 但是老师这里没有实现
			tTaskSchedUnReady(task);
			
			//如果这个任务是当前任务, 就切换到其他任务
			if(task == currentTask)
			{
				tTaskSched();
			}
		}
		
	}
	
	tTaskExitCritical(status);
}

//4.1 从挂起到恢复的函数, 可以恢复到运行或者就绪
void tTaskWakeUp(tTask* task)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//先判断task是否是挂起状态
	if(task->state & TINYOS_TASK_STATE_SUSPEND)
	{
		//挂起-1, 如果-1之后, 挂起计数器 == 0, 恢复之前的状态: 就绪
		if(--task->suspendCount == 0)
		{
			task->state &= ~(TINYOS_TASK_STATE_SUSPEND);
			
			//放入就绪队列
			tTaskSchedReady(task); //对比: tTaskSchedUnReady(task);是将task从就绪列表中移除
			
			//调度: 也就是说如果这个task是最高优先级的头部task, 那这个task就会变成运行, 如果不是的话, 这个task依旧是就绪状态
			tTaskSched();
		}
	}
	tTaskExitCritical(status);
}	

//4.2 清理的回调函数
void tTaskSetCleanCallFunc(tTask* task, void (*clean) (void* param), void* param)
{
	task->clean = clean;
	task->cleanParam = param;
}	

//4.2 强制删除函数
void tTaskForceDelete(tTask* task)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//任务是否在延时队列中, 然后判断是不是在挂起状态, 如果都不是, task就是就绪. 总之task就要从它可能存在的任何一个(就绪/延时)队列中删除. 如果是挂起状态, 就直接什么都不做
	if(task->state &= TINYOS_TASK_STATE_DELAYED) //说明延时队列中
	{
		tTimeTaskRemove(task);
	}
	else if( !(task->state &= TINYOS_TASK_STATE_SUSPEND) ) //说明不是挂起, 那就只能是在就绪队列中
	{
		tTaskSchedRemove(task); //注意, 这里不是tTaskSchedUnReady(), 虽然他俩的实现都一样
	}
	
	//调用task的clean函数, 为了严谨, 先判断存不存在这个clean函数
	if(task->clean) //存在clean函数
	{
		task->clean(task->cleanParam); //调用该函数
	}
	
	//如果task就是当前运行的任务, 就切换到其他任务: 我觉得这里如果进入了if, 就是属于, 自己删除自己了
	if(task == currentTask)
	{
		tTaskSched();
	}
	
	tTaskExitCritical(status);
}

//4.2 请求删除的接口, 设置task的请求删除标记
void tTaskRequestDelete(tTask* task)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//设置标记
	task->requestDeleteFlag = 1; //说明要被删除
	
	tTaskExitCritical(status);
}

//4.2 检查task是否有删除请求, 并且返回结果给调用者(调用者应该就是task自己, 因为函数里面写的是currentTask->xxx)
uint8_t tTaskIsRequestedDeleted(void)
{
	uint8_t result; 
	
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	result = currentTask->requestDeleteFlag;
	
	tTaskExitCritical(status);

	return result;
}

//4.2 删除task自己的函数(调用者应该就是task自己, 因为函数里面写的是currentTask->xxx)
void tTaskDeleteSelf(void)
{
	//临界区
	uint32_t status = tTaskEnterCritical();
	
	//因为是删除自己, 说明是currentTask就是自己(因为只有currentTask才能够运行, 才会走到这个函数里面)
	tTaskSchedRemove(currentTask); //因为currentTask是自己, 所以说明自己就在就绪队列中
	
	//curerntTask不可能处于延时状态, 不可能在延时对六种
	
	//调用清理函数
	if(currentTask->clean)
	{
		currentTask->clean(currentTask->cleanParam);
	}
	
	//所以一定要记得, 切换到其他任务
	tTaskSched();
	
	tTaskExitCritical(status);
}	




