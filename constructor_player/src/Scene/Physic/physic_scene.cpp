#include "./physic_scene.h"
#include <NxCooking.h>
#include "./cook_stream.h"

// !!!DEPRECATED
#include <osg/Timer>
#include "./vehicle/vehicle.h"

class cPhysicContactReport: public NxUserContactReport
{
public:
	virtual void onContactNotify(NxContactPair& pair, NxU32 events);
};

void cPhysicContactReport::onContactNotify( NxContactPair& pair, NxU32 events )
{
	_asm nop;
}

// -------------------------------------------------------------------------------
cPhysicScene::cPhysicScene(): _pScene( NULL ), _pContactReport( NULL ), _pCooking( NULL )
{
	_pContactReport = new cPhysicContactReport();
}

bool cPhysicScene::create( NxPhysicsSDK* pPhysicsSDK, NxCookingInterface* pCooking, osg::ref_ptr<osg::Group> root, float fGravity )
{
	_osgRoot = root;
	_pCooking = pCooking;

	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3( 0, fGravity, 0 );
	sceneDesc.userContactReport = _pContactReport;
	sceneDesc.userTriggerReport = NULL;
	sceneDesc.timeStepMethod = NX_TIMESTEP_VARIABLE;
	sceneDesc.simType = NX_SIMULATION_HW;
	_pScene = pPhysicsSDK->createScene( sceneDesc );

	if( NULL == _pScene )
	{
		sceneDesc.simType = NX_SIMULATION_SW;
		_pScene = pPhysicsSDK->createScene( sceneDesc );
		assert( _pScene );   
		if( NULL == _pScene )
			return false;
	}	

	// init mats ----------------------------------------------------------------------
	NxMaterial* pDefaultMaterial = _pScene->getMaterialFromIndex(0); 

	pDefaultMaterial->setDynamicFriction( 0.2f );
	pDefaultMaterial->setStaticFriction( 0.2f );
	//pDefaultMaterial->setRestitutionCombineMode( NX_CM_AVERAGE );
	//pDefaultMaterial->setRestitution(0.001f);
	float tt = pDefaultMaterial->getRestitution();
	__asm nop;
	/*NxMaterialDesc mat_desc;
	mat_desc.setToDefault();
	mat_desc.dynamicFriction = 0.9f*37.f;
	mat_desc.staticFriction	= 0.9f*37.f;
	mat_desc.restitution = 0.f;

	pDefaultMaterial->loadFromDesc( mat_desc );*/

	//pDefaultMaterial->setRestitution( 0.f );
	//pDefaultMaterial->setStaticFriction( 0.8f );
	//pDefaultMaterial->setDynamicFriction( 0.6f );

	/*mat_desc.restitution = 0.1f;
	NxMaterial* pMat1 = _pScene->createMaterial( mat_desc );
	int t = pMat1->getMaterialIndex();*/

	//update( 0.f );

	

	return true;
}

void cPhysicScene::destroy( NxPhysicsSDK* pPhysicsSDK )
{
	pPhysicsSDK->releaseScene( *_pScene );
	_pScene = NULL;
}

cPhysicScene::~cPhysicScene()
{
	delete _pContactReport;	_pContactReport = NULL;
}

struct sPred_physicObjectUpdatePose
{
	sPred_physicObjectUpdatePose( float f_dt ): _f_dt( f_dt )
	{
	}

	void operator()( const pair_physic_object& pairPhysicObject ) const
	{
		if( pairPhysicObject.second->isDynamic() )
		{
			pairPhysicObject.second->updateTM( _f_dt );
		}
	}

	float _f_dt;
};

void cPhysicScene::update( float dDt )
{
	static osg::Timer_t tick1 = osg::Timer::instance()->tick();
	double fElapsedTime = osg::Timer::instance()->delta_s( tick1,osg::Timer::instance()->tick() );
	tick1 = osg::Timer::instance()->tick();

	 //std::cout << fElapsedTime << std::endl;
	 //_asm nop;

	if( fElapsedTime > 0.033f )
	{
		fElapsedTime = 0.033f;
	}

	//_pVehicle->updateVehicle( fElapsedTime );

	_pScene->simulate( fElapsedTime );
	_pScene->flushStream();

	while( !_pScene->fetchResults( NX_ALL_FINISHED, false ) )
	{
		_asm nop;
	}

	// update pose
	std::for_each( _mObjects.begin(), _mObjects.end(), sPred_physicObjectUpdatePose( fElapsedTime ) );
}

bool cPhysicScene::reset()
{
	cleanup();
	return true;
}

struct sPred_physicObjectDestroy
{
	sPred_physicObjectDestroy( NxScene* pScene ): _pScene( pScene )	{}

	void operator()( const pair_physic_object& pairPhysicObject )
	{
		pairPhysicObject.second->destroy( _pScene );
		delete pairPhysicObject.second;
	}

	NxScene* _pScene;
};

struct sPred_physicJointDestroy
{
	sPred_physicJointDestroy( NxScene* pScene ): _pScene( pScene )	{}

	void operator()( const pair_physic_joint& pairPhysicJoint )
	{
		pairPhysicJoint.second->destroy( _pScene );
		delete pairPhysicJoint.second;
	}

	NxScene* _pScene;
};

