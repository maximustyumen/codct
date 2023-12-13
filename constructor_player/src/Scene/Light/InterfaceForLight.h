#ifndef INTERFACEFORLIGHT
#define INTERFACEFORLIGHT

#include <iostream>
#include <osgViewer/Viewer>
#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>

#include "../Visitors/FindNodeByName.h"
#include "../Visitors/FindLightByName.h"

class InterfaceForLight
{	
public:
	//конструктор
	InterfaceForLight(osg::ref_ptr<osg::Group> _root);
	~InterfaceForLight();
	//сброс
	bool Reset();
	//начальная настройка света
	void SetStartLight();
	//устанавливает Diffuse-цвет указанного светового источника
	bool SetLightColor(std::string name, float,float,float);
	//Включает/выключает свет
	bool SetLightEnabled(std::string name, bool);
	//Включает/выключает учет освещения объектом
	//изменять атрибуты, например "объект освещается или нет" (stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);)
	bool SetLightingEnable(std::string name, bool);
private:
	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;
	//Включает/выключает весь свет на сцене
	bool SetAllLightOFF();
	
};

#endif