#ifndef TINYOS_H
#define TINYOS_H

//������׼��ͷ�ļ�,������uint32_t
#include <stdint.h>

//��ΪtinyOS.h�Ǹ��û������ǵ���Ŀ������Щ��Ա��,���Խ���������ʽ������

//����,�Ƕ���һ������,��С��uint32_t, ������ΪtTaskStack,��˼��һ�������stack
typedef uint32_t tTaskStack;

//���,����һ���ṹ��,Ҳ���Ǳ�������һ�����������������Ϣ,����ֻ������stackһ����Ϣ
//������˵,��һ��tTaskStack��ָ��,�������ǿ���ͨ�����ָ��,ȥ�ҵ�����stack
typedef struct _tTask
{
		tTaskStack * stack;
}tTask;

//��������, todo: ����extern�ɲ�����,��Ϊ����ֻ������,û�ж���
extern tTask* currentTask;
extern tTask* nextTask;

//�����������������Ǻ�cpu��ص�,���ǵ���,��������pendSV�쳣,Ȼ��pendSVʹ��asmд��.�����������Ķ�����switch.c��
void tTaskRunFirst(void); //todo,�������Ĳ�����дvoid�в���?��Ϊswitch.c�еĲ�������()
void tTaskSwitch(void);

#endif
