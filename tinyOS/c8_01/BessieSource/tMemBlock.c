#include "tMemBlock.h"

//8.1 
void tMemBlockInit(tMemBlock* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t maxBlockCount)
{
	//定义两个指针变量, 分别指向 存储区域 的起始地址, 终止地址
	uint8_t* memBlockStart = (uint8_t*) memStart;
	uint8_t* memBlockEnd = memBlockStart + blockSize * maxBlockCount; //我的疑问, 两个uint32_t相乘, 之后赋给uint8_t, 不会溢出吗
	
	//我们的block是要存储一个tNode的, 记得吗一个tNode里面存了两个tNode*, 如果blockSize不够大, 久存不了
	if(blockSize <sizeof(tNode))
	{
		return;
	}
	
	//初始化tList
	memBlock->memStart = memStart;
	memBlock->blockSize = blockSize;
	memBlock->maxBlockCount = maxBlockCount;
	tListInit(&memBlock->blockList);
	
	//初始化ECB
	tEventInit(&memBlock->eventECB, tEventTypeMemBlock); //tEventTypeMemBlock, //说明这个tEvent是为了存储块而建立的

	//初始化tNode, 然后加入tList的前端, 然后初始化下一个tNode
	while(memBlockStart < memBlockEnd)
	{
		tNodeInit((tNode*) memBlockStart);
		tListAddFirst(&memBlock->blockList, (tNode*)memBlockStart);
		
		//指向下一个tNode的位置
		memBlockStart += blockSize;
	}
	

}

