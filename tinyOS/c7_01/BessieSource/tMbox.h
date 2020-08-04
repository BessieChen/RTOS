//7.1 添加邮箱的结构定义
#ifndef TMBOX_H
#define TMBOX_H

//7.1 添加ECB的头文件
#include "tEvent.h"

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

#endif
