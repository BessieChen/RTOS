//7.1 ����Ķ���
#include "tMbox.h"

//7.1
void tMboxInit(tMbox* mbox, void** msgBuffer, uint32_t maxCount)
{
	tEventInit(&mbox->eventECB, tEventTypeMbox); //�������ECB���������͵�
	mbox->maxCount = maxCount; //����
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;  //ʵ�ʴ洢
}
