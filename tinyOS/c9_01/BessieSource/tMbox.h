//7.1 �������Ľṹ����
#ifndef TMBOX_H
#define TMBOX_H

//7.1 ���ECB��ͷ�ļ�
#include "tEvent.h"

//7.2 �궨��
#define tMBOXSendNormal 			0x00 //����ǰ���ͨ����������msg, Ҳ������write+1�ĵط�����msg
#define tMBOXSendFront				0x01 //���˵�������msg�����ȼ��Ƚϸ�, ������read-1�ĵط�����msg, ����task��һ�����ľ������msg
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

//7.2 
uint32_t tMboxWait(tMbox* mbox, void** msg, uint32_t waitTicks); //void** msg: ���ڴ����Ϣ��ָ��, uint32_t waitTicks: �ȴ���ʱ��
uint32_t tMboxNoWaitGet(tMbox* mbox, void** msg);
uint32_t tMboxNotify(tMbox* mbox, void* msg, uint32_t notifyOption); // uint32_t notifyOption ָ������������msg�Ǹ����ȼ���, �ᱻ���ȶ�ȡ


//7.3
void tMboxFlush(tMbox* mbox);
uint32_t tMboxDestory(tMbox* mbox);

//7.4 mbox�Ĳ�ѯ�ṹ
typedef struct _tMboxInfo
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t taskCount;
}tMboxInfo;

//7.4 mbox״̬��ѯ
void tMboxGetInfo(tMbox* mbox, tMboxInfo* info);


#endif
