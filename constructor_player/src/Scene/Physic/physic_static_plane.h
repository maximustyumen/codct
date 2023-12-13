#ifndef PROTO_OPEN_PHYSIC_STATIC_PLANE_H
#define PROTO_OPEN_PHYSIC_STATIC_PLANE_H

#pragma once

#include "./physic_object.h"

class cPhysicStaticPlane: public cPhysicObject
{
public:
	void create( NxScene* pScene, NxVec3& vNormal, NxReal fD );
private:
};

#endif