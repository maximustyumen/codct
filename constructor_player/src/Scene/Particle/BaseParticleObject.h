#ifndef BASEPARTICLEOBJECTCLASS
#define BASEPARTICLEOBJECTCLASS

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>

#include <iostream>

class BaseParticleObject: public osg::MatrixTransform
{
public:
	//конструктор
	BaseParticleObject(std::string Name, osg::ref_ptr<osg::Group> _root);
	//деструктор
	~BaseParticleObject();

	//1. время жизни частицы
	bool ParticleSetLifeTime(float value);
	//1. разброс размеров
	bool ParticleSetSizeRange(float minValue, float maxValue);
	//3. разброс прозрачности
	bool ParticleSetAlphaRange(float minValue, float maxValue);
	//4. разброс цвета
	bool ParticleSetColorRange(float x1, float r1, float g1, float b1, float x2, float r2, float g2, float b2);
	//5. первоначальный радиус
	bool ParticleSetRadius(float value);
	//6. масса
	bool ParticleSetMass(float value);
	//7. угол поворота проскостей
	bool ParticleSetAngle (float x, float y, float z);
	//8. текструра,  emissive? , освещаются или нет, текстурный юнит=0
	bool ParticleSetDefaultAttributes( std::string filename , bool Emissive, bool Lighting);
	//9. Разброс количества генерируемых частиц в секунду
	bool ParticleSetRateRange(float minValue, float maxValue);  
	//10. Указание углов разброса по двум плоскастям (замечание Ассель)
	bool ParticleSetEmitterAngle(float alpha1,float alpha2 ,float beta1 ,float beta2);
	//11. Указание радиуса сегмента, из которого появляются частицы (замечание Ассель)
	bool ParticleSetEmitterRadius(float r1,float r2);

	//
	bool ParticleSetVelocity (float, float);


private:
	osgParticle::Particle* ptemplate;
	osg::ref_ptr<osgParticle::ParticleSystem> ps;
	osg::ref_ptr<osgParticle::ModularEmitter> emitter;
	osg::ref_ptr<osgParticle::RandomRateCounter> rrc;
	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu;
	osg::ref_ptr<osgParticle::RadialShooter>	shooter;
	//или или
	osg::ref_ptr<osgParticle::SectorPlacer> placer;
	//созданные обертки (для возможности работать с текстом командами типа RSetPosition..)
	//эта матрица которой можно управлять (RsetRotate, position ...)
	//this
	//В этой матрице эмиттер (должен быть строго в Root)
	osg::ref_ptr<osg::MatrixTransform> otherMatrix;
	
	//ссылка на root
	osg::ref_ptr<osg::Group> root;
};


#endif