#ifndef INTERFACEFORPARTICLE
#define INTERFACEFORPARTICLE

#include <iostream>
#include <osg/MatrixTransform>

#include "./BaseParticleObject.h"
#include "../Visitors/FindNodeByName.h"
//#include "./ParticleHelper.h"

//Обработка критических ошибок
#include "../../error/errorclass.h"

class InterfaceForParticle
{	
public:
	//конструктор
	InterfaceForParticle(osg::ref_ptr<osg::Group> _root);
	//
	~InterfaceForParticle();
	//сброс
	bool Reset();
	//Добавить частицу
	bool AddParticle(std::string name);
	//время жизни частицы
	bool ParticleSetLifeTime(std::string name, float value);
	//разброс размеров
	bool ParticleSetSizeRange(std::string name, float minValue, float maxValue);
	//разброс прозрачности
	bool ParticleSetAlphaRange(std::string name, float minValue, float maxValue);
	//разброс цвета
	bool ParticleSetColorRange( std::string name, float x1, float r1, float g1, float b1, float x2, float r2, float g2, float b2);
	//первоначальный радиус
	bool ParticleSetRadius(std::string name, float value);
	//масса
	bool ParticleSetMass(std::string name, float value);
	//угол поворота проскостей
	bool ParticleSetAngle (std::string name, float x, float y, float z);
	//текструра,  emissive? , освещаются или нет, текстурный юнит=0
	bool ParticleSetDefaultAttributes(std::string name , std::string filename , bool Emissive, bool Lighting);
	//Разброс количества генерируемых частиц в секунду
	bool ParticleSetRateRange(std::string name, float minValue, float maxValue);  
	//Указание углов разброса по двум плоскастям (замечание Ассель)
	bool ParticleSetEmitterAngle(std::string name,float alpha1,float alpha2 ,float beta1 ,float beta2);
	//Указание радиуса сегмента, из которого появляются частицы (замечание Ассель)
	bool ParticleSetEmitterRadius(std::string name,float r1,float r2);
	//
	bool ParticleSetVelocity (std::string Name, float, float);

private:
	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;
	//и сами элементы
	//std::map <std::string, BaseParticle*> particles;
	//процедура проверки имени
	//bool NameIsExsist(std::string);

	//сервисная функция
	osg::ref_ptr<BaseParticleObject> FindParticleObject(std::string name);
};

#endif
