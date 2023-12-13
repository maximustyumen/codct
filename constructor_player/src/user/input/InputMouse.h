#ifndef MOUSECLASS
#define MOUSECLASS

#include "../../Configure/Configure.h"

#include <osgGA/GUIEventHandler>
#include <osg/Timer>
#include <osgViewer/Viewer>

#include <iostream>
#include "../HeadDataClass.h"
#include "../HandsDataClass.h"

class MouseEventHandler : public osgGA::GUIEventHandler
{
public:
	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
//	virtual void accept(osgGA::GUIEventHandlerVisitor& v);
	//конструктор	 
	MouseEventHandler( Configure* configuration); 
	//режим управоения или просмотра 1 - просмотр 2- управление 3- перемещение
	void setMode(bool, bool, bool);
        //установка углов головы
        void SetHeadOrientation(float,float);
	//возврат параметров головы
	HeadData GetHeadData();
	//возврат параметров руки
	HandsData GetHandsData();

private:
	//флаг разрешения режима просмотра (изменения направления взгляда)
	bool viewModeEnable;
	//флаг, если тру, то обработчик находится в режиме просмотра
	bool viewModeState;
	//флаг разрешения режима управления
	bool commandModeEnable;
	//флаг, если тру, то обработчик находится в режиме управления
	bool commandModeState;
	//флаг разрешения обработки перемещения пользователя с помощью прокрутки мыши
	bool moveModeEnable;
	//
	float HeadX, HeadY;
	float dHeadX, dHeadY;
	//
	std::vector <float> statx;
	std::vector <float> staty;
	int i;
	int effectFactor; //1..100...1000

	//ширина и высота экрана
	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;
	//параметры мыши
	//чувствительность 1% до 500% (по умолчанию 100%)
	float MouseSens;
	//время последнего события в секундах
	double dTime;
	//флаг пропуска обработки 1 следующего перемещения мыши
	bool flag_none;
	//процедура проверки допустимых углов поворота головы
	void CheckCorrectAngleHeat();
	
	//временный буфер событий
	std::vector <HandsData::InterpretationOperationsClass> eventsBuffer;
	//текущие координаты мыши
	float mousex,mousey;


	Configure* configuration;
};

#endif

