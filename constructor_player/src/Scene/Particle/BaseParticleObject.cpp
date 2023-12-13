/***************************************************************************
BaseParticleObject.cpp  -  описание класса источника частиц
-------------------
begin                : 24 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./BaseParticleObject.h"

//конструктор
BaseParticleObject::BaseParticleObject(std::string Name, osg::ref_ptr<osg::Group> _root)
{
	//Запоминаем ссылку на root
	root = _root;

	//задаем имя
	setName (Name);

	//Particle
	ptemplate = new osgParticle::Particle();
	ptemplate->setLifeTime(3);        // 3 seconds of life
	ptemplate->setSizeRange(osgParticle::rangef(0.75f, 3.0f));
    ptemplate->setAlphaRange(osgParticle::rangef(0.0f, 1.5f));
    ptemplate->setColorRange(osgParticle::rangev4(
        osg::Vec4(1, 0.5f, 0.3f, 1.5f), 
        osg::Vec4(0, 0.0f, 0.0f, 0.0f)));
	// these are physical properties of the particle
    ptemplate->setRadius(0.05f);    // 5 cm wide particles
    ptemplate->setMass(0.05f);    // 50 g heavy

	//ParticleSystem
	ps = new osgParticle::ParticleSystem;
        //ps->setFreezeOnCull(false);
	ps->setDefaultAttributes("", false, false);
	// assign the particle template to the system.
    ps->setDefaultParticleTemplate(*ptemplate);

	//emitter
    emitter = new osgParticle::ModularEmitter;
	
	//По замечаниям Ассель...
	//Углы вылете частиц
	//RadialShooter
	shooter = new osgParticle::RadialShooter();
	emitter->setShooter (shooter.get());
	float k =64.0f;
	float k2 =4.0f;
	shooter->setPhiRange (-0.05f*k,0.05f*k);
	shooter->setThetaRange (-0.05f*k2, 0.05f*k2);
	
	//SectorPlacer
	placer = new osgParticle::SectorPlacer();
	placer->setRadiusRange (0.1f,0.2f);
	emitter->setPlacer(placer.get());
	
    emitter->setParticleSystem(ps.get());
    rrc = static_cast<osgParticle::RandomRateCounter *>(emitter->getCounter());
    rrc->setRateRange(20, 30);    // generate 20 to 30 particles per second
//    rrc->setClamp (true);
    
	osg::ref_ptr <osg::Geode> geode = new osg::Geode;    
    geode->addDrawable(ps.get());
	
	//В этой матрице эмиттер (должен быть строго в Root)
	otherMatrix  = new osg::MatrixTransform();
	otherMatrix->setName (Name + "_emitter");
	otherMatrix->addChild (geode);
	
	psu = new osgParticle::ParticleSystemUpdater;
    psu->addParticleSystem(ps.get());
    
	root->addChild (otherMatrix);
	addChild(psu.get());
	addChild(emitter.get());

	//созданные обертки (для возможности работать с текстом командами типа RSetPosition..)
	root->addChild(this);
}

bool BaseParticleObject::ParticleSetVelocity (float x, float y)
{
	shooter->setInitialSpeedRange(x,y);
	// ptemplate->setVelocity(osg::Vec3(x,y,z));
	 return true;
}

//деструктор
BaseParticleObject::~BaseParticleObject()
{
	std::cout << "Destructor BaseParticleObject: " << getName() << std::endl;
	delete ptemplate;
	//otherMatrix

}

//1. время жизни частицы
bool BaseParticleObject::ParticleSetLifeTime(float value)
{
	ptemplate->setLifeTime((double)value);
	ps->setDefaultParticleTemplate(*ptemplate);
	return true;
}

//2. разброс размеров
bool BaseParticleObject::ParticleSetSizeRange(float minValue, float maxValue)
{
	ptemplate->setSizeRange(osgParticle::rangef(minValue, maxValue));
	ps->setDefaultParticleTemplate(*ptemplate);
	return true;
}

//3. разброс прозрачности
bool BaseParticleObject::ParticleSetAlphaRange(float minValue, float maxValue)
{
	ptemplate->setAlphaRange(osgParticle::rangef(minValue, maxValue));
	ps->setDefaultParticleTemplate(*ptemplate);
	return true;
}

//4. разброс цвета
bool BaseParticleObject::ParticleSetColorRange(float x1, float r1, float g1, float b1, float x2, float r2, float g2, float b2)
{
	ptemplate->setColorRange(osgParticle::rangev4(
        osg::Vec4(x1, r1, g1, b1), 
        osg::Vec4(x2, r2, g2, b2)));
	ps->setDefaultParticleTemplate(*ptemplate);
	return true;
}

//5. первоначальный радиус
bool BaseParticleObject::ParticleSetRadius(float value)
{
	ptemplate->setRadius(value); 
	ps->setDefaultParticleTemplate(*ptemplate);
	return true;
}

//6. масса
bool BaseParticleObject::ParticleSetMass(float value)
{
	ptemplate->setMass(value);
	ps->setDefaultParticleTemplate(*ptemplate);
	return true;
}

//7. угол поворота проскостей
bool BaseParticleObject::ParticleSetAngle (float x, float y, float z)
{
	ptemplate->setAngle (osg::Vec3(x,y,z));
	ps->setDefaultParticleTemplate(*ptemplate);
	return true;
}

//8. текструра,  emissive? , освещаются или нет, текстурный юнит=0
bool BaseParticleObject::ParticleSetDefaultAttributes(std::string filename , bool Emissive, bool Lighting)
{
	ps->setDefaultAttributes(filename, Emissive, Lighting);
	return true;
}

//9. Разброс количества генерируемых частиц в секунду
bool BaseParticleObject::ParticleSetRateRange(float minValue, float maxValue)
{
	rrc->setRateRange(minValue, maxValue);
	return true;
}

//10. Указание углов разброса по двум плоскастям (замечание Ассель)
bool BaseParticleObject::ParticleSetEmitterAngle(float alpha1,float alpha2 ,float beta1 ,float beta2)
{
	shooter->setPhiRange (alpha1,alpha2);
	shooter->setThetaRange (beta1, beta2);
	return true;
}

//11. Указание радиуса сегмента, из которого появляются частицы (замечание Ассель)
bool BaseParticleObject::ParticleSetEmitterRadius(float r1,float r2)
{
	placer->setRadiusRange (r1,r2);
	return true;
}



