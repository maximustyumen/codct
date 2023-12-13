// 86open@gmail.com		12.04.2010
#ifndef PHYSIC_VEHICLE_H
#define PHYSIC_VEHICLE_H
#pragma once

#include "vehicle_desc.h"
#include "wheel.h"
#include <NxUserContactReport.h>
#include "./../physic_decomposition.h"

class cPhysicScene;

class cVehicle//: public cPhysicDecomposition
{
//private:
public:
	NxArray<cWheel*>		_wheels;
//private:
	cPhysicDecomposition* _pActor;
	NxActor*				_bodyActor;
	NxScene*				_nxScene;

	NxReal					_steeringWheelState;
	NxReal					_accelerationPedal;
	NxReal					_brakePedal;
	bool					_brakePedalChanged;
	bool					_handBrake;
	NxReal					_acceleration;

	NxReal					_digitalSteeringDelta;
	NxVec3					_steeringTurnPoint;
	NxVec3					_steeringSteerPoint;
	NxReal					_steeringMaxAngleRad;
	NxReal					_motorForce;
	NxReal					_transmissionEfficiency;
	NxReal					_differentialRatio;

	NxVec3					_localVelocity;
	bool					_braking;
	bool					_releaseBraking;
	NxReal					_maxVelocity;
	NxMaterial*				_carMaterial;
	NxReal					_cameraDistance;

	/*NxVec3				_trailBuffer[NUM_TRAIL_POINTS];
	NxU32					_nextTrailSlot;
	NxReal					_lastTrailTime;*/

	NxActor*				_mostTouchedActor;
	void					_computeMostTouchedActor();
	void					_computeLocalVelocity();
	NxReal					_computeAxisTorque();
	NxReal					_computeRpmFromWheels();
	NxReal					_computeMotorRpm( NxReal rpm );

	void					_updateRpms();

	NxReal					_getGearRatio();

	void					_controlSteering(NxReal steering, bool analogSteering);
	void					_controlAcceleration(NxReal acceleration, bool analogAcceleration);

public:
	void*					userData;

	cVehicle();
	~cVehicle();

	void updateVehicle(NxReal lastTimeStepSize);
	void control (NxReal steering, bool analogSteering, NxReal acceleration, bool analogAcceleration, bool handBrake);

	void draw(bool debug = false);

	void applyRandomForce();
	void standUp();

	NxReal getDriveVelocity() const { return NxMath::abs(_localVelocity.x); }

	NxReal					getMaxVelocity() { return _maxVelocity; }
	NxActor*				getActor() { return _bodyActor; }

	NxU32					getNbWheels() { return _wheels.size(); }
	const cWheel*			getWheel(NxU32 i) { NX_ASSERT(i < _wheels.size()); return _wheels[i]; }
	NxReal					getCameraDistance() { return _cameraDistance; }

	NxMat34 getGlobalPose() { return _bodyActor->getGlobalPose(); }

	static cVehicle* create_vehicle( /*NxScene* scene,*/ cPhysicScene* pPhysicScene, cVehicleDesc* vehicleDesc );

	float getSteeringVelocityKoef() const;
};

#endif