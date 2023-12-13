// [86open@gmail.com]		20.05.2009
#ifndef PROTO_OPEN_PHYSIC_H
#define PROTO_OPEN_PHYSIC_H
#pragma once

#include "NxPhysics.h"
#include "./singleton.h"
#include "./physic_scene.h"
#include <osg/Group>
//#include <osg/ref_ptr>

//Обработка критических ошибок
#include "../../error/errorclass.h"

//Библиотеки подключаем в свойствах линкера! #pragma comment - помоему не работает в Линухе! или пиши #ifdef WIN32 ..... 
/*
#ifdef _DEBUG
#pragma comment(lib, "PhysXLoaderCHECKED.lib")
#pragma comment(lib, "NxCookingCHECKED.lib")
#pragma comment(lib, "NxCharacterCHECKED.lib")
#else
#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "NxCooking.lib")
#pragma comment(lib, "NxCharacter.lib")
#endif
*/

#define PhysicScale		24.6f//37.f
//24.6f

#define PhysicEngine	cPhysic::instance()

class cPhysic: public cSingleton<cPhysic>
{
public:
	cPhysic();
	virtual ~cPhysic();

	cPhysicScene* createScene( osg::ref_ptr<osg::Group> root, float fGravity );
	void destroyScene( cPhysicScene* pScene );

private:
	NxPhysicsSDK* _pPhysicsSDK;
	NxCookingInterface* _pCooking;	
};


#endif
