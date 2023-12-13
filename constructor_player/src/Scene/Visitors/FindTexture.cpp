/***************************************************************************
FindTexture.cpp  -  Класс для нахождения текстуры и цвета материала указанного обыекта
-------------------
begin                : 15 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "FindTexture.h"


void FindTextureVisitor::apply(osg::Node& node)
{
	traverse(node);
}

void FindTextureVisitor::apply(osg::Geode& geode)
{
	for(unsigned int i=0;i<geode.getNumDrawables();i++) //++i 
    {
		if (Sucessful==true) return;
		//std::cout << "+";
		if (geode.getDrawable(i)->getStateSet())
		{
			drawable = geode.getDrawable(i);
			//текстура
			stateset = geode.getDrawable(i)->getStateSet(); //getOrCreateStateSet(); 
			texture = dynamic_cast<osg::Texture2D*>(geode.getDrawable(i)->getStateSet()->getTextureAttribute(Channel,osg::StateAttribute::TEXTURE));
			if (texture.get()) 
			{
				image = texture->getImage();
				Sucessful=true;
			}
			else
			{
				image=NULL;
			}
			//размер  и т.д.
			//цвет
			//material->setDiffuse(osg::Material::FRONT_AND_BACK, color);
			//stateset->setAttributeAndModes(material, osg::StateAttribute::ON);

			material = dynamic_cast<osg::Material*>(geode.getDrawable(i)->getStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
			if (material.get()) 
			{
				//std::cout << "@";
			}
		}
	}
}

void FindTextureVisitor::apply(osg::Group& group) 
{
	traverse(group); 
}
        
void FindTextureVisitor::apply(osg::MatrixTransform& node)
{
      apply((osg::Node&)node);
}

	
