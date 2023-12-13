/***************************************************************************
FindAndUnRefTexture.cpp  -  Класс для нахождения текстуры и "очистки памяти"
-------------------
begin                : 28 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "FindAndUnrefTexture.h"


void FindAndUnrefTextureVisitor::apply(osg::Node& node)
{
	traverse(node);
}

void FindAndUnrefTextureVisitor::apply(osg::Geode& geode)
{
    for(unsigned int i=0;i<geode.getNumDrawables();i++) //++i
    {
		if (geode.getDrawable(i)->getStateSet())
		{
			//текстура
                        int channel=0;
                        while( texture = dynamic_cast<osg::Texture2D*>(geode.getDrawable(i)->getStateSet()->getTextureAttribute(channel,osg::StateAttribute::TEXTURE)))
                        {
                            ++channel;
                            if (texture.get())
                            {
                                std::cout << "*";
                                texture->setUnRefImageDataAfterApply( false ); //false
                            }
                        }
		}
	}
}

void FindAndUnrefTextureVisitor::apply(osg::Group& group)
{
	traverse(group); 
}
        
void FindAndUnrefTextureVisitor::apply(osg::MatrixTransform& node)
{
      apply((osg::Node&)node);
}

	
