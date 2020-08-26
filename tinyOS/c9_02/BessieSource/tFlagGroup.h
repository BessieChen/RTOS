//9.1 ���: �¼���־��Ľṹ����
#ifndef TFLAGGROUP_H
#define TFLAGGROUP_H

#include "tEvent.h"

//9.1
typedef struct _tFlagGroup
{
	tEvent eventECB;
	uint32_t flag; //���԰���32���¼���ʶ
}tFlagGroup;

//9.2 �궨��: ϣ�������¼�����
#define TFLAGGROUP_CLEAR				(0x0 << 0) //ϣ��ĳһλ��ʶ��0								00
#define TFLAGGROUP_SET					(0x1 << 0)	//����							01
#define TFLAGGROUP_ANY					(0x0 << 1)	//�ȴ�����ı�־λ		00
#define TFLAGGROUP_ALL					(0x1 << 1) //�ȴ����еı�־λ	10

//9.2 �������ֺ궨������
#define TFLAGGROUP_SET_ALL (TFLAGGROUP_SET | TFLAGGROUP_ALL) //���ó����б�ʶ�����ֲ��� 11
#define TFLAGGROUP_SET_ANY (TFLAGGROUP_SET | TFLAGGROUP_ANY) //���ó�ֻҪ��һ����ʶ�������� 01
#define TFLAGGROUP_CLEAR_ALL (TFLAGGROUP_CLEAR | TFLAGGROUP_ALL) //������б�ʶ 10
#define TFLAGGROUP_CLEAR_ANY (TFLAGGROUP_CLEAR | TFLAGGROUP_ANY) //�������һ����ʶ 00

//9.2 �궨��: ����ȴ����¼�����, �˳���ʱ��, �費��Ҫ�ѱ�־λ����
#define TFLAGGROUP_CONSUME 			(1 << 7) //1000 0000

//9.1 ��ʼ��
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags);

//9.2 �ȴ�����
uint32_t tFlagGroupWait(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag, uint32_t waitTicks);//waitType: �ȴ�������, requestFlag:�ȴ���Щ��ʶ, resultFlag(ע���ǵ�ַ, uint32_t*)���ȵ�����Щ��ʶ, waitTick:��ʱ
//9.2 ��ȡ�¼���ʶ
uint32_t tFlagGroupNoWaitGet(tFlagGroup* flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t* resultFlag);
//9.2 ֪ͨ��Ҫ�ı�־λ������
void tFlagGroupNotify(tFlagGroup* flagGroup, uint8_t isSet, uint32_t flag);

#endif

