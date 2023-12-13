#ifndef PROTO_OPEN_PHYSIC_SPHERE_H
#define PROTO_OPEN_PHYSIC_SPHERE_H

#pragma once

#include "./physic_object.h"

class cPhysicSphere: public cPhysicObject
{
public:
	void create( NxScene* pScene, NxReal fMassa, NxReal fRadius );
private:
};

#endif