//8.1 存储块的声明
#ifndef TMEMBLOCK_H
#define TMEMBLOCK_H

#include "tEvent.h"

//8.1
typedef struct _tMemBlock
{
	tEvent eventECB; //等待存储块的task存在这里
	void* memStart; //存储区域的起始地址
	uint32_t blockSize; //每个存储块的大小
	uint32_t maxBlockCount; //存储块的最大个数
	tList blockList;	//存放存储块的列表
}tMemBlock;

//8.1 初始化
void tMemBlockInit(tMemBlock* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t maxBlockCount);

//8.2
uint32_t tMemBlockWait(tMemBlock* memBlock, uint8_t** mem, uint32_t waitTicks);
uint32_t tMemBlockNoWaitGet(tMemBlock* memBlock, void** mem);
void tMemBlockNotify(tMemBlock* memBlock, uint8_t* mem);

#endif

