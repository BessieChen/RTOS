//8.1 �洢�������
#ifndef TMEMBLOCK_H
#define TMEMBLOCK_H

#include "tEvent.h"

//8.1
typedef struct _tMemBlock
{
	tEvent eventECB; //�ȴ��洢���task��������
	void* memStart; //�洢�������ʼ��ַ
	uint32_t blockSize; //ÿ���洢��Ĵ�С
	uint32_t maxBlockCount; //�洢���������
	tList blockList;	//��Ŵ洢����б�
}tMemBlock;

//8.1 ��ʼ��
void tMemBlockInit(tMemBlock* memBlock, uint8_t* memStart, uint32_t blockSize, uint32_t maxBlockCount);

//8.2
uint32_t tMemBlockWait(tMemBlock* memBlock, uint8_t** mem, uint32_t waitTicks);
uint32_t tMemBlockNoWaitGet(tMemBlock* memBlock, void** mem);
void tMemBlockNotify(tMemBlock* memBlock, uint8_t* mem);

#endif

