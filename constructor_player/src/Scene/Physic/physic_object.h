// -----------------------------------------------
// vip_open		[86open@gmail.com]		20.05.2009
// -----------------------------------------------
#ifndef PROTO_OPEN_PHYSIC_OBJECT_H
#define PROTO_OPEN_PHYSIC_OBJECT_H

#pragma once

#include <NxActor.h>
#include <NxScene.h>
#include <string>
#include <osg/MatrixTransform>
#include <NxJointDesc.h>

// internal debug macros
#ifdef _DEBUG
	#define PHYSIC_OBJECT_SET_NAME( physic_object, szName )		(physic_object)->setName( szName );
#else
	#define PHYSIC_OBJECT_SET_NAME( physic_object, szName )
#endif

// базовый физический объект
class cPhysicObject: public osg::MatrixTransform
{
public:
	cPhysicObject();
	virtual ~cPhysicObject();

	void create( NxScene* pScene, NxActorDesc* pDesc );
	virtual void destroy( NxScene* pScene );

	void setTM( osg::ref_ptr<osg::MatrixTransform> matrix );
	virtual void updateTM( float f_dt );

	bool isStatic();
	bool isDynamic();

#ifdef _DEBUG
	void setName( const std::string& strName );
	void setName( const char* szName );
	const std::string& name() const;
#endif

	void setPosition( NxVec3& vPos );

	void setShapeLocalPosition( NxVec3& vPos );
	
	enum eForceMode
	{
		e_FORCE = NX_FORCE,
		e_IMPULSE = NX_IMPULSE,
		emax
	};
	void addForce( NxVec3& vForce, cPhysicObject::eForceMode mode );
	void addLocalForce( NxVec3& vForce, cPhysicObject::eForceMode mode );

	void addLocalTorque( NxVec3& vTorque, cPhysicObject::eForceMode mode );

	void setJoint( NxJointDesc* pDesc, int first );

	inline NxActor* actor()	{	return _pActor;	}

protected:
	NxActor* _pActor;

#ifdef _DEBUG
	std::string _strName;
#endif

/*private:
	class cPhysicTransformCallback: public osg::NodeCallback
	{
	public:
		cPhysicTransformCallback( cPhysicObject* pActor );
	private:
		virtual void operator()( osg::Node* pNode, osg::NodeVisitor* pNV );
	
		cPhysicObject* _pPhysicObject;
	};*/

protected:
	osg::ref_ptr<osg::MatrixTransform> _mTransform;
};


// inline
inline bool cPhysicObject::isStatic()
{
	return !_pActor->isDynamic();
}
inline bool cPhysicObject::isDynamic()
{
	return _pActor->isDynamic();
}


#endif