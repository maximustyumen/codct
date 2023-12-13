#ifndef INTERFACEFORCHUD
#define INTERFACEFORCHUD

#include <iostream>
#include <osgViewer/Viewer>
#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>

class InterfaceForHUD
{	
public:
	//конструктор
	InterfaceForHUD(osg::ref_ptr<osg::Group> _root, int width, int height );
	~InterfaceForHUD();
	//сброс
	bool Reset();

	osg::Node* get_menu_node();
private:
	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;
	//
	//создание камеры для проекции на экран
	osg::Camera* createHUDCamera( double left, double right, double bottom, double top );
	osg::ref_ptr<osg::Camera> HUDcamera;
	osg::ref_ptr<osg::MatrixTransform> HUDmatrix;
	void CreateCamera(int width, int height);
};

#endif