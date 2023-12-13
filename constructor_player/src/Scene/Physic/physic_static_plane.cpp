#include "./physic_static_plane.h"
#include "NxPlaneShapeDesc.h"

void cPhysicStaticPlane::create( NxScene* pScene, NxVec3& vNormal, NxReal fD )
{
	NxPlaneShapeDesc planeDesc;
	planeDesc.normal = vNormal;
	planeDesc.d = fD;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack( &planeDesc );

	cPhysicObject::create( pScene, &actorDesc );
}