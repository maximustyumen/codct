// [86open@gmail.com]		01.06.2010
#ifndef PROTO_OPEN_RTI_FEDERATE_AMBASSADOR_H
#define PROTO_OPEN_RTI_FEDERATE_AMBASSADOR_H
#pragma once

#include <assert.h>
#include "../error/errorclass.h"
#include <RTI.hh>
#include <NullFederateAmbassador.hh>
#include "fedtime.hh"
#include <vector>

#define FED_READY_TO_RUN "ReadyToRun"

// helper-callback classes
/*class cAbstractFederateOnChangeAttribute
{
public:
	virtual void update( const char* sz_object_name, const char* sz_attribute_name, 
		const char* sz_value, const char* sz_tag ) = 0;
};

class cAbstractFederateOnChangeAttributeWithTime
{
public:
	virtual void update( const char* sz_object_name, const char* sz_attribute_name, 
		const char* sz_value, const char* sz_tag, double time ) = 0;
};*/

class cFederate;

class cFederateAmbassador: public NullFederateAmbassador
{
public:
	cFederateAmbassador( cFederate* p_federate );
        virtual ~cFederateAmbassador() throw( RTI::FederateInternalError );

	///////////////////////////////////
	// synchronization point methods
	virtual void synchronizationPointRegistrationSucceeded( const char *label )
		throw( RTI::FederateInternalError );

	virtual void synchronizationPointRegistrationFailed( const char *label )
		throw( RTI::FederateInternalError );

	virtual void announceSynchronizationPoint( const char *label, const char *tag )
		throw( RTI::FederateInternalError );

	virtual void federationSynchronized( const char *label )
		throw( RTI::FederateInternalError );

	//////////////////////////
	// time related methods
	virtual void timeRegulationEnabled( const RTI::FedTime& theFederateTime )
			throw( RTI::InvalidFederationTime,
				   RTI::EnableTimeRegulationWasNotPending,
				   RTI::FederateInternalError );

	virtual void timeConstrainedEnabled( const RTI::FedTime& theFederateTime )
			throw( RTI::InvalidFederationTime,
				   RTI::EnableTimeConstrainedWasNotPending,
				   RTI::FederateInternalError );

	virtual void timeAdvanceGrant( const RTI::FedTime& theTime )
			throw( RTI::InvalidFederationTime,
				   RTI::TimeAdvanceWasNotInProgress,
				   RTI::FederateInternalError );


	///////////////////////////////
	// object management methods //
	///////////////////////////////
	virtual void discoverObjectInstance( RTI::ObjectHandle theObject,
										 RTI::ObjectClassHandle theObjectClass,
										 const char* theObjectName )
			throw( RTI::CouldNotDiscover,
				   RTI::ObjectClassNotKnown,
				   RTI::FederateInternalError );

	virtual void reflectAttributeValues( RTI::ObjectHandle theObject,
										 const RTI::AttributeHandleValuePairSet& theAttributes,
										 const RTI::FedTime& theTime,
										 const char *theTag,
										 RTI::EventRetractionHandle theHandle)
			throw( RTI::ObjectNotKnown,
				   RTI::AttributeNotKnown,
				   RTI::FederateOwnsAttributes,
				   RTI::InvalidFederationTime,
				   RTI::FederateInternalError );

	virtual void reflectAttributeValues( RTI::ObjectHandle theObject,
										 const RTI::AttributeHandleValuePairSet& theAttributes,
										 const char *theTag )
			throw( RTI::ObjectNotKnown,
				   RTI::AttributeNotKnown,
				   RTI::FederateOwnsAttributes,
				   RTI::FederateInternalError );

	virtual void receiveInteraction( RTI::InteractionClassHandle theInteraction,
									 const RTI::ParameterHandleValuePairSet& theParameters,
									 const RTI::FedTime& theTime,
									 const char *theTag,
									 RTI::EventRetractionHandle theHandle )
			throw( RTI::InteractionClassNotKnown,
				   RTI::InteractionParameterNotKnown,
				   RTI::InvalidFederationTime,
				   RTI::FederateInternalError );

	virtual void receiveInteraction( RTI::InteractionClassHandle theInteraction,
									 const RTI::ParameterHandleValuePairSet& theParameters,
									 const char *theTag )
			throw( RTI::InteractionClassNotKnown,
				   RTI::InteractionParameterNotKnown,
				   RTI::FederateInternalError );

	virtual void removeObjectInstance( RTI::ObjectHandle theObject,
									   const RTI::FedTime& theTime,
									   const char *theTag,
									   RTI::EventRetractionHandle theHandle)
			throw( RTI::ObjectNotKnown,
				   RTI::InvalidFederationTime,
				   RTI::FederateInternalError );

	virtual void removeObjectInstance( RTI::ObjectHandle theObject, const char *theTag )
			throw( RTI::ObjectNotKnown, RTI::FederateInternalError );

	void remove_object_instance( RTI::ObjectHandle theObject );


	bool is_announced() const		{	return _b_announced;	}
	bool is_ready_to_run() const	{	return _b_ready_to_run;	}
	bool is_regulating() const		{	return _b_regulating;	}
	bool is_constrained() const		{	return _b_constrained;	}
	bool is_advancing() const		{	return  _b_advancing;	}

	double federate_time() const	{	return _federate_time;	}
	double federate_lookahead() const	{	return _federate_lookahead;	}

	void set_advancing( bool b_value )	{	_b_advancing = b_value;	}

public:
	struct sReflectAttribute
	{
		std::string str_name;
		std::string str_value;
	};

	struct sReflectObject
	{
		std::string str_name;
		std::string str_tag;
		double time;
		std::vector<sReflectAttribute> v_attributes;
	};

	std::vector<sReflectObject> _v_reflect_objects;
	//std::vector<sReflectObject> _v_reflect_objects_with_time;

private:
	void reflect_attribute_values( RTI::ObjectHandle theObject,
		const RTI::AttributeHandleValuePairSet& theAttributes,
		const RTI::FedTime* p_time, const char *theTag, std::vector<sReflectObject>* p_out );

public:
	struct sReflectParameter
	{
		std::string str_name;
		std::string str_value;
	};

	struct sReflectInteraction
	{
		std::string str_name;
		std::string str_tag;
		double time;
		std::vector<sReflectParameter> v_params;
	};

	std::vector<sReflectInteraction> _v_reflect_interactions;
	//std::vector<sReflectInteraction> _v_reflect_interactions_with_time;

private:
	void reflect_parameter_values( RTI::InteractionClassHandle theInteraction,
		const RTI::ParameterHandleValuePairSet& theParameters,
		const RTI::FedTime* p_time, const char *theTag, std::vector<sReflectInteraction>* p_out );


private:
	double convertTime( const RTI::FedTime& theTime );

	/*void reflect_attribute_values( RTI::ObjectHandle theObject,
		const RTI::AttributeHandleValuePairSet& theAttributes,
		const RTI::FedTime* p_time, const char *theTag );*/

	bool _b_announced;
	bool _b_ready_to_run;
	bool _b_regulating;
	bool _b_constrained;
	bool _b_advancing;

	double _federate_time;
	double _federate_lookahead;

	//RTI::RTIambassador* _rtiamb;
	cFederate* _p_federate;
};

#endif
