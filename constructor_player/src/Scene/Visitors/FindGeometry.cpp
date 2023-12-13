/***************************************************************************
FindGeometry.cpp  -  Класс для нахождения геометрии в объекте
-------------------
begin                : 21 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/



#include "FindGeometry.h"


void FindGeometryVisitor::apply(osg::Node& node)
{
	traverse(node);
}

void FindGeometryVisitor::apply(osg::Geode& geode)
{
    for(unsigned int i=0;i<geode.getNumDrawables();++i)
    {
		if (geode.getDrawable(i)->getStateSet())
		{
			osg::ref_ptr<osg::Geometry> test = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
			if (test.get())
			{
				geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
				// Changing these flags will tickle different cases in 
				// Geometry::drawImplementation. They should all work fine 
				// with the shared array. Setting VertexIndices will hit 
				// some other cases.
                                geom->setUseVertexBufferObjects(false);
                                geom->setUseDisplayList(false);
                                // geom->setFastPathHint(false);
				return;
			}
		}
    }
}

void FindGeometryVisitor::apply(osg::Group& group) 
{
	traverse(group); 
}
        
void FindGeometryVisitor::apply(osg::MatrixTransform& node)
{
	apply((osg::Node&)node);
}

