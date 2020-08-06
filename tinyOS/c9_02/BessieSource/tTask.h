//5.2 把任务相关的接口独立出来, 存放任务相关的结构定义和函数声明
#ifndef TTASK_H
#define TTASK_H

//#include "tinyOS.h" //5.2 byb, 不确定对不对
#include <stdint.h> //5.2 byb, 包含了uint32_t
#include "tLib.h" //5.2 byb,包含了tNode

struct _tEvent;//5.2 这里是前向引用
//解释: 因为tTask.h中用到了tEvent, 然后tEvent.h中用到了tTask. 如果头文件中都包括了对方的.h文件, 感觉就是无限递归了, 不好
//所以这里设置了这个struct _tEvent的结构体, 告诉编译器, 有这么个东西, 之后, 我们line42 是struct _tEvent*, 说明是这个结构体的指针, 指针一般都是4个字节, 所以编译器知道要分配多少字节
//假设line42 是struct _tEvent, 这样不行, 因为编译器不知道_tEvent有多大, 不知道分配多少空间

//3.6 宏定义: 任务是否准备就绪
#define TINYOS_TASK_STATE_READY				0				//就绪
#define TINYOS_TASK_STATE_DESTROYED		(1<<1)	//4.2 已删除的状态位
#define TINYOS_TASK_STATE_DELAYED			(1<<2)	//延时中
#define TINYOS_TASK_STATE_SUSPEND			(1<<3)	//4.1 挂起的状态位

//5.2 掩盖码, 将之前的设置的状态位都清空
#define TINYOS_TASK_WAIT_MASK 				(0xFF<<16) //5.2 也就是1111 1111 0000 0000

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
		uint32_t suspendCount; //4.1 挂起的计数器
	
		void (*clean) (void* param); //4.2 这是一个函数, (*clean)是函数指针(你可以认为就是函数的意思), 返回值是void, 接受一个参数param是void*类型
		void* cleanParam; //4.2 定义一个指针,保存clean函数中的 参数param
		uint8_t requestDeleteFlag; //4.2 删除请求的标记
	
		struct _tEvent* waitEvent; //5.2 等待那个ECB
		void* eventMsg;//5.2 等待的数据的存放的位置, 用于邮箱
		uint32_t waitEventResult; //5.2 等待的结果,例如错误码
	
		uint32_t waitFlagsType;//9.2 请求的事件类型
		uint32_t eventFlags;//9.2 请求的事件标识
		
}tTask;

//4.3 存储task的信息的结构体. 为什么要额外设置一个存info的结构体? 这是因为, 在存的过程, 就像有一个快照, 这样我们获得的info就是快照时候的info, 例如delayTicks, tTask的delayTicks是会变化的, 但是tTaskInfo的是保留了快照时候的值
typedef struct _tTaskInfo
{
	uint32_t delayTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
}tTaskInfo;

//4.1 初始化app
void tAppInit(void);

//4.2 将任务删除
void tTaskSchedRemove(tTask* task); //从就绪队列删除
void tTaskSetCleanCallFunc(tTask* task, void (*clean) (void* param), void* param);\
void tTaskForceDelete(tTask* task);
void tTaskRequestDelete(tTask* task);
uint8_t tTaskIsRequestedDeleted(void);
void tTaskDeleteSelf(void);

//4.3 存储task的info
void tTaskGetInfo(tTask* task, tTaskInfo* info);

#endif
