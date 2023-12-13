#include "./physic_mesh.h"
#include "NxTriangleMeshShapeDesc.h"

bool cPhysicMesh::create( NxScene* pScene, const NxStream& stream )
{
	_pTriangleMesh = pScene->getPhysicsSDK().createTriangleMesh( stream );
	if( _pTriangleMesh )
	{
		NxTriangleMeshShapeDesc triangleMeshShapeDesc;
		triangleMeshShapeDesc.meshData = _pTriangleMesh;

		NxActorDesc actorDesc;
		actorDesc.shapes.push_back( &triangleMeshShapeDesc );

		cPhysicObject::create( pScene, &actorDesc );
		return true;
	}
	return false;
}

void cPhysicMesh::destroy( NxScene* pScene )
{
	pScene->getPhysicsSDK().releaseTriangleMesh( *_pTriangleMesh );
	cPhysicObject::destroy( pScene );
}