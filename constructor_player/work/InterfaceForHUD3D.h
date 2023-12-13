#ifndef INTERFACEFORCHUD3D
#define INTERFACEFORCHUD3D

#include <osg/MatrixTransform>

class InterfaceForHUD3D
{	
private:
	osg::ref_ptr<osg::MatrixTransform> m_node;
public:
	InterfaceForHUD3D(osg::Group* root);
public:
	/// обновить положение 3D худа
	void update(osg::Camera* camera);
};

#endif