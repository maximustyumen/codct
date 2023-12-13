#ifndef INTERFACEFOROLD
#define INTERFACEFOROLD

#include <iostream>
#include <osgViewer/Viewer>
#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>


//OLD - поддержка
#include "./Base3DObject.h"

class InterfaceForOLD
{	
public:
	//конструктор
	InterfaceForOLD(osg::ref_ptr<osg::Group> _root);
	~InterfaceForOLD();
	//сброс
	bool Reset();

	//добавление нового объекта
	void NewObject(std::string Name, std::string FileName);
	//получение OSG объекта
	osg::ref_ptr<osg::PositionAttitudeTransform> GetModel(std::string Name);
	//получение Base3DObjectClass
	Base3DObjectClass* GetObject(std::string Name); 
	//аccоциативный список - контейнер 
	std::map <std::string , Base3DObjectClass*> objects;
private:
	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;
	//вспомогательная функция, выполняет загрузку объектов (EASY)
        void LoadAnimModel(osg::ref_ptr<osg::PositionAttitudeTransform> object, std::string FileName);
};

#endif