void cPhysicScene::cleanup()
{
	std::for_each( _mObjects.begin(), _mObjects.end(), sPred_physicObjectDestroy( _pScene ) );
	_mObjects.clear();

	std::for_each( _mJoints.begin(), _mJoints.end(), sPred_physicJointDestroy( _pScene ) );
	_mJoints.clear();
}

bool cPhysicScene::isPhysicObjectInPool( const std::string& strName )
{
	map_physic_object::iterator it_find = _mObjects.find( strName );
	if( _mObjects.end() == it_find )
		return false;

	return true;
	/*return findPhysicObjectInPool( strName ) != NULL;*/
}

cPhysicObject* cPhysicScene::findPhysicObjectInPool( const std::string& strName )
{
	map_physic_object::iterator it_find = _mObjects.find( strName );
	if( _mObjects.end() == it_find )
		return NULL;
	
	return it_find->second;

	/*FindNamedNodeVisitor fnnv( strName );
	_osgRoot->accept( fnnv );

	if( fnnv._foundNodes.empty() )
	{
		std::cout << "Warning: Physic object '" << strName.c_str() << "' not in a root!" << std::endl;
		return NULL;
	}

	if( fnnv._foundNodes.size() > 1 )
	{
		std::cout << "Warning: Physic '" << strName.c_str() << "' already exist!" << std::endl;
		return NULL;
	}

	cPhysicObject* pPhysicObject = NULL;
	if( pPhysicObject = dynamic_cast<cPhysicObject*>( fnnv._foundNodes.front().get() ) )
	{
		return pPhysicObject;
	}
	
	std::cout << "Warning: It not physical object '" << strName.c_str() << "'!" << std::endl;
	return NULL;*/
}

osg::ref_ptr<osg::MatrixTransform> cPhysicScene::findTransformMatrix( const std::string& strName )
{
	FindNamedNodeVisitor fnnv( strName );

	if( findRenderNode( fnnv ) )
		return fnnv._foundNodes.front().get();

	return osg::ref_ptr<osg::MatrixTransform>();
}

bool cPhysicScene::findRenderNode( FindNamedNodeVisitor& fnnv )
{
	_osgRoot->accept( fnnv );

	if( fnnv._foundNodes.empty() )
	{
		std::cout << "Find objects in Root - FALSE! " << fnnv._name << std::endl;
	}
	else
	{
		if( fnnv._foundNodes.size() > 1 ) 
		{
			std::cout << "Warning: Duplicate names - " << fnnv._name << std::endl;
		}

		return true;
	}

	return false;
}

cPhysicStaticPlane* cPhysicScene::addStaticPlane( std::string& strName, NxVec3& vNormal, float fD )
{
	if( !isPhysicObjectInPool( strName ) )
	{
		//osg::ref_ptr<cPhysicStaticPlane> pPlane = new cPhysicStaticPlane();
		cPhysicStaticPlane* pPlane = new cPhysicStaticPlane();
		pPlane->create( _pScene, vNormal, fD );
		PHYSIC_OBJECT_SET_NAME( pPlane, strName );		// debug

		//pPlane->setName( strName );
		//_osgRoot->addChild( pPlane.get() );

		_mObjects.insert( pair_physic_object( strName, pPlane ) );
		return pPlane;
	}
	
	return NULL;
}

cPhysicSphere* cPhysicScene::addSphereBody( std::string& strName, float fMassa, float fRadius )
{
	if( !isPhysicObjectInPool( strName ) )
	{
		osg::ref_ptr<osg::MatrixTransform> matrix = findTransformMatrix( strName );
		if( matrix.get() )
		{
			//osg::ref_ptr<cPhysicSphere> pSphere = new cPhysicSphere();
			cPhysicSphere* pSphere = new cPhysicSphere();
			pSphere->setTM( matrix );
			pSphere->create( _pScene, fMassa, fRadius );		
			PHYSIC_OBJECT_SET_NAME( pSphere, strName );		// debug

			//pSphere->setName( strName );
			//_osgRoot->addChild( pSphere.get() );

			_mObjects.insert( pair_physic_object( strName, pSphere ) );
			return pSphere;
		}
	}
	return NULL;
}

cPhysicBox* cPhysicScene::addBoxBody( std::string& strName, float fMassa, NxVec3& vDimension )
{
	if( !isPhysicObjectInPool( strName ) )
	{
		osg::ref_ptr<osg::MatrixTransform> matrix = findTransformMatrix( strName );
		if( matrix.get() )
		{
			//osg::ref_ptr<cPhysicBox> pBox = new cPhysicBox();
			cPhysicBox* pBox = new cPhysicBox();
			pBox->setTM( matrix );
			pBox->create( _pScene, fMassa, vDimension );		
			PHYSIC_OBJECT_SET_NAME( pBox, strName );		// debug

			//pBox->setName( strName );
			//_osgRoot->addChild( pBox.get() );

			_mObjects.insert( pair_physic_object( strName, pBox ) );
			return pBox;
		}
	}
	return NULL;
}

