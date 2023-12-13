#include "./physic.h"
#include "NxCooking.h"
//#include "SamplesVRDSettings.h"

PROTO_SINGLETON_IMPL(cPhysic)

cPhysic::cPhysic():  _pPhysicsSDK( NULL ), _pCooking( NULL )
{
	std::cout << "nvidia PhysX version " << NX_SDK_VERSION_MAJOR << "." << NX_SDK_VERSION_MINOR << "." << NX_SDK_VERSION_BUGFIX << std::endl;
	_pCooking = NxGetCookingLib( NX_PHYSICS_SDK_VERSION );
	assert( _pCooking );

	_pCooking->NxInitCooking();

	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	_pPhysicsSDK = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, NULL, NULL, desc, &errorCode );
	_pPhysicsSDK->setParameter( NX_ASYNCHRONOUS_MESH_CREATION, 1 );
	//_pPhysicsSDK->setParameter( NX_VISUALIZATION_SCALE, 24.6f );

	/*float f_sleep_linear_velocity = 0.15f * PhysicScale;
	_pPhysicsSDK->setParameter( NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, f_sleep_linear_velocity*f_sleep_linear_velocity );
	float f_sleep_ang_velocity = 0.14f * PhysicScale;
	_pPhysicsSDK->setParameter( NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, f_sleep_ang_velocity*f_sleep_ang_velocity );

	_pPhysicsSDK->setParameter( NX_SKIN_WIDTH, 0.025f * PhysicScale );
*/
	//_pPhysicsSDK->setParameter( NX_DEFAULT_SLEEP_ENERGY, 0.005f * PhysicScale );
	

	


	if (_pPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !_pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		_pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect( "localhost", NX_DBG_DEFAULT_PORT, NX_DBG_EVENTMASK_EVERYTHING );
}

cPhysic::~cPhysic()
{
	_pCooking->NxCloseCooking();
	_pCooking = NULL;

	NxReleasePhysicsSDK( _pPhysicsSDK );
	_pPhysicsSDK = NULL;
}

cPhysicScene* cPhysic::createScene( osg::ref_ptr<osg::Group> root, float fGravity )
{
    try
    {
		cPhysicScene* pScene = new cPhysicScene();
		if( pScene->create( _pPhysicsSDK, _pCooking, root, fGravity * PhysicScale ) == false )
		{
			delete pScene;
					throw (ErrorClass (__FILE__, __LINE__ , "cPhysic::createScene", "cPhysic::createScene FAILED!", 8));
		}
		std::cout << "Physic create scene SUCCES!" << std::endl;
		return pScene;
    }
    catch (ErrorClass error)
    {
        error.print();
    }
	return NULL;
}

void cPhysic::destroyScene( cPhysicScene* pScene )
{
	pScene->destroy( _pPhysicsSDK );
	delete pScene;
}
