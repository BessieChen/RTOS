//跟时间相关的
#include "tinyOS.h"

//3.6 需要修改 //3.4 这里需要将修改 //2.4 实现延时函数, 因为这个函数肯定是currentTask调用的,所以里面直接写currentTask->xxx
void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	//3.6 删除3.4部分
	/*
	currentTask->delayTicks = delay; //也就是为currentTask设置需要延时多久
	tBitmapClear(&taskPrioBitmap, currentTask->prio); //3.4 因为延时说明currentTask不需要cpu,既然不需要,说明就绪表中对应的元素就设置成0
	//3.4 注意:即便clear()了,但是currentTask的prio还是没有变化, taskTable中也还是有这个任务
	*/
	
	//3.6 将task放入延时队列, 将task从就绪队列中删除
	tTimeTaskWait(currentTask, delay); //注意, 因为我们这里没有task传入, 而是传入currentTask, 因为这个函数肯定是currentTask传入的
	tTaskSchedUnReady(currentTask);
	
	tTaskExitCritical(status);
	tTaskSched();//所以现在currentTask是放弃了cpu,现在由tTaskSched()来判断还有谁是需要cpu的
}