osg::ref_ptr<osg::Vec3Array> cPhysicScene::convertVertices( FindNamedNodeVisitor& fnnv )
{
	FindGeometryVisitor fgv;
	fnnv._foundNodes.front().get()->accept( fgv );

	osg::ref_ptr<osg::Vec3Array> pPhysxVertices = new osg::Vec3Array();
	{	// need convert
		osg::Vec3Array* pVertices = dynamic_cast<osg::Vec3Array*>( fgv.geom->getVertexArray() );
		size_t numVertices = pVertices->size();

		osg::Vec3Array* vData = pPhysxVertices.get();
		vData->reserve( numVertices );

		osg::Vec3 vector;
		for( size_t i=0; i<numVertices; i++ )
		{
			vector.set( pVertices->at(i).x(), pVertices->at(i).z(), pVertices->at(i).y() );
			vData->push_back( vector );
		}
	}
	return pPhysxVertices;
}

cPhysicMesh* cPhysicScene::addStaticMesh( std::string& strName )
{
	if( !isPhysicObjectInPool( strName ) )
	{
		FindNamedNodeVisitor fnnv( strName );
		if( findRenderNode( fnnv ) )
		{
			osg::ref_ptr<osg::Vec3Array> pPhysxVertices = convertVertices( fnnv );

			NxTriangleMeshDesc meshDesc;	// описатель меша
			meshDesc.pointStrideBytes = sizeof( osg::Vec3 );
			meshDesc.triangleStrideBytes = 0;
			meshDesc.flags = NX_MF_16_BIT_INDICES | NX_MF_FLIPNORMALS;
			meshDesc.heightFieldVerticalAxis = NX_NOT_HEIGHTFIELD;
			meshDesc.numVertices = pPhysxVertices.get()->size();
			meshDesc.numTriangles = meshDesc.numVertices/3;
			meshDesc.points = (void*) ( pPhysxVertices.get()->getDataPointer() );
			meshDesc.triangles = NULL;

			cCookStreamBuffer stream;
			if( _pCooking->NxCookTriangleMesh( meshDesc, stream ) )
			{
				//cCookStreamBuffer stream_scale;
				//stream.toRead();
				//_pCooking->NxScaleCookedConvexMesh( stream, 10.f, stream_scale );

				stream.toRead();

				cPhysicMesh* pMesh = new cPhysicMesh();
				pMesh->setTM( fnnv._foundNodes.front().get() );
				if( !pMesh->create( _pScene, stream ) )
				{
					delete pMesh;
					return NULL;
				}

				/*osg::ref_ptr<cPhysicMesh> ref_pMesh = pMesh;
				ref_pMesh->setName( strName );
				_osgRoot->addChild( ref_pMesh.get() );*/

				PHYSIC_OBJECT_SET_NAME( pMesh, strName );		// debug
				_mObjects.insert( pair_physic_object( strName, pMesh ) );

				return pMesh;
			}
		}
	}
	return NULL;
}

cPhysicDecomposition* cPhysicScene::addDecompositionBody( const std::string& strName,
	const std::string& strRenderName, float fMassa, int iDepth )
{
	if( !isPhysicObjectInPool( strName ) )
	{
		FindNamedNodeVisitor fnnv( strName );
		FindNamedNodeVisitor fnnv_render( strRenderName );
		
		if( findRenderNode( fnnv ) && findRenderNode( fnnv_render ) )
		{
			osg::ref_ptr<osg::Vec3Array> pPhysxVertices = convertVertices( fnnv );
			size_t numVertices = pPhysxVertices.get()->size();

			// create indices
			unsigned int* pIndices = new unsigned int[numVertices]; 
			for( size_t i=0; i<numVertices; i++ )
			{
				pIndices[i] = i;
			}

			DecompDesc desc;
			desc.mVcount = numVertices;
			desc.mVertices =  (const float*)pPhysxVertices.get()->getDataPointer();
			desc.mTcount = numVertices/3;
			desc.mIndices = pIndices;

			desc.mDepth = iDepth;
			desc.mCpercent = 5.f;
			desc.mPpercent = 15.f;
			desc.mMaxVertices = 16;
			desc.mSkinWidth = 0.f;

			cBuildConvexDecomposition buildConvexDecomposition( _pCooking );
			desc.mCallback = &buildConvexDecomposition;

			ConvexBuilder cb( desc.mCallback );
			cb.process( desc );

			delete[] pIndices;	pIndices = NULL;

			cPhysicDecomposition* pDecompObject = new cPhysicDecomposition();
			pDecompObject->setTM( fnnv_render._foundNodes.front().get() );

			if( !pDecompObject->create( _pScene, buildConvexDecomposition.getResult(), fMassa ) )
			{
				delete pDecompObject;
				return NULL;
			}

			if( strName != strRenderName )
				fnnv._foundNodes.front().get()->setNodeMask( 0x0 );

			/*osg::ref_ptr<cPhysicDecomposition> ref_pDecompObject = pDecompObject;
			ref_pDecompObject->setName( strName );
			_osgRoot->addChild( ref_pDecompObject.get() );*/

			PHYSIC_OBJECT_SET_NAME( pDecompObject, strName );
			_mObjects.insert( pair_physic_object( strName, pDecompObject ) );
			return pDecompObject;
		}
	}
	return NULL;
}

