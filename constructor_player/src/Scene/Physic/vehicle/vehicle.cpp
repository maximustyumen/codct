#include <map>
#include "vehicle.h"
#include <NxActorDesc.h>
#include <NxAllocateable.h>
#include <NxBoxShape.h>
#include <NxConvexShape.h>
#include <NxConvexShapeDesc.h>
#include <NxConvexMesh.h>
#include <NxMaterial.h>
#include "../physic_scene.h"

// ?
//#include "../../../MAXplayer.h"

cVehicle::cVehicle(): _steeringWheelState( 0 ), _nxScene( NULL ), _carMaterial( NULL )
{ 
	/*memset(_trailBuffer, 0, sizeof(NxVec3) * NUM_TRAIL_POINTS);
	_nextTrailSlot = 0;
	_lastTrailTime = 0.f;*/
}

cVehicle::~cVehicle()
{
	if( _carMaterial )
		_nxScene->releaseMaterial( *_carMaterial );

	/*if( _bodyActor )
		pNxScene->releaseActor(*_bodyActor);*/

	_pActor->destroy( _nxScene );

	for( NxU32 i = 0; i < _wheels.size(); i++ )
	{
		if(_wheels[i])
		{
			delete _wheels[i];
			_wheels[i] = NULL;
		}
	}
}

cVehicle* cVehicle::create_vehicle( cPhysicScene* pPhysicScene, /*NxScene* scene,*/ cVehicleDesc* vehicleDesc )
{
	NX_ASSERT( vehicleDesc );
	if( !vehicleDesc->isValid() )
	{
		printf("Vehicle Desc not valid!!\n");
		return NULL;
	}

	cVehicle* vehicle = new cVehicle;
	vehicle->_pActor = pPhysicScene->addDecompositionBody( std::string("car"), std::string("car_render"), 
		vehicleDesc->mass, 3 );

	vehicle->_bodyActor = vehicle->_pActor->actor();
	vehicle->_nxScene = pPhysicScene->scene();

	// setup material
	if( NULL == vehicle->_carMaterial )
	{
		NxMaterialDesc carMaterialDesc;
		carMaterialDesc.dynamicFriction = 0.9f;
		carMaterialDesc.staticFriction = 0.9f;
		carMaterialDesc.restitution = 0;
		carMaterialDesc.frictionCombineMode = NX_CM_MULTIPLY;
		vehicle->_carMaterial = vehicle->_nxScene->createMaterial( carMaterialDesc );
	}

	NxMaterialIndex mat_index = vehicle->_carMaterial->getMaterialIndex();
	NxU32 num_shapes = vehicle->_pActor->actor()->getNbShapes();
	for( NxU32 i=0; i<num_shapes; i++ )
	{
		vehicle->_bodyActor->getShapes()[i]->setMaterial( mat_index );
	}

	//NxActorDesc actorDesc;
	/*for(NxU32 i = 0; i < vehicleDesc->carShapes.size(); i++)
	{
		actorDesc.shapes.pushBack(vehicleDesc->carShapes[i]);
		if (actorDesc.shapes[i]->materialIndex == 0)
			actorDesc.shapes[i]->materialIndex = vehicle->_carMaterial->getMaterialIndex();
	}*/

	//vehicle->_bodyActor->setSleepEnergyThreshold( 0.05f );
	//vehicle->_bodyActor->setGlobalPosition( vehicleDesc->position );

	vehicle->_bodyActor->setCMassOffsetLocalPosition(vehicleDesc->centerOfMass);

	vehicle->_bodyActor->setLinearVelocity( NxVec3(0,0,0) );
	vehicle->_bodyActor->setAngularVelocity( NxVec3(0,0,0) );

	//NxBodyDesc bodyDesc;
	//bodyDesc.mass = vehicleDesc->mass;
	//bodyDesc.sleepEnergyThreshold = 0.05f;
	//actorDesc.body = &bodyDesc;
	//actorDesc.globalPose.t = vehicleDesc->position;


	//scene
	//_pActor = PhysicEngine->scene
	/*vehicle->_bodyActor = scene->createActor(actorDesc);
	if(vehicle->_bodyActor == NULL)
	{
		delete vehicle;
		return NULL;
	}*/
	//vehicle->_bodyActor->userData = vehicle;
	vehicle->_motorForce = vehicleDesc->motorForce;

	for(NxU32 i = 0; i < vehicleDesc->carWheels.size(); i++)
	{
		cWheel* wheel;// = new cRayCastWheel( vehicle->_bodyActor, *vehicleDesc->carWheels[i] );
		if(wheel)
		{
			vehicle->_wheels.pushBack(wheel);
		}
		else
		{
			delete vehicle;
			return NULL;
		}
	}

	vehicle->_digitalSteeringDelta		= vehicleDesc->digitalSteeringDelta;
	vehicle->_steeringSteerPoint		= vehicleDesc->steeringSteerPoint;
	vehicle->_steeringTurnPoint			= vehicleDesc->steeringTurnPoint;
	vehicle->_steeringMaxAngleRad		= NxMath::degToRad(vehicleDesc->steeringMaxAngle);
	vehicle->_transmissionEfficiency	= vehicleDesc->transmissionEfficiency;
	vehicle->_differentialRatio			= vehicleDesc->differentialRatio;
	vehicle->_maxVelocity				= vehicleDesc->maxVelocity;
	vehicle->_cameraDistance			= vehicleDesc->cameraDistance;


	//don't go to sleep.
	//vehicle->_bodyActor->wakeUp(1e10);

	vehicle->control( 0, true, 0, true, false );
	return vehicle;
}

