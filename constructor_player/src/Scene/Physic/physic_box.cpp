#include "./physic_box.h"
#include "NxBoxShapeDesc.h"
#include <iostream>

void cPhysicBox::create( NxScene* pScene, NxReal fMassa, NxVec3& vDimension )
{
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = vDimension;
	//boxDesc.localPose.t.set( 0.f, 12.f, 0.f );
	//sphereDesc.mass = fMassa;
	//boxDesc.materialIndex = 1;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack( &boxDesc );
	//actorDesc.globalPose.t.set( NxVec3( 0, 240, 0 ) );

	NxBodyDesc bodyDesc;
	bodyDesc.mass = fMassa;

	if( fMassa != 0.f )
	{		
		actorDesc.body = &bodyDesc;
	}	

	cPhysicObject::create( pScene, &actorDesc );
}

void cPhysicBox::updateTM( float f_dt )
{
	osg::MatrixTransform* pM = _mTransform.get();
	const osg::Matrix& mm = pM->getMatrix();

	osg::Vec3 v = mm.getTrans();
	//std::cout << v.x() << "  " << v.y() << "  " << v.z() << std::endl;

	cPhysicObject::updateTM( f_dt );

}