#include "./physic_object.h"
#define NOMINMAX
#include <Windows.h>
#include <iostream>

cPhysicObject::cPhysicObject(): _pActor( NULL )
{
}

cPhysicObject::~cPhysicObject()
{
	/*if( _pActor )
	{
		destroy( &_pActor->getScene() );
	}*/
	
	assert( (_pActor == NULL) && "remove actor!" );
}

void cPhysicObject::create( NxScene* pScene, NxActorDesc* pDesc )
{
	osg::MatrixTransform* pMT;
	if( pMT = _mTransform.get() )
	{
		// pos
		osg::Matrix ogl_matrix = pMT->getMatrix();
		osg::Vec3 ogl_pos = ogl_matrix.getTrans();
		pDesc->globalPose.t.set( ogl_pos.x(), ogl_pos.z(), ogl_pos.y() );

		// rotate
		osg::Quat ogl_quat;
		ogl_matrix.get( ogl_quat );

		NxQuat physxQuat;
		physxQuat.x = ogl_quat.x();
		physxQuat.y = ogl_quat.z();
		physxQuat.z = ogl_quat.y();
		physxQuat.w = ogl_quat.w();
		physxQuat.invert();

		pDesc->globalPose.M.fromQuat( physxQuat );
	}
	
	pDesc->userData = (void*)this;
	_pActor = pScene->createActor( *pDesc );
	assert( _pActor );
	//setUpdateCallback( new cPhysicTransformCallback( this ) );
}

void cPhysicObject::destroy( NxScene* pScene )
{
	pScene->releaseActor( *_pActor );
	_pActor = NULL;
}

#ifdef _DEBUG
void cPhysicObject::setName( const std::string& strName )
{
	_strName = strName;
	_pActor->setName( _strName.c_str() );
}

void cPhysicObject::setName( const char* szName )
{
	_strName.assign( szName );
	_pActor->setName( szName );
}

const std::string& cPhysicObject::name() const
{
	return _strName;
}
#endif

void cPhysicObject::setTM( osg::ref_ptr<osg::MatrixTransform> matrix )
{
	_mTransform = matrix;
}

void cPhysicObject::updateTM( float f_dt )
{
	// танцы с бубном
	NxMat34 physxMatrix = _pActor->getGlobalPose();

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

	//osg::Matrix m_scale;
	//m_scale.makeScale( osg::Vec3f( 24.6f, 24.6f, 24.6f ) );

	//m_osg = m_scale*m_osg;

	_mTransform.get()->setMatrix( m_osg );
}

void cPhysicObject::setPosition( NxVec3& vPos )
{
	_pActor->setGlobalPosition( vPos );
}

void cPhysicObject::setShapeLocalPosition( NxVec3& vPos )
{
	NxShape* pShape = 	_pActor->getShapes()[0];
	pShape->setLocalPosition( vPos );
}

void cPhysicObject::addForce( NxVec3& vForce, cPhysicObject::eForceMode mode )
{
	_pActor->addForce( vForce, (NxForceMode)mode ); 
}

void cPhysicObject::addLocalForce( NxVec3& vForce, cPhysicObject::eForceMode mode )
{
	_pActor->addLocalForce( vForce, (NxForceMode)mode ); 
}

void cPhysicObject::setJoint( NxJointDesc* pDesc, int first )
{
	assert( _pActor );
	pDesc->actor[first] = _pActor;
}

void cPhysicObject::addLocalTorque( NxVec3& vTorque, cPhysicObject::eForceMode mode )
{
	_pActor->addLocalTorque( vTorque, (NxForceMode)mode );
}

// ------------------------------------------------------------------------------
/*cPhysicObject::cPhysicTransformCallback::cPhysicTransformCallback( cPhysicObject* pActor ): _pPhysicObject( pActor )
{
}

void  cPhysicObject::cPhysicTransformCallback::operator()( osg::Node* pNode, osg::NodeVisitor* pNV )
{
	//std::cout << _pPhysicObject->getName() << std::endl;
	if( _pPhysicObject->isDynamic() )
		_pPhysicObject->updateTM();
}*/