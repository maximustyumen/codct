/***************************************************************************
UseFoot.cpp  -  класс расчета всего, что связано с передвижением локального персонажа
-------------------
begin                : 1 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "UserFoot.h"

//константа PI/180
const float piover180 = 0.0174532925f;

UserFootClass::UserFootClass(osg::ref_ptr<osg::Node> _root, float _HumanHeight,  Configure* configuration)
{
	std::cout << std::endl << "UserFootClass constructor ... " <<std::endl;

	//ссылка на геометрию сцены для обработки столкновений
	root = _root;
	//
	oldBodyX=0;
	oldBodyY=0;
	oldBodyZ=_HumanHeight;
	BodyX=0;
	BodyY=0;
	BodyZ=_HumanHeight;
	//
	HumanHeight = _HumanHeight;
	//
	Distance = 10.0f;

	if (configuration->GetRecordString("#HTC_VIVE_ON") == "ON") Distance = 8.0f;

	//
	TimeOfFlyLimit = 0.1f; // через 0.3 секунды при прыжке - опускаемся
	//
	StepNoise=1.0f;
	//
	JumpEffect=3.0f;
        JumpDown = 0;
	//
	TransparentObjectsName="AIR";//"AIR";
	//
	Zoom=1.0f;
}

//установка новых координат пользователя
void UserFootClass::SetNewPosition(float x,float y,float z)
{
	oldBodyX=x;
	oldBodyY=y;
	oldBodyZ=z;
	BodyX=x;
	BodyY=y;
	BodyZ=z;
	if (BodyZ==0.0f) BodyZ = HumanHeight;
}


//возврат текущих координат ног пользователя по X
float UserFootClass::GetX()
{
	return BodyX;
}
//возврат текущих координат ног пользователя по Y
float UserFootClass::GetY()
{
	return BodyY;
}
//возврат текущих координат ног пользователя по Z
float UserFootClass::GetZ()
{
	return BodyZ+JumpDown+StepNoise;
}
//возврат коэффициента "нагибания к объекту" (ZOOM) 1..10
float UserFootClass::GetZOOM()
{
	return Zoom;
}

//процедура расчета перемещения пользователя в зависимости от направления вектора взгляда и вектора движения
void UserFootClass::setDataAndCalc(FootData foot, HeadData head)
{
	static double stepcount=0;
	//Рассчитываем dT
	static double deltaT=0;
	{
		static osg::Timer_t old_tick = osg::Timer::instance()->tick();
		osg::Timer_t current_tick = osg::Timer::instance()->tick();
		// за последнюю итерацию прошло   
		deltaT = osg::Timer::instance()->delta_s(old_tick,current_tick);
		old_tick=current_tick;
                //корректировка при низком FPS чтобы пользователь не дергался слишком резко
                if (deltaT>(1.0f/20.0f)) deltaT = 1.0f/20.0f;
	}
	//Обработка шатания при ходьбе
	if (foot.GetMode()==FootData::VECTORS )
	{
		if ((foot.getVectorX() != 0)||(foot.getVectorY() != 0))
		{
			stepcount+=deltaT;
			StepNoise = sin (stepcount*12)*HumanHeight/30.0f; //"высота шага"
		}
		else
		{
			if (StepNoise>0.01) StepNoise -= 0.01f;
			if (StepNoise<-0.01) StepNoise += 0.01f;
			if (abs(StepNoise)<0.02) StepNoise = 0;
			stepcount=0;
		}
		
	}

	//Обработка ZOOM'а
        if (foot.GetMode()==FootData::VECTORS )
	{
		if (foot.GetZoom() == true)
		{
			Zoom += 4.0f*deltaT; 
		}
		else
		{
			Zoom -= 10.0f*deltaT; 
		}
		//если пошевилились
		if ((foot.getVectorX()!=0)|| (foot.getVectorY()!=0)) //|| (foot.getVectorZ()!=0)
		{
			Zoom -= 10.0f*deltaT; 
		}
		//если зашкалили, то
		if (Zoom > 3.0f)
		{
			Zoom = 3.0f;
		}
		if (Zoom < 1.0f)
		{
			Zoom = 1.0f; 
		}

		
	}

	//Обработка присесть - прыгнуть
	if (foot.GetMode()==FootData::VECTORS )
	{
		//!!foot.getVectorZ  -1 - присесть 1 - прыгнуть
		//Обработка присесть - прыгнуть
		if ((foot.getVectorZ() != 0)&&(timeOfFly<TimeOfFlyLimit))
		{
			if (foot.getVectorZ()==1) JumpDown+=HumanHeight*3.0f*deltaT; 
			if (foot.getVectorZ()==-1) JumpDown+=HumanHeight*-2.0f*deltaT; 
		}
		//если зашкалили, то
		if (JumpDown >= HumanHeight/2.0f)
		{
			JumpDown=HumanHeight/2.0f; 
			timeOfFly += deltaT; 
		}
		if (JumpDown < -HumanHeight/4.0f)
		{
			JumpDown=-HumanHeight/4.0f; 
		}
		//возврат в нормальное состояние
		if (((foot.getVectorZ() == 0)&&(JumpDown>0))||(timeOfFly>TimeOfFlyLimit))
		{
			JumpDown-=HumanHeight*1.25f*deltaT;
			if (JumpDown<=0) 
			{
				JumpDown = 0;
				timeOfFly = 0; 				
			}
		}
		//возврат в нормальное состояние
		if ((foot.getVectorZ() == 0)&&(JumpDown<0))
		{
			JumpDown+=HumanHeight*1.25f*deltaT;
			if (JumpDown>0) JumpDown = 0;
		}
	}
	//
	//Обработка движения...
	///////////////////////////////////////////////////////////////////////////////////////////
	float HeadX = head.GetYaw();
	float dX = foot.getVectorX();
	float dY = foot.getVectorY();
        //float dZ = foot.getVectorZ();
	float MoveVectorX=0;
	float MoveVectorY=0;
	float MoveVectorZ=0;
	//Движение вперед-назад
	if (dY!=0)
	{
			MoveVectorX += dY * (float)sin(HeadX*piover180) * 60.0f * deltaT;
			MoveVectorY += dY* (float)cos(HeadX*piover180) * 60.0f * deltaT;
	}
	//Стрейф
	if (dX!=0)
	{
			MoveVectorX += dX * (float)sin( 90.0f*piover180 + HeadX*piover180) * 60.0f * deltaT;
			MoveVectorY += dX * (float)cos( 90.0f*piover180 + HeadX*piover180) * 60.0f * deltaT;
	}
	///////////////////////////////////////////////////////////////////////////////////////////

	oldBodyX = BodyX;
	oldBodyY = BodyY;
	oldBodyZ = BodyZ;
	//рассчитываем желаемые
	BodyX = BodyX + MoveVectorX;
	BodyY = BodyY + MoveVectorY;
	BodyZ = BodyZ + MoveVectorZ;
	//проверяем возможность передвижения
	if (CheckUserIntersects()==false)
	{
		//сбрасываем изменение высоты при ходьбе
		StepNoise=0;
		stepcount=0;
		//уперлись в стену, восстанавливаем текущие координаты пользователя
		BodyX = oldBodyX;
		BodyY = oldBodyY;
		BodyZ = oldBodyZ;
	}
}

//функция проверки возможности перемещения пользователя
bool UserFootClass::CheckUserIntersects()
{
	//Вычисляем dT
	//Рассчитываем dT
	static double deltaT=0;
	{
		static osg::Timer_t old_tick = osg::Timer::instance()->tick();
		osg::Timer_t current_tick = osg::Timer::instance()->tick();
		// за последнюю итерацию прошло   
		deltaT = osg::Timer::instance()->delta_s(old_tick,current_tick);
		old_tick=current_tick;
                //корректировка при низком FPS чтобы пользователь не провалился нахрен и не дергался слишком резко
                if (deltaT>(1.0f/20.0f)) deltaT = 1.0f/20.0f;
	}

	//1. Определяем высоту над новую объектом (обработка пересеченеия с той поверхностью, которая под ногами)
	{
		//максимальная/минимальная зафиксированная высота под наблюдателем
		float maxZ=0;
		float minZ=-5;
		//определение высоты над объектом 1 лучем вниз
		osg::ref_ptr<osgUtil::LineSegmentIntersector> terrainIsect;
		//создание сегмента линии
		terrainIsect = new osgUtil::LineSegmentIntersector(
				osg::Vec3f(BodyX, BodyY, BodyZ +0) ,
				osg::Vec3f(BodyX, BodyY, BodyZ -HumanHeight*1.2) );
		//включение сегмента в IntersectVisitor
		osgUtil::IntersectionVisitor isectVisitor( terrainIsect.get() );

		// проверка на пересечение с ROOM / или со всеми объектами сцены

		root->accept(isectVisitor);
		//получение объектов, пересеченных сегментом линии с индексом 0
		osgUtil::LineSegmentIntersector::Intersections hitList = terrainIsect->getIntersections();
		//перебор объектов ( начиная с самого ближнего по пересечению и  заканчивая самим дальним)
		int temp_loops=0;
		std::string _ObjectName="";
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr=hitList.begin();
			hitr!=hitList.end();
			hitr++)
			{
				osg::Vec3d shrubPosition = hitr->getWorldIntersectPoint();
				if (maxZ < shrubPosition.z()) maxZ = shrubPosition.z();
				if (minZ > shrubPosition.z()) minZ = shrubPosition.z();
				//
				_ObjectName = GetName(hitr);
				//"прозрачные" объекты
				if (_ObjectName.find(TransparentObjectsName)!=std::string::npos) 
				{
					continue;
				}
				else
				{
					temp_loops++;
				}
			}
			//новая высота
			//если под нами что-то есть, тогда 
                        if (temp_loops>0)
			{
				BodyZ= HumanHeight + maxZ;
				//если слишком высоко - то находимся на той-же высоте и на тай-же позиции
				if (sqrt((BodyZ-oldBodyZ)*(BodyZ-oldBodyZ))>HumanHeight/4.0f+JumpDown/JumpEffect) 
				{
					//BodyZ = oldBodyZ;
					return false;
					//delete terrainIsect[0];
				}
				//PhysicON=false;
			}
			else
			{
				//падаем
				BodyZ-=100.0f*deltaT;						
				//PhysicON=true;
			}
			//передаем на чем мы стоим в соответствующее место
			if (_ObjectName!="")
			{
				AddUserStepEvent(_ObjectName); 
			}
			//walkbias=walkbias+JumpWalkbias+DownWalkbias;   //ВОТ !!!!
			//delete[] terrainIsect;
	}
	// проверка новой высоты закончена

	//2. определение пересечений сцены с объектом 2 лучеми по XY
	{
   		
		osg::ref_ptr <osgUtil::LineSegmentIntersector> terrainIsect1; //для X и Y
		//если перемещения небыло, заканчиваем, иначе получим неверный вектор
		if ((BodyX==oldBodyX)&&(BodyY==oldBodyY)) return true;
		// нахождение продолжения вектора, для выдержки дистанции до объектов (см. файл MathCad)
		// ОПТИМИЗИРОВАТЬ!!!! 
		float k =  
			(sqrt((BodyY - oldBodyY)*(BodyY - oldBodyY)))
			/
			(sqrt(  (BodyX-oldBodyX)*(BodyX-oldBodyX)  +  (BodyY - oldBodyY)*(BodyY - oldBodyY)));
		float alfa = asin(k);
		float R = sqrt(  (BodyX-oldBodyX)*(BodyX-oldBodyX)  +  (BodyY - oldBodyY)*(BodyY - oldBodyY));
		float new_BodyX;
		if (BodyX <= oldBodyX )	
		{
			new_BodyX	= oldBodyX-cos(alfa)*(R+Distance);
		}
		else
		{
			new_BodyX	= oldBodyX+cos(alfa)*(R+Distance);
		}
		float new_BodyY;
		if (BodyY <= oldBodyY )	
		{
			new_BodyY	 = oldBodyY-sin(alfa)*(R+Distance);
		}
		else
		{
			new_BodyY	 = oldBodyY+sin(alfa)*(R+Distance);
		}
		//создание сегмента линии - по направлению движения
		terrainIsect1 = new osgUtil::LineSegmentIntersector(
				osg::Vec3f(oldBodyX, oldBodyY, BodyZ-HumanHeight/2.0f) ,
				osg::Vec3f(new_BodyX, new_BodyY, BodyZ-HumanHeight/2.0f)); 
		//включение сегмента в IntersectVisitor
		osgUtil::IntersectionVisitor checkVisitor( terrainIsect1.get() );
		// пересечение с задаными объектами по X Y Z
		root->accept(checkVisitor);
		osgUtil::LineSegmentIntersector::Intersections hitList = terrainIsect1->getIntersections();
		std::string _ObjectName="";
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr=hitList.begin();
				hitr!=hitList.end();
				hitr++)
				{
					//
					_ObjectName = GetName(hitr);
					//std::cout << _ObjectName << std::endl;
					//"прозрачные" объекты
					if (_ObjectName.find(TransparentObjectsName)!=std::string::npos) 
					{
						//прозрачный объект
						continue;
					}
					else
					{
						//есть пересечения, проход закрыт
						return false;
						//delete [] terrainIsect1;
					}
				}

		//еще одно пересечение, для того чтобы не подходить близко к объектам
		{
			//создание сегмента линии
			terrainIsect1 = new osgUtil::LineSegmentIntersector(
					osg::Vec3f(new_BodyX, new_BodyY, BodyZ+JumpDown) ,
					osg::Vec3f(new_BodyX, new_BodyY, BodyZ - HumanHeight*0.6+JumpDown/JumpEffect)); 
			
			//включение сегмента в IntersectVisitor
			osgUtil::IntersectionVisitor checkVisitor( terrainIsect1.get() );
			//пересечение с задаными объектами по X Y Z
			root->accept(checkVisitor);
			osgUtil::LineSegmentIntersector::Intersections hitList = terrainIsect1->getIntersections();
			std::string _ObjectName="";
			for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr=hitList.begin();
				hitr!=hitList.end();
				hitr++)
				{
					//
					_ObjectName = GetName(hitr);
					//"прозрачные" объекты
					if (_ObjectName.find(TransparentObjectsName)!=std::string::npos) 
					{
						//прозрачный объект
						continue;
					}
					else
					{
						//есть пересечения, проход закрыт
						return false;
						//delete [] terrainIsect1;
					}
				}
				//пересечений нет, можно двигаться
				return true;
				//delete [] terrainIsect1;
		}
	}

}


//возврат копии буфера накопленных событий во внутреннем формате...
InternalEventsClass UserFootClass::getFootEvents()
{
	InternalEventsClass data;
	for (unsigned int i=0; i < events.size();i++)
	{
		data.addEvent (events.at(i));
	}
	events.clear();
	return data;
}

//Событие "наступания" на какой-либо объект
void UserFootClass::AddUserStepEvent(std::string val)
{
	
	static double deltaT=0;
	//Расчет FPS
	static osg::Timer_t old_tick = osg::Timer::instance()->tick();
	osg::Timer_t current_tick = osg::Timer::instance()->tick();
	// за последнюю итерацию прошло    osg::Timer::instance()->delta_s(old_tick,current_tick);
	deltaT += osg::Timer::instance()->delta_s(old_tick,current_tick);
	old_tick=current_tick;
	if (deltaT > 0.5f)
	{
		deltaT-=0.5f;
	}
	else
	{
		return;
	}
	

	InternalEventsClass::OneInternalEventClass temp;
	temp.Event = val + (std::string)"_STEP";
	temp.time = osg::Timer::instance()->time_s();
	events.push_back (temp);
}

//получение имени объекта
std::string UserFootClass::GetName(osgUtil::LineSegmentIntersector::Intersections::iterator hitr)
{
	std::string CurrentObjectName="";
    //определяем имя
    CurrentObjectName = hitr->drawable->getName();

	if (CurrentObjectName=="")
	{
		if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
		{
			// the geodes are identified by name.
			CurrentObjectName = hitr->nodePath.back()->getName();
		}
		else if (hitr->drawable.valid())
		{
			CurrentObjectName =  hitr->drawable->getName();
		}
	}

   
    if (CurrentObjectName=="")
    {
        CurrentObjectName = hitr->drawable->getParent(0)->getName();
    }
    if (CurrentObjectName=="")
    {
	  // osg::NodePath::const_iterator hitNodeIt = hitr->nodePath.begin();
      // CurrentObjectName = (*hitNodeIt)->getParent(0)->getParent(0)->getName();

		CurrentObjectName = hitr->nodePath.back()->getParent(0)->getParent(0)->getName();
    }
    if (CurrentObjectName=="")
    {
        CurrentObjectName = hitr->drawable->getParent(0)->getParent(0)->getName();
    }
    return CurrentObjectName;

	/*
	std::string CurrentObjectName="";
	//определяем имя
	CurrentObjectName = hitr->_geode->getName();
	if (CurrentObjectName=="")
	{
		CurrentObjectName = hitr->_geode->getParent(0)->getName();
	}
	if (CurrentObjectName=="")
	{
		CurrentObjectName = hitr->_drawable->getName();
	}
	if (CurrentObjectName=="")
	{
		CurrentObjectName = hitr->_drawable->getParent(0)->getName();
	}
	if (CurrentObjectName=="")
	{
		CurrentObjectName = hitr->_geode->getParent(0)->getParent(0)->getName();
	}
	if (CurrentObjectName=="")
	{
		CurrentObjectName = hitr->_drawable->getParent(0)->getParent(0)->getName();
	}
	return CurrentObjectName;
	*/
}
