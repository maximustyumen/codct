/***************************************************************************
FindAnimationByName.cpp  -  Класс для нахождения анимации указанного обыекта
-------------------
begin                : 22 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "FindAnimationByName.h"


void FindAnimationVisitor::apply(osg::Transform& node)
{
    //apc = dynamic_cast<osg::AnimationPathCallback*>(node.getUpdateCallback());
	osg::ref_ptr<osg::AnimationPathCallback> test = dynamic_cast<osg::AnimationPathCallback*>(node.getUpdateCallback());
    if (test.get())
    {
		//std::cout << "I find animation!!!!";
		//Запоминаем анимацию
		_foundNodes.push_back (dynamic_cast<osg::AnimationPathCallback*>(node.getUpdateCallback()));
		//и объект к которому она прикреплена
		_foundTransforms.push_back (&node);
    }
    traverse(node);
}

void FindAnimationVisitor::apply(osg::Group& group) 
{
	traverse(group); 
}  

/*
//????
void FindAnimationVisitor::apply(osg::MatrixTransform& node)
{
      apply((osg::Transform&)node);
}

//?????
void FindAnimationVisitor::apply(osg::AnimationPathCallback& node)
{
	std::cout << "I find animation 2" << std::endl; 
}
*/