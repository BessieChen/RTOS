//7.1 邮箱的定义
#include "tMbox.h"

//7.1
void tMboxInit(tMbox* mbox, void** msgBuffer, uint32_t maxCount)
{
	tEventInit(&mbox->eventECB, tEventTypeMbox); //所以这个ECB是邮箱类型的
	mbox->maxCount = maxCount; //容量
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;  //实际存储
}
