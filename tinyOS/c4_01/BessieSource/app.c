//放应用相关的代码
#include "tinyOS.h"

//2.2	每个task里面都有需要反转的flag
int task1Flag;
int task2Flag;
int task3Flag;

//2.1 设置完了tinyOS.h后,我们就设置两个stack,并且设置两个task, 再设置两个task执行的函数
tTaskStack tTask1Env[1024];		//因为一个tTaskStack只是一个uint32_t,只能代表stack里面的一个元素.所以我们设置成数组的形式
tTaskStack tTask2Env[1024];
tTaskStack tTask3Env[1024];		//3.7 

tTask tTask1; //设置一个任务,还记得吗,tTask是一个结构体,里面包含一个tTaskStack*,之后我们对这个tTaskStack*初始化,就可以将tTask1和tTask1Env联系起来了
tTask tTask2;
tTask tTask3; //3.7 为了测试, 我们需要将task2和task3的优先级设置为同一优先级

//2.1 设置task1任务需要执行的函数
void task1Entry(void * param) 
{
	tSetSysTickPeriod(1);

	for(;;){

		task1Flag = 0;
		/*
		4.1 删除
		//delay(10);
		//tTaskDelay(1);
		*/
		tTaskSuspend(currentTask); //4.1 task1将自己挂起
		task1Flag = 1;
		/*
		4.1 删除
		//delay(10);
		//tTaskDelay(1);
		*/
		tTaskSuspend(currentTask); //4.1 task1第二次将自己挂起
	}
	
}
//3.7 修改
void task2Entry(void * param) 
{
	for(;;){
		task2Flag = 0;
		//3.7 删除 tTaskDelay(1); 因为tTaskDelay()会主动让出资源,但是我们这里需要task2一直占用资源,然后就来检测我们的时间片是否有用,如果有用,task2即便想一直占有资源也不做不到
		//3.7 为了让task2一直占用资源,使用delay()
		//4.1 删除delay(0xFF);
		tTaskDelay(1); //4.1
		tTaskWakeUp(&tTask1); //4.1 让task2唤醒task1
		task2Flag = 1;
		//3.7 删除: tTaskDelay(1);
		//3.7 为了让task2一直占用资源,使用delay()
		//4.1 删除delay(0xFF);
		tTaskDelay(1); //4.1
		tTaskWakeUp(&tTask1); //4.1 让task2第二次唤醒task1

	}
}

//3.7 task3的任务
void task3Entry(void * param) 
{
	for(;;){

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

//4.1 app的初始化函数
void tAppInit(void)
{
	taskInit(&tTask1, task1Entry, (void*)0x11111111, 0, &tTask1Env[TINYOS_IDELTASK_STACK_SIZE]); //3.4 设置成最高优先级
	taskInit(&tTask2, task2Entry, (void*)0x00000000, 1, &tTask2Env[TINYOS_IDELTASK_STACK_SIZE]); 
	taskInit(&tTask3, task3Entry, (void*)0x00000000, 1, &tTask3Env[TINYOS_IDELTASK_STACK_SIZE]);  //3.7 初始化task3, 注意优先级和task2一样
}
