//放应用相关的代码
#include "tinyOS.h"

//2.2	每个task里面都有需要反转的flag
int task1Flag;
int task2Flag;
int task3Flag;
//4.2 现在添加任务4, 所以为了检测我们的函数: task2 强制删除 task1, task4 请求删除 task3, task3自己删除自己
int task4Flag; //4.2

//2.1 设置完了tinyOS.h后,我们就设置两个stack,并且设置两个task, 再设置两个task执行的函数
tTaskStack tTask1Env[TINYOS_IDELTASK_STACK_SIZE];		//因为一个tTaskStack只是一个uint32_t,只能代表stack里面的一个元素.所以我们设置成数组的形式
tTaskStack tTask2Env[TINYOS_IDELTASK_STACK_SIZE];
tTaskStack tTask3Env[TINYOS_IDELTASK_STACK_SIZE];		//3.7 
tTaskStack tTask4Env[TINYOS_IDELTASK_STACK_SIZE];		//4.2 

tTask tTask1; //设置一个任务,还记得吗,tTask是一个结构体,里面包含一个tTaskStack*,之后我们对这个tTaskStack*初始化,就可以将tTask1和tTask1Env联系起来了
tTask tTask2;
tTask tTask3; //3.7 为了测试, 我们需要将task2和task3的优先级设置为同一优先级
tTask tTask4; //4.2 测试用

//4.2 模拟对资源的释放
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //只是模拟
}

//2.1 设置task1任务需要执行的函数
void task1Entry(void * param) 
{
	tSetSysTickPeriod(1);

	//4.2 调用清理函数: 第一个参数:要清理的任务(currentTask), 第二个: 如何清理,清理的函数, 第三个: 该函数的参数
	tTaskSetCleanCallFunc(currentTask, task1DestroyFunc, (void*)0); //第三个参数写成(void*) 0, 其实在这里没有用到
	//之所以加载这里, 是因为
	/*
	这个只是设置函数:
	void tTaskSetCleanCallFunc(tTask* task, void (*clean) (void* param), void* param)
	{
		task->clean = clean;
		task->cleanParam = param;
	}	
	*/
	
	for(;;){

		task1Flag = 0;
		tTaskDelay(1); //4.2
		/*
		4.1 删除
		//delay(10);
		//tTaskDelay(1);
		*/
		//4.2 删除tTaskSuspend(currentTask); //4.1 task1将自己挂起
		task1Flag = 1;
		tTaskDelay(1); //4.2
		
		/*
		4.1 删除
		//delay(10);
		//tTaskDelay(1);
		*/
		//4.2 删除 tTaskSuspend(currentTask); //4.1 task1第二次将自己挂起
	}
	
}
//4.2 修改 3.7 修改
void task2Entry(void * param) 
{
	//4.2 下面会用到的标志位
	int task1Deleted = 0;
	for(;;){
		task2Flag = 0;
		//3.7 删除 tTaskDelay(1); 因为tTaskDelay()会主动让出资源,但是我们这里需要task2一直占用资源,然后就来检测我们的时间片是否有用,如果有用,task2即便想一直占有资源也不做不到
		//3.7 为了让task2一直占用资源,使用delay()
		//4.1 删除delay(0xFF);
		tTaskDelay(1); //4.1
		//4.2 删除tTaskWakeUp(&tTask1); //4.1 让task2唤醒task1
		task2Flag = 1;
		//3.7 删除: tTaskDelay(1);
		//3.7 为了让task2一直占用资源,使用delay()
		//4.1 删除delay(0xFF);
		tTaskDelay(1); //4.1
		//4.2 删除tTaskWakeUp(&tTask1); //4.1 让task2第二次唤醒task1
		
		//4.2 判断task1是否被删除, 如果没有就去删除. 之所以放在for loop就是为了我们可以循环执行下面的代码, 只不过我怎么觉得, 只会进入一次if啊, 放在for loop里面为何?
		if(!task1Deleted)
		{
			tTaskForceDelete(&tTask1); //这里是传入task1的地址
			task1Deleted = 1;
		}
	}
}

//4.2 task3删除自己
void task3DestroyFunc(void* param)
{
	task3Flag = 0;
	tTaskDeleteSelf();
}

//4.2 修改 //3.7 task3的任务
void task3Entry(void * param) 
{
	for(;;){
		
		//4.2 先检查有没有人要强制删除自己
		if(tTaskIsRequestedDeleted())
		{
			task3DestroyFunc((void*) 0);
		}

		task3Flag = 0;
		//3.7 为了让task2一直占用资源,使用delay()
		//4.1 删除 delay(0xFF);
		tTaskDelay(1);
		task3Flag = 1;
		//3.7 为了让task2一直占用资源,使用delay()
		//4.1 删除 delay(0xFF);
		tTaskDelay(1);
		
		
	}
}

//4.2 task4的任务
void task4Entry(void * param) 
{
	//4.2 下面会用到的标志位
	int task3Deleted = 0;
	
	for(;;){

		task4Flag = 0;
		//3.7 为了让task2一直占用资源,使用delay()
		//4.1 删除 delay(0xFF);
		tTaskDelay(1);
		task4Flag = 1;
		//3.7 为了让task2一直占用资源,使用delay()
		//4.1 删除 delay(0xFF);
		tTaskDelay(1);
		
		//4.2 判断task3是否被删除, 如果没有就去删除. 之所以放在for loop就是为了我们可以循环执行下面的代码, 只不过我怎么觉得, 只会进入一次if啊, 放在for loop里面为何?
		if(!task3Deleted)
		{
			tTaskRequestDelete(&tTask3); //这里是传入task3的地址
			task3Deleted = 1;
		}
	}
}

//4.1 app的初始化函数
void tAppInit(void)
{
	taskInit(&tTask1, task1Entry, (void*)0x11111111, 0, &tTask1Env[TINYOS_IDELTASK_STACK_SIZE]); //3.4 设置成最高优先级
	taskInit(&tTask2, task2Entry, (void*)0x00000000, 1, &tTask2Env[TINYOS_IDELTASK_STACK_SIZE]); 
	taskInit(&tTask3, task3Entry, (void*)0x22222222, 0, &tTask3Env[TINYOS_IDELTASK_STACK_SIZE]);  //3.7 初始化task3, 注意优先级和task2一样
	taskInit(&tTask4, task4Entry, (void*)0x44444444, 1, &tTask4Env[TINYOS_IDELTASK_STACK_SIZE]);
	//4.2 所以你看上面, 优先级最高的是task1,3; 之后是task2,4
	/*所以先运行task1 -> task3 -> task2(删除task1) -> task4(删除task3)*/
}
