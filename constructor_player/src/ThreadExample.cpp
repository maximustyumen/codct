/*
#include "./ThreadExample.h"


LiftOff::LiftOff(int count, int ident): countDown(count), id (ident) 
{
	quit=false;
}

LiftOff::~LiftOff()
{
	std::cout << id << " complited" << std::endl;
}

void LiftOff::run()
{
	while(quit==false)
	{
		countDown--;
		ZThread::Thread::sleep (1000);
		std::cout << id << " : " << countDown << std::endl;
	}
	std::cout << "Liftoff!" << std::endl;
}
*/


	/*
	LiftOff* newL = new  LiftOff(1000000);
	try
	{
		ZThread::Thread t (newL);	
	}
	catch (ZThread::Synchronization_Exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	...
	newL->quit=true;
	*/
