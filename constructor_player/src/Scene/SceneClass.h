#ifndef SCENECLASS
#define SCENECLASS

#include "../BuildConfig.h"

#include <stdlib.h>
#include <stdio.h> 
#include <iostream>
#include <osg/Version>
#include <osgViewer/Viewer>
#include <osgDB/WriteFile>
#include <osgDB/DatabasePager>

#ifdef WIN32
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>
#endif

//интерфейсы
#include "./SkinAnimation/interface_for_skin_animation.h"
#include "./HUD/InterfaceForHUD.h"
#include "./OLD/InterfaceForOLD.h"
#include "./Text/InterfaceForText.h"
#include "./Light/InterfaceForLight.h"
#include "./Shaders/InterfaceForShaders.h"
#include "./Video/InterfaceForVideo.h"
#include "./Particle/InterfaceForParticle.h"
#include "./CustomGeometry/CustomGeometry.h"
#include "./Cameras/interfaceforcameras.h"
//#include "./Physic/PhysicBULLET.h"
//#include "./Physic/PhysicNvidia.h"

#ifdef PHYSX
	#if (_MSC_VER < 1700)
	#include "./Physic/physic.h"
	#endif
#endif

#include "./Graphic/GraphicSceneClass.h"
#include "./Audio/AudioSceneClass.h"

#include "../SSAO/ffssao.h"

class Configure;

class SceneClass
{
public:
	SceneClass(Configure* configuration);
	~SceneClass();

	//передача указателя на группу сцены для Viewer'a
	osg::ref_ptr<osg::Group> GetDataToVideoViewer();



	//интерфейсы
	GraphicSceneClass* Graphic;
	AudioSceneClass* Audio;

	//	InterfaceForCharacters* Characters;
	//InterfaceForFBX *FBX;
	InterfaceForSkinAnimation* skin_animation;
	InterfaceForHUD *HUD;
	InterfaceForOLD *OLD;
	InterfaceForText *Texts;
	InterfaceForLight *Light;
	InterfaceForShaders *Shaders;
	InterfaceForVideo* Video;
	InterfaceForParticle* Particles;

        InterfaceForCameras* Cameras;

	//InterfaceForPhysic* Physic;
	//InterfaceForPhysic2* Physic2;

private:
#ifdef PHYSX
	#if (_MSC_VER < 1700)
	cPhysic* _pPhysic;
	cPhysicScene* _pPhysicScene;
#endif
	#endif

public:
#ifdef PHYSX
	#if (_MSC_VER < 1700)
	inline cPhysicScene* physic()	{	return _pPhysicScene;	}
#endif
	#endif

	//очистка всех данных ...
	bool Reset(); 

	//Удаление объекта из сцены
	bool UnloadObject(std::string ObjectName);

	//ТЕСТ!!!
	bool TESTTEST();

	//update...
	bool Update(float userX, float userY, float userZ);

	//Установка ширины/высоты экрана
	void setViewResolution(int width, int height);
private:
	
	//Главная группа сцены
        //#define SHADOW = true;
        osg::ref_ptr<osg::Group> TopRoot;
	#ifndef SHADOW
                osg::ref_ptr<osg::Group> root;
        #else
                osg::ref_ptr<osgShadow::ShadowedScene> root;
		int ReceivesShadowTraversalMask;
		int CastsShadowTraversalMask;
	#endif


};



#endif




