/***************************************************************************
InterfaceForLight.cpp  -  интерфейс для работы с источниками света
-------------------
begin                : 28 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./InterfaceForLight.h"

//конструктор
InterfaceForLight::InterfaceForLight(osg::ref_ptr<osg::Group> _root)
{
	root = _root;
}
//
InterfaceForLight::~InterfaceForLight()
{
	std::cout << "Destructor InterfaceForLight."  << std::endl;
	Reset();
}

//сброс всего, что на HUD'е
bool InterfaceForLight::Reset()
{
	return SetAllLightOFF();
}


//начальная настройка света
void InterfaceForLight::SetStartLight()
{
	//Источник света №0
	osg::ref_ptr<osg::Light> light0;
	light0 = new osg::Light;
	light0->setLightNum(0);
	light0->setDiffuse(osg::Vec4(0.3, 0.3, 0.3, 1));
	light0->setSpecular(osg::Vec4(0.8, 0.8, 0.8, 1));
	light0->setAmbient(osg::Vec4(-0.1, -0.1, -0.1, 1));
	light0->setPosition(osg::Vec4(-410, 172, 40, 1));
	light0->setDirection(osg::Vec3(-410, 172, 0));

	osg::ref_ptr<osg::LightSource> Light0 = new osg::LightSource;
	Light0->setLight(light0.get());
	Light0->setLocalStateSetModes();
	Light0->setStateSetModes(*root->getOrCreateStateSet(),osg::StateAttribute::ON);
	root->addChild(Light0.get());
}

//устанавливает Diffuse-цвет указанного светового источника
bool InterfaceForLight::SetLightColor(std::string ObjectName, float r,float g,float b)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем световой источник
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindLightVisitor> zzz = new FindLightVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если световой источник есть
		if (zzz->LS.empty() == true)
		{
			std::cout << "Lights NOT found" << std::endl;
			//delete zzz;
			//delete fnnv;
			return false;
		}
		for (unsigned int i = 0; i < zzz->LS.size();i++)
		{
			zzz->LS.at(i)->getLight()->setDiffuse (osg::Vec4(r,g,b,1.0f));
		}
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}

//Включает/выключает свет
bool InterfaceForLight::SetLightEnabled(std::string ObjectName, bool enabled)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//Если такой объект есть, ищем световой источник
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		osg::ref_ptr<FindLightVisitor> zzz = new FindLightVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz.get());
		//Если световой источник есть
		if (zzz->LS.empty() == true)
		{
			std::cout << "Lights NOT found" << std::endl;
			//delete zzz;
			//delete fnnv;
			return false;
		}

		for (unsigned int i = 0; i < zzz->LS.size();i++)
		{
			if (enabled)
			{
				root->getOrCreateStateSet()->setAttributeAndModes(zzz->LS.at(i)->getLight(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE); 
			}
			else
			{
				root->getOrCreateStateSet()->setAttributeAndModes( zzz->LS.at(i)->getLight(),osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
			}
		}
		//Чистим мусор
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}

//Включает/выключает весь свет на сцене
bool InterfaceForLight::SetAllLightOFF()
{
	osg::ref_ptr<FindLightVisitor> zzz = new FindLightVisitor();
	root->accept (*zzz.get());
	//Если световой источник есть
	if (zzz->LS.empty() == true)
	{
		std::cout << "Lights NOT found" << std::endl;
		//delete zzz;
		return false;
	}
	for (unsigned int i = 0; i < zzz->LS.size();i++)
	{
		//root->getOrCreateStateSet()->setAttributeAndModes( zzz->LS.at(i)->getLight(),osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
	}
	//Чистим мусор
	//delete zzz;
	//Чистим мусор
	return true;
}

//Включает/выключает учет освещения объектом
//изменять атрибуты, например "объект освещается или нет" (stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);)
bool InterfaceForLight::SetLightingEnable(std::string ObjectName, bool enabled)
{
	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	//такой объект есть?....
	if (fnnv->_foundNodes.empty()==true)
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;

	if (enabled)
	{
		fnnv->_foundNodes.front().get()->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		fnnv->_foundNodes.front().get()->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	//Чистим мусор
	//delete fnnv;
	return true;
}