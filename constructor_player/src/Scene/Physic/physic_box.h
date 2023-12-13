#ifndef PROTO_OPEN_PHYSIC_BOX_H
#define PROTO_OPEN_PHYSIC_BOX_H

#pragma once

#include "./physic_object.h"

class cPhysicBox: public cPhysicObject
{
public:
	void create( NxScene* pScene, NxReal fMassa, NxVec3& vDimension );
	virtual void updateTM( float f_dt );
private:
};

#endif