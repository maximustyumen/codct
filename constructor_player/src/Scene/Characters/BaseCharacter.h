#ifndef BASECHARACTER
#define BASECHARACTER

#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgUtil/Optimizer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgSim/OverlayNode>
#include <osgViewer/Viewer>
#include <iostream>


#include <osgCal/CoreModel>
#include <osgCal/Model>



//callback функция для анимации персонажа
class CharecterUpdateCallback : public osg::NodeCallback
{
public:
	CharecterUpdateCallback();
	float currentX;
	float currentY;
	float currentZ;
	float newX;
	float newY;
        float newZ;
	float currentRotation;
	float newRotation;
	bool HappyExecuted;
	// Скорость перемещения (единиц в секунду).
    float pSpeed;
	// Скорость поворота (радиан в секунду).
    float rSpeed;
private:
	virtual void operator() (osg::Node* node, osg::NodeVisitor* nv);
	float oldtime;
};

class BaseCharater
{
public:
	//конструктор
	BaseCharater(std::string CharacterName, std::string filename);
	//Обертка
	osg::ref_ptr<osg::MatrixTransform> Character_MatrixTransform;
	//принудительное перемещение персонажа
	void SetPosition (float x, float y, float z);
	//анимационное перемещение объекта в указанные координаты
	void GoTo(float x,float y, float z);
	//Переключение анимации
	void SetAnimation(int NumAnimation);
	//Прикрепляетобъект к кости
	void ConnectObjectToBone(osg::ref_ptr<osg::Node> node, int boneID);

    
private:
	std::vector< std::string > animationNames;
	osg::ref_ptr<osg::Node> makeModel( osg::ref_ptr<osgCal::CoreModel> cm, osg::ref_ptr<osgCal::BasicMeshAdder> ma );
	osg::ref_ptr<osgCal::Model> model;
	osg::ref_ptr<CharecterUpdateCallback> CharacterAnimation;

};



#endif
