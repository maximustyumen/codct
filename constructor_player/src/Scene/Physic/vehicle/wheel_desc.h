// 86open@gmail.com		11.04.2010
#ifndef PHYSIC_WHEEL_DESC_H
#define PHYSIC_WHEEL_DESC_H
#pragma once
#include <NxVec3.h>
#include <string>

enum eWheelFlags
{
	WF_STEERABLE_INPUT			= (1 << 0),
	WF_STEERABLE_AUTO			= (1 << 1),
	WF_AFFECTED_BY_HANDBRAKE	= (1 << 2),
	WF_ACCELERATED				= (1 << 3),

	WF_BUILD_LOWER_HALF		= (1 << 8),
	WF_USE_WHEELSHAPE		= (1 << 9),
};

class cWheelDesc
{
public:
	NxVec3 position;

	NxReal wheelRadius;
	NxReal wheelWidth;

	NxReal wheelSuspension;
	NxReal inverseWheelMass;

	NxReal springRestitution;
	NxReal springDamping;
	NxReal springBias;

	NxReal maxBrakeForce;
	NxReal frictionToSide;
	NxReal frictionToFront;

	NxU32 wheelApproximation;

	NxU32 wheelFlags;

	void* userData;
	std::string str_wheel_name;	// рендер-объект колеса
	//std::string str_body_name;	// рендер-объект корпуса

	inline cWheelDesc();
	inline void setToDefault();
	inline bool isValid() const;
};

inline cWheelDesc::cWheelDesc()
{
	setToDefault();
}

inline void cWheelDesc::setToDefault()
{
	position.set(0,0,0);
	userData = NULL;
	wheelFlags = 0;
	wheelRadius = 1;
	wheelWidth = 0.1f;

	springBias = 0;
	springRestitution = 1.f;
	springDamping = 0.f;

	wheelSuspension = 1.f;
	maxBrakeForce = 0.f;
	frictionToSide = 1.0f;
	frictionToFront = 1.0f;
	inverseWheelMass = 0.05f;
}

inline bool cWheelDesc::isValid() const
{
	if (wheelApproximation > 0 && wheelApproximation < 4) {
		fprintf(stderr, "wheelApproximation must be either 0 or bigger than 3\n");
		return false;
	}
	if ((wheelFlags & WF_STEERABLE_AUTO) && (wheelFlags & WF_STEERABLE_INPUT)) {
		fprintf(stderr, "WF_STEERABLE_AUTO and WF_STEERABLE_INPUT not permitted at the same time\n");
		return false;
	}
	return true;
}

#endif