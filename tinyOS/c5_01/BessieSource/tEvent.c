#include "tinyOS.h"

void tEventInit(tEvent* event, tEventType* type) //5.2 ����typeû���õ�ѽ..
{
	event->type = tEventTypeUnknown;
	tListInit(&(event->waitList));
}
