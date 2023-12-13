// 86open@gmail.com		11.04.2010
#ifndef PHYSIC_WHEEL_H
#define PHYSIC_WHEEL_H
#pragma once

#include <cstdio>
#include "wheel_desc.h"
#include <NxActor.h>
#include <NxScene.h>
#include <NxWheelShape.h>
#include "../physic_object.h"

struct sContactInfo 
{
	sContactInfo() { reset(); }
	void reset() { otherActor = NULL; relativeVelocity = 0; }
	bool isTouching() const { return otherActor != NULL; }
	NxActor* otherActor;
	NxVec3 contactPosition;
	NxVec3 contactPositionLocal;
	NxVec3 contactNormal;
	NxReal relativeVelocity;
	NxReal relativeVelocitySide;
};


class cWheel: public cPhysicObject
{
public:
	virtual				~cWheel() {}
	virtual void		tick(bool handbrake, NxReal motorTorque, NxReal brakeTorque, NxReal dt) = 0;
	virtual NxActor*	getTouchedActor() const = 0;
	virtual NxVec3		getWheelPos() const = 0;
	virtual NxVec3		getGlobalWheelPos() const = 0;
	virtual void		setAngle(NxReal angle) = 0;
	virtual void		drawWheel(NxReal approx, bool debug = false) const = 0;
	virtual NxReal		getRpm() const = 0;
	virtual NxVec3		getGroundContactPos() const = 0;
	virtual float		getRadius() const = 0;

	inline bool			hasGroundContact() const { return getTouchedActor() != NULL; }
	inline bool			getWheelFlag(eWheelFlags flag) const { return (wheelFlags & flag) != 0; }

	virtual NxMat34 getWheelLocalPose() const = 0;

	void* userData;

protected:
	NxU32 wheelFlags;
};

class cRayCastWheel: public cWheel
{
public:
	cRayCastWheel( NxActor* actor, const cWheelDesc& wheelDesc, osg::ref_ptr<osg::MatrixTransform>& mTransform );
	virtual ~cRayCastWheel();

	virtual void		tick( bool handbrake, NxReal motorTorque, NxReal brakeTorque, NxReal dt );
	virtual NxActor*	getTouchedActor() const;
	virtual NxVec3		getWheelPos() const;
	virtual NxVec3		getGlobalWheelPos() const;
	virtual void		setAngle( NxReal angle );
	virtual void		drawWheel( NxReal approx, bool debug = false ) const;
	virtual NxReal		getRpm() const;
	virtual NxVec3		getGroundContactPos() const { return getWheelPos()+NxVec3(0, -wheelShape->getRadius(), 0); }
	virtual float		getRadius() const { return wheelShape->getRadius(); }

	virtual NxMat34 getWheelLocalPose() const;

	virtual void updateTM( float f_dt );

//private:
	//NxActor* _pVehicleActor;
	NxWheelShape* wheelShape;
	float _f_cur_rotate;
	NxReal _height_modifier;
};

/*class Wheel1 : public cWheel
{
public:
	friend class cWheel;

	Wheel1(NxScene * s);
	~Wheel1();

	//Wheel interface:

	virtual void			tick(bool handbrake, NxReal motorTorque, NxReal brakeTorque, NxReal dt);
	virtual NxActor *		getTouchedActor() const		{ return contactInfo.otherActor; }
	virtual NxVec3			getWheelPos() const { return wheelCapsule->getLocalPosition(); }
	virtual void			setAngle(NxReal angle);
	virtual void			drawWheel(NxReal approx, bool debug = false) const;
	virtual NxReal			getRpm() const { return NxMath::abs(_turnVelocity * 60.f); }
	virtual NxVec3			getGroundContactPos() const { return getWheelPos(); }
	virtual float			getRadius() const { return _radius; }

	ContactInfo				contactInfo;
private:

	void					getSteeringDirection(NxVec3& dir);
	void					updateContactPosition();
	void					updateAngularVelocity(NxReal lastTimeStepSize, bool handbrake);
	void					setWheelOrientation(const NxMat33& m) { wheelCapsule->setLocalOrientation(m); if (wheelConvex != NULL) wheelConvex->setLocalOrientation(m); }

	NxCapsuleShape*			wheelCapsule;
	NxConvexShape*			wheelConvex;
	NxScene *				scene;

	NxMaterial*				material;
	NxReal					_frictionToSide;
	NxReal					_frictionToFront;

	NxReal					_turnAngle;
	NxReal					_turnVelocity;
	NxReal					_radius;
	NxReal					_perimeter;

	NxReal					_angle;
	NxReal					_wheelWidth;
	NxReal					_maxSuspension;
	//NxReal					_rpm;
	NxVec3					_maxPosition;

};*/

#endif