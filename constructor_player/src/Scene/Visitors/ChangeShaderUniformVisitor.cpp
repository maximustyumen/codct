/***************************************************************************
AnalyseGeometryVisitor.cpp  -  Класс для нахождения геометрии в объекте
-------------------
begin                : 21 марта 2023
modeif               : 24 апреля 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/



#include "ChangeShaderUniformVisitor.h"

ChangeShaderUniformVisitor::ChangeShaderUniformVisitor(std::string _uniformname, float _value): osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	setTraversalMask(0xffffffff);
	setNodeMaskOverride(0xffffffff);

	uniformname=_uniformname;
	value=_value;
}

void ChangeShaderUniformVisitor::apply(osg::Node& node)
{
	traverse(node);
}

void ChangeShaderUniformVisitor::apply(osg::Geode& geode)
{
	osg::ref_ptr<osg::Texture2D> texture;
	osg::ref_ptr<osg::Image> image;
	osg::ref_ptr<osg::Material> material;
	osg::ref_ptr<osg::StateSet> stateset;
	osg::ref_ptr<osg::Drawable> drawable;
	osg::ref_ptr<osg::TexEnvCombine>  TexEnvCombine;
	int numberChannels = 0;

	for(unsigned int i=0;i<geode.getNumDrawables();i++) 
    {
		if (geode.getDrawable(i)->getStateSet())
		{
			drawable = geode.getDrawable(i);
			stateset = geode.getDrawable(i)->getStateSet(); 
			
			if (stateset->getAttribute(osg::StateAttribute::PROGRAM))
			{
				//const osg::Program* prog = dynamic_cast<const osg::Program*>(stateset->getAttribute(osg::StateAttribute::PROGRAM));
				// print uniforms of the unit
				for (osg::StateSet::UniformList::const_iterator jt = stateset->getUniformList().begin() ; jt != stateset->getUniformList().end(); jt++)
				{
					if ((jt->second.first->getType() == osg::Uniform::FLOAT)&&(jt->second.first->getName()==uniformname))
					{
						//float fval = -1.0;
						//jt->second.first->get(fval);
						jt->second.first->set(value);
					}
				}
			}

////
		}
	}
}

void ChangeShaderUniformVisitor::apply(osg::Group& group) 
{
	traverse(group); 
}
        
void ChangeShaderUniformVisitor::apply(osg::MatrixTransform& node)
{
      apply((osg::Node&)node);
}








