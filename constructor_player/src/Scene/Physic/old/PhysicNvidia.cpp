/***************************************************************************
PhysicNvidia.cpp  -  интерфейс для работы с физикой на основе NVIDIA PhysX
-------------------
begin                : 21 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/



#include "./PhysicNvidia.h"

//конструктор
InterfaceForPhysic2::InterfaceForPhysic2(osg::Group* _root)
{
	root = _root;
	std::cout << "PhysX version" << NX_PHYSICS_SDK_VERSION << std::endl;


	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	NxPhysicsSDK* gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, NULL, desc, &errorCode);
	if(gPhysicsSDK == NULL) 
	{
		std::cout << "Unable to initialize the PhysX SDK, exiting the sample." << std::endl;
		return;
	}

	
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05f);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f);
	NxScene* gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		std::cout << "\nError: Unable to create a PhysX scene, exiting the sample." << std::endl;
		return;
	}

	// Set default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);
	
}