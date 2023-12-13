#ifndef KEYSCLASS
#define KEYSCLASS

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <iostream>

//класс описания собятий для режима интерпритации мыши
class KeysOperationsClass
{
public:
	//операции (для режима интерпретации мыши)	
	enum KeyOperation
	{
		PUSH,
		RELEASE,
	};
	//тип собятия
	KeyOperation operation;
	//код клавиши
	int keyCode;
        //код клавиши баз обработки (сырой)
        int keyRawCode;
	//модификаторы
	bool SHIFT;
	bool CTRL;
	bool ALT;
	//время  события в секундах
	double time;
};

class KeysEventHandler : public osgGA::GUIEventHandler
{
public:
	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
	//virtual void accept(osgGA::GUIEventHandlerVisitor& v);
	
        //возврат состояния клавиш
	std::vector <KeysOperationsClass> GetKeys();
private:
	std::vector <KeysOperationsClass> operations;
};


#endif


