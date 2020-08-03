#ifndef TINYOS_H
#define TINYOS_H

//������׼��ͷ�ļ�,������uint32_t
#include <stdint.h>
//3.3 ��tinyOS.h������tLib.h��ԭ��: ����main.c���������tinyOS.h, Ҳ�Ϳ���ʹ��tLib.h�еĳ�Ա��
//ע��: �ڴ���tLib.h��ʱ��,Ҫ��֤�������ǵ�BessieSource�ļ�����, ����ᱨ��: error:  #5: cannot open source input file "tLib.h": No such file or directory
#include "tLib.h"
//3.4 ����tinyOS.h������tConfig.h��ԭ��: ����main.c���������tinyOS.h, Ҳ�Ϳ���ʹ��tConfig.h�еĳ�ԱTINYOS_PRIO_COUNT��
#include "tConfig.h"

//3.6 �궨��: �����Ƿ�׼������
#define TINYOS_TASK_STATE_READY				0				//����
#define TINYOS_TASK_STATE_DESTROYED		(1<<1)	//4.2 ��ɾ����״̬λ
#define TINYOS_TASK_STATE_DELAYED			(1<<2)	//��ʱ��
#define TINYOS_TASK_STATE_SUSPEND			(1<<3)	//4.1 �����״̬λ

//��ΪtinyOS.h�Ǹ��û������ǵ���Ŀ������Щ��Ա��,���Խ���������ʽ������

//����,�Ƕ���һ������,��С��uint32_t, ������ΪtTaskStack,��˼��һ�������stack
typedef uint32_t tTaskStack;

//���,����һ���ṹ��,Ҳ���Ǳ�������һ�����������������Ϣ,����ֻ������stackһ����Ϣ
//������˵,��һ��tTaskStack��ָ��,�������ǿ���ͨ�����ָ��,ȥ�ҵ�����stack
typedef struct _tTask
{
		tTaskStack * stack;
		uint32_t delayTicks;//2.4�������ӳٵļ�����
		uint32_t prio; //3.4 �������ȼ����ֶ�
		tNode delayNode; //3.6 ��ʱ�ڵ�, ע��, �������tNode, ������tNode*
		uint32_t state; //3.6 �����״̬: �Ƿ���ʱ, ...
		tNode linkNode; //3.7 �����ڵ�,����ͬһ�����ȼ�, �����ж������, Ҳ����tList(ĳ�����ȼ�)�д洢���linkNode(�����ҵ���Ӧ��task)
		uint32_t slice; //3.7 ʱ��Ƭ
		uint32_t suspendCount; //4.1 ����ļ�����
	
		void (*clean) (void* param); //4.2 ����һ������, (*clean)�Ǻ���ָ��(�������Ϊ���Ǻ�������˼), ����ֵ��void, ����һ������param��void*����
		void* cleanParam; //4.2 ����һ��ָ��,����clean�����е� ����param
		uint8_t requestDeleteFlag; //4.2 ɾ������ı��
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

//��������, todo: ����extern�ɲ�����,��Ϊ����ֻ������,û�ж���
extern tTask* currentTask;
extern tTask* nextTask;
extern tTask* idleTask; //��Ҫ������д����һ��,����main.c�ᱨ��

//3.4 ��ʼ��
void taskInit(tTask* task, void (*func)(void*), void* param, uint32_t prio, tTaskStack* stack);
	
//2.4 systick
void tSetSysTickPeriod(uint32_t ms); //����
void tTaskSysTickHandler(void);

//3.4 ��ʱ����
void tTaskDelay(uint32_t delay);

//3.1 Ҫ�ǵý����ú��˳��ٽ�������������
uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t param);

//�����������������Ǻ�cpu��ص�,���ǵ���,��������pendSV�쳣,Ȼ��pendSVʹ��asmд��.�����������Ķ�����switch.c��
void tTaskRunFirst(void); //todo,�������Ĳ�����дvoid�в���?��Ϊswitch.c�еĲ�������()
void tTaskSwitch(void);
void tTaskSched(void); //3.2 ��ʦ��3.2ʱ��ӵ�,Ӧ����ͼ���, ע��Ҫ���ϲ�����void, �������warning:  #1295-D: Deprecated declaration tTaskSched - give arg types

//3.2 ������
void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskSchedEnable(void);

//3.4 ������
tTask* tTaskHighestTaskReady(void);

//3.7 ��ʱ����
void tTimeTaskWait(tTask* task, uint32_t ticks);
void tTimeTaskWakeUp(tTask* task);

//3.7 ��������
void tTaskSchedReady(tTask* task);
void tTaskSchedUnReady(tTask* task);

//4.1 ��ʼ��app
void tAppInit(void);

//һֱռ��cpu����ʱ����
void delay(int ticks);

//4.1 ����͹���ָ�
void tTaskSuspend(tTask* task);
void tTaskWakeUp(tTask* task);

//4.2 ������ɾ��
void tTaskSchedRemove(tTask* task); //�Ӿ�������ɾ��
void tTimeTaskRemove(tTask* task);  //����ʱ����ɾ��
void tTaskSetCleanCallFunc(tTask* task, void (*clean) (void* param), void* param);\
void tTaskForceDelete(tTask* task);
void tTaskRequestDelete(tTask* task);
uint8_t tTaskIsRequestedDeleted(void);
void tTaskDeleteSelf(void);

//4.3 �洢task��info
void tTaskGetInfo(tTask* task, tTaskInfo* info);

#endif