float cVehicle::getSteeringVelocityKoef() const
{
	float f_velocity_koef = 1.f - getDriveVelocity()/550.f;
	if( f_velocity_koef < 0.32f )
		f_velocity_koef = 0.32f;

	f_velocity_koef = sinf(f_velocity_koef);

	//std::cout << f_velocity_koef << std::endl;
	return f_velocity_koef;
}

void cVehicle::updateVehicle(NxReal lastTimeStepSize)
{
	//printf("updating %x\n", this);
	
	NxReal distanceSteeringAxisCarTurnAxis = _steeringSteerPoint.x  - _steeringTurnPoint.x;
	NX_ASSERT(_steeringSteerPoint.z == _steeringTurnPoint.z);

	float steeringMaxAngleRad = _steeringMaxAngleRad*getSteeringVelocityKoef()*1.f;

	NxReal distance2 = 0;
	if (NxMath::abs(_steeringWheelState) > 0.01f)
		distance2 = distanceSteeringAxisCarTurnAxis / NxMath::tan(_steeringWheelState * steeringMaxAngleRad);

	//printf("d1 = %2.3f, d2 = %2.3f, a1 = %2.3f, a2 = %2.3f\n",
	//	distanceSteeringAxisCarTurnAxis, distance2,
	//	_steeringWheelState, _steeringWheelState * _steeringMaxAngleRad);


	/*_lastTrailTime += lastTimeStepSize;

	if(_lastTrailTime > TRAIL_FREQUENCY)
	{
		_lastTrailTime = 0.0f;
	}*/

	NxU32 nbTouching = 0;
	NxU32 nbNotTouching = 0;
	NxU32 nbHandBrake = 0;
	for(NxU32 i = 0; i < _wheels.size(); i++)
	{
		cWheel* wheel = _wheels[i];

		/*if (_lastTrailTime  == 0.0f)
		{
			if(_wheels[i]->hasGroundContact())
			{
				if (++_nextTrailSlot >= NUM_TRAIL_POINTS)
					_nextTrailSlot = 0;
				_trailBuffer[_nextTrailSlot] = _bodyActor->getGlobalPose() * _wheels[i]->getGroundContactPos();
				
			}
		}*/

		if(wheel->getWheelFlag( WF_STEERABLE_INPUT ))
		{
			if(distance2 != 0)
			{
				NxReal xPos = wheel->getWheelPos().x;
				NxReal zPos = wheel->getWheelPos().z;
				NxReal dz = -zPos + distance2;
				NxReal dx = xPos - _steeringTurnPoint.x;
				wheel->setAngle(NxMath::atan(dx/dz));
			} else {
				wheel->setAngle(0.f);
			}
			//printf("%2.3f\n", wheel->getAngle());

		} else if(wheel->getWheelFlag( WF_STEERABLE_AUTO ))
			{
			NxVec3 localVelocity = _bodyActor->getLocalPointVelocity(wheel->getWheelPos());
			NxQuat local2Global = _bodyActor->getGlobalOrientationQuat();
			local2Global.inverseRotate(localVelocity);
//			printf("%2.3f %2.3f %2.3f\n", wheel->getWheelPos().x,wheel->getWheelPos().y,wheel->getWheelPos().z);
			localVelocity.y = 0;
			if(localVelocity.magnitudeSquared() < 0.01f)
			{
				wheel->setAngle(0.0f);
			} else {
				localVelocity.normalize();
//				printf("localVelocity: %2.3f %2.3f\n", localVelocity.x, localVelocity.z);
				if(localVelocity.x < 0)
					localVelocity = -localVelocity;
				NxReal angle = NxMath::clamp((NxReal)atan(localVelocity.z / localVelocity.x), 0.3f, -0.3f);
				wheel->setAngle(angle);
			}
		}

		// now the acceleration part
		if( !wheel->getWheelFlag( WF_ACCELERATED ) )
			continue;

		if(_handBrake && wheel->getWheelFlag( WF_AFFECTED_BY_HANDBRAKE ))
		{
			nbHandBrake++;
		} else {
			if (!wheel->hasGroundContact())
			{
				nbNotTouching++;
			} else {
				nbTouching++;
			}
		}
	}
	
	NxReal motorTorque = 0.0f; 
	if(nbTouching && NxMath::abs(_accelerationPedal) > 0.01f) 
	{
		NxReal axisTorque = _computeAxisTorque();
		NxReal wheelTorque = axisTorque / (NxReal)(_wheels.size() - nbHandBrake);
		NxReal wheelTorqueNotTouching = nbNotTouching>0?wheelTorque*(NxMath::pow(0.5f, (NxReal)nbNotTouching)):0;
		NxReal wheelTorqueTouching = wheelTorque - wheelTorqueNotTouching;
		motorTorque = wheelTorqueTouching / 4;//(NxReal)nbTouching; 
	} else {
		_updateRpms();
	}
//printf("wt: %f %f\n", motorTorque, _brakePedal);
	for(NxU32 i = 0; i < _wheels.size(); i++) 
	{
		cWheel* wheel = _wheels[i];
		//if( i>2 )
		{
			//motorTorque = 0;
			wheel->tick(_handBrake, motorTorque, _brakePedal, lastTimeStepSize);
		}
	}

	//printf("---\n");
}

