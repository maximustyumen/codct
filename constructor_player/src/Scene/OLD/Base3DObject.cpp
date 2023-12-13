/***************************************************************************
Base3DObject.cpp  -  Проект виртуальной лаборатории
-------------------
begin                : 1 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
//Определение базового объекта 3D сцены
#include "Base3DObject.h"

//конструктор
Base3DObjectClass::Base3DObjectClass()
{
	_Model = new osg::PositionAttitudeTransform();
	Visible=true;
}

Base3DObjectClass::~Base3DObjectClass()
{
	std::cout << "Destructor Base3DObjectClass."  << std::endl;
}


//установка координат точки привязки объекта
void Base3DObjectClass::SetCenterPosition(float x, float y, float z)
{
	CenterPosition = osg::Vec3(x,y,z);
}

//Установка позиции объекта
void Base3DObjectClass::SetPosition(float x, float y, float z)
{
	CurrentPosition=osg::Vec3(x,y,z);
	_Model->setPosition(CurrentPosition);
	_Model->setPivotPoint(CenterPosition);
	_Model->setPosition(CurrentPosition);
}

//Установка флага видимости
void Base3DObjectClass::SetVisible(bool value)
{
	Visible = value;
	if (!Visible)	
		{
		_Model->setNodeMask (0x000000);
		}
	else
		{
		_Model->setNodeMask (0xffffff);
		}
}

//чтение флага видимости
bool Base3DObjectClass::GetVisible()
{
	return Visible;
}

//Масштабирование
void Base3DObjectClass::SetScale(float dx, float dy, float dz)
{
	_Model->setScale(osg::Vec3d(dx,dy,dz));
}

//Вращение
void Base3DObjectClass::SetRotate(float angle, float x, float y, float z)
{
	_Model->setPosition(CurrentPosition);
	_Model->setPivotPoint(CenterPosition);
	_Model->setAttitude( osg::Quat(angle,osg::Vec3(x,y,z)));
	_Model->setPosition(CurrentPosition);
}

//Вращение сложное
void Base3DObjectClass::SetRotate2(float angle1, float x1, float y1, float z1, float angle2, float x2, float y2, float z2,float angle3, float x3, float y3, float z3)
{
	_Model->setPosition(CurrentPosition);
	_Model->setPivotPoint(CenterPosition);
	_Model->setAttitude(osg::Quat(angle1,osg::Vec3d(x1,y1,z1),angle2,osg::Vec3d(x2,y2,z2),angle3,osg::Vec3d(x3,y3,z3)));
	_Model->setPosition(CurrentPosition);
}

