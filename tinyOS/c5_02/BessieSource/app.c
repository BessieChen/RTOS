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

tEvent eventWaitTimeout; //5.2 测试用, 超时等待
tEvent eventWaitNormal; //5.2 测试用, 普通等待

//4.2 模拟对资源的释放
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //只是模拟
}

//4.3 测试tTaskGetInfo() //2.1 设置task1任务需要执行的函数
void task1Entry(void * param) 
{
	/*5.2 删
	//4.3 为了后面获得info
	tTaskInfo info;
	*/
	
	tSetSysTickPeriod(1);
	
	//5.2 测试用, 初始化
	tEventInit(&eventWaitTimeout, tEventTypeUnknown); 
	
	for(;;){

		/*
		5.2 删除
		//4.3 查询自己的info
		tTaskGetInfo(currentTask, &info); //通过观察, 发现prio == 0, 对的
		
		//4.3 查询其他task的info
		tTaskGetInfo(&tTask4, &info); //通过观察, 发现prio == 1, 对的
		*/
		
		
		//5.2 测试用: 让task1超时等待. 在这里,先添加超时等待的调用, 是当前任务等待, timeout == 5, 然后state是0. 所以结果是:从就绪队列删除, 加入延时队列和等待队列
		tEventWait(&eventWaitTimeout, currentTask, (void*)0, 0, 5); //void tEventWait(tEvent* eventECB, tTask* task, void* msg, uint32_t state, uint32_t timeout) //msg:传来的消息, state:等待状态, timeout:用户设置的超时时间

		//5.2 任务调度函数
		tTaskSched(); //切换到其他任务运行
		
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
		
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
	
	for(;;){
		
		//5.2 将task2插入等待队列中. 注意, 结束后, task2从就绪队列中删除, 加入等待队列, 但是没有加入延时队列因为timeout==0 
		tEventWait(&eventWaitNormal, currentTask, (void*)0, 0, 0);
		//5.2 调度到其他任务, 因为就绪队列已经没有task2
		tTaskSched();
		
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
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
	
	//5.2 测试用, 让task23等待(常规等待),让task4唤醒task23
		//这里先初始化(正常等待)队列, 在task3的函数task3Entry()中初始化, 因为task3的优先级比task24高, 程序先会运行这里
		tEventInit(&eventWaitNormal, tEventTypeUnknown);
	
	for(;;){
		
		//5.2 将task3插入等待队列中. 注意, 结束后, task3从就绪队列中删除, 加入等待队列, 但是没有加入延时队列因为timeout==0 
		tEventWait(&eventWaitNormal, currentTask, (void*)0, 0, 0);
		//5.2 调度到其他任务, 因为就绪队列已经没有task3
		tTaskSched();
		
		task3Flag = 0;
		tTaskDelay(1);
		task3Flag = 1;
		tTaskDelay(1);
		
		
	}
}

//4.2 task4的任务
void task4Entry(void * param) 
{
	
	
	for(;;){
		//5.2 task4将普通等待队列中的第一个任务唤醒(这里就相当于模仿了事件发生), 结果是: 第一个task是task3,它从等待队列移除, 加入就绪队列
		tTask* readyTask = tEventWakeUp(&eventWaitNormal, (void*)0, 0);
		//5.2 调度
		tTaskSched();
		
		task4Flag = 0;
		tTaskDelay(1);
		task4Flag = 1;
		tTaskDelay(1);
	
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