void cPhysicScene::setPosition( std::string& strName, NxVec3& vPos )
{
	cPhysicObject* pObject = findPhysicObjectInPool( strName );

	if( pObject )
	{
		//NxVec3 vConvertPos( vPos.x, vPos.z, vPos.y );
		pObject->setPosition( vPos );
	}
}

void cPhysicScene::addForce( std::string& strName, NxVec3& vForce, cPhysicObject::eForceMode mode )
{
	cPhysicObject* pObject = findPhysicObjectInPool( strName );

	if( pObject )
	{
		//NxVec3 vConvertedForce( vForce.x, vForce.z, vForce.y );
		pObject->addForce( vForce, mode );
	}
}

void cPhysicScene::addLocalForce( std::string& strName, NxVec3& vForce, cPhysicObject::eForceMode mode )
{
	cPhysicObject* pObject = findPhysicObjectInPool( strName );

	if( pObject )
	{
		//NxVec3 vConvertedForce( vForce.x, vForce.z, vForce.y );
		pObject->addLocalForce( vForce, mode );
	}
}

void cPhysicScene::addLocalTorque( std::string& strName, NxVec3& vTorque, cPhysicObject::eForceMode mode )
{
	cPhysicObject* pObject = findPhysicObjectInPool( strName );
	if( pObject )
	{
		pObject->addLocalTorque( vTorque, mode );
	}
}

void cPhysicScene::setMotorAtRevoluteJoint( std::string& strName, float fVelTarget, float fMaxForce, bool bFreeSpin )
{
	cPhysicJoint* pJoint = findJointInPool( strName );
	if( pJoint )
	{
		NxMotorDesc desc;
		desc.velTarget = fVelTarget;
		desc.maxForce = fMaxForce;
		desc.freeSpin = bFreeSpin;
		pJoint->setMotor( desc );
	}
}

// -------------------------------------------------------------------------------
void cPhysicScene::setShapeLocalPosition( std::string& strName, NxVec3& vPos )
{
	cPhysicObject* pObject = findPhysicObjectInPool( strName );

	if( pObject )
	{
		pObject->setShapeLocalPosition( vPos );
	}
}

// -------------------------------------------------------------------------------
cPhysicJoint* cPhysicScene::findJointInPool( const std::string& strName )
{
	map_physic_joint::iterator it_find = _mJoints.find( strName );
	if( _mJoints.end() == it_find )
		return NULL;

	return it_find->second;
}

bool cPhysicScene::isJointInPool( const std::string& strName )
{
	map_physic_joint::iterator it_find = _mJoints.find( strName );
	if( _mJoints.end() == it_find )
		return false;

	std::cout << "Warning: Physic joint '" << strName.c_str() << "' already exist!" << std::endl;
	return true;
}


cPhysicJoint* cPhysicScene::addFixedJoint( sJointNameDesc& nameDesc )
{
	if( !isJointInPool( nameDesc.strJointName ) )
	{
		cPhysicObject* pActors[2];
		NxFixedJointDesc fixedDesc;
		initFromNameDesc( nameDesc, fixedDesc, pActors );
		return createJoint( nameDesc.strJointName, fixedDesc );
	}
	return NULL;
}

cPhysicJoint* cPhysicScene::addDistanceJoint( sJointNameDesc& nameDesc, float fMinDistance, float fMaxDistance )
{
	if( !isJointInPool( nameDesc.strJointName ) )
	{
		cPhysicObject* pActors[2];
		NxDistanceJointDesc distDesc;
		initFromNameDesc( nameDesc, distDesc, pActors );

		distDesc.minDistance = fMinDistance;
		distDesc.maxDistance = fMaxDistance;

		if( fMinDistance > 0.f )
			distDesc.flags |= NX_DJF_MIN_DISTANCE_ENABLED;
		
		if( fMaxDistance > 0.f )
			distDesc.flags |= NX_DJF_MAX_DISTANCE_ENABLED;
			
		return createJoint( nameDesc.strJointName, distDesc );
	}
	return NULL;
}

cPhysicJoint* cPhysicScene::addCylindricalJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc )
{
	if( !isJointInPool( nameDesc.strJointName ) )
	{
		cPhysicObject* pActors[2];
		NxCylindricalJointDesc cylindricalDesc;
		initFromNameDesc( nameDesc, cylindricalDesc, pActors );
		initFromLocalDesc( cylindricalDesc, localDesc );
		return createJoint( nameDesc.strJointName, cylindricalDesc );
	}
	return NULL;
}

cPhysicJoint* cPhysicScene::addSphericalJoint( sSphericalJointDesc& desc )
{
	if( !isJointInPool( desc.nameDesc.strJointName ) )
	{
		cPhysicObject* pActors[2];
		NxSphericalJointDesc sphericalDesc;
		initFromNameDesc( desc.nameDesc, sphericalDesc, pActors );
		initFromLocalDesc( sphericalDesc, desc.localDesc );

		if( desc.bTwist )
		{
			sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
			sphericalDesc.twistLimit.low.value = desc.fLowTwist;
			sphericalDesc.twistLimit.high.value = desc.fHighTwist;
		}

		if( desc.bSwing )
		{
			sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
			sphericalDesc.swingLimit.value = desc.fSwingValue;
			sphericalDesc.swingAxis = desc.vSwingAxis;
		}		

		return createJoint( desc.nameDesc.strJointName, sphericalDesc );
	}
	return NULL;
}

