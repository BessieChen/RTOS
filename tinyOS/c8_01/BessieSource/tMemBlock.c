#include "tMemBlock.h"

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

