#include "tinyOS.h"
#include "ARMCM3.h" //包含了SysTick

//3.4 定义: 任务就绪表, taskTable
tBitmap taskPrioBitmap;



//3.6 taskTable就是tList数组了, 也就是每个元素都是tList //3.4 taskTable现在里面有TINYOS_PRIO_COUNT个元素: tTask* taskTable[TINYOS_PRIO_COUNT]; 
tList taskTable[TINYOS_PRIO_COUNT]; 


//2.1 设置完了tinyOS.h后,我们就设置两个stack,并且设置两个task, 再设置两个task执行的函数
tTaskStack idleTaskEnv[TINYOS_IDELTASK_STACK_SIZE]; //2.4

tTask tTaskIdle; //2.4

//2.2 初始化了任务之后,自然还需要调度任务,所以需要设置current和next task
tTask* currentTask;
tTask* nextTask;
tTask* idleTask; //2.4

//3.6 定义延时队列
tList tTaskDelayedList; //注意, 延时节点delayNode是每个tTask里面有的成员, 但是延时队列却是main.c里面的全局变量. //注意, 这个是一个tList, 而不是tList*, 因为我们这里需要的是一个结构体, 而不是一个地址




//3.2 定义调度锁的计数器, 是uint8, 只有当调度锁==0的时候,才可以触发调度函数tTaskSched()
uint8_t schedLockCount;

//3.7 初始化tasktable[] 3.4 虽然是初始化调度锁,但是这里也包括了bitmap的初始化,因为main()第一个调用的就是这个函数.  调度锁区别于临界区:临界区是使用__get_primask()来关闭中断,也就是使用了ARMCM3.h文件的函数,但是调度锁使我们自己定义的
void tTaskSchedInit(void)
{
	int i;
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);//3.4
	
	//3.7 初始化tasktable[]
	for(i = 0; i < TINYOS_PRIO_COUNT; i++)
	{
		tListInit(&taskTable[i]);
	}
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



//3.6 延时队列tTaskDelayedList的初始化, 注意这里的参数不需要是tList*, 因为tTaskDelayedList就是全局变量
void tTaskDelayedInit(void)
{
	tListInit(&tTaskDelayedList);
}

//3.6 将任务加入延时队列, 并声明任务需要延时多久
void tTimeTaskWait(tTask* task, uint32_t ticks)
{
	//task的修改:
	task->delayTicks = ticks;
	task->state |= TINYOS_TASK_STATE_DELAYED;
	
	//task加入延时队列
	tListAddLast(&tTaskDelayedList, &(task->delayNode)); //注意是加入task的delayNode地址,也就是tNode指针
}

//3.6 当delayTicks == 0之后, 将任务从延时队列中删除
void tTimeTaskWakeUp(tTask* task)
{
	//task的修改
	task->state &= ~(TINYOS_TASK_STATE_DELAYED); //注意这里只是改变第二位, 将第二位设置成0
	
	//task从延时队列删除
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}

//3.7 修改 //3.6 加入就绪队列: 当task从延时队列删除, 我们就把task加入就绪队列: 也就是加入taskTable, 并且设置tBitmap
void tTaskSchedReady(tTask* task)
{
	//3.7
	tListAddFirst(&(taskTable[task->prio]), &(task->linkNode)); //第一个参数是tList*, 第二个是tNode*
	//3.7 如果发现这个task是tasktable[]中的第一个task, 就要将bitmap的对应位置设置成1
	if(tListCount(&taskTable[task->prio]) == 1) //我的bug, 是==1, 说明刚加进去的是第一个!
	{
		tBitmapSet(&taskPrioBitmap, task->prio);
	}
}

