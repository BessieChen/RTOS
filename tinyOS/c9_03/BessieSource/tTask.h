//5.2 ��������صĽӿڶ�������, ���������صĽṹ����ͺ�������
#ifndef TTASK_H
#define TTASK_H

//#include "tinyOS.h" //5.2 byb, ��ȷ���Բ���
#include <stdint.h> //5.2 byb, ������uint32_t
#include "tLib.h" //5.2 byb,������tNode

struct _tEvent;//5.2 ������ǰ������
//����: ��ΪtTask.h���õ���tEvent, Ȼ��tEvent.h���õ���tTask. ���ͷ�ļ��ж������˶Է���.h�ļ�, �о��������޵ݹ���, ����
//�����������������struct _tEvent�Ľṹ��, ���߱�����, ����ô������, ֮��, ����line42 ��struct _tEvent*, ˵��������ṹ���ָ��, ָ��һ�㶼��4���ֽ�, ���Ա�����֪��Ҫ��������ֽ�
//����line42 ��struct _tEvent, ��������, ��Ϊ��������֪��_tEvent�ж��, ��֪��������ٿռ�

//3.6 �궨��: �����Ƿ�׼������
#define TINYOS_TASK_STATE_READY				0				//����
#define TINYOS_TASK_STATE_DESTROYED		(1<<1)	//4.2 ��ɾ����״̬λ
#define TINYOS_TASK_STATE_DELAYED			(1<<2)	//��ʱ��
#define TINYOS_TASK_STATE_SUSPEND			(1<<3)	//4.1 �����״̬λ

//5.2 �ڸ���, ��֮ǰ�����õ�״̬λ�����
#define TINYOS_TASK_WAIT_MASK 				(0xFF<<16) //5.2 Ҳ����1111 1111 0000 0000

//����,�Ƕ���һ������,��С��uint32_t, ������ΪtTaskStack,��˼��һ�������stack
typedef uint32_t tTaskStack;

//���,����һ���ṹ��,Ҳ���Ǳ�������һ�����������������Ϣ,����ֻ������stackһ����Ϣ
//������˵,��һ��tTaskStack��ָ��,�������ǿ���ͨ�����ָ��,ȥ�ҵ�����stack
typedef struct _tTask
{
		tTaskStack * stack;
		uint32_t delayTicks;//2.4������ӳٵļ�����
		uint32_t prio; //3.4 �������ȼ����ֶ�
		tNode delayNode; //3.6 ��ʱ�ڵ�, ע��, �������tNode, ������tNode*
		uint32_t state; //3.6 �����״̬: �Ƿ���ʱ, ...
		tNode linkNode; //3.7 �����ڵ�,����ͬһ�����ȼ�, �����ж������, Ҳ����tList(ĳ�����ȼ�)�д洢���linkNode(�����ҵ���Ӧ��task)
		uint32_t slice; //3.7 ʱ��Ƭ
		uint32_t suspendCount; //4.1 ����ļ�����
	
		void (*clean) (void* param); //4.2 ����һ������, (*clean)�Ǻ���ָ��(�������Ϊ���Ǻ�������˼), ����ֵ��void, ����һ������param��void*����
		void* cleanParam; //4.2 ����һ��ָ��,����clean�����е� ����param
		uint8_t requestDeleteFlag; //4.2 ɾ������ı��
	
		struct _tEvent* waitEvent; //5.2 �ȴ��Ǹ�ECB
		void* eventMsg;//5.2 �ȴ������ݵĴ�ŵ�λ��, ��������
		uint32_t waitEventResult; //5.2 �ȴ��Ľ��,���������
	
		uint32_t waitFlagsType;//9.2 ������¼�����
		uint32_t eventFlags;//9.2 ������¼���ʶ
		
}tTask;

//4.3 �洢task����Ϣ�Ľṹ��. ΪʲôҪ��������һ����info�Ľṹ��? ������Ϊ, �ڴ�Ĺ���, ������һ������, �������ǻ�õ�info���ǿ���ʱ���info, ����delayTicks, tTask��delayTicks�ǻ�仯��, ����tTaskInfo���Ǳ����˿���ʱ���ֵ
typedef struct _tTaskInfo
{
	uint32_t delayTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
}tTaskInfo;

//4.1 ��ʼ��app
void tAppInit(void);

//4.2 ������ɾ��
void tTaskSchedRemove(tTask* task); //�Ӿ�������ɾ��
void tTaskSetCleanCallFunc(tTask* task, void (*clean) (void* param), void* param);\
void tTaskForceDelete(tTask* task);
void tTaskRequestDelete(tTask* task);
uint8_t tTaskIsRequestedDeleted(void);
void tTaskDeleteSelf(void);

//4.3 �洢task��info
void tTaskGetInfo(tTask* task, tTaskInfo* info);

#endif
