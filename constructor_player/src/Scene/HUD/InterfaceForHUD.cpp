/***************************************************************************
InterfaceForHUD.cpp  -  интерфейс для работы с меню (2D)
-------------------
begin                : 25 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./InterfaceForHUD.h"

//конструктор
InterfaceForHUD::InterfaceForHUD(osg::ref_ptr<osg::Group> _root, int width, int height)
{
	root = _root;
	CreateCamera(width, height);
}

InterfaceForHUD::~InterfaceForHUD()
{
	std::cout << "Destructor InterfaceForHUD."  << std::endl;
	Reset();
}

//сброс всего, что на HUD'е
bool InterfaceForHUD::Reset()
{
	return HUDmatrix->removeChild(0, HUDmatrix->getNumChildren());
}


//создание камеры для проекции на экран
void InterfaceForHUD::CreateCamera(int width, int height)
{
	HUDcamera = createHUDCamera (0,width,0,height);
	HUDcamera->setName("__HUDCAMERA__");
	HUDcamera->getOrCreateStateSet()->setMode(GL_LIGHTING,	osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
	HUDcamera->setViewMatrix(osg::Matrix::identity());
	
	//
	root->addChild (HUDcamera.get());
	//
	HUDmatrix = new osg::MatrixTransform();
	HUDmatrix->setName("HUD");
	HUDcamera->addChild (HUDmatrix.get());

	HUDcamera->setNodeMask(0x2);
}



osg::Camera* InterfaceForHUD::createHUDCamera( double left, double right, double bottom, double top )
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;

	camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
	camera->setClearMask( GL_DEPTH_BUFFER_BIT );
	camera->setRenderOrder( osg::Camera::POST_RENDER );
	camera->setAllowEventFocus( false );
	camera->setProjectionMatrix(	osg::Matrix::ortho2D(left, right, bottom, top) );
	//camera->getOrCreateStateSet()->setMode(	GL_LIGHTING, osg::StateAttribute::OFF );
	return camera.release();
}

osg::Node* InterfaceForHUD::get_menu_node()
{
	return HUDmatrix.get();
}
