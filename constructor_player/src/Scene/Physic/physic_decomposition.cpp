#include "./physic_decomposition.h"
#include <NxConvexShapeDesc.h>
#include <NxCooking.h>

bool cPhysicDecomposition::create( NxScene* pScene, std::vector<sBuildConvexResult*>* pvStreams, float fMassa )
{
	NxConvexShapeDesc* pmConvexShapeDescs = new NxConvexShapeDesc[pvStreams->size()];
	NxActorDesc actorDesc;

	for( size_t i=0; i<pvStreams->size(); i++ )
	{
		NxConvexMesh* pConvex = pScene->getPhysicsSDK().createConvexMesh( pvStreams->at(i)->stream );
		_vConvexMeshes.push_back( pConvex );

		pmConvexShapeDescs[i].meshData = pConvex;
		pmConvexShapeDescs[i].localPose.t =  pvStreams->at(i)->pos;
		actorDesc.shapes.push_back( pmConvexShapeDescs + i );
	}

	NxBodyDesc bodyDesc;
	if( fMassa != 0.f )
	{
		bodyDesc.mass = fMassa;
		actorDesc.body = &bodyDesc;
	}

	cPhysicObject::create( pScene, &actorDesc );

	// hide physic
	//_mTransform.get()->setNodeMask( 0x0 );

	delete[] pmConvexShapeDescs;
	return true;
}

void cPhysicDecomposition::destroy( NxScene* pScene )
{
	for( std::vector<NxConvexMesh*>::iterator itera = _vConvexMeshes.begin(); itera != _vConvexMeshes.end(); ++itera )
	{
		pScene->getPhysicsSDK().releaseConvexMesh( **itera );
	}
	
	cPhysicObject::destroy( pScene );
}

// ---------------------------------------------------------------------
cBuildConvexDecomposition::cBuildConvexDecomposition( NxCookingInterface* pCooking ): _pCooking( pCooking )
{
	
}

template<class Type>
struct sPred_delete
{
	void operator() ( Type& elem )
	{
		delete elem;
	}
};

cBuildConvexDecomposition::~cBuildConvexDecomposition()
{
	std::for_each( _vResult.begin(), _vResult.end(), sPred_delete<sBuildConvexResult*>() );
}

void cBuildConvexDecomposition::ConvexDecompResult( ConvexDecomposition::ConvexResult &result )
{
	sBuildConvexResult* pResult = new sBuildConvexResult;

	// calc centroid, to shift vertices around center of mass
	NxVec3 vCentroid( 0.f, 0.f, 0.f );
	//NxVec3 vLocalScale( 1.f, 1.f, 1.f );

	NxVec3 vertex;
	for( unsigned int i = 0; i < result.mHullVcount; i++ )
	{
		vertex.set( result.mHullVertices[i*3], result.mHullVertices[i*3+1], result.mHullVertices[i*3+2] );
		
		/*vertex.x *= vLocalScale.x;		// vertex = vertex * vLocalScale;
		vertex.y *= vLocalScale.y;
		vertex.z *= vLocalScale.z;*/

		//pVertices[i] = vertex;

		vCentroid += vertex;
	}
	vCentroid *= 1.f/(float(result.mHullVcount) );

	/*NxVec3* pVertices = new NxVec3[result.mHullTcount*3];
	for( size_t i=0; i<result.mHullTcount; i++ )
	{
		unsigned int index0 = result.mHullIndices[3*i];
		unsigned int index1 = result.mHullIndices[3*i + 1];
		unsigned int index2 = result.mHullIndices[3*i + 2];

		pVertices[3*i] = NxVec3(result.mHullVertices + 3*i) - vCentroid;
		pVertices[3*i + 1] = NxVec3(result.mHullVertices + 3*i + 1) - vCentroid;
		pVertices[3*i + 2] = NxVec3(result.mHullVertices + 3*i + 2) - vCentroid;
	}*/

	NxVec3* pVertices = new NxVec3[result.mHullVcount];
	for( size_t i=0; i<result.mHullVcount; i++ )
	{
		pVertices[i] = NxVec3(result.mHullVertices + 3*i) - vCentroid;
	}

	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices = result.mHullVcount; //mHullTcount*3;
	convexDesc.pointStrideBytes = sizeof(NxVec3);
	convexDesc.points = pVertices;
	convexDesc.flags = NX_CF_COMPUTE_CONVEX | NX_CF_FLIPNORMALS;

	//cCookStreamBuffer stream;
	if( _pCooking->NxCookConvexMesh( convexDesc, pResult->stream ) )
	{
		pResult->stream.toRead();
		pResult->pos = vCentroid;
		_vResult.push_back( pResult );
	}

	delete[] pVertices;	pVertices = NULL;
}