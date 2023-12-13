#include "./physic_sphere.h"
#include "NxSphereShapeDesc.h"

void cPhysicSphere::create( NxScene* pScene, NxReal fMassa, NxReal fRadius )
{
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = fRadius;
	//sphereDesc.mass = fMassa;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack( &sphereDesc );
	//actorDesc.globalPose.t.set( NxVec3( 0, 240, 0 ) );

	NxBodyDesc bodyDesc;
	bodyDesc.mass = fMassa;

	if( fMassa != 0.f )
	{		
		actorDesc.body = &bodyDesc;
	}	

	cPhysicObject::create( pScene, &actorDesc );
}