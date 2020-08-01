#ifndef TINYOS_H
#define TINYOS_H

//包含标准的头文件,包含了uint32_t
#include <stdint.h>
//3.3 在tinyOS.h中添加tLib.h的原因: 这样main.c如果调用了tinyOS.h, 也就可以使用tLib.h中的成员了
//注意: 在创建tLib.h的时候,要保证是在我们的BessieSource文件夹下, 否则会报错: error:  #5: cannot open source input file "tLib.h": No such file or directory
#include "tLib.h"
//3.4 在在tinyOS.h中添加tConfig.h的原因: 这样main.c如果调用了tinyOS.h, 也就可以使用tConfig.h中的成员TINYOS_PRIO_COUNT了
#include "tConfig.h"

//3.6 宏定义: 任务是否准备就绪
#define TINYOS_TASK_STATE_READY				0				//就绪
#define TINYOS_TASK_STATE_DELAYED			(1<<1)	//延时中

//因为tinyOS.h是给用户看我们的项目都有哪些成员的,所以接下来是正式的内容

//首先,是定义一个类型,大小是uint32_t, 重命名为tTaskStack,意思是一个任务的stack
typedef uint32_t tTaskStack;

//其次,定义一个结构体,也就是保存我们一个任务里面的所有信息,现在只包括了stack一个信息
//具体来说,是一个tTaskStack的指针,所以我们可以通过这个指针,去找到整个stack
typedef struct _tTask
{
		tTaskStack * stack;
		uint32_t delayTicks;//2.4添加软延迟的计数器
		uint32_t prio; //3.4 设置优先级的字段
		tNode delayNode; //3.6 延时节点, 注意, 这里就是tNode, 而不是tNode*
		uint32_t state; //3.6 任务的状态: 是否延时, ...
		tNode linkNode; //3.7 就绪节点,所以同一个优先级, 可以有多个任务, 也就是tList(某个优先级)中存储多个linkNode(可以找到对应的task)
		uint32_t slice; //3.7 时间片
}tTask;

//其他声明, todo: 不用extern可不可以,因为这里只是声明,没有定义
extern tTask* currentTask;
extern tTask* nextTask;
extern tTask* idleTask; //需要在这里写上这一句,否则main.c会报错

//3.4 初始化
void taskInit(tTask* task, void (*func)(void*), void* param, uint32_t prio, tTaskStack* stack);
	
//2.4 systick
void tSetSysTickPeriod(uint32_t ms); //配置
void tTaskSysTickHandler(void);

//3.4 延时函数
void tTaskDelay(uint32_t delay);

//3.1 要记得将设置和退出临界区的声明加上
uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t param);

//以下这两个函数都是和cpu相关的,还记得吗,都触发了pendSV异常,然后pendSV使用asm写的.这两个函数的定义在switch.c中
void tTaskRunFirst(void); //todo,如果这里的参数不写void行不行?因为switch.c中的参数就是()
void tTaskSwitch(void);
void tTaskSched(void); //3.2 老师在3.2时候加的,应该早就加了, 注意要加上参数名void, 否则出现warning:  #1295-D: Deprecated declaration tTaskSched - give arg types

//3.2 调度锁
void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);

//3.4 就绪表
tTask* tTaskHighestTaskReady(void);

//3.7 延时队列
void tTimeTaskWait(tTask* task, uint32_t ticks);
void tTimeTaskWakeUp(tTask* task);

//3.7 就绪队列
void tTaskSchedReady(tTask* task);
void tTaskSchedUnReady(tTask* task);

//4.1 初始化app
void tAppInit(void);

//一直占用cpu的延时函数
void delay(int ticks);

#endif
