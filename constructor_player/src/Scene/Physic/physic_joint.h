#ifndef PROTO_OPEN_PHYSIC_JOINT_H
#define PROTO_OPEN_PHYSIC_JOINT_H

#pragma once

#include <NxJointDesc.h>
#include <NxScene.h>
#include <NxMotorDesc.h>

class cPhysicJoint
{
public:
	cPhysicJoint();
	virtual ~cPhysicJoint();

	bool create( NxScene* pScene, NxJointDesc& desc );
	void destroy( NxScene* pScene );

	bool setBreakable( float fMaxForce, float fMaxTorque );
	void setGlobalAxis( NxVec3& vGlobalAxis );
	void setGlobalAnchor( NxVec3& vGlobalAnchor );
	void setLimitPoint( NxVec3& vPoint, bool bPointIsOnActor2 );
	void addLimitPlane( NxVec3& vNormal, NxVec3& vPointInPlane );

	void setMotor( NxMotorDesc& desc );
private:
	NxJoint* _pJoint;
};

#endif