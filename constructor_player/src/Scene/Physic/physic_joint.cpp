#include "./physic_joint.h"
#include <NxRevoluteJoint.h>
cPhysicJoint::cPhysicJoint(): _pJoint( NULL )
{
}

cPhysicJoint::~cPhysicJoint()
{
	assert( NULL == _pJoint );
}

bool cPhysicJoint::create( NxScene* pScene, NxJointDesc& desc )
{
	desc.userData = this;
	_pJoint = pScene->createJoint( desc );
	return _pJoint ? true : false;
}

void cPhysicJoint::destroy( NxScene* pScene )
{
	pScene->releaseJoint( *_pJoint );
	_pJoint = NULL;
}

bool cPhysicJoint::setBreakable( float fMaxForce, float fMaxTorque )
{
	_pJoint->setBreakable( fMaxForce, fMaxTorque );
	return true;
}

void cPhysicJoint::setGlobalAxis( NxVec3& vGlobalAxis )
{
	_pJoint->setGlobalAxis( vGlobalAxis );
}

void cPhysicJoint::setGlobalAnchor( NxVec3& vGlobalAnchor )
{
	_pJoint->setGlobalAnchor( vGlobalAnchor );
}

void cPhysicJoint::setLimitPoint( NxVec3& vPoint, bool bPointIsOnActor2 )
{
	_pJoint->setLimitPoint( vPoint, bPointIsOnActor2 );
}

void cPhysicJoint::addLimitPlane( NxVec3& vNormal, NxVec3& vPointInPlane )
{
	_pJoint->addLimitPlane( vNormal, vPointInPlane );
}

void cPhysicJoint::setMotor( NxMotorDesc& desc )
{
	NxRevoluteJoint* pRevoluteJoint = _pJoint->isRevoluteJoint();
	if( pRevoluteJoint )
	{
		pRevoluteJoint->setMotor( desc );
	}
}