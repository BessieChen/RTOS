//7.1 ����Ķ���
#include "tMbox.h"
//7.2 tTask�ȵĲ���:
#include "tinyOS.h"

//7.1
void tMboxInit(tMbox* mbox, void** msgBuffer, uint32_t maxCount)
{
	tEventInit(&mbox->eventECB, tEventTypeMbox); //�������ECB���������͵�
	
	mbox->msgBuffer = msgBuffer;//bug!!! ����ô����û��д��һ��!! 	void** msgBuffer; //����������һ����ַ, ָ���void*�ĵ�ַ, ���Ƕ���ָ��
	mbox->maxCount = maxCount; //����
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;  //ʵ�ʴ洢
}

//7.2 �ȴ�����(�����task���õ�)
uint32_t tMboxWait(tMbox* mbox, void** msg, uint32_t waitTicks) //void** msg: ���ڴ����Ϣ��ָ��, uint32_t waitTicks: �ȴ���ʱ��
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	
	//�����û����Ϣ
	if(mbox->count >0 )//����Ϣ
	{
		--mbox->count; //��Ϣ-1
		*msg = mbox->msgBuffer[mbox->read++]; //����ȡ����Ϣ,ͨ��read������ȡ��, Ȼ��readָ��+1
		if(mbox->read >= mbox->maxCount) //������ʵ����д�����read == maxCount, ����maxCount��ʵ����capacity, ����������. ����һ��read == maxCount, read���ǷǷ���
		{
			mbox->read = 0; 
		}
		tTaskExitCritical(status);
		return tErrorNoError; //û�д���, ˵����ȷ����Ϣ, Ȼ��������msg
	}
	else//û����Ϣ, task��Ҫ�ȴ�
	{
		tEventWait(&mbox->eventECB, currentTask, (void*)0, tEventTypeMbox, waitTicks); //task�ĵȴ�������mbox����, �����������waitTicks��systick֮��û�еȵ�,�Ͳ�����
		tTaskExitCritical(status);//�˳��ٽ���, ��Ϊ��������TaskSched()ҲҪ�����ٽ���
		
		//��ʱtask�Ѿ�����ȴ�����, ��Ҫ�л�������task
		tTaskSched();
		
		//�ߵ���һ���ʱ��, ���ֻص������currentTask, �ص���ԭ��: msg����, ������ʱ������
		*msg = currentTask->eventMsg; //�����Ϣ
		return currentTask->waitEventResult; //���嵽����: msg����, ������ʱ������, �ǿ�������;������ʲô
	}
}

//7.2 ʵ��: ���û����Ϣ�Ͳ�����
uint32_t tMboxNoWaitGet(tMbox* mbox, void** msg)
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();
	if(mbox->count > 0)//����Ϣ
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else //û��Ϣ
	{ 
		tTaskExitCritical(status);
		return tErrorResourceUnavailable;
	}
}

//7.2 ��Ϣ��֪ͨ, Ҳ������������һ����Ϣmsg, ������Щ�ڵȴ������е�task��
uint32_t tMboxNotify(tMbox* mbox, void* msg, uint32_t notifyOption) // uint32_t notifyOption ָ������������msg�Ǹ����ȼ���, �ᱻ���ȶ�ȡ
{
	//�ٽ���
	uint32_t status = tTaskEnterCritical();

	//�ж���û�������ڵȴ�
	if(tEventWaitCount(&mbox->eventECB) > 0) //˵����task�ڵ�, �Ǿ�ֱ�ӽ�msg�����task, msgҲû��Ҫ��buffer�������
	{
		//����һ��task
		tTask* task = tEventWakeUp(&mbox->eventECB, (void*)msg, tErrorNoError); //msg�ᴫ��task->eventMsg��, Ȼ��tErrorNoError�ᴫ��task->eventMsgResult��, ֮��᷵�ص�tMboxWait()�е�*msg = currentTask->eventMsg; currentTask->waitEventResult; 
		
		//��Ȼһ��task��������, ���task�������ȼ�����, �������, ��Ҫ�л�����
		if(task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	else //˵��û��task�ڵ�, msg��Ҫ����buffer��, ��������Ҫ����, �ǲ��������Ѿ�����
	{
		if(mbox->count >= mbox->maxCount) //���˾�Ҫ�����˳�
		{
			tTaskExitCritical(status);
			return tErrorResourceFull; //��������
		}
		
		//�ߵ���һ��, ˵��û����, msg���Լ���buffer��
		// tMBOXSendNormal 			0x00 //����ǰ���ͨ����������msg, Ҳ������write+1�ĵط�����msg
		// tMBOXSendFront				0x01 //���˵�������msg�����ȼ��Ƚϸ�, ������read-1�ĵط�����msg, ����task��һ�����ľ������msg
		if(notifyOption & tMBOXSendFront) //˵��notifyOption��ĩλ == 1
		{
			if(mbox->read <= 0) //������read == 0
			{
				mbox->read = mbox->maxCount - 1; 
			}
			else
			{
				--mbox->read;
			}
			
			//����read����-1, �����, ���readָ����Ԫ�ص�λ��
			mbox->msgBuffer[mbox->read] = msg;
		}
		else //��ͨ�ķ�ʽд��
		{	
			//����ͱȽ�������, ������writeָ��ľ���һ��û�з�Ԫ�ص�λ��, Ȼ�������msg��, write��+1, ����ж�write�ĺϷ���
			mbox->msgBuffer[mbox->write++] = msg;
			if(mbox->write >= mbox->maxCount)
			{
				mbox->write = 0;
			}
		}
		
		//����Ϣ������+1
		mbox->count++;
	}
	
	tTaskExitCritical(status);
	return tErrorNoError;
}




