#ifndef PROTO_OPEN_PHYSIC_MESH_H
#define PROTO_OPEN_PHYSIC_MESH_H

#pragma once

#include "./physic_object.h"

class NxTriangleMesh;

class cPhysicMesh: public cPhysicObject
{
public:
	bool create( NxScene* pScene, const NxStream& stream );
	virtual void destroy( NxScene* pScene );
private:
	NxTriangleMesh* _pTriangleMesh;
};

#endif