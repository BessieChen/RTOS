#include "tMemBlock.h"
#include "tinyOS.h" //�ٽ�����

//8.1 
void tMemBlockInit(tMemBlock* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t maxBlockCount)
{
	//��������ָ�����, �ֱ�ָ�� �洢���� ����ʼ��ַ, ��ֹ��ַ
	uint8_t* memBlockStart = (uint8_t*) memStart;
	uint8_t* memBlockEnd = memBlockStart + blockSize * maxBlockCount; //�ҵ�����, ����uint32_t���, ֮�󸳸�uint8_t, ���������
	
	//���ǵ�block��Ҫ�洢һ��tNode��, �ǵ���һ��tNode�����������tNode*, ���blockSize������, �ô治��
	if(blockSize <sizeof(tNode))
	{
		return;
	}
	
	//��ʼ��tList
	memBlock->memStart = memStart;
	memBlock->blockSize = blockSize;
	memBlock->maxBlockCount = maxBlockCount;
	tListInit(&memBlock->blockList);
	
	//��ʼ��ECB
	tEventInit(&memBlock->eventECB, tEventTypeMemBlock); //tEventTypeMemBlock, //˵�����tEvent��Ϊ�˴洢���������

	//��ʼ��tNode, Ȼ�����tList��ǰ��, Ȼ���ʼ����һ��tNode
	while(memBlockStart < memBlockEnd)
	{
		tNodeInit((tNode*) memBlockStart);
		tListAddFirst(&memBlock->blockList, (tNode*)memBlockStart);
		
		//ָ����һ��tNode��λ��
		memBlockStart += blockSize;
	}
	

}


//8.2 �ȴ�block
uint32_t tMemBlockWait(tMemBlock* memBlock, uint8_t** mem, uint32_t waitTicks)//����ַ��ֵ: ��block�ĵ�ַ�浽����mem��, ���û��block�͵ȴ�waitTick��systick
{
	uint32_t status = tTaskEnterCritical();
	
	if(tListCount(&memBlock->blockList) > 0) //�ж����block
	{
		*mem = (uint8_t*)tListRemoveFirst(&memBlock->blockList); //��tList��ͷ���Ƴ����block, �ѵ�ַ����mem
		
		tTaskExitCritical(status);
		return tErrorNoError;
	}

	//˵�����task��Ҫ����ECB�еȴ�
	tEventWait(&memBlock->eventECB, currentTask, (void*)0, tEventTypeMemBlock, waitTicks); //��ʱwaitTicks
	
	tTaskExitCritical(status);
	
	//currentTask�Ѿ�����ȴ�������, �����л�
	tTaskSched();
	
	//�����ǻص�currentTask��ʱ��ִ�е�
	*mem = currentTask->eventMsg;
	return currentTask->waitEventResult;
}

//8.2 ���û��block�Ͳ���
uint32_t tMemBlockNoWaitGet(tMemBlock* memBlock, void** mem)
{
	uint32_t status = tTaskEnterCritical();
	
	if(tListCount(&memBlock->blockList) > 0) //�ж����block
	{
		*mem = (uint8_t*)tListRemoveFirst(&memBlock->blockList); //��tList��ͷ���Ƴ����block, �ѵ�ַ����mem
		
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	
	tTaskExitCritical(status);
	return tErrorResourceUnavailable;
}

//8.2 �ͷ�block, Ҳ����task������
void tMemBlockNotify(tMemBlock* memBlock, uint8_t* mem)
{
	uint32_t status = tTaskEnterCritical();
	
	if(tEventWaitCount(&memBlock->eventECB) > 0)//˵����task�ڵ�
	{
		tTask* task = tEventWakeUp(&memBlock->eventECB, (void*)mem, tErrorNoError); //��block�ĵ�ַ��mem, ����ǵ���msg�������. task->eventMsg = msg;, ����֮��block�ĵ�ַ��浽task->eventMsg��
		if(task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	else
	{
		tListAddLast(&memBlock->blockList, (tNode*)mem); //��mem��ΪtNode��ָ��, ����tList��ĩβ
	}
	tTaskExitCritical(status);
}

//8.3 ����״̬
void tMemBlockGetInfo(tMemBlock* memBlock, tMemBlockInfo* info)
{
	uint32_t status = tTaskEnterCritical();
	
	info->count = tListCount(&memBlock->blockList);
	info->maxBlockCount = memBlock->maxBlockCount;
	info->blockSize = memBlock->blockSize;
	info->taskCount = tEventWaitCount(&memBlock->eventECB);
	
	tTaskExitCritical(status);

}

//8.3 ɾ���洢��
uint32_t tMemBlockDestory(tMemBlock* memBlock)
{
	uint32_t status = tTaskEnterCritical();

	uint32_t count = tEventRemoveAll(&memBlock->eventECB, (void*)0, tErrorDel);
	tTaskExitCritical(status);
	
	if(count > 0)//˵�����µ�task�����˾�������
	{
		tTaskSched();
	}
	return count;
}




