/***************************************************************************
InputKeys.cpp  -  обработчики клавиатуры (нажатые/отпущенные клавиши)
-------------------
begin                : 31 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
// 
// обработчики ввода с клавиатуры (клавиши а не перемещение)
// 

#include "InputKeys.h"

//возврат состояния клавишь
std::vector <KeysOperationsClass> KeysEventHandler::GetKeys()
{
	std::vector <KeysOperationsClass> returnOperations;
	returnOperations.clear();
	for (unsigned int i=0; i < operations.size();i++)
	{
		KeysOperationsClass oneKey;
		oneKey.operation = operations.at(i).operation;
		oneKey.keyCode = operations.at(i).keyCode;
                oneKey.keyRawCode = operations.at(i).keyRawCode;
		oneKey.SHIFT  = operations.at(i).SHIFT;
		oneKey.ALT   = operations.at(i).ALT;
		oneKey.CTRL   = operations.at(i).CTRL;
		oneKey.time    = operations.at(i).time;
		returnOperations.push_back (oneKey);
	}
	operations.clear();
	return returnOperations;
}

//обработчик
bool KeysEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
    switch(ea.getEventType())
    {		
		//нажатие клавиши
		case(osgGA::GUIEventAdapter::KEYDOWN ):
        {
			KeysOperationsClass key;

			key.keyCode = ea.getKey();
                        key.keyRawCode = ea.getKey();

                        //std::cout << "*key = " << key.keyCode << std::endl;

                        //if ((key.keyCode > 96) && (key.keyCode < 123)) key.keyCode -= 32;
			//if ((key.keyCode<0xff00)&&(key.keyCode!=0)&&(key.keyCode!=0x20)) key.keyCode&=0xFFDF;

			key.operation = KeysOperationsClass::PUSH ;
			unsigned int _modeficator = ea.getModKeyMask();
			key.ALT = false;
			key.CTRL = false;
			key.SHIFT = false;

                        unsigned int maskLCtrl = osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL;
                        unsigned int maskRCtrl = osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL;

                        unsigned int maskLAlt = osgGA::GUIEventAdapter::MODKEY_LEFT_ALT;
                        unsigned int maskRAlt = osgGA::GUIEventAdapter::MODKEY_RIGHT_ALT;

                        unsigned int maskLShift = osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT;
                        unsigned int maskRShift = osgGA::GUIEventAdapter::MODKEY_RIGHT_SHIFT;

			if (((_modeficator & maskLCtrl) == maskLCtrl) || ((_modeficator & maskRCtrl) == maskRCtrl)) key.CTRL=true;
			if (((_modeficator & maskLAlt) == maskLAlt) || ((_modeficator & maskRAlt) == maskRAlt)) key.ALT=true;
			if (((_modeficator & maskLShift) == maskLShift) || ((_modeficator & maskRShift) == maskRShift)) key.SHIFT=true;

			//std::cout << ALT << CTRL << SHIFT << std::endl;

			operations.push_back (key);
			return true;
        }
		//отпускание клавиши
		case(osgGA::GUIEventAdapter::KEYUP):
        {
			KeysOperationsClass key;

			key.keyCode = ea.getKey();
                        key.keyRawCode = ea.getKey();

                        //if ((key.keyCode > 96) && (key.keyCode < 123)) key.keyCode -= 32;

			key.operation = KeysOperationsClass::RELEASE ;
			unsigned int _modeficator = ea.getModKeyMask();
			key.ALT = false;
			key.CTRL = false;
			key.SHIFT = false;

                        unsigned int maskLCtrl = osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL;
                        unsigned int maskRCtrl = osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL;

                        unsigned int maskLAlt = osgGA::GUIEventAdapter::MODKEY_LEFT_ALT;
                        unsigned int maskRAlt = osgGA::GUIEventAdapter::MODKEY_RIGHT_ALT;

                        unsigned int maskLShift = osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT;
                        unsigned int maskRShift = osgGA::GUIEventAdapter::MODKEY_RIGHT_SHIFT;

			if (((_modeficator & maskLCtrl) == maskLCtrl) || ((_modeficator & maskRCtrl) == maskRCtrl)) key.CTRL=true;
			if (((_modeficator & maskLAlt) == maskLAlt) || ((_modeficator & maskRAlt) == maskRAlt)) key.ALT=true;
			if (((_modeficator & maskLShift) == maskLShift) || ((_modeficator & maskRShift) == maskRShift)) key.SHIFT=true;

			//std::cout << ALT << CTRL << SHIFT << std::endl;

			operations.push_back (key);
			return true;
        }
	    default:
        return false;		
    }
}

//void KeysEventHandler::accept(osgGA::GUIEventHandlerVisitor& v)
//{
//        v.visit(*this);
//}
