//5.1 �¼����ƿ�Ľṹ��ͺ�������
#ifndef TEVENT_H //ע�ⲻҪд��
#define TEVENT_H

#include "tLib.h" //������tList

//5.1 ÿһ���¼����ƿ����涼��һ���¼�����
typedef enum _tEventType
{
	tEventTypeUnknown,
}tEventType;

//5.1 �¼����ƿ�ECB
typedef struct _tEvent
{
	tEventType type;
	tList waitList; //����ǵȴ��¼���task��tNode
}tEvent;
	
//5.1 ECB�ĳ�ʼ������
void tEventInit(tEvent* event, tEventType* type);

#endif

