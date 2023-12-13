#ifndef USERFOOTCLASS
#define USERFOOTCLASS

#include <osgUtil/IntersectVisitor>
#include <osgViewer/Viewer>

#include "FootDataClass.h"
#include "HeadDataClass.h"

#include "./events/InternalEventClass.h"

#include "../Configure/Configure.h"

class UserFootClass
{
public:
	//конструктор
	UserFootClass(osg::ref_ptr<osg::Node> _root, float,  Configure* configuration);
	//процедура расчета перемещения пользователя в зависимости от направления вектора взгляда и вектора движения
	void setDataAndCalc(FootData , HeadData);
	//возврат текущих координат ног пользователя по X
	float GetX();
	//возврат текущих координат ног пользователя по Y
	float GetY();
	//возврат текущих координат ног пользователя по Z
	float GetZ();
	//возврат коэффициента "нагибания к объекту" (ZOOM) 1..10
	float GetZOOM();
	//возврат копии буфера накопленных событий во внутреннем формате...
	InternalEventsClass getFootEvents();
	//установка новых координат пользователя
	void SetNewPosition(float,float,float);
private:
	//текущие координаты локального пользователя
	float BodyX,BodyY,BodyZ;
	//предыдущие координаты локального пользователя
	float oldBodyX,oldBodyY,oldBodyZ;
	//принимаемая высота пользователя
	float HumanHeight;
	//указатель на группу сцены
	osg::ref_ptr<osg::Node> root;
	//процедура проверки пересечений при движений
	bool CheckUserIntersects();
	//переменная, определяющая минимальное допустимое расстояние между стеной и поьзователем при движении
	float Distance;
	//события, происходящие при перемещении
	std::vector <InternalEventsClass::OneInternalEventClass> events;
	//
	void AddUserStepEvent(std::string val);
	//состояние приседания/прыжка
	float JumpDown;
	//коэффициент "нагибания к объекту" (ZOOM) 1..10
	float Zoom;
	//коэфф. Эффективности прыжка 1-максимальный барьер 10-минимальный эффект (чем меньше тем выше можно перепрыгнуть объекты)
	float JumpEffect;
	//состояния шатания при ходьбе
	float StepNoise;
	//время в воздухе (при прыжке)
	double timeOfFly;
	double TimeOfFlyLimit;
	//получение имени объекта
	std::string GetName(osgUtil::LineSegmentIntersector::Intersections::iterator hitr);
	std::string TransparentObjectsName;

};

#endif

