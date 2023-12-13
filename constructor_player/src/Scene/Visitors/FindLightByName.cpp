/***************************************************************************
FindLightByName.cpp  -  Класс для нахождения источников света
-------------------
begin                : 2 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "FindLightByName.h"


void FindLightVisitor::apply(osg::MatrixTransform& node)
{
    traverse(node);
}

void FindLightVisitor::apply(osg::Group& group) 
{
	traverse(group); 
}  

void FindLightVisitor:: apply(osg::LightSource& ls)
{
	//std::cout << "I find Light source!!!!";
	//ls.getLight()->setDiffuse(osg::Vec4(1,0,0,1));
	//LS = &ls;
	LS.push_back (&ls);
	traverse(ls);
}