//finds the actor that is touched by most wheels.
void cVehicle::_computeMostTouchedActor()
{
	std::map<NxActor*, NxU32> actors;
	typedef std::map<NxActor*, NxU32> Map;
	for(NxU32 i = 0; i < _wheels.size(); i++)
	{
		NxActor* curActor = _wheels[i]->getTouchedActor();
		Map::iterator it = actors.find(curActor);
		if (it == actors.end())
		{
			actors[curActor] = 1;
		} else {
			it->second++;
		}
	}

	NxU32 count = 0;
	_mostTouchedActor = NULL;
	for(Map::iterator it = actors.begin(); it != actors.end(); ++it)
	{
		if(it->second > count)
		{
			count = it->second;
			_mostTouchedActor = it->first;
		}
	}
}

void cVehicle::_controlSteering(NxReal steering, bool analogSteering)
{
	

	if(analogSteering)
	{
		_steeringWheelState = steering;
	} else if (NxMath::abs(steering) > 0.0001f) {
		_steeringWheelState += NxMath::sign(steering) * _digitalSteeringDelta * getSteeringVelocityKoef();
	} else if (NxMath::abs(_steeringWheelState) > 0.0001f) {
		_steeringWheelState -= NxMath::sign(_steeringWheelState) * _digitalSteeringDelta * getSteeringVelocityKoef();
	}
	_steeringWheelState = NxMath::clamp(_steeringWheelState, 1.f, -1.f);
	//printf("SteeringWheelState: %2.3f\n", _steeringWheelState);
}

