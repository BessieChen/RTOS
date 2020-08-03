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

//6.2 测试用
tSem sem1; //无超时的等待(tSemWait(&sem1, 0);)
tSem sem2; //超时的等待

//4.2 模拟对资源的释放
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //只是模拟
}

//4.3 测试tTaskGetInfo() //2.1 设置task1任务需要执行的函数
void task1Entry(void * param) 
{
	
	tSetSysTickPeriod(1);
	
	//6.2 测试, 初始化sem1
	tSemInit(&sem1, 0, 10);//说明这个资源有10个, 现在只有0个能使用, 说明所有的资源都被使用了
	
	for(;;){

		//6.2 测试: task1看是否有能使用的资源, 有的话就是使用, 没有的话等待0个systick, 所以是无超时的等待: 只会放到等待队列, 不会放到延时队列, 所以systick不管触发了多少次, task1都会一直等着sem1资源
		tSemWait(&sem1, 0);
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
	
}
//4.2 修改 3.7 修改
void task2Entry(void * param) 
{
	//6.2 供下面使用
	uint32_t error;
	
	for(;;){
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
		//6.2 task2向task1等待的资源sem1, 发出通知. 结局应该是task1被唤醒, 然后从task2切换到优先级更高的task1, 并且sem1的资源又被使用了
		tSemNotify(&sem1);
		
		//6.2 看是否还有sem1资源可以用, 没有的话就不等
		error = tSemNoWaitGet(&sem1); //因为刚才已经将sem1个task1了, 所以现在error 英嘎是等于 ResourceUnavailable
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
	//6.2 初始化sem2资源, 这里的意思是, sem2有无限多的资源(maxCount == 0), 但是现在剩余0个资源
	tSemInit(&sem2, 0, 0);
	
	for(;;){
		
		//6.2 task3会超时等待sem2资源, task3看是否有能使用的资源, 有的话就是使用, 没有的话等待10个systick, 所以是超时的等待: 会放到等待队列 and 延时队列, 所以systick触发了10次, task3就不会等了, 就会去就绪队列了
		tSemWait(&sem2, 10);
		
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
	taskInit(&tTask3, task3Entry, (void*)0x22222222, 1, &tTask3Env[TINYOS_IDELTASK_STACK_SIZE]); //5.3 task1优先级最高, task234最低 //3.7 初始化task3, 注意优先级和task2一样
	taskInit(&tTask4, task4Entry, (void*)0x44444444, 1, &tTask4Env[TINYOS_IDELTASK_STACK_SIZE]);
	//4.2 所以你看上面, 优先级最高的是task1,3; 之后是task2,4
	/*所以先运行task1 -> task3 -> task2(删除task1) -> task4(删除task3)*/
}
