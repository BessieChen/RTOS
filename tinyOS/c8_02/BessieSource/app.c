//放应用相关的代码
#include "tinyOS.h"
#include "string.h" //8.2 加上, 否则 warning:  #223-D: function "memset" declared implicitly

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

//8.2 测试用, 初始化存储区域
typedef uint8_t (*tBlock)[100]; //这个是定义一个类型 uint8_t[100]. 如果tBlock block; 相当于定义了uint8_t block[100].
uint8_t mem1[20][100]; //存储区域中, 一共20个存储块, 一个存储块一共100字节
tMemBlock memBlock1;

//4.2 模拟对资源的释放
void task1DestroyFunc(void* param)
{
	task1Flag = 0; //只是模拟
}

//4.3 测试tTaskGetInfo() //2.1 设置task1任务需要执行的函数
void task1Entry(void * param) 
{	
	//8.2
	uint8_t i;
	tBlock block[20]; //因为tBlock block; 相当于定义了uint8_t block[100]. 所以这里tBlock block[20];相当于uint8_t block[20][100]; 也是  一共20个存储块, 一个存储块一共100字节
	tSetSysTickPeriod(1);
	
	//8.2 测试
	tMemBlockInit(&memBlock1, (uint8_t*)mem1, 100, 20); // 一共20个存储块, 一个存储块一共100字节
	for(i = 0; i < 20; i++) //8.2 把所有的存储块都取出来
	{
		//8.2 用光所有的存储区, 把这些存储区的地址赋给block[i]. 将tMemBlock中的存储块mem1的第一个部分的地址, 赋值给&block[i], 注意这里传地址取值, 所以这一句话结束后, 我发现memBlock中的count一直减少, 并且block[0],[1]..存的值是mem1[0],[1]的地址
		tMemBlockWait(&memBlock1, (uint8_t**)&block[i], 0); //uint32_t tMemBlockWait(tMemBlock* memBlock, uint8_t** mem, uint32_t waitTicks)//传地址存值: 将block的地址存到mem中, 如果没有block就等待waitTick个systick
	}
	
	//8.2 延时
	tTaskDelay(2);
	
	//8.2 将之前block获得的存储块, 释放回去, 就是还给memBlock1
	for(i = 0; i < 20; i++)
	{
		memset(block[i], i+7, 100); //8.2 第二个参数: 将block[i]里面都赋值为i, 第三个参数: block[i]的长度
		tMemBlockNotify(&memBlock1, (uint8_t*)block[i]); //8.2 释放回去, 就是还给memBlock1, 此时因为task2已经在等待了, 所以切换回task2
		tTaskDelay(2); //8.2 延时
	}
	
	for(;;){	
		
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
		
		//8.2 获取
		tBlock block;
		tMemBlockWait(&memBlock1, (uint8_t**)&block, 0); //走到这里, 是从task1来的, task1此时已经执行完了使用所有的存储区, 所以这里需要等待, 所以task2进入等待队列, 下一步是切换到task1
		
		//8.2 取出第一个字节的值
		//从block中取出第一个字节
		task2Flag = *(uint8_t*)block; //因为memset(block[i], i+7, 100);, 所以第一个字节是0+7=7
		
		/*task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);*/
		
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
