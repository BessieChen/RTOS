//7.1 �������Ľṹ����
#ifndef TMBOX_H
#define TMBOX_H

//7.1 ���ECB��ͷ�ļ�
#include "tEvent.h"

typedef struct _tMbox
{
	//7.1 ECB����Ҫ���ʼ���task
	tEvent eventECB;
	uint32_t count; //��ǰ�ʼ���ʵ������
	uint32_t read; //������
	uint32_t write; //д����
	uint32_t maxCount; //���������
	void** msgBuffer; //����������һ����ַ, ָ���msgBuffer�ĵ�ַ
}tMbox;

//7.1 
void tMboxInit(tMbox* mbox, void** msgBuffer, uint32_t maxCount);

#endif
