#ifndef HANDSDATACLASS
#define HANDSDATACLASS

#include <stdlib.h>
#include <vector> 

class HandsData
{
public:
	//конструктор
	HandsData();

	//режимы
	enum HandsMode
	{
		NONE,					//режим не определен, данные не достоверны
		INTERPRETATION2D,		//режим интерпритации экранных координат (2D-3D) - мышь
		TRACKER					//режим абсолютных 3D координат (трекер) - 3D
	};
	void SetMode(HandsMode);
	HandsMode GetMode();

	//класс описания собятий для режима интерпритации мыши
	class InterpretationOperationsClass
	{
	public:
		//операции (для режима интерпретации мыши)	
		enum InterpretationModeOperations 
		{
			NO,
			PUSH,
			DRAG,
			RELEASE,
                        USE
		};
		//тип собятия
		InterpretationModeOperations operation;
		//координата курсора по x и y
		float x;
		float y;
		//время  события в секундах
		double time;
	};
	void addOperation (InterpretationOperationsClass::InterpretationModeOperations operation, float x, float y, double time);
	std::vector <InterpretationOperationsClass> getOperations();

private:
	std::vector <InterpretationOperationsClass> operations;
	HandsMode mode;
};



#endif
	


