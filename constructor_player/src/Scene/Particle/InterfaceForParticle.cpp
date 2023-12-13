/***************************************************************************
InterfaceForParticle.cpp  -  интерфейс для работы с источниками частиц
-------------------
begin                : 6 марта 2023
modify               : 24 апреля 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./InterfaceForParticle.h"

//конструктор
InterfaceForParticle::InterfaceForParticle(osg::ref_ptr<osg::Group> _root)
{
	root = _root;
}

InterfaceForParticle::~InterfaceForParticle()
{
	std::cout << "Destructor InterfaceForParticle."  << std::endl;
	Reset();
}

//сброс всех частиц
bool InterfaceForParticle::Reset()
{
	return true;
}

bool InterfaceForParticle::ParticleSetVelocity (std::string Name, float x, float y)
{
	FindParticleObject(Name)->ParticleSetVelocity(x,y);
	return true;
}

bool InterfaceForParticle::AddParticle(std::string Name)
{
    try
    {
	//проверка на совпадение имен с уже имеющимися
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(Name);
        root->accept(*fnnv.get());
	if (fnnv->_foundNodes.empty()!=true)
	{
            throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForParticle::AddParticle", "Add new Particle: Name already use: " + Name, 3));
	}
	
	//добавляем в сцену (добавлять в root не нужно.. оно само))
	osg::ref_ptr<BaseParticleObject> newParticle = new BaseParticleObject(Name, root);
	return true;
    }
    catch (ErrorClass error)
    {
        error.print();
    }
}

//время жизни частицы
bool InterfaceForParticle::ParticleSetLifeTime(std::string name, float value)
{
	FindParticleObject(name)->ParticleSetLifeTime(value);
	return true;
}

//разброс размеров
bool InterfaceForParticle::ParticleSetSizeRange(std::string name, float minValue, float maxValue)
{
	FindParticleObject(name)->ParticleSetSizeRange (minValue, maxValue);
	return true;
}

//разброс прозрачности
bool InterfaceForParticle::ParticleSetAlphaRange(std::string name, float minValue, float maxValue)
{
	FindParticleObject(name)->ParticleSetAlphaRange (minValue, maxValue);
	return true;
}

//разброс цвета
bool InterfaceForParticle::ParticleSetColorRange( std::string name, float x1, float r1, float g1, float b1, float x2, float r2, float g2, float b2)
{
	FindParticleObject(name)->ParticleSetColorRange (x1, r1, g1, b1, x2, r2, g2, b2);
	return true;
}

//первоначальный радиус
bool InterfaceForParticle::ParticleSetRadius(std::string name, float value)
{
	FindParticleObject(name)->ParticleSetRadius(value);
	return true;
}

//масса
bool InterfaceForParticle::ParticleSetMass(std::string name, float value)
{
	FindParticleObject(name)->ParticleSetMass(value);
	return true;
}

//угол поворота проскостей
bool InterfaceForParticle::ParticleSetAngle (std::string name, float x, float y, float z)
{
	FindParticleObject(name)->ParticleSetAngle (x,y,z);
	return true;
}

//текструра,  emissive? , освещаются или нет, текстурный юнит=0
bool InterfaceForParticle::ParticleSetDefaultAttributes(std::string name , std::string filename , bool Emissive, bool Lighting)
{
	FindParticleObject(name)->ParticleSetDefaultAttributes (filename , Emissive, Lighting);
	return true;
}

//Разброс количества генерируемых частиц в секунду
bool InterfaceForParticle::ParticleSetRateRange(std::string name, float minValue, float maxValue)
{
	FindParticleObject(name)->ParticleSetRateRange (minValue, maxValue);
	return true;
}

//Указание углов разброса по двум плоскастям (замечание Ассель)
bool InterfaceForParticle::ParticleSetEmitterAngle(std::string name, float alpha1,float alpha2 ,float beta1 ,float beta2)
{
	FindParticleObject(name)->ParticleSetEmitterAngle (alpha1,alpha2, beta1, beta2);
	return true;
}

//Указание радиуса сегмента, из которого появляются частицы (замечание Ассель)
bool InterfaceForParticle::ParticleSetEmitterRadius(std::string name, float r1,float r2)
{
	FindParticleObject(name)->ParticleSetEmitterRadius(r1, r2);
	return true;
}



//--------------------------------------------------------------------------------------------------------
//сервисная функция
osg::ref_ptr<BaseParticleObject> InterfaceForParticle::FindParticleObject(std::string name)
{
    try
    {
	//ищем в графе сцены
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(name);
        root->accept(*fnnv.get());
	//
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << name << std::endl;
		//попытка преобразовать в BaseSoundObject
		if ( BaseParticleObject* _foundParticleObject = dynamic_cast<BaseParticleObject*>( fnnv->_foundNodes.front().get() ) )
		{
			osg::ref_ptr<BaseParticleObject> foundParticleObject =  _foundParticleObject;
			return foundParticleObject.get();
		}
                throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForParticle::FindParticleObject", "Object - NOT particle! " + name, 3));
	}
	else
	{
                throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForParticle::FindParticleObject", "Find objects in Root - FALSE!  " + name, 3));
	}
    }
    catch (ErrorClass error)
    {
        error.print();
    }

}
//--------------------------------------------------------------------------------------------------------
