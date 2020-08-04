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

//8.3 �洢���״̬��ѯ�ṹ
typedef struct _tMemBlockInfo
{
	uint32_t count; //���д洢��ĸ���
	uint32_t maxBlockCount; //�洢���������
	uint32_t blockSize;
	uint32_t taskCount; //�ȴ���task�ĸ���
}tMemBlockInfo;

//8.3
void tMemBlockGetInfo(tMemBlock* memBlock, tMemBlockInfo* info); //����״̬
uint32_t tMemBlockDestory(tMemBlock* memBlock); //ɾ���洢��

#endif

