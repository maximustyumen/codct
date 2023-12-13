#include "wheel.h"
#include <NxMaterial.h>

cRayCastWheel::cRayCastWheel( NxActor* a, const cWheelDesc& wheelDesc,
	osg::ref_ptr<osg::MatrixTransform>& mTransform ): _f_cur_rotate( 0.f )
{
	_pActor = a;
	NxScene * scene = &_pActor->getScene();
	
	//create a shared car wheel material to be used by all wheels
	static NxMaterial * wsm = 0;
	if( !wsm )
	{
		NxMaterialDesc m;
		m.flags |= NX_MF_DISABLE_STRONG_FRICTION;//NX_MF_DISABLE_FRICTION;
		wsm = scene->createMaterial(m);
	}

	NxWheelShapeDesc wheelShapeDesc;

	wheelShapeDesc.localPose.t = wheelDesc.position;
	NxQuat q;
	q.fromAngleAxis(90.0f, NxVec3(0,1,0));
	wheelShapeDesc.localPose.M.fromQuat(q);
	wheelShapeDesc.materialIndex = wsm->getMaterialIndex();
	wheelFlags = wheelDesc.wheelFlags;

	_height_modifier = (wheelDesc.wheelSuspension + wheelDesc.wheelRadius) / wheelDesc.wheelSuspension;

	wheelShapeDesc.suspension.spring = wheelDesc.springRestitution*_height_modifier;
	wheelShapeDesc.suspension.damper = 0;//wheelDesc->springDamping*heightModifier;
	wheelShapeDesc.suspension.targetValue = wheelDesc.springBias*_height_modifier;

	wheelShapeDesc.radius = wheelDesc.wheelRadius;
	wheelShapeDesc.suspensionTravel = wheelDesc.wheelSuspension; 
	wheelShapeDesc.inverseWheelMass = wheelDesc.inverseWheelMass;//0.1f;//0.02f;	//not given!? TODO

	wheelShapeDesc.lateralTireForceFunction.stiffnessFactor *= wheelDesc.frictionToSide;	
	wheelShapeDesc.longitudalTireForceFunction.stiffnessFactor *= wheelDesc.frictionToFront;	

	wheelShape = static_cast<NxWheelShape *>(_pActor->createShape(wheelShapeDesc));

	_mTransform = mTransform;
}

cRayCastWheel::~cRayCastWheel()
{
	//_pActor->releaseShape( *wheelShape );
	wheelShape = NULL;
	_pActor = NULL;
}

void cRayCastWheel::tick( bool handBrake, NxReal motorTorque, NxReal brakeTorque, NxReal dt )
{
	//motorTorque *= 0.1f;
	//brakeTorque *= 500.0f;
	if(handBrake && getWheelFlag( WF_AFFECTED_BY_HANDBRAKE ))
	{
		//brakeTorque = 1950.0f;
		wheelShape->setBrakeTorque(brakeTorque);

		wheelShape->setMotorTorque(0);
	}
	else
	{
		wheelShape->setBrakeTorque( brakeTorque );

		if(getWheelFlag( WF_ACCELERATED )) 
			wheelShape->setMotorTorque(motorTorque);
	}

	

	//wheelShape->setBrakeTorque(brakeTorque);
}

NxActor* cRayCastWheel::getTouchedActor() const
{
	NxWheelContactData wcd;
	NxShape * s = wheelShape->getContact(wcd);	
	return s ? &s->getActor() : 0;
}


NxMat34 cRayCastWheel::getWheelLocalPose() const
{
	return wheelShape->getLocalPose();
}

NxVec3 cRayCastWheel::getWheelPos() const
{
	return wheelShape->getLocalPosition();
}

NxVec3 cRayCastWheel::getGlobalWheelPos() const
{
	return wheelShape->getGlobalPosition();
}

void cRayCastWheel::setAngle(NxReal angle)
{
	wheelShape->setSteerAngle(-angle);
}

void cRayCastWheel::drawWheel(NxReal approx, bool debug) const
{
	//nothing, taken care of by built in visualization.
	NxWheelContactData wcd;
	NxShape* s = wheelShape->getContact(wcd);	
	if(!s) return;

	//printf(" f = %f %f %f\n",wcd.contactForce, wcd.longitudalSlip, wcd.longitudalImpulse);
/*
	glPushMatrix();
	glLoadIdentity();

	NxVec3 pVecBuffer[4];
	pVecBuffer[0] = wcd.contactPoint;
	pVecBuffer[1] = wcd.contactPoint + wcd.longitudalDirection;
	pVecBuffer[2] = wcd.contactPoint;
	pVecBuffer[3] = wcd.contactPoint + wcd.lateralDirection;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pVecBuffer);
	glDrawArrays(GL_LINES, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
*/
}

NxReal cRayCastWheel::getRpm() const
{
	return NxMath::abs(wheelShape->getAxleSpeed())/NxTwoPi * 60.0f;
}

void cRayCastWheel::updateTM( float f_dt )
{
	NxWheelContactData contact_data;
	wheelShape->getContact( contact_data );

	float f_podveska = wheelShape->getSuspensionTravel() * _height_modifier;
	float value = contact_data.contactPosition;
	float f_abs_contact_position = abs( contact_data.contactPosition );
	if( f_abs_contact_position < 0.00000000001f || f_abs_contact_position > 65536.f )
		value = f_podveska;

	//printf("%.2f   f_podveska=%.2f\n", contact_data.contactPosition, f_podveska );
	//std::cout << contact_data.contactPosition << std::endl;

	NxMat34 m_wheel = getWheelLocalPose();
	m_wheel.t.y += f_podveska - value;

	NxMat34 m_rotate;
	m_rotate.M.rotY( NxHalfPiF32 + wheelShape->getSteerAngle() );

	NxMat34 m_rotate_by_speed;

	float f_angle_axle_speed = wheelShape->getAxleSpeed()/NxPiF32 /*NxTwoPi*/ * 60.0f / 24.6f;
	float razniza = f_angle_axle_speed;
	razniza *= f_dt;
	//_f_last_axle_speed = f_angle_axle_speed;

	_f_cur_rotate += razniza;

	m_rotate_by_speed.M.rotZ( _f_cur_rotate );

	m_rotate = m_rotate*m_rotate_by_speed;

	m_wheel = m_wheel*m_rotate;


	NxMat34 m_car = _pActor->getGlobalPose();
	m_wheel = m_car*m_wheel;

	/*std::string str_wheel_name( "wheel0" );
	char buf[256];
	sprintf_s( buf, 256, "%d", index );
	str_wheel_name.append( buf );

	FindNamedNodeVisitor fnnv( str_wheel_name );
	if( findRenderNode( fnnv ) )*/

	{
		NxMat34 physxMatrix = m_wheel;
		//osg::ref_ptr<osg::MatrixTransform> mTransform = fnnv._foundNodes.front().get();


		NxQuat physxQuat;
		physxMatrix.M.toQuat( physxQuat );

		float temp = physxQuat.y;
		physxQuat.y = physxQuat.z;
		physxQuat.z = temp;
		physxQuat.invert();
		physxMatrix.M.fromQuat( physxQuat );

		temp = physxMatrix.t.y;
		physxMatrix.t.y = physxMatrix.t.z;
		physxMatrix.t.z = temp;

		float glMat[16];
		physxMatrix.getColumnMajor44( glMat );
		osg::Matrix m_osg(glMat);
		_mTransform.get()->setMatrix( m_osg );
	}	
}