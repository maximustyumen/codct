// 86open@gmail.com		11.04.2010
#ifndef PHYSIC_VEHICLE_DESC_H
#define PHYSIC_VEHICLE_DESC_H
#pragma once

#include "wheel_desc.h"
#include <NxArray.h>
#include <NxShapeDesc.h>

class cVehicleDesc
{
public:
	NxArray<NxShapeDesc*> carShapes;
	NxArray<cWheelDesc*> carWheels;

	NxVec3 position;
	NxReal mass;
	NxReal motorForce;
	NxReal transmissionEfficiency;
	NxReal differentialRatio;

	NxVec3 steeringTurnPoint;
	NxVec3 steeringSteerPoint;
	NxReal steeringMaxAngle;

	NxVec3 centerOfMass;

	NxReal digitalSteeringDelta;
	NxReal maxVelocity;
	NxReal cameraDistance;

	void* userData;

	inline cVehicleDesc();
	inline void setToDefault();
	inline bool isValid() const;
};

inline cVehicleDesc::cVehicleDesc()
{
	setToDefault();
}

inline void cVehicleDesc::setToDefault()
{
	userData = NULL;
	transmissionEfficiency = 1.0f;
	differentialRatio = 1.0f;
	maxVelocity = 80;
	cameraDistance = 15.f;
	carWheels.clear();
}

inline bool cVehicleDesc::isValid() const
{
	for (NxU32 i = 0; i < carWheels.size(); i++) {
		if (!carWheels[i]->isValid())
			return false;
	}

	if (mass < 0)
		return false;

	return true;
}

#endif