cPhysicJoint* cPhysicScene::addPointOnLineJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc )
{
	if( !isJointInPool( nameDesc.strJointName ) )
	{
		cPhysicObject* pActors[2];
		NxPointOnLineJointDesc pointOnLineDesc;
		initFromNameDesc( nameDesc, pointOnLineDesc, pActors );
		initFromLocalDesc( pointOnLineDesc, localDesc );
		return createJoint( nameDesc.strJointName, pointOnLineDesc );
	}
	return NULL;
}


cPhysicJoint* cPhysicScene::addPointInPlaneJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc )
{
	if( !isJointInPool( nameDesc.strJointName ) )
	{
		cPhysicObject* pActors[2];
		NxPointInPlaneJointDesc pointInPlaneDesc;
		initFromNameDesc( nameDesc, pointInPlaneDesc, pActors );
		initFromLocalDesc( pointInPlaneDesc, localDesc );
		return createJoint( nameDesc.strJointName, pointInPlaneDesc );
	}
	return NULL;
}


cPhysicJoint* cPhysicScene::addRevoluteJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc )
{
	if( !isJointInPool( nameDesc.strJointName ) )
	{
		cPhysicObject* pActor[2];
		NxRevoluteJointDesc revoluteDesc;
		initFromNameDesc( nameDesc, revoluteDesc, pActor );
		initFromLocalDesc( revoluteDesc, localDesc );

		/*desc.flags |= NX_RJF_LIMIT_ENABLED;
		desc.limit.low.value = -0.6f;
		desc.limit.high.value = 0.6f;*/

		return createJoint( nameDesc.strJointName, revoluteDesc );
	}
	return NULL;
}

cPhysicJoint* cPhysicScene::addD6Joint( sD6JointDesc& desc )
{
	if( !isJointInPool( desc.nameDesc.strJointName ) )
	{
		cPhysicObject* pActor[2];
		NxD6JointDesc d6Desc;
		initFromNameDesc( desc.nameDesc, d6Desc, pActor );
		initFromLocalDesc( d6Desc, desc.localDesc );

		d6Desc.twistMotion = desc.bTwistMotion;
		d6Desc.swing1Motion = desc.bSwing1Motion;
		d6Desc.swing2Motion = desc.bSwing2Motion;

		d6Desc.xMotion = desc.bXMotion;
		d6Desc.yMotion = desc.bYMotion;
		d6Desc.zMotion = desc.bZMotion;

		d6Desc.linearLimit.value = desc.fLinearLimit;
		d6Desc.twistLimit.low.value = desc.fTwistLimitLow;
		d6Desc.twistLimit.high.value = desc.fTwistLimitHigh;

		d6Desc.swing1Limit.value = desc.fSwing1Limit;
		d6Desc.swing2Limit.value = desc.fSwing2Limit;

		return createJoint( desc.nameDesc.strJointName, d6Desc );
	}
	return NULL;
}

void cPhysicScene::initFromNameDesc( sJointNameDesc& nameDesc, NxJointDesc& desc, cPhysicObject** p2Actors )
{
	for( int i=0; i<2; i++ )
	{
		if( nameDesc.strActorName[i].empty() )
		{
			p2Actors[i] = NULL;
		}
		else
		{
			p2Actors[i] = findPhysicObjectInPool( nameDesc.strActorName[i] );

			if( p2Actors[i] )
			{
				p2Actors[i]->setJoint( &desc, i );
			}
			else
			{
				std::cout << "Error find physic actor: " << nameDesc.strActorName[i] << std::endl;
			}
		}
	}
}

void cPhysicScene::initFromLocalDesc( NxJointDesc& desc, sJointLocalDesc& localDesc )
{
	desc.localAnchor[0] = localDesc.vLocalAnchor[0];
	desc.localAnchor[1] = localDesc.vLocalAnchor[1];
	desc.localAxis[0] = localDesc.vLocalAxis[0];
	desc.localAxis[1] = localDesc.vLocalAxis[1];
	desc.localNormal[0] = localDesc.vLocalNormal[0];
	desc.localNormal[1] = localDesc.vLocalNormal[1];
}

cPhysicJoint* cPhysicScene::createJoint( std::string& strJointName, NxJointDesc& desc )
{
	cPhysicJoint* pJoint = new cPhysicJoint();
	if( pJoint->create( _pScene, desc ) )
	{
		_mJoints.insert( pair_physic_joint( strJointName, pJoint ) );
		return pJoint;
	}
	delete pJoint;
	return NULL;
}

void cPhysicScene::setBreakableJoint( std::string& strJointName, float fMaxForce, float fMaxTorque )
{
	cPhysicJoint* pJoint = findJointInPool( strJointName );

	if( pJoint )
	{
		pJoint->setBreakable( fMaxForce, fMaxTorque );
	}
}

void cPhysicScene::setGlobalAxisAtJoint( std::string& strJointName, NxVec3& vGlobalAxis )
{
	cPhysicJoint* pJoint = findJointInPool( strJointName );
	if( pJoint )
	{
		pJoint->setGlobalAxis( vGlobalAxis );
	}
}

