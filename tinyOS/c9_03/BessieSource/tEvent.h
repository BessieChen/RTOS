//5.1 �¼����ƿ�Ľṹ��ͺ�������
#ifndef TEVENT_H //ע�ⲻҪд��
#define TEVENT_H

#include "tLib.h" //������tList
#include "tTask.h" //������tTask

//5.1 ÿһ���¼����ƿ����涼��һ���¼�����
typedef enum _tEventType
{
	tEventTypeUnknown,
	tEventTypeSem, //�ź����еĵȴ�����
	tEventTypeMbox, //�����еĵȴ�����
	tEventTypeMemBlock, //˵�����tEvent��Ϊ�˴洢���������
	tEventTypeFlagGroup, //˵�����tEvent��Ϊ���¼���־���������
}tEventType;

//5.1 �¼����ƿ�ECB
typedef struct _tEvent
{
	tEventType type;
	tList waitList; //����ǵȴ��¼���task��tNode
}tEvent;
	
//5.1 ECB�ĳ�ʼ������
void tEventInit(tEvent* event, tEventType type); //5.2 ����typeû���õ�ѽ..
void tEventWait(tEvent* event, tTask* task, void* msg, uint32_t state, uint32_t timeout);//5.2 ��task����ECB�ĵȴ�����
tTask* tEventWakeUp(tEvent* event, void* msg, uint32_t result);//5.2 ��task��ECB�ĵȴ������л���
void tEventRemoveTask(tTask* task, void* msg, uint32_t result);//5.2 ��task��ECB��ǿ���Ƴ�
uint32_t tEventRemoveAll(tEvent* eventECB, void* msg, uint32_t result);//5.3
uint32_t tEventWaitCount(tEvent* eventECB);//5.3
tTask* tEventWakeUpSpecificTask(tEvent* eventECB, tTask* task, void* msg, uint32_t result); //9.2



#endif

