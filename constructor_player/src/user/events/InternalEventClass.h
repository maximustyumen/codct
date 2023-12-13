#ifndef INTERNALEVENTCLASS
#define INTERNALEVENTCLASS

#include <vector>

#include <stdlib.h>
#include <stdio.h> 
#include <sstream>
#include <iostream>

class InternalEventsClass
{
public:
	//внутреннее определения типа событий
	class OneInternalEventClass
	{
	public:
		std::string Event;
                std::vector <double> number_param;
		std::vector <std::string> string_param;
		double time;
	};
	//
	void addEvent (OneInternalEventClass);
	std::vector <OneInternalEventClass> getEvents();
private:
	std::vector <OneInternalEventClass> events;
};

#endif


