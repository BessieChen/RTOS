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

//7.1 测试: 邮箱的初始
tMbox mbox1;
tMbox mbox2; //7.2 测试, 用于超时等待

void* mbox1MsgBuffer[20]; //可以容纳20个元素的void类型, 然后是它的指针
void* mbox2MsgBuffer[20]; //可以容纳20个元素的void类型, 然后是它的指针

//7.2 定义消息缓冲区
uint32_t msg[20];

//4.2 模拟对资源的释放
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //只是模拟
}

//4.3 测试tTaskGetInfo() //2.1 设置task1任务需要执行的函数
void task1Entry(void * param) 
{
	
	tSetSysTickPeriod(1);
	
	//7.1 测试:
	tMboxInit(&mbox1, (void*)mbox1MsgBuffer, 20); //容量是20个


	for(;;){	
		//7.2 通过普通的方式, 发送msg给mbox1
		uint32_t i = 0;
		for(i = 0; i < 20; i++)
		{
			msg[i] = i+3;
			tMboxNotify(&mbox1, &msg[i], tMBOXSendNormal); //普通发送给mbox1
		}
		
		//7.2 加一个长一点的延时, 因为上面的for loop已经将mbox1给装满了, 所以需要延迟久一点让task2去读取mbox1
		tTaskDelay(100);
		
		//7.2 将高优先级方式, 加到缓存区中
		for(i = 0; i < 20 ;i++)
		{
			msg[i] = i+7;
			tMboxNotify(&mbox1, &msg[i], tMBOXSendFront);
		}
		
		//7.2 加一个长一点的延时, 因为上面的for loop已经将mbox1给装满了, 所以需要延迟久一点让task2去读取mbox1
		tTaskDelay(100);
		//tTaskDelay(100);
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
	
}
//4.2 修改 3.7 修改
void task2Entry(void * param) 
{
	for(;;){
		
		//7.2
		void* msg2; 
		
		//7.2 等待mbox1
		uint32_t err = tMboxWait(&mbox1, &msg2, 0);
		
		//7.2
		if(err == tErrorNoError)//说明消息等到了
		{
			//消息的转换
			uint32_t value = *(uint32_t*)msg2;
			task2Flag = value;
			tTaskDelay(1);
		}
		
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
	//7.2 测试邮箱等待的超时
	//tMboxInit(&mbox2, mbox2MsgBuffer, 20); //初始化mbox2
	
	for(;;){
		
		//7.2  超时等待		
		//void* msg;
		//tMboxWait(&mbox2, &msg, 100); //超时时间是100
		
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
