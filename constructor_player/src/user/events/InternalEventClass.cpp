/***************************************************************************
InternalEventClass.cpp  -  класс определения типа представления внутренних событий
-------------------
begin                : 13 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "InternalEventClass.h"

void InternalEventsClass::addEvent (InternalEventsClass::OneInternalEventClass oneevent)
{
	events.push_back (oneevent);
}


std::vector <InternalEventsClass::OneInternalEventClass> InternalEventsClass::getEvents()
{
	return events;
}





