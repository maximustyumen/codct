/***************************************************************************
InputMouse.cpp  -  Взаимодействие с устр. ввода - мышь
-------------------
begin                : 10 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
// 
// основная подсистема взаимодействия с манипулятором мышь
// 


#include "InputMouse.h"

//
MouseEventHandler::MouseEventHandler( Configure* _configuration)
{

	std::cout << std::endl << "\tMouseEventHandler constructor ... " <<std::endl;

	configuration = _configuration;

	WINDOW_WIDTH = 0;
	WINDOW_HEIGHT = 0;
	HeadX=0;
	HeadY=0;
	dHeadX=0;
	dHeadY=0;
	mousex=0;
	mousey=0;
	

	//режим по умолчанию - просмотр
	viewModeEnable=true;
	viewModeState=true;
	commandModeEnable=true;
	commandModeState=false;
	moveModeEnable=true;
	//пропуск обработки 1 следующего перемещения мыши
	flag_none=true;
	
	// 0.3; //сувствительность мыши 1= 100%
        MouseSens = configuration->GetRecordFloat("#MouseSens");
		
	//инициализация переменной времени
	//LastMouseActionTime = osg::Timer::instance()->time_s();

	//фактор размытия движения головы (1 - нет размытия, до 100)
        effectFactor = configuration->GetRecordFloat("#MouseBlur");


	for (i=0;i<effectFactor;i++)
	{
		statx.push_back (0);
		staty.push_back (0);
	}
}

//установка ориентации головы
void MouseEventHandler::SetHeadOrientation(float x, float y)
{
	HeadX=x;
	HeadY=y;
}

//возврат параметров головы
HeadData MouseEventHandler::GetHeadData()
{
	//плавное движение
	if (i>=effectFactor) i=0;
	statx.at(i)=dHeadX;
	staty.at(i)=dHeadY;
	i++;
	float summDX=0;
	float summDY=0;
	for (int k=0;k<effectFactor;k++)
	{
		summDX+=statx.at(k);
		summDY+=staty.at(k);
	}
    
	if ((viewModeState==true)&&(configuration->GetRecordString("#alternativemousecontrol")!="ON"))
	{
        HeadX +=summDX/(float)effectFactor ;
        HeadY +=summDY/(float)effectFactor ;
		//проверка углов поворота головы
		CheckCorrectAngleHeat();
	}
	if ((viewModeEnable==true)&&(configuration->GetRecordString("#alternativemousecontrol")=="ON"))
	{
        HeadX +=summDX/(float)effectFactor ;
        HeadY +=summDY/(float)effectFactor ;
		//проверка углов поворота головы
		CheckCorrectAngleHeat();
	}

	
	HeadData data;
	data.SetMode (HeadData::ORIENTATION);
	data.SetPitch (HeadY);
	data.SetYaw (HeadX);

    //GAMMER
    dHeadX=0;
    dHeadY=0;

	data.SetRoll(0);
	return data;
}


HandsData MouseEventHandler::GetHandsData()
{
	HandsData data;
	bool ok=false;
	if ((commandModeState==true)&&(viewModeState==false)&&(configuration->GetRecordString("#alternativemousecontrol")!="ON")) ok =true;
	if ((commandModeEnable==true)&&(configuration->GetRecordString("#alternativemousecontrol")=="ON")) ok =true;

	//если режим управления руками, то 
	if (ok)
	{
               // std::cout << "stranno2"  << std::endl;
		data.SetMode (HandsData::INTERPRETATION2D);
		for (unsigned int i=0; i < eventsBuffer.size();i++)
		{
			data.addOperation (eventsBuffer.at(i).operation,eventsBuffer.at(i).x,eventsBuffer.at(i).y,eventsBuffer.at(i).time);
		}
		eventsBuffer.clear();
		//для отслеживание перемещения курсора
		data.addOperation (HandsData::InterpretationOperationsClass::NO,mousex,mousey,osg::Timer::instance()->time_s());
		return data;
	}

	//странная ситуация.... 
    //std::cout << "stranno3"  << std::endl;
	data.SetMode (HandsData::NONE);
	return data;
}


//режим управоения или просмотра 1 - просмотр 2- управление 3- перемещение
void MouseEventHandler::setMode(bool _viewModeEnable, bool _commandModeEnable, bool _moveModeEnable)
{
	viewModeEnable=_viewModeEnable;
	commandModeEnable = _commandModeEnable;
    moveModeEnable=_moveModeEnable;

	//если пользователь в режиме управления, выйти из этого режима
	if ((commandModeEnable==false)&&(commandModeState==true)) 
	{
		commandModeState=false;
		viewModeState=viewModeEnable;
                //std::cout << "ups1" << std::endl;
	}

	//если пользователь в режиме просмотра, а просмотр запрещен, а управление разрешено...
	if ((viewModeEnable==false)&&(commandModeEnable==true)) 
	{
		commandModeState=true;
		viewModeState=false;
	}

    //FIX
    if (viewModeEnable==false)
    {
        viewModeState=false;
    }
    if (commandModeEnable==false)
    {
        commandModeState=false;
    }
    if ((viewModeState==false)&&(commandModeState==false))
    {
        if (viewModeEnable==true)
        {
            viewModeState=true;
            commandModeState=false;
        }
        if (commandModeEnable==true)
        {
            viewModeState=false;
            commandModeState=true;
        }
    }
        //if (viewModeState) std::cout << "viewModeState" << std::endl;
        //if (commandModeState) std::cout << "commandModeState" << std::endl;

}


//Обработка событий мыши
bool MouseEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
	//узнаем размеры экрана	
	WINDOW_WIDTH  = ea.getXmin()+ea.getXmax();
	WINDOW_HEIGHT = ea.getYmin()+ea.getYmax();

	//расчет прошедшего времени (в секундах)
	dTime = osg::Timer::instance()->time_s(); // - LastMouseActionTime;

	//разбор типа события
	switch(ea.getEventType())
        {
		//курсор в окне
		case(osgGA::GUIEventAdapter::FRAME):
		{
			mousex = (float)ea.getX();
			mousey = (float)ea.getY();	
		}


		//Событие перемещение мыши (не важно с нажатыми кнопками или нет, частные случаи рассматриваются дальше)
		case(osgGA::GUIEventAdapter::MOVE):
        {
			if (configuration->GetRecordString("#alternativemousecontrol")!="ON")
			{
				//если режим просмотра, тогда мышь в центр и в принципе - ВСЕ)
				if ((viewModeEnable==true)&&(viewModeState==true))
				{
					//не более 1 выполнения на кадр!
					if (flag_none==true) 
					{
						flag_none=false;
						// us.requestRedraw();
						us.requestWarpPointer (WINDOW_WIDTH/2.0f,WINDOW_HEIGHT/2.0f);
						return true;
					}
					//считаем новое положение поворота головы

					//2 вариант
					if  ((dHeadX==0) && (abs(ea.getX() - (float)WINDOW_WIDTH/2.0f)>1.5))  dHeadX = ((float)ea.getX() - (float)WINDOW_WIDTH/2.0f)  * MouseSens/6.0f;
					if  ((dHeadY==0) && (abs(ea.getY() - (float)WINDOW_HEIGHT/2.0f)>1.5)) dHeadY = ((float)ea.getY() - (float)WINDOW_HEIGHT/2.0f) * MouseSens/6.0f;         
                                

					//мышь в центр (если уже не в центре!)
					if (((float)ea.getX()!=WINDOW_WIDTH/2.0f) || ((float)ea.getY()!=WINDOW_HEIGHT/2.0f))
					{
						// us.requestRedraw();
						us.requestWarpPointer (WINDOW_WIDTH/2.0f,WINDOW_HEIGHT/2.0f);
						//us.requestWarpPointer((ea.getXmin()+ea.getXmax())/2.0f,(ea.getYmin()+ea.getYmax())/2.0f);
					}
					
				}
			}
			else
			{
				float k = 35;
				float mmin =  k ;
				if (ea.getX() < mmin)
				{
					dHeadX = -(mmin  - ea.getX())  * MouseSens/3.0f;
					us.requestWarpPointer (mmin ,mousey);
				}
				float mmax = (float)WINDOW_WIDTH  - k;
				if (ea.getX() > mmax)
				{
					dHeadX = -(mmax  - ea.getX())  * MouseSens/3.0f;
					us.requestWarpPointer (mmax ,mousey);
				}

				mmin =  k ;
				if (ea.getY() < mmin)
				{
					dHeadY = -(mmin  - ea.getY())  * MouseSens/3.0f;
					us.requestWarpPointer (mousex ,mmin);
				}
				mmax = (float)WINDOW_HEIGHT  -  k;
				if (ea.getY() > mmax)
				{
					dHeadY = -(mmax  - ea.getY())  * MouseSens/3.0f;
					us.requestWarpPointer (mousex ,mmax);
				}
			}
			return true;
			break;
		}

		//Колесики)
		case (osgGA::GUIEventAdapter::SCROLL):
		{
			switch (ea.getScrollingMotion())
			{
				case osgGA::GUIEventAdapter::SCROLL_DOWN:
                                {
                                }
                                case osgGA::GUIEventAdapter::SCROLL_UP:
                                {
                                }
                                case osgGA::GUIEventAdapter::SCROLL_NONE:
                                {
                                }
                                case osgGA::GUIEventAdapter::SCROLL_2D:
                                {
                                }
                                case osgGA::GUIEventAdapter::SCROLL_LEFT:
                                {
                                }
                                case osgGA::GUIEventAdapter::SCROLL_RIGHT:
                                {
                                }
			}
		}


		//нажатие кнопок мыши
		case(osgGA::GUIEventAdapter::PUSH):
		{
			//уточняем, какая клавиша мыши нажата
			switch (ea.getButton())
			{
				//правая кнопка отвечает за переключение режима работы курсора (навигация/управление)
				case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
				{
					//переключение из режима просмотра в режим управления
					if ((viewModeState==true)&&(commandModeEnable==true))
					{
                                                //std::cout << "oh1" << std::endl;
						commandModeState=true;
						viewModeState=false;
						//не обрабатываем следующее перемещение мыши, а курсор в центр
						flag_none=true;
						break;
					}
					//переключение из режима управления в режим просмотра
					if ((commandModeState==true)&&(viewModeEnable==true))
					{
						//
						commandModeState=false;
						viewModeState=true;
						//не обрабатываем следующее перемещение мыши, а курсор в центр
						flag_none=true;
						//???Генерируем отпускание объекта
						//сбрасываем буфер смаза движения головы
						for (int k=0;k<effectFactor;k++)
						{
							statx.at(k)=0;
							staty.at(k)=0;
						}
						break;
					}
					break;
				}


				//средняя кнопка отвечает за использование объекта
				case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON:
				{
					bool ok = false;
					if ((configuration->GetRecordString("#alternativemousecontrol")!="ON") && (commandModeState==true)) ok = true;
					if ((configuration->GetRecordString("#alternativemousecontrol")=="ON") && (commandModeEnable==true)) ok = true;
					if (ok)
					{
						HandsData::InterpretationOperationsClass operation;
						operation.operation = HandsData::InterpretationOperationsClass::USE;
						operation.x = (float)ea.getX();
						operation.y = (float)ea.getY();
						operation.time = osg::Timer::instance()->time_s();
						eventsBuffer.push_back (operation);
					}
					break;
				}
				//Левая кнопка отвечает за ... функцию обработки действий пользователя при нажатии...
				case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON :
				{
					bool ok = false;
					if ((configuration->GetRecordString("#alternativemousecontrol")!="ON") && (commandModeState==true)) ok = true;
					if ((configuration->GetRecordString("#alternativemousecontrol")=="ON") && (commandModeEnable==true)) ok = true;
					if (ok)
					{
						HandsData::InterpretationOperationsClass operation;
						operation.operation = HandsData::InterpretationOperationsClass::PUSH;
						operation.x = (float)ea.getX();
						operation.y = (float)ea.getY();
						operation.time = osg::Timer::instance()->time_s();
						eventsBuffer.push_back (operation);
					}
					break;
				}
			}
			return true; //эта строка не допускает вызова DRAG при первом PUSH
		}


		//перемещение мыши с нажатой кнопкой (перетаскивание)
		case(osgGA::GUIEventAdapter::DRAG):
		{
			//если мышь "перемещают" с нажатой левой кнопкой, то ...
			if  (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
			{
				bool ok = false;
				if ((configuration->GetRecordString("#alternativemousecontrol")!="ON") && (commandModeState==true)) ok = true;
				if ((configuration->GetRecordString("#alternativemousecontrol")=="ON") && (commandModeEnable==true)) ok = true;
				if (ok)
				{
					HandsData::InterpretationOperationsClass operation;
					operation.operation = HandsData::InterpretationOperationsClass::DRAG;
					operation.x = (float)ea.getX();
					operation.y = (float)ea.getY();
					operation.time = osg::Timer::instance()->time_s();
					eventsBuffer.push_back (operation);
				}
			}
			break;
		}

		//отпускание клавиш мыши...
		case(osgGA::GUIEventAdapter::RELEASE):
		{
			//если клавишу кнопку отпускают , то ...
			if  (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
			{
				bool ok = false;
				if ((configuration->GetRecordString("#alternativemousecontrol")!="ON") && (commandModeState==true)) ok = true;
				if ((configuration->GetRecordString("#alternativemousecontrol")=="ON") && (commandModeEnable==true)) ok = true;
				if (ok)
				{
					//если НЕ режим курсора, ничего не делаем !
					//if (Scene.GetCursorMode()==false) break;
					HandsData::InterpretationOperationsClass operation;
					operation.operation = HandsData::InterpretationOperationsClass::RELEASE;
					operation.x = (float)ea.getX();
					operation.y = (float)ea.getY();
					operation.time = osg::Timer::instance()->time_s();
					eventsBuffer.push_back (operation);
				}
			}
			break;
		}

		
		

		default:
		return false;
	}
	return false;
}

//void MouseEventHandler::accept(osgGA::GUIEventHandlerVisitor& v)
//{
//	v.visit(*this);
//}


//проверяем допустимые повороты головы (вверх/вниз)
void MouseEventHandler::CheckCorrectAngleHeat()
{
	if (HeadY <-75) HeadY = -75;
	if (HeadY >65) HeadY = 65;
}



