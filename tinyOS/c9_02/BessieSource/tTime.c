//��ʱ����ص�
#include "tinyOS.h"

//3.6 ��Ҫ�޸� //3.4 ������Ҫ���޸� //2.4 ʵ����ʱ����, ��Ϊ��������϶���currentTask���õ�,��������ֱ��дcurrentTask->xxx
void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	//3.6 ɾ��3.4����
	/*
	currentTask->delayTicks = delay; //Ҳ����ΪcurrentTask������Ҫ��ʱ���
	tBitmapClear(&taskPrioBitmap, currentTask->prio); //3.4 ��Ϊ��ʱ˵��currentTask����Ҫcpu,��Ȼ����Ҫ,˵���������ж�Ӧ��Ԫ�ؾ����ó�0
	//3.4 ע��:����clear()��,����currentTask��prio����û�б仯, taskTable��Ҳ�������������
	*/
	
	//3.6 ��task������ʱ����, ��task�Ӿ���������ɾ��
	tTimeTaskWait(currentTask, delay); //ע��, ��Ϊ��������û��task����, ���Ǵ���currentTask, ��Ϊ��������϶���currentTask�����
	tTaskSchedUnReady(currentTask);
	
	tTaskExitCritical(status);
	tTaskSched();//��������currentTask�Ƿ�����cpu,������tTaskSched()���жϻ���˭����Ҫcpu��
}