void cVehicle::_computeLocalVelocity()
{
	_computeMostTouchedActor();
	NxVec3 relativeVelocity;
	if (_mostTouchedActor == NULL || !_mostTouchedActor->isDynamic())
	{
		relativeVelocity = _bodyActor->getLinearVelocity();
	} else {
		relativeVelocity = _bodyActor->getLinearVelocity() - _mostTouchedActor->getLinearVelocity();
	}
	NxQuat rotation = _bodyActor->getGlobalOrientationQuat();
	NxQuat global2Local;
	_localVelocity = relativeVelocity;
	rotation.inverseRotate(_localVelocity);
	//printf("Velocity: %2.3f %2.3f %2.3f\n", _localVelocity.x, _localVelocity.y, _localVelocity.z);
}

void cVehicle::_controlAcceleration(NxReal acceleration, bool analogAcceleration)
{
	if(NxMath::abs(acceleration) < 0.001f)
		_releaseBraking = true;
	if(!_braking)
	{
		_accelerationPedal = NxMath::clamp(acceleration, 1.f, -1.f);
		_brakePedalChanged = _brakePedal == 0;
		_brakePedal = 0;
	} else {
		_accelerationPedal = 0;
		NxReal newv = NxMath::clamp(NxMath::abs(acceleration), 1.f, 0.f);
		_brakePedalChanged = _brakePedal == newv;
		_brakePedal = newv;
	}
	//printf("Acceleration: %2.3f, Braking %2.3f\n", _accelerationPedal, _brakePedal);
}

void cVehicle::control(NxReal steering, bool analogSteering, NxReal acceleration, bool analogAcceleration, bool handBrake)
{
	if (steering != 0 || acceleration != 0 || handBrake)
		_bodyActor->wakeUp(0.05f);

	_controlSteering(steering, analogSteering);
	_computeLocalVelocity();
	if (!_braking || _releaseBraking)
	{
		_braking = _localVelocity.x * acceleration < (-0.1f /* NxMath::sign(-acceleration)*/);
		_releaseBraking = false;
	}
	//printf("Braking: %s, Handbrake: %s\n", _braking?"true":"false", handBrake?"true":"false");
	if(_handBrake != handBrake)
	{
		_handBrake = handBrake;
		_brakePedalChanged;
	}
	_controlAcceleration(acceleration, analogAcceleration);


}

void cVehicle::draw(bool debug) 
{
	/*glPushMatrix();

	float glmat[16];
	_bodyActor->getGlobalPose().getColumnMajor44(glmat);
	glMultMatrixf(glmat);

	if(debug) glDisable(GL_LIGHTING);

	for(unsigned int i=0;i<_wheels.size(); i++) 
	{
		_wheels[i]->drawWheel(5, debug);
	}

	if(debug) glEnable(GL_LIGHTING);

	glPopMatrix();

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glPointSize(10.0f);

	const float alphaStart = 0.3f;
	float alpha = alphaStart;

	static NxVec3* pBufferTrailPos = NULL;
	static float* pBufferTrailColor = NULL;
	if(pBufferTrailPos == NULL)
	{
		pBufferTrailPos = new NxVec3[NUM_TRAIL_POINTS];
		pBufferTrailColor = new float[NUM_TRAIL_POINTS*4];
	}

	for(int x = NUM_TRAIL_POINTS-1; x >= 0; x--)
	{
		alpha-=alphaStart/NUM_TRAIL_POINTS;
		pBufferTrailColor[x*4+0] = 0.1f;
		pBufferTrailColor[x*4+1] = 0.1f;
		pBufferTrailColor[x*4+2] = 0.1f;
		pBufferTrailColor[x*4+3] = alpha;
		pBufferTrailPos[x]=_trailBuffer[(_nextTrailSlot+x)%NUM_TRAIL_POINTS];
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), pBufferTrailPos);
	glColorPointer(4, GL_FLOAT, 0, pBufferTrailColor);
	glDrawArrays(GL_POINTS, 0, NUM_TRAIL_POINTS);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);*/

}