//3.7 修改 //3.6 当task需要延时, 或者其他情况要将task从就绪队列中删除
void tTaskSchedUnReady(tTask* task)
{
	//3.7
	tListRemove(&(taskTable[task->prio]), &(task->linkNode)); //第一个参数是tList*, 第二个是tNode*
	//注意, 这里不要将linkNode加入到taskTable[prio]的末尾, 因为这个函数是把task从就绪队列删除, 而不是什么时间片的轮转
	//3.7 如果发现tasktable空了, 就要将bitmap的对应位置设置成0
	if(tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}
//3.7 加入时间片轮转的处理, 因为每个systick, 所有的同一个优先级的所有任务的剩余时间片都要减一. 3.6 需要修改, 因为我们需要扫描的是延时队列了, 也就是tList tTaskDelayedList. 3.4 需要修改,每次systick都需要把taskTable中的每一个任务的delayTicks减一 // 2.4 因为现在每个任务都有一个计数器delayTicks, 现在设计一个函数,能够递减delayTicks(相当于软延迟)
//所以systick发生,会有两件事情: 1. 延时队列的task的延时时间都减一, 2. 就绪队列的task的时间片都减一
void tTaskSysTickHandler()
{
	//3.6 这是之后for loop需要用到的:
	tNode* node;
	uint32_t status = tTaskEnterCritical();
	//3.6 删除3.4部分:
	/*
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
	*/
	
	//3.6 因为tTaskDelayList里面存的都是正在延时的task的成员tNode delayNode, 所以我们就可以很容易找到正在延时的task, 就不用像3.4里面的, 全部task都遍历一遍
	// 找到正在延时的task之后,就可以对他们的delayTicks - 1, 如果发现tick == 0, 就可以把那个task 从延时队列删除, 并且加入就绪对列
	// 这里需要注意的, 的确容易出错: 那就是tTaskDelayedList是一个tList, 而不是tList*, 所以我们需要用. : tTaskDelayedList.headNode, 而不是用-> : 所以tTaskDelayedList->headNode是错的
	for(node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		//求出那个task的地址
		tTask* task = (tTask*)tNodeParentAddr(tTask, delayNode, node); //第一个参数: 结构体, 第二个: 结构体的成员, 第三个: 该成员的实际地址. 求: 结构体的实际首地址

		//将task的delayTIck递减
		//如果tick == 0, 从延迟队列移除(tTaskDelayedList, task->state), 加入就绪队列(bitmap, taskTable, task->state)
		if(--task->delayTicks == 0)
		{
			//5.2 如果task还在等待事件, 需要把它从ECB中删除, 错误码是:超时了tErrorTimeout (所以: 逻辑是, task可能会同时放到 等待队列 和 延时队列中, 但是只要延时结束, 等待就结束并且等待有错误码)
			//逻辑是: 假设一个task加入了等待队列和延时队列, 只要其中一个被破坏(被唤醒或者delay==0), 就从两个对垒中删除
			if(task->waitEvent)
			{
				tEventRemoveTask(task, (void*)0, tErrorTimeout);
			}
			tTimeTaskWakeUp(task); //在这里, nextTask会等于这个task
			tTaskSchedReady(task);
		}
	}
	
	//3.7 就绪队列的task的时间片都减一
	if(--currentTask->slice == 0)
	{
		//将linkNode从tList的第一位删除
		tListRemoveFirst(&(taskTable[currentTask->prio]));
		//将linkNode放到末尾
		tListAddLast(&(taskTable[currentTask->prio]), &(currentTask->linkNode));
		//重置currentTask的时间片
		currentTask->slice = TINYOS_SLICE_MAX;
		
	}
	tTaskExitCritical(status);
	//因为上面递减了delayTicks,所以有可能有一个以上的任务延迟完毕,可以被开启了. 以下的tTaskSched()里面就可以判断是否delayTicks==0,是否可以被开启
	tTaskSched();
}

//3.3 我自己设计的, 检测报错的函数
void ifError(void)
{
	
}

//2.4 添加一个空闲任务, 里面什么都不做, 等着其他任务的结束延迟
void idleTaskEntry(void* param)
{
	for(;;)
	{}
}





//3.7 需要修改了 //3.4 返回当前最高优先级的任务
tTask* tTaskHighestTaskReady(void) //注意,这里不需要传入&taskPrioBitmap作为参数,因为默认是全局变量. 当然你想改成要输入参数的也可以
{
	//3.7 这一步不变, 还是通过bitmap知道最高优先级是第几个优先级
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	
	//3.7 然后从taskTable中找到对应的优先级, 也就是一个tList, 我们需要从这个tList中找出第一个tNode,(注意不是tNode*) 这个tNode虽然存着两个tNode*, 但是tNode本身也有自己的地址(其实tList的headNode.nextNode存的就是这个tNode自己的地址), 我们通过这个地址, 找到这个tNode所属的tTask, 将这个tTask的首地址返回
	tNode* linkNodeAddr = tListFirst(&(taskTable[highestPrio])); //注意返回的就是tNode*
	
	//3.7 我们知道linkNode的实际地址, 想求出tTask的实际首地址
	tTask* taskAddr = tNodeParentAddr(tTask, linkNode, linkNodeAddr); 
	return taskAddr;
}

int main()
{
	
	//3.4 因为shareCount是我们自己的定义的调度锁,所以需要初始化, 这个函数里面包含了 初始化就绪表, 也就是bitmap的初始化
	tTaskSchedInit();
	
	//3.6 初始化延时队列
	tTaskDelayedInit();
	
	tAppInit();

	
	//将tTask绑定上相应的stack和func
	
	taskInit(&tTaskIdle, idleTaskEntry, (void*)0x00000001, TINYOS_PRIO_COUNT - 1, &idleTaskEnv[TINYOS_IDELTASK_STACK_SIZE]); //3.4 设置成最低优先级,31. 这里使用的是配置文件tConfig.h中的宏定义
	
	//为我们的taskTable设置
	//3.7 删除 taskTable[0] = &tTask1;
	//3.7 删除 taskTable[1] = &tTask2;
	idleTask = &tTaskIdle; //2.4
	
	//3.4 nextTask是最高优先级的任务
	nextTask = tTaskHighestTaskReady(); //删除: nextTask = taskTable[0]; //因为我们是刚开始,还没有执行的任务,也就是currentTask还没有被初始化,所以这里应该是nextTask被初始化
	
	//2.2 开始执行第一个任务: 和一般的任务切换不同的地方(前者:直接把stack里面的东西存到register,想象去柜台的时候没有人,第一个客人直接把东西放桌面 | 后者:当前的人先把register的东西存到stack中,新来的扔把自己stack里面的东西存到register,想象去柜台的时候有人,前面的客人:军铺盖走人,后面的客人:再放东西)
	tTaskRunFirst(); //这个函数,也要触发pendSV异常, 然后因为pendSV会执行asm代码,可以控制pc,所以可以控制下一个要去的函数
	//注意: 因为tTaskSwitch();tTaskRunFirst(); 都要用asm代码来管理硬件,所以我们就把这两个函数的定义,放到switch.c中,因为这个c文件中处理有关硬件的部分.但是这个两个函数的声明,需要放到tinyOS.h中
	
	return 0; //注意,这里不会执行到return 0,因为tTaskRunFirst()里面就会一直走到for loop
}


//4.2 将任务从优先级队列(就绪队列)移除, 这个函数虽然现在内容和tTaskSchedUnReady()一样, 但是以后会不一样的
void tTaskSchedRemove(tTask* task)
{
	
	//移除
	tListRemove(&taskTable[task->prio], &(task->linkNode));
	
	//移除完之后,如果==0,就把bitmap也清空
	if(tListCount(&taskTable[task->prio]) == 0)
	{
		tBitmapClear(&taskPrioBitmap, task->prio);
	}
}	

//4.2 将任务从延时队列删除
void tTimeTaskRemove(tTask* task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}







