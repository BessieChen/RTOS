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

//9.1 ��ʼ��
void tFlagGroupInit(tFlagGroup* flagGroup, uint32_t flags);

#endif

