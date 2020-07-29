#ifndef TINYOS_H
#define TINYOS_H

//������׼��ͷ�ļ�,������uint32_t
#include <stdint.h>
//3.3 ��tinyOS.h�����tLib.h��ԭ��: ����main.c���������tinyOS.h, Ҳ�Ϳ���ʹ��tLib.h�еĳ�Ա��
//ע��: �ڴ���tLib.h��ʱ��,Ҫ��֤�������ǵ�BessieSource�ļ�����, ����ᱨ��: error:  #5: cannot open source input file "tLib.h": No such file or directory
#include "tLib.h"
//3.4 ����tinyOS.h�����tConfig.h��ԭ��: ����main.c���������tinyOS.h, Ҳ�Ϳ���ʹ��tConfig.h�еĳ�ԱTINYOS_PRIO_COUNT��
#include "tConfig.h"

//��ΪtinyOS.h�Ǹ��û������ǵ���Ŀ������Щ��Ա��,���Խ���������ʽ������

//����,�Ƕ���һ������,��С��uint32_t, ������ΪtTaskStack,��˼��һ�������stack
typedef uint32_t tTaskStack;

//���,����һ���ṹ��,Ҳ���Ǳ�������һ�����������������Ϣ,����ֻ������stackһ����Ϣ
//������˵,��һ��tTaskStack��ָ��,�������ǿ���ͨ�����ָ��,ȥ�ҵ�����stack
typedef struct _tTask
{
		tTaskStack * stack;
		uint32_t delayTicks;//2.4������ӳٵļ�����
		uint32_t prio; //3.4 �������ȼ����ֶ�
}tTask;

//��������, todo: ����extern�ɲ�����,��Ϊ����ֻ������,û�ж���
extern tTask* currentTask;
extern tTask* nextTask;
extern tTask* idleTask; //��Ҫ������д����һ��,����main.c�ᱨ��

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
#endif
