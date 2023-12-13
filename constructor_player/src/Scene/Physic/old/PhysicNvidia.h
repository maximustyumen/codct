#ifndef PHYSICNVIDIA
#define PHYSICNVIDIA

#include <NxPhysics.h>
#include <stdio.h>
#include <iostream>
#include <osg/MatrixTransform>
#include "../Visitors/FindNodeByName.h"
#include "../Visitors/FindGeometry.h"

class InterfaceForPhysic2
{	
public:
	//конструктор
	InterfaceForPhysic2(osg::Group* _root);
	~InterfaceForPhysic2();

private:
	//указатель на главную группу сцены
	osg::Group* root;
};

#endif