void cPhysicScene::setGlobalAnchorAtJoint( std::string& strJointName, NxVec3& vGlobalAnchor )
{
	cPhysicJoint* pJoint = findJointInPool( strJointName );
	if( pJoint )
	{
		pJoint->setGlobalAnchor( vGlobalAnchor );
	}
}

cPhysicJoint* cPhysicScene::addPrismaticJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc )
{
	if( !isJointInPool( nameDesc.strJointName ) )
	{
		cPhysicObject* pActor[2];
		NxPrismaticJointDesc prismaticDesc;
		initFromNameDesc( nameDesc, prismaticDesc, pActor );
		initFromLocalDesc( prismaticDesc, localDesc );
		return createJoint( nameDesc.strJointName, prismaticDesc );
	}
	return NULL;
}

void cPhysicScene::setLimitPointAtJoint( std::string& strJointName, NxVec3& vPoint, bool bPointIsOnActor2 )
{
	cPhysicJoint* pJoint = findJointInPool( strJointName );
	if( pJoint )
	{
		pJoint->setLimitPoint( vPoint, bPointIsOnActor2 );
	}
}

void cPhysicScene::addLimitPlaneAtJoint( std::string& strJointName, NxVec3& vNormal, NxVec3& vPointInPlane )
{
	cPhysicJoint* pJoint = findJointInPool( strJointName );
	if( pJoint )
	{
		pJoint->addLimitPlane( vNormal, vPointInPlane );
	}
}

void cPhysicScene::create_vehicle()
{
	const float scale = 24.6f;

	cVehicleDesc vehicle_desc;
	vehicle_desc.centerOfMass.set( 0, -0.3*scale, 0 );
	//vehicle_desc.centerOfMass.set( 0.f, 0.f, 0.f );

	vehicle_desc.position.set( 0, 0, 37.f );
	vehicle_desc.mass = 1200;
	vehicle_desc.digitalSteeringDelta = 0.038f;
	vehicle_desc.steeringMaxAngle = 34.f;
	vehicle_desc.motorForce = 10000.f;
	vehicle_desc.maxVelocity = 80.f;
	vehicle_desc.cameraDistance = 8.0f*scale;

	bool monsterTruck = false;
	bool corvette = false;
	NxReal wheelRadius = 0.3f*scale;

	cWheelDesc wheelDesc[4];
	for(NxU32 i=0;i<4;i++)
	{
		//wheelDesc[i].wheelAxis.set(0,0,1);
		//wheelDesc[i].downAxis.set(0,-1,0);
		wheelDesc[i].wheelApproximation = 10;
		//wheelDesc[i].wheelFlags |= NX_WF_BUILD_LOWER_HALF;
		wheelDesc[i].wheelRadius = wheelRadius;//(monsterTruck)?wheelRadius*3.f:wheelRadius;
		wheelDesc[i].wheelWidth = 0.4f*scale;//(monsterTruck)?0.3f:0.1f;
		wheelDesc[i].wheelSuspension = 0.1f*scale;//(monsterTruck)?0.6f:0.2f;
		wheelDesc[i].springRestitution = 3000*scale;//monsterTruck?(corvette?5000:4000):7000;
		wheelDesc[i].springDamping = 400*scale;//800;
		wheelDesc[i].springBias = 0.0f;
		wheelDesc[i].maxBrakeForce = 4.8f;//monsterTruck?0.5f:1.f;
		wheelDesc[i].frictionToSide = 0.1f;
		wheelDesc[i].frictionToFront = 1.f;//0.1f;
		/*if (oldStyle)
		{
			wheelDesc[i].frictionToFront = 0.1f;
			wheelDesc[i].frictionToSide = corvette ? 0.1f : 0.99f;
		}
		else
		{
			wheelDesc[i].wheelFlags |= NX_WF_USE_WHEELSHAPE;
		}*/
		vehicle_desc.carWheels.pushBack(&wheelDesc[i] );
	}

	NxReal widthPos = 1.0f;//(monsterTruck)?1.4f:1.09f;
	widthPos *= scale;
	NxReal heightPos = -0.15f*scale;	//(monsterTruck)?1.f:
	wheelDesc[0].position.set( 1.41f*scale, heightPos, widthPos);
	wheelDesc[1].position.set( 1.41f*scale, heightPos,-widthPos);
	wheelDesc[2].position.set(-1.45f*scale, heightPos, widthPos);
	wheelDesc[3].position.set(-1.45f*scale, heightPos,-widthPos);
	NxU32 flags = WF_BUILD_LOWER_HALF || WF_USE_WHEELSHAPE;
	bool frontWheelDrive = true;
	bool backWheelDrive = true;
	wheelDesc[0].wheelFlags |= (frontWheelDrive?WF_ACCELERATED:0) | WF_STEERABLE_INPUT | flags;
	wheelDesc[1].wheelFlags |= (frontWheelDrive?WF_ACCELERATED:0) | WF_STEERABLE_INPUT | flags;
	wheelDesc[2].wheelFlags |= (backWheelDrive?WF_ACCELERATED:0) | WF_AFFECTED_BY_HANDBRAKE | flags;
	wheelDesc[3].wheelFlags |= (backWheelDrive?WF_ACCELERATED:0) | WF_AFFECTED_BY_HANDBRAKE | flags;

	vehicle_desc.steeringSteerPoint.set( 1.41*scale, 0, 0 );
	vehicle_desc.steeringTurnPoint.set( -1.45*scale, 0, 0 );

	//_pVehicle = cVehicle::create_vehicle( /*_pScene,*/this,  &vehicle_desc );
}

