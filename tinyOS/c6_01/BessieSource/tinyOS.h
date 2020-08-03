#ifndef TINYOS_H
#define TINYOS_H

//������׼��ͷ�ļ�,������uint32_t
#include <stdint.h>
//3.3 ��tinyOS.h�����tLib.h��ԭ��: ����main.c���������tinyOS.h, Ҳ�Ϳ���ʹ��tLib.h�еĳ�Ա��
//ע��: �ڴ���tLib.h��ʱ��,Ҫ��֤�������ǵ�BessieSource�ļ�����, ����ᱨ��: error:  #5: cannot open source input file "tLib.h": No such file or directory
#include "tLib.h"
//3.4 ����tinyOS.h�����tConfig.h��ԭ��: ����main.c���������tinyOS.h, Ҳ�Ϳ���ʹ��tConfig.h�еĳ�ԱTINYOS_PRIO_COUNT��
#include "tConfig.h"
//5.1 ��tinyOS.h�����tEvent.h��ԭ��: ����tEvent.c���������tinyOS.h, Ҳ�Ϳ���ʹ��tEvent.h�еĳ�Ա��
#include "tEvent.h"
//5.2 ��tinyOS.h�Ĺ���task�����ݶ��Ƶ���tTask.h��.	//x.x ��ΪtinyOS.h�Ǹ��û������ǵ���Ŀ������Щ��Ա��,���Խ���������ʽ������
#include "tTask.h"
//6.1 ����sem
#include "tSem.h"


//5.2 ������
typedef enum _tError
{
	tErrorNoError = 0, //û�д���
	tErrorTimeout = 1, //��ʱ��
}tError;




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


//һֱռ��cpu����ʱ����
void delay(int ticks);

//4.1 ����͹���ָ�
void tTaskSuspend(tTask* task);
void tTaskWakeUp(tTask* task);

//4.3 ������ɾ��
void tTimeTaskRemove(tTask* task);  //����ʱ����ɾ��


#endif
