//和cpu相关的函数接口

#include "tinyOS.h"
#include "ARMCM3.h" //读作ARM CM3, 包括了systick的系统函数

//2.4 这里是配置我们的计时器,传入的参数是我们希望的毫秒
void tSetSysTickPeriod(uint32_t ms)
{
	//重置计数器的时候的值:也就是说,每次计数器中断的时候,会将(递减计数器)的值-1,当递减计数器的值==0的时候,会将递减计数器的值设置成这个重置计数器的值
	SysTick->LOAD = ms / 1000 * SystemCoreClock - 1; //其实我不是很清楚为什么-1, 可能是因为觉得设置成重置计数器的时候,也需要花费时间,所以-1相当于用于这个花费时间?
	
	//见下补充
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	
	//递减计数器
	SysTick->VAL = 0;
	
	//其他配置
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
									SysTick_CTRL_TICKINT_Msk | //产生中断使能的标志位
									SysTick_CTRL_ENABLE_Msk;	//使能计数器的标志位
	
	//补充:
	/*
	1. SysTick_IRQn是指systick的基地?
	2. __NVIC_PRIO_BITS是4的宏定义,这里指的是占先优先级为4,因为m3内核中占先优先级和响应由县级一共是4位,这里占先优先级已经有4位,所以响应优先级只有0位
	这里代表的是最低优先级: 1<<4 -1, 也就是16-1=15,也就是1111, 注意优先级是0的时候是最高优先级
	*/
}


//2.4 发生时钟中断时的处理函数, 就像是pendSV一样,是系统会自动调用到这里的,注意函数名不要写错
void SysTick_Handler()
{
	//中断的时候应该做什么? 应该是切换任务: 需要先调度下一个任务,也就是确定好nextTask是什么 -> 接下来就是触发systick异常
	
	//1. 调度下一个任务
	//tTaskSched(); //2.4中就不能直接调用这个函数
	
	//2.4, 需要先递减delayTicks,然后调用tTaskSched()
	tTaskSysTickHandler(); //这个函数里面会调用tTasksched()
	
}