void cPhysicScene::control_vehicle( bool forward, bool backward, bool left, bool right, bool handbrake, bool b_stand_up )
{
	/*bool left = keyCode == 'j';
	bool right = keyCode == 'l';
	bool forward = keyCode == 'i';
	bool backward = keyCode == 'k';*/

	NxReal steering = 0;
	if (left && !right) steering = -1;
	else if (right && !left) steering = 1;
	NxReal acceleration = 0;
	if (forward && !backward) acceleration = 1;
	else if (backward && !forward) acceleration = -1;
	//bool handbrake = keyCode == ' ';
	
	/*_pVehicle->control(steering, false, acceleration, false, handbrake);

	if( b_stand_up )
	{
		_pVehicle->standUp();
	}*/
}

//static float m_f_last_axle_speed[4] = {0};
//static float m_f_cur_rotate[4] = {0};

/*const NxVec3 cPhysicScene::get_wheel_pos( int index, float dt )
{
	cRayCastWheel* pWheel = dynamic_cast<cRayCastWheel*>(_pVehicle->_wheels[index]);
	//pWheel->wheelShape->setSuspensionTravel( 10 );
	//float ff = pWheel->wheelShape->getSuspensionTravel();
	//std::cout << ff << std::endl;

	NxWheelContactData contact_data;
	pWheel->wheelShape->getContact( contact_data );

	float f_podveska = 0.4f*24.6f;
	float value = contact_data.contactPosition;
	if( abs( contact_data.contactPosition ) < 0.00000000001f )
		value = f_podveska;

	//wheel_pos.y += f_podveska - value;

	NxMat34 m_wheel = pWheel->getWheelLocalPose();
	m_wheel.t.y += f_podveska - value;

	NxMat34 m_rotate;
	m_rotate.M.rotY( NxHalfPiF32 + pWheel->wheelShape->getSteerAngle() );

	NxMat34 m_rotate_by_speed;
	
	float f_angle_axle_speed = pWheel->wheelShape->getAxleSpeed()/ NxTwoPi * 60.0f / 24.6f;
	float razniza = f_angle_axle_speed;// - m_f_last_axle_speed[index];
	razniza *= dt;
	m_f_last_axle_speed[index] = f_angle_axle_speed;

	m_f_cur_rotate[index] += razniza;

	//float f_time = osg::Timer::instance()->time_s();
	m_rotate_by_speed.M.rotZ( m_f_cur_rotate[index] );

	if( index==0 )
	{
		//;

		//std::cout << m_f_cur_rotate[index] << std::endl;
	}

	m_rotate = m_rotate*m_rotate_by_speed;

	m_wheel = m_wheel*m_rotate;


	NxMat34 m_car = _pVehicle->_bodyActor->getGlobalPose();
	//wheel_pos = m_car*wheel_pos;
	m_wheel = m_car*m_wheel;

	std::string str_wheel_name( "wheel0" );
	char buf[256];
	sprintf_s( buf, 256, "%d", index );
	str_wheel_name.append( buf );

	FindNamedNodeVisitor fnnv( str_wheel_name );
	if( findRenderNode( fnnv ) )
	{
		NxMat34 physxMatrix = m_wheel;
		osg::ref_ptr<osg::MatrixTransform> mTransform = fnnv._foundNodes.front().get();


		NxQuat physxQuat;
		physxMatrix.M.toQuat( physxQuat );

		float temp = physxQuat.y;
		physxQuat.y = physxQuat.z;
		physxQuat.z = temp;
		physxQuat.invert();
		physxMatrix.M.fromQuat( physxQuat );

		temp = physxMatrix.t.y;
		physxMatrix.t.y = physxMatrix.t.z;
		physxMatrix.t.z = temp;

		float glMat[16];
		physxMatrix.getColumnMajor44( glMat );
		osg::Matrix m_osg(glMat);
		mTransform.get()->setMatrix( m_osg );
	}

	//NxQuat local2Global = _pVehicle->_bodyActor->getGlobalOrientationQuat();
	//local2Global.inverseRotate(localVelocity);

	//return NxVec3(localVelocity.x, localVelocity.z, localVelocity.y );


	//NxVec3 wheel_pos = _pVehicle->_wheels[index]->getGlobalWheelPos();
	//return wheel_pos;
	//return NxVec3(wheel_pos.x, wheel_pos.z, wheel_pos.y);
	return NxVec3(0,0,0);
}*/

int cPhysicScene::create_material( const NxMaterialDesc& material_desc )
{
	NxMaterial* pMaterial = _pScene->createMaterial( material_desc );
	return pMaterial->getMaterialIndex();
}

void cPhysicScene::destroy_material( int material_index )
{
	NxMaterial* pMaterial = _pScene->getMaterialFromIndex( material_index );
	if( pMaterial )
	{
		_pScene->releaseMaterial( *pMaterial );
	}
	else
	{
		std::cout << "error to find material by index " << material_index << std::endl;
	}
}

