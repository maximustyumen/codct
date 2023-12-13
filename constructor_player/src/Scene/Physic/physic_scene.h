// -----------------------------------------------
// vip_open		[86open@gmail.com]		20.05.2009
// -----------------------------------------------
#ifndef PROTO_OPEN_PHYSIC_SCENE_H
#define PROTO_OPEN_PHYSIC_SCENE_H

#pragma once

#include <hash_map>
#include "../Visitors/FindNodeByName.h"
#include "../Visitors/FindGeometry.h"
#include "NxPhysics.h"
#include "./physic_object.h"
#include "./physic_joint.h"
#include "./physic_static_plane.h"
#include "./physic_sphere.h"
#include "./physic_box.h"
#include "./physic_mesh.h"
#include "./physic_decomposition.h"
#include "./vehicle/wheel.h"
#include <NxQuat.h>

typedef stdext::hash_map<std::string, cPhysicObject*> map_physic_object;
typedef std::pair<std::string, cPhysicObject*> pair_physic_object;
typedef stdext::hash_map<std::string, cPhysicJoint*> map_physic_joint;
typedef std::pair<std::string, cPhysicJoint*> pair_physic_joint;

struct sJointNameDesc
{
	std::string strJointName;
	std::string strActorName[2];
};

struct sJointLocalDesc
{
	sJointLocalDesc()
	{
		vLocalAnchor[0].set(0.f,0.f,0.f);
		vLocalAnchor[1].set(0.f,0.f,0.f);
		vLocalAxis[0].set(0.f,0.f,1.f);
		vLocalAxis[1].set(0.f,0.f,1.f);
		vLocalNormal[0].set(1.f,0.f,0.f);
		vLocalNormal[1].set(1.f,0.f,0.f);
	}

	NxVec3 vLocalAnchor[2];
	NxVec3 vLocalAxis[2];
	NxVec3 vLocalNormal[2];
};

struct sSphericalJointDesc
{
	sJointNameDesc nameDesc;
	sJointLocalDesc localDesc;

	bool bTwist;
	float fLowTwist;
	float fHighTwist;

	bool bSwing;
	float fSwingValue;
	NxVec3 vSwingAxis;
};

struct sD6JointDesc
{
	sJointNameDesc nameDesc;
	sJointLocalDesc localDesc;

	NxD6JointMotion bTwistMotion;
	NxD6JointMotion bSwing1Motion;
	NxD6JointMotion bSwing2Motion;

	NxD6JointMotion bXMotion;
	NxD6JointMotion bYMotion;
	NxD6JointMotion bZMotion;

	float fLinearLimit;
	float fTwistLimitLow;
	float fTwistLimitHigh;
	float fSwing1Limit;
	float fSwing2Limit;
};

class cPhysicContactReport;

class cVehicle;

// сцена физики
class cPhysicScene
{
public:
	cPhysicScene();
	virtual ~cPhysicScene();

	// internal cPhysic
	bool create( NxPhysicsSDK* pPhysicsSDK, NxCookingInterface* pCooking, osg::ref_ptr<osg::Group> root, float fGravity );
	void destroy( NxPhysicsSDK* pPhysicsSDK );

	bool reset();
	void update( float fDt );

	// добавить на сцену статическую плоскость
	cPhysicStaticPlane* addStaticPlane( std::string& strName, NxVec3& vNormal, float fD );
	// добавить на сцену статический меш
	cPhysicMesh* addStaticMesh( std::string& strName );
	// добавить на сцену описание сферической формой
	cPhysicSphere* addSphereBody( std::string& strName, float fMassa, float fRadius );
	// описание боксом
	cPhysicBox* addBoxBody( std::string& strName, float fMassa, NxVec3& vDimension );
	// описание выпуклыми телами
	cPhysicDecomposition* addDecompositionBody( const std::string& strName, const std::string& strRenderName, float fMassa, int iDepth );
		
	void setPosition( std::string& strName, NxVec3& vPos );
	void addForce( std::string& strName, NxVec3& vForce, cPhysicObject::eForceMode mode );
	void addLocalForce( std::string& strName, NxVec3& vForce, cPhysicObject::eForceMode mode );
	void addLocalTorque( std::string& strName, NxVec3& vTorque, cPhysicObject::eForceMode mode );
	void setMotorAtRevoluteJoint( std::string& strName, float fVelTarget, float fMaxForce, bool bFreeSpin );

