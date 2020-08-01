#include "tinyOS.h"

void tEventInit(tEvent* event, tEventType* type) //5.2 这里type没有用到呀..
{
	event->type = tEventTypeUnknown;
	tListInit(&(event->waitList));
}