NxReal cVehicle::_computeAxisTorque()
{
	/*if(_vehicleMotor != NULL)
	{
		NxReal rpm = _computeRpmFromWheels();
		NxReal motorRpm = _computeMotorRpm(rpm);
		_vehicleMotor->setRpm(motorRpm);
		NxReal torque = _accelerationPedal * _vehicleMotor->getTorque();
		NxReal v = _bodyActor->getLinearVelocity().magnitude();
		//printf("v: %2.3f m/s (%2.3f km/h)\n", v, v*3.6f);
		//printf("rpm %2.3f, motorrpm %2.3f, torque %2.3f, realtorque %2.3f\n",
		//	rpm, motorRpm, torque, torque*_getGearRatio()*_differentialRatio*_transmissionEfficiency);
		return torque * _getGearRatio() * _differentialRatio * _transmissionEfficiency;
	} else*/ {
		//_computeRpmFromWheels();
		return _accelerationPedal * _motorForce;
	}
}


NxReal cVehicle::_computeRpmFromWheels()
{
	NxReal wheelRpms = 0;
	NxI32 nbWheels = 0;
	for(NxU32 i = 0; i < _wheels.size(); i++)
	{
		cWheel* wheel = _wheels[i];
		if (wheel->getWheelFlag( WF_ACCELERATED ))
		{
			nbWheels++;
			wheelRpms += wheel->getRpm();
		}
	}
	return wheelRpms / (NxReal)nbWheels;
}

NxReal cVehicle::_computeMotorRpm(NxReal rpm)
{
	NxReal temp = _getGearRatio() * _differentialRatio;
	NxReal motorRpm = rpm * temp;
	/*if(_vehicleMotor)
	{
		NxI32 change;
		if(_vehicleGears && (change = _vehicleMotor->changeGears(_vehicleGears, 0.2f)))
		{
			if(change == 1)
			{
				gearUp();
			} else {
				NX_ASSERT(change == -1);
				gearDown();
			}
		}
		temp = _getGearRatio() * _differentialRatio;
		motorRpm = NxMath::max(rpm * temp, _vehicleMotor->getMinRpm());
	}*/
	return motorRpm;
}

NxReal cVehicle::_getGearRatio()
{
	//if(_vehicleGears == NULL)
	{
		return 1;
	}/* else {
		return _vehicleGears->getCurrentRatio();
	}*/
}

/*void cVehicle::gearUp()
{
	if (_vehicleGears)
	{
		printf("Changing gear from %d to", _vehicleGears->getGear());
		_vehicleGears->gearUp();
		printf(" %d\n", _vehicleGears->getGear());
	} else {
		printf("gearUp not supported if no gears available\n");
	}
}
void cVehicle::gearDown()
{
	if(_vehicleGears)
	{
		printf("Changing gear from %d to", _vehicleGears->getGear());
		_vehicleGears->gearDown();
		printf(" %d\n", _vehicleGears->getGear());
	} else {
		printf("gearDown not supported if no gears available\n");
	}
}*/

void cVehicle::applyRandomForce()
{
	NxVec3 pos(NxMath::rand(-4.f,4.f),NxMath::rand(-4.f,4.f),NxMath::rand(-4.f,4.f));
	NxReal force = NxMath::rand(_bodyActor->getMass()*0.5f, _bodyActor->getMass() * 2.f);
	_bodyActor->addForceAtLocalPos(NxVec3(0, force*100.f, 0), pos);
}

void cVehicle::standUp()
{
	NxVec3 pos = getActor()->getGlobalPosition() + NxVec3(0,2,0);
	NxQuat rot = getActor()->getGlobalOrientationQuat();
	NxVec3 front(1,0,0);
	rot.rotate(front);
	front.y = 0;
	front.normalize();

	NxReal dotproduct  = front.x;

	NxReal angle = NxMath::sign(-front.z) * NxMath::acos(dotproduct);

	rot.fromAngleAxis(NxMath::radToDeg(angle), NxVec3(0,1,0));
	getActor()->setGlobalPosition(pos);
	getActor()->setGlobalOrientationQuat(rot);
	getActor()->setLinearVelocity(NxVec3(0,0,0));
	getActor()->setAngularVelocity(NxVec3(0,0,0));
}

void cVehicle::_updateRpms()
{
	NxReal rpm = _computeRpmFromWheels();
	/*if(_vehicleMotor != NULL)
	{
		NxReal motorRpm = _computeMotorRpm(rpm);
		_vehicleMotor->setRpm(motorRpm);
	}*/
}
