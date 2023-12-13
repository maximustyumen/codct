#ifndef BASE3DOBJECT
#define BASE3DOBJECT


#include <iostream>
#include <osg/PositionAttitudeTransform>

//Класс базового 3D объекта сцены
class Base3DObjectClass
{

public:
	//имя
	std::string Name;
	//модель
	osg::ref_ptr<osg::PositionAttitudeTransform> _Model;

	//координаты точки привязки
	osg::Vec3 CenterPosition;
	//текущая позиция
	osg::Vec3 CurrentPosition;
	//
	bool Visible;
	
	//установка координат точки привязки объекта
	void SetCenterPosition(float x, float y, float z);
	//Установка позиции объекта
	void SetPosition(float x, float y, float z);
	//Установка флага видимости
	void SetVisible(bool value);
	//чтение флага видимости
	bool GetVisible();
	//установка масштабирования
	void SetScale(float dx, float dy, float dz);
	//установка вращения объекта
	void SetRotate(float angle, float x, float y, float z);
	//установка вращения объекта (сложное)
	void SetRotate2(float angle1, float x1, float y1, float z1, float angle2, float x2, float y2, float z2,float angle3, float x3, float y3, float z3);

	//конструктор
	Base3DObjectClass();
	//
	~Base3DObjectClass();

};
#endif