	// установить центр масс для физического объекта
	void setShapeLocalPosition( std::string& strName, NxVec3& vPos );

	// joint
	cPhysicJoint* addFixedJoint( sJointNameDesc& nameDesc );
	cPhysicJoint* addDistanceJoint( sJointNameDesc& nameDesc, float fMinDistance, float fMaxDistance );
	cPhysicJoint* addSphericalJoint( sSphericalJointDesc& desc );
	cPhysicJoint* addRevoluteJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc );
	cPhysicJoint* addPrismaticJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc );
	cPhysicJoint* addCylindricalJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc );
	cPhysicJoint* addPointInPlaneJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc );
	cPhysicJoint* addPointOnLineJoint( sJointNameDesc& nameDesc, sJointLocalDesc& localDesc );
	cPhysicJoint* addD6Joint( sD6JointDesc& desc );

	// установить параметры разрушения соединения
	void setBreakableJoint( std::string& strJointName, float fMaxForce, float fMaxTorque );

	// установить глобальную ось для соединения
	void setGlobalAxisAtJoint( std::string& strJointName, NxVec3& vGlobalAxis );
	// установить глобальную точку соединения
	void setGlobalAnchorAtJoint( std::string& strJointName, NxVec3& vGlobalAnchor );

	// установить ограничение в виде точки
	void setLimitPointAtJoint( std::string& strJointName, NxVec3& vPoint, bool bPointIsOnActor2 );
	// установить ограничение плоскостью
	void addLimitPlaneAtJoint( std::string& strJointName, NxVec3& vNormal, NxVec3& vPointInPlane );

	inline NxScene* scene()	{	return _pScene;	}
	
	
	void create_vehicle();
	void control_vehicle( bool forward, bool backward, bool left, bool right, bool handbrake, bool b_stand_up );
	

	int create_material( const NxMaterialDesc& material_desc );
	void destroy_material( int material_index );
	unsigned int actor_get_num_shapes( const std::string& str_actor_name );
	bool actor_set_material_for_shape( const std::string& str_actor_name, unsigned int index_shape, int index_material );
	void actor_set_linear_velocity( const std::string& str_actor_name, const NxVec3& vVelocity );
	void actor_set_angular_velocity( const std::string& str_actor_name, const NxVec3& vVelocity );
	NxVec3 actor_get_linear_velocity( const std::string& str_actor_name );
	
	
	cRayCastWheel* create_wheel( const std::string& str_actor_name, const cWheelDesc& wheel_desc );
	cRayCastWheel* wheel( const std::string& str_wheel_name );

	void actor_wakeup( const std::string& str_actor_name, float wakeCounterValue );

	cPhysicObject* physic_object( const std::string& str_actor_name );

	bool raycast( const std::string& str_render_name, float* pf_distance_out );

private:
	void cleanup();
	
	// поиск рендер-модели в сцене ( osgRoot )
	osg::ref_ptr<osg::MatrixTransform> findTransformMatrix( const std::string& strName );
	bool findRenderNode( FindNamedNodeVisitor& fnnv );

	// поиск физического объекта в пуле
	bool isPhysicObjectInPool( const std::string& strName );
	cPhysicObject* findPhysicObjectInPool( const std::string& strName );

	// поиск соединения в пуле
	cPhysicJoint* findJointInPool( const std::string& strName );
	bool isJointInPool( const std::string& strName );

	// osg vertices -> physx vertices
	osg::ref_ptr<osg::Vec3Array> convertVertices( FindNamedNodeVisitor& fnnv );

	// инициализация
	void initFromNameDesc( sJointNameDesc& nameDesc, NxJointDesc& desc, cPhysicObject** p2Actors );
	void initFromLocalDesc( NxJointDesc& desc, sJointLocalDesc& localDesc );
	cPhysicJoint* createJoint( std::string& strJointName, NxJointDesc& desc );
 
	// members
	NxScene* _pScene;
	NxCookingInterface* _pCooking;
	cPhysicContactReport* _pContactReport;

	osg::ref_ptr<osg::Group> _osgRoot;

	map_physic_object _mObjects;
	map_physic_joint _mJoints;

	cVehicle* _pVehicle;
};

#endif