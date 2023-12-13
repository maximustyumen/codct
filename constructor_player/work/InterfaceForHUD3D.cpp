/***************************************************************************
InterfaceForHUD.cpp  -  интерфейс для работы с меню (2D)
-------------------
begin                : 25 марта 2008
copyright            : (C) 2008 by Гаммер Максим Дмитриевич (maximum2000)
email                : GammerMax@yandex.ru
***************************************************************************/

#include "InterfaceForHUD3D.h"
#include <osg/Camera>

//#include <osgDB/ReadFile>

InterfaceForHUD3D::InterfaceForHUD3D(osg::Group* root)
: m_node(new osg::MatrixTransform())
{
	m_node->setName("HUD3D");
	//m_node->addChild(osgDB::readNodeFile("menu3d.osgt"));

	root->addChild(m_node);

	osg::StateSet* stateset = m_node->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	

	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	stateset->setRenderBinDetails( 100, "RenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);

	//ТОЖЕ САМОЕ мЫ ДОЛЖНЫ СДЕЛАТЬ ДЛЯ ВСЕЙ ГЕОМЕТРИИ КОТОРАЯ ОКАЗЫВАЕТСЯ В ЭТОМ МЕНЮ!
}
/// обновить положение 3D худа
void InterfaceForHUD3D::update(osg::Camera* camera)
{
	//m_node->setMatrix(osg::Matrixd::inverse(camera->getViewMatrix()));
	m_node->setMatrix(camera->getInverseViewMatrix());
}
