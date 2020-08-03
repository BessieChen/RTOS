#ifndef TINYOS_H
#define TINYOS_H

//包含标准的头文件,包含了uint32_t
#include <stdint.h>
//3.3 在tinyOS.h中添加tLib.h的原因: 这样main.c如果调用了tinyOS.h, 也就可以使用tLib.h中的成员了
//注意: 在创建tLib.h的时候,要保证是在我们的BessieSource文件夹下, 否则会报错: error:  #5: cannot open source input file "tLib.h": No such file or directory
#include "tLib.h"
//3.4 在在tinyOS.h中添加tConfig.h的原因: 这样main.c如果调用了tinyOS.h, 也就可以使用tConfig.h中的成员TINYOS_PRIO_COUNT了
#include "tConfig.h"
//5.1 在tinyOS.h中添加tEvent.h的原因: 这样tEvent.c如果调用了tinyOS.h, 也就可以使用tEvent.h中的成员了
#include "tEvent.h"
//5.2 将tinyOS.h的关于task的内容都移到了tTask.h中.	//x.x 因为tinyOS.h是给用户看我们的项目都有哪些成员的,所以接下来是正式的内容
#include "tTask.h"
//6.1 加入sem
#include "tSem.h"


//5.2 错误码
typedef enum _tError
{
	tErrorNoError = 0, //没有错误
	tErrorTimeout = 1, //超时了
}tError;




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


//一直占用cpu的延时函数
void delay(int ticks);

//4.1 挂起和挂起恢复
void tTaskSuspend(tTask* task);
void tTaskWakeUp(tTask* task);

//4.3 将任务删除
void tTimeTaskRemove(tTask* task);  //从延时队列删除


#endif