unsigned int cPhysicScene::actor_get_num_shapes( const std::string& str_actor_name )
{
	cPhysicObject* pPhysicObject = findPhysicObjectInPool( str_actor_name );
	if( pPhysicObject )
		return pPhysicObject->actor()->getNbShapes();

	return -1;
}

bool cPhysicScene::actor_set_material_for_shape( const std::string& str_actor_name, unsigned int index_shape,
										  int index_material )
{
	cPhysicObject* pPhysicObject = findPhysicObjectInPool( str_actor_name );
	if( pPhysicObject )
	{
		if( index_shape < pPhysicObject->actor()->getNbShapes() )
		{
			pPhysicObject->actor()->getShapes()[index_shape]->setMaterial( index_material );
			return true;
		}
	}
	return false;
}

void cPhysicScene::actor_set_linear_velocity( const std::string& str_actor_name, const NxVec3& vVelocity )
{
	if( cPhysicObject* pPhysicObject = findPhysicObjectInPool( str_actor_name ) )
	{
		pPhysicObject->actor()->setLinearVelocity( vVelocity );
	}
}

void cPhysicScene::actor_set_angular_velocity( const std::string& str_actor_name, const NxVec3& vVelocity )
{
	if( cPhysicObject* pPhysicObject = findPhysicObjectInPool( str_actor_name ) )
	{
		pPhysicObject->actor()->setAngularVelocity( vVelocity );
	}
}

NxVec3 cPhysicScene::actor_get_linear_velocity( const std::string& str_actor_name )
{
	if( cPhysicObject* pPhysicObject = findPhysicObjectInPool( str_actor_name ) )
	{
		return pPhysicObject->actor()->getLinearVelocity();
	}
	return NxVec3();
}

cRayCastWheel* cPhysicScene::create_wheel( const std::string& str_body_name, const cWheelDesc& wheel_desc )
{
	cPhysicObject* pPhysicObject = findPhysicObjectInPool( str_body_name );
	osg::ref_ptr<osg::MatrixTransform> mTransform = findTransformMatrix( wheel_desc.str_wheel_name );
	if( pPhysicObject && NULL == findPhysicObjectInPool( wheel_desc.str_wheel_name ) )
	{
		cRayCastWheel* pWheel = new cRayCastWheel( pPhysicObject->actor(), wheel_desc, mTransform );
		_mObjects.insert( pair_physic_object( wheel_desc.str_wheel_name, pWheel ) );
		return pWheel;
	}
	
	return NULL;
}

cRayCastWheel* cPhysicScene::wheel( const std::string& str_wheel_name )
{
	cPhysicObject* pPhysicObject = findPhysicObjectInPool( str_wheel_name );
	if( pPhysicObject )
	{
		cRayCastWheel* pWheel = dynamic_cast<cRayCastWheel*>( pPhysicObject );
		assert( pWheel );
		return pWheel;
	}
	return NULL;
}

void cPhysicScene::actor_wakeup( const std::string& str_actor_name, float wakeCounterValue )
{
	cPhysicObject* pPhysicObject = findPhysicObjectInPool( str_actor_name );
	if( pPhysicObject )
	{
		pPhysicObject->actor()->wakeUp( wakeCounterValue );
	}
}

cPhysicObject* cPhysicScene::physic_object( const std::string& str_actor_name )
{
	return findPhysicObjectInPool( str_actor_name );
}

bool cPhysicScene::raycast( const std::string& str_render_name, float* pf_distance_out )
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor( str_render_name, true );
	_osgRoot->accept(*fnnv.get());
	const osg::Matrix& ogl_matrix = fnnv->accumulated;

	/*FindNamedNodeVisitor fnnv( strName );

	if( findRenderNode( fnnv ) )
		return fnnv.*/

	//osg::ref_ptr<osg::MatrixTransform> m = findTransformMatrix( str_render_name, true );
	//const osg::Matrix& ogl_matrix = m-> .get()->getMatrix();
	osg::Vec3 ogl_pos = ogl_matrix.getTrans();

	NxRay world_ray;
	world_ray.orig.set( ogl_pos.x(), ogl_pos.z(), ogl_pos.y() );
	world_ray.dir.set( ogl_matrix(0,0), ogl_matrix(1, 0), ogl_matrix(2, 0) );
	world_ray.dir.normalize();

	//std::cout << world_ray.dir.x << " "  << world_ray.dir.y << " "  << world_ray.dir.z << std::endl;


	//NxRay world_ray;

	NxRaycastHit raycast_hit;
	NxShape* pShape = _pScene->raycastClosestShape( world_ray, NX_ALL_SHAPES, raycast_hit );
	if( pShape )
	{
		NxActor& actor = pShape->getActor();
		if( true )
		{
			if( actor.getName() )
			{
				

			}
			//if( actor.getName()
			

		*pf_distance_out = raycast_hit.distance;

		//std::cout << world_ray.orig.x << " " << world_ray.orig.y << " " << world_ray.orig.z << " " << std::endl;
		//std::cout << world_ray.dir.x << " "  << world_ray.dir.y << " "  << world_ray.dir.z << " " <<  actor.getName() << " " << raycast_hit.distance << std::endl;
		std::cout << *pf_distance_out << std::endl;
		return true;
		}
	}

	return false;
}