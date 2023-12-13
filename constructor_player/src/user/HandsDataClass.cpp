/***************************************************************************
HandsDataClass.cpp  -  стандартный интерфейс для описания состояния "рук"
-------------------
begin                : 29 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "HandsDataClass.h"


HandsData::HandsData()
{
	mode = NONE;
}

void HandsData::SetMode(HandsMode value)
{
	mode = value;
}

HandsData::HandsMode HandsData::GetMode()
{
	return mode;
}

void HandsData::addOperation (HandsData::InterpretationOperationsClass::InterpretationModeOperations operation, float x, float y, double time)
{
	InterpretationOperationsClass newOperations;
	//
	newOperations.operation = operation;
	newOperations.x = x;
	newOperations.y = y;
	newOperations.time =time;
	//
	operations.push_back (newOperations);
}

std::vector <HandsData::InterpretationOperationsClass> HandsData::getOperations()
{
	return operations;
}





