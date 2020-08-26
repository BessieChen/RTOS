//7.1 添加邮箱的结构定义
#ifndef TMBOX_H
#define TMBOX_H

//7.1 添加ECB的头文件
#include "tEvent.h"

//7.2 宏定义
#define tMBOXSendNormal 			0x00 //这个是按普通方法来缓存msg, 也就是在write+1的地方加入msg
#define tMBOXSendFront				0x01 //这个说明加入的msg的优先级比较高, 所以在read-1的地方加入msg, 这样task第一个读的就是这个msg
typedef struct _tMbox
{
	//7.1 ECB存需要读邮件的task
	tEvent eventECB;
	uint32_t count; //当前邮件的实际数量
	uint32_t read; //读索引
	uint32_t write; //写索引
	uint32_t maxCount; //邮箱的容量
	void** msgBuffer; //所以里面是一个地址, 指向的msgBuffer的地址
}tMbox;

//7.1 
void tMboxInit(tMbox* mbox, void** msgBuffer, uint32_t maxCount);

//7.2 
uint32_t tMboxWait(tMbox* mbox, void** msg, uint32_t waitTicks); //void** msg: 用于存放消息的指针, uint32_t waitTicks: 等待的时间
uint32_t tMboxNoWaitGet(tMbox* mbox, void** msg);
uint32_t tMboxNotify(tMbox* mbox, void* msg, uint32_t notifyOption); // uint32_t notifyOption 指明了如果插入的msg是高优先级的, 会被优先读取


//7.3
void tMboxFlush(tMbox* mbox);
uint32_t tMboxDestory(tMbox* mbox);

//7.4 mbox的查询结构
typedef struct _tMboxInfo
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t taskCount;
}tMboxInfo;

//7.4 mbox状态查询
void tMboxGetInfo(tMbox* mbox, tMboxInfo* info);


#endif
