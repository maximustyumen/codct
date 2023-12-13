#ifndef PROTO_OPEN_PHYSIC_DECOMPOSITION_H
#define PROTO_OPEN_PHYSIC_DECOMPOSITION_H

#pragma once

#include <vector>
#include <algorithm>
#include "./physic_object.h"
#include "./extras decomposition/ConvexBuilder.h"
#include <NxConvexMeshDesc.h>
#include "./cook_stream.h"

struct sBuildConvexResult
{
	cCookStreamBuffer stream;
	NxVec3 pos;
};

class cPhysicDecomposition: public cPhysicObject
{
public:
	bool create( NxScene* pScene, std::vector<sBuildConvexResult*>* pvStreams, float fMassa );
	virtual void destroy( NxScene* pScene );
private:
	std::vector<NxConvexMesh*> _vConvexMeshes;
};

class NxCookingInterface;

class cBuildConvexDecomposition: public ConvexDecomposition::ConvexDecompInterface
{
public:
	// ctor
	cBuildConvexDecomposition( /*osg::ref_ptr<osg::MatrixTransform> _root*/ NxCookingInterface* pCooking );
	virtual ~cBuildConvexDecomposition();

	// result
	virtual void ConvexDecompResult( ConvexDecomposition::ConvexResult &result );

	std::vector<sBuildConvexResult*>* getResult()	{	return &_vResult;	}

private:
	NxCookingInterface* _pCooking;
	std::vector<sBuildConvexResult*> _vResult;
};

#endif