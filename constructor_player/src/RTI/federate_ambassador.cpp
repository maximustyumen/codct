#include "../RTI/federate_ambassador.h"
#include "../RTI/federate.h"

cFederateAmbassador::cFederateAmbassador( cFederate* p_federate ):
	_b_announced( false ),
	_b_ready_to_run( false ),
	_b_regulating( false ),
	_b_constrained( false ),
	_b_advancing( false ),
	_federate_time( 0.0 ),
    _federate_lookahead( 1.0 ),
	_p_federate( p_federate )//,
	//_p_on_change_callback( p_on_change_callback ),
	//_p_on_change_t_callback( p_on_change_t_callback )
{
}

cFederateAmbassador::~cFederateAmbassador()
        throw( RTI::FederateInternalError )
{
}


///////////////////////////////////////////////////////////////////////////////
/////////////////////// Synchronization Point Callbacks ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void cFederateAmbassador::synchronizationPointRegistrationSucceeded( const char* label )
	throw(RTI::FederateInternalError)
{
	std::cout << "Successfully registered sync point: " << label << std::endl;
}

void cFederateAmbassador::synchronizationPointRegistrationFailed( const char *label )
	throw(RTI::FederateInternalError)
{
	std::cout << "Failed to register sync point: " << label << std::endl;
}

void cFederateAmbassador::announceSynchronizationPoint( const char *label, const char *tag )
	throw(RTI::FederateInternalError)
{
	std::cout << "Synchronization point announced: " << label << std::endl;
        if( strcmp( label, FED_READY_TO_RUN ) == 0 )
		_b_announced = true;
}

void cFederateAmbassador::federationSynchronized( const char *label )
	throw(RTI::FederateInternalError)
{
	std::cout << "Federation Synchronized: " << label << std::endl;
        if( strcmp( label, FED_READY_TO_RUN ) == 0 )
		_b_ready_to_run = true;
}




///////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Time Callbacks ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double cFederateAmbassador::convertTime( const RTI::FedTime& theTime )
{
        RTIfedTime castedTime = (RTIfedTime)theTime;
        return castedTime.getTime();
}

void cFederateAmbassador::timeRegulationEnabled( const RTI::FedTime& theFederateTime )
        throw( RTI::InvalidFederationTime,
               RTI::EnableTimeRegulationWasNotPending,
               RTI::FederateInternalError )
{
	_b_regulating = true;
	_federate_time = convertTime( theFederateTime );
}

void cFederateAmbassador::timeConstrainedEnabled( const RTI::FedTime& theFederateTime )
        throw( RTI::InvalidFederationTime,
               RTI::EnableTimeConstrainedWasNotPending,
               RTI::FederateInternalError)
{
	_b_constrained = true;
	_federate_time = convertTime( theFederateTime );
}

void cFederateAmbassador::timeAdvanceGrant( const RTI::FedTime& theTime )
        throw( RTI::InvalidFederationTime,
               RTI::TimeAdvanceWasNotInProgress,
               RTI::FederateInternalError)
{
	_b_advancing = false;
	_federate_time = convertTime( theTime );
}








///////////////////////////////////////////////////////////////////////////////
///////////////////////// Object Management Callbacks /////////////////////////
///////////////////////////////////////////////////////////////////////////////

//                         //
// Discover Object Methods //
//                         //
void cFederateAmbassador::discoverObjectInstance( RTI::ObjectHandle theObject,
                                            RTI::ObjectClassHandle theObjectClass,
                                            const char* theObjectName )
        throw( RTI::CouldNotDiscover,
               RTI::ObjectClassNotKnown,
               RTI::FederateInternalError )
{
	/*std::cout << "Discoverd Object: handle=" << theObject
             << ", classHandle=" << theObjectClass
             << ", name=" << theObjectName << std::endl;*/

	cFederate::sObjectInstanceHandle object_instance_handle;
	object_instance_handle.handle = theObject;
	
	bool b_find = false;
	for( std::map<std::string, cFederate::sAttributes>::const_iterator itera = _p_federate->_m_classes_with_attributes_to_subscribe.begin(),
		end_itera = _p_federate->_m_classes_with_attributes_to_subscribe.end(); itera != end_itera; ++itera )
	{
		if( itera->second.class_handle == theObjectClass )
		{
			object_instance_handle.class_itera = itera;
			b_find = true;
			break;
		}
	}

	if( !b_find )
	{
		ErrorClass error( __FILE__, __LINE__ , "discoverObjectInstance()", "error find subscribe attribute class", 0 );
		error.print();
	}

	std::pair<std::map<std::string, cFederate::sObjectInstanceHandle>::iterator, bool> res_insert =
		_p_federate->_m_reflected_object_instance_handles.insert( 
			std::pair<std::string, cFederate::sObjectInstanceHandle>( theObjectName, object_instance_handle ) );

	// запоминаем итератор нового обнаруженного инстанса объекта
	_p_federate->_v_discover_object_instances.push_back( res_insert.first );

}

//                                 //
// Reflect Attribute Value Methods //
//                                 //
void cFederateAmbassador::reflectAttributeValues( RTI::ObjectHandle theObject,
                                            const RTI::AttributeHandleValuePairSet& theAttributes,
                                            const char *theTag )
        throw( RTI::ObjectNotKnown,
               RTI::AttributeNotKnown,
               RTI::FederateOwnsAttributes,
               RTI::FederateInternalError )
{
        /*std::cout << "Reflection Received:";

        // print the handle
        std::cout << " object=" << theObject;
        // print the tag
        std::cout << ", tag=" << theTag;

        // print the attribute information
        std::cout << ", attributeCount=" << theAttributes.size() << std::endl;
        for( RTI::ULong i = 0; i < theAttributes.size(); i++ )
        {
                // print the attribute handle
                std::cout << "\tattrHandle=" << theAttributes.getHandle(i);
                // print the attribute value
                RTI::ULong length = theAttributes.getValueLength(i);
                char *value = theAttributes.getValuePointer(i,length);

                std::cout << ", attrValue=" << value << std::endl;
        }*/		
	
	
	reflect_attribute_values( theObject, theAttributes, NULL, theTag, &_v_reflect_objects );

	//reflect_attribute_values( theObject, theAttributes, NULL, theTag );
}

void cFederateAmbassador::reflectAttributeValues( RTI::ObjectHandle theObject,
                                            const RTI::AttributeHandleValuePairSet& theAttributes,
                                            const RTI::FedTime& theTime,
                                            const char *theTag,
                                            RTI::EventRetractionHandle theHandle )
        throw( RTI::ObjectNotKnown,
               RTI::AttributeNotKnown,
               RTI::FederateOwnsAttributes,
               RTI::InvalidFederationTime,
               RTI::FederateInternalError )
{
        /*std::cout << "Reflection Received:";

        // print the handle
        std::cout << " object=" << theObject;
        // print the tag
        std::cout << ", tag=" << theTag;
        // print the time
        std::cout << ", time=" << convertTime( theTime );

        // print the attribute information
        std::cout << ", attributeCount=" << theAttributes.size() << std::endl;
        for( RTI::ULong i = 0; i < theAttributes.size(); i++ )
        {
                // print the attribute handle
                std::cout << "\tattrHandle=" << theAttributes.getHandle(i);
                // print the attribute value
                RTI::ULong length = theAttributes.getValueLength(i);
                char *value = theAttributes.getValuePointer(i,length);

                std::cout << ", attrValue=" << value << std::endl;
        }*/

        reflect_attribute_values( theObject, theAttributes, &theTime, theTag, &_v_reflect_objects );

	//reflect_attribute_values( theObject, theAttributes, NULL, theTag, &_v_reflect_objects_with_time );
}

void cFederateAmbassador::reflect_attribute_values( RTI::ObjectHandle theObject,
							  const RTI::AttributeHandleValuePairSet& theAttributes,
							  const RTI::FedTime* p_time, const char *theTag,
							  std::vector<sReflectObject>* p_out )
{
	/*const char* sz_object_name = NULL;
	try
	{
		sz_object_name = _rtiamb->getObjectInstanceName( theObject );
	}
	catch(...)
	{
		ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "getObjectInstanceName", 0 );
		error.print();
	}

	// узнаем хэндл класса объекта
	RTI::ObjectClassHandle class_handle;
	try
	{
		class_handle = _rtiamb->getObjectClass( theObject );
	}
	catch(...)
	{
		ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "getObjectClass", 0 );
		error.print();
	}

	for( RTI::ULong i = 0; i < theAttributes.size(); i++ )
	{
		const char* sz_attribute_name = NULL;
		try
		{
			sz_attribute_name = _rtiamb->getAttributeName( theAttributes.getHandle(i), class_handle );
		}
		catch(...)
		{
			ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "getAttributeName", 0 );
			error.print();
		}

		RTI::ULong length = theAttributes.getValueLength(i);
		const char* sz_value = theAttributes.getValuePointer( i, length );

		if( p_time )
		{
			if( _p_on_change_t_callback )
			{
				_p_on_change_t_callback->update( sz_object_name, sz_attribute_name, sz_value, theTag, 
					convertTime( *p_time ) );
			}
		}
		else
		{
			if( _p_on_change_callback )
			{
				_p_on_change_callback->update( sz_object_name, sz_attribute_name, sz_value, theTag );
			}
		}
	}*/

	

	// find object name
	for( std::map<std::string, cFederate::sObjectInstanceHandle>::const_iterator object_instance_handle_itera =
		_p_federate->_m_reflected_object_instance_handles.begin(), itera_end = _p_federate->_m_reflected_object_instance_handles.end();
		object_instance_handle_itera != itera_end; ++object_instance_handle_itera )
	{
		//std::cout << "theObject = " << theObject << "  object_instance_handle_itera->second.handle = " <<
		//	object_instance_handle_itera->second.handle << std::endl;

		if( object_instance_handle_itera->second.handle == theObject )
		{
			// now, we found object name :)
			sReflectObject reflect_object;
			reflect_object.str_name.assign( object_instance_handle_itera->first );

			reflect_object.str_tag.assign( theTag );
			reflect_object.time = p_time ? convertTime( *p_time ) : 0.;

			p_out->push_back( reflect_object );

			std::vector<sReflectObject>::iterator itera_reflect_objects = --p_out->end();
			for( RTI::ULong i = 0; i < theAttributes.size(); i++ )
			{
				sReflectAttribute attribute;
				
				// find attribute name
				const std::string* pstr_attr_name = object_instance_handle_itera->second.class_itera->second.find_by_attribute_handle( theAttributes.getHandle(i) );

					
				//attribute.str_name.assign( _rtiamb->getAttributeName( theAttributes.getHandle(i), class_handle ) );
				
				attribute.str_name.assign( *pstr_attr_name );

				RTI::ULong length = theAttributes.getValueLength(i);
				attribute.str_value.assign( theAttributes.getValuePointer( i, length ) );

				itera_reflect_objects->v_attributes.push_back( attribute );
			}

			return;
		}
	}
	
	ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "error find object instance name", 0 );
	error.print();
	
	
	/*sReflectObject reflect_object;
	try
	{
		char* sz_name = _rtiamb->getObjectInstanceName( theObject );
		
		//reflect_object.str_name.assign( sz_name );
		//delete sz_name;

		//reflect_object.str_name.assign("test");
	}
	catch(...)
	{
		ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "getObjectInstanceName", 0 );
		error.print();
	}*/

	/*reflect_object.str_tag.assign( theTag );
	reflect_object.time = p_time ? convertTime( *p_time ) : 0.;

	p_out->push_back( reflect_object );
*/
	// узнаем хэндл класса объекта
	/*RTI::ObjectClassHandle class_handle;
	try
	{
		class_handle = _rtiamb->getObjectClass( theObject );
	}
	catch(...)
	{
		ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "getObjectClass", 0 );
		error.print();
	}*/


	/*std::vector<sReflectObject>::iterator itera_reflect_objects = --p_out->end();
	for( RTI::ULong i = 0; i < theAttributes.size(); i++ )
	{
		sReflectAttribute attribute;
		try
		{
			attribute.str_name.assign( _rtiamb->getAttributeName( theAttributes.getHandle(i), class_handle ) );
		}
		catch(...)
		{
			ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "getAttributeName", 0 );
			error.print();
		}

		RTI::ULong length = theAttributes.getValueLength(i);
		attribute.str_value.assign( theAttributes.getValuePointer( i, length ) );

		itera_reflect_objects->v_attributes.push_back( attribute );
	}*/
}

//                             //
// Receive Interaction Methods //
//                             //
void cFederateAmbassador::receiveInteraction( RTI::InteractionClassHandle theInteraction,
                                        const RTI::ParameterHandleValuePairSet& theParameters,
                                        const char *theTag )
        throw( RTI::InteractionClassNotKnown,
               RTI::InteractionParameterNotKnown,
               RTI::FederateInternalError )
{
        /*std::cout << "Interaction Received:";

        // print the handle
        std::cout << " handle=" << theInteraction;
        // print the tag
        std::cout << ", tag=" << theTag;

        // print the attribute information
        std::cout << ", parameterCount=" << theParameters.size() << std::endl;
        for( RTI::ULong i = 0; i < theParameters.size(); i++ )
        {
                // print the parameter handle
                std::cout << "\tparamHandle=" << theParameters.getHandle(i);
                // print the parameter value
                RTI::ULong length = theParameters.getValueLength(i);
                char *value = theParameters.getValuePointer(i,length);

                std::cout << ", paramValue=" << value << std::endl;
        }*/

	reflect_parameter_values( theInteraction, theParameters, NULL, theTag,
		&_v_reflect_interactions );
}

void cFederateAmbassador::receiveInteraction( RTI::InteractionClassHandle theInteraction,
                                        const RTI::ParameterHandleValuePairSet& theParameters,
                                        const RTI::FedTime& theTime,
                                        const char *theTag,
                                        RTI::EventRetractionHandle theHandle )
        throw( RTI::InteractionClassNotKnown,
               RTI::InteractionParameterNotKnown,
               RTI::InvalidFederationTime,
               RTI::FederateInternalError )
{
        /*std::cout << "Interaction Received:";

        // print the handle
        std::cout << " handle=" << theInteraction;
        // print the tag
        std::cout << ", tag=" << theTag;
        // print the time
        std::cout << ", time=" << convertTime( theTime );

        // print the attribute information
        std::cout << ", parameterCount=" << theParameters.size() << std::endl;
        for( RTI::ULong i = 0; i < theParameters.size(); i++ )
        {
                // print the parameter handle
                std::cout << "\tparamHandle=" << theParameters.getHandle(i);
                // print the parameter value
                RTI::ULong length = theParameters.getValueLength(i);
                char *value = theParameters.getValuePointer(i,length);

                std:: cout << ", paramValue=" << value << std::endl;
        }*/

    reflect_parameter_values( theInteraction, theParameters, &theTime, theTag,
                &_v_reflect_interactions );

// 	reflect_parameter_values( theInteraction, theParameters, &theTime, theTag,
// 		&_v_reflect_interactions_with_time );
}

void cFederateAmbassador::reflect_parameter_values( RTI::InteractionClassHandle theInteraction,
							  const RTI::ParameterHandleValuePairSet& theParameters,
							  const RTI::FedTime* p_time, const char *theTag, std::vector<sReflectInteraction>* p_out )
{
	sReflectInteraction reflect_interaction;
	reflect_interaction.str_tag.assign( theTag );
	reflect_interaction.time = p_time ? convertTime( *p_time ) : 0.;

	//std::cout << "theInteraction = " << theInteraction << std::endl;

	std::map<std::string, cFederate::sParameters>::const_iterator parameters_itera = _p_federate->_m_parameters.begin();
	for( std::map<std::string, cFederate::sParameters>::const_iterator itera_end = _p_federate->_m_parameters.end(); parameters_itera != itera_end; ++parameters_itera )
	{
		if( parameters_itera->second.interaction_class_handle == theInteraction )
		{
			reflect_interaction.str_name.assign( parameters_itera->first );
			break;
		}
	}

	if( reflect_interaction.str_name.empty() )
	{
		ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflect_parameter_values", "find interaction name", 1 );
		error.print();
	}

	p_out->push_back( reflect_interaction );
	std::vector<sReflectInteraction>::iterator itera_reflect_interaction = --p_out->end();
	for( RTI::ULong i = 0; i < theParameters.size(); i++ )
	{
		sReflectParameter parameter;

		try
		{
			RTI::ParameterHandle param_handle = theParameters.getHandle(i);

			for( std::vector<cFederate::sParameter>::const_iterator itera_param = parameters_itera->second.v_params.begin(),
				itera_param_end = parameters_itera->second.v_params.end(); itera_param != itera_param_end; ++itera_param )
			{
				if( itera_param->handle == param_handle )
				{
					parameter.str_name.assign( itera_param->str_name );
					break;
				}
			}

			if( parameter.str_name.empty() )
			{
				throw ErrorClass ( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "getAttributeName", 0 );
			}
			
			//parameter.str_name.assign( _rtiamb->getParameterName( theParameters.getHandle(i), theInteraction ) );
		}
		catch( ErrorClass& error )
		{
			error.print();
		}

		RTI::ULong length = theParameters.getValueLength(i);
		parameter.str_value.assign( theParameters.getValuePointer( i, length ) );

		itera_reflect_interaction->v_params.push_back( parameter );
	}
	

	/*sReflectInteraction reflect_interaction;
	try
	{
		reflect_interaction.str_name.assign( _rtiamb->getInteractionClassName( theInteraction ) );
	}
	catch(...)
	{
		ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflect_parameter_values", "getInteractionClassName", 1 );
		error.print();
	}

	reflect_interaction.str_tag.assign( theTag );
	reflect_interaction.time = p_time ? convertTime( *p_time ) : 0.;

	p_out->push_back( reflect_interaction );

	std::vector<sReflectInteraction>::iterator itera_reflect_interaction = --p_out->end();
	for( RTI::ULong i = 0; i < theParameters.size(); i++ )
	{
		sReflectParameter parameter;

		try
		{
			parameter.str_name.assign( _rtiamb->getParameterName( theParameters.getHandle(i), theInteraction ) );
		}
		catch(...)
		{
			ErrorClass error( __FILE__, __LINE__ , "cFederateAmbassador::reflectAttributeValues", "getAttributeName", 0 );
			error.print();
		}

		RTI::ULong length = theParameters.getValueLength(i);
		parameter.str_value.assign( theParameters.getValuePointer( i, length ) );

		itera_reflect_interaction->v_params.push_back( parameter );
	}*/
}
//                       //
// Remove Object Methods //
//                       //
void cFederateAmbassador::removeObjectInstance( RTI::ObjectHandle theObject, const char *theTag )
        throw( RTI::ObjectNotKnown, RTI::FederateInternalError )
{
	remove_object_instance( theObject );
	std::cout << "Reflect Object Removed: handle=" << theObject << std::endl;
}

void cFederateAmbassador::removeObjectInstance( RTI::ObjectHandle theObject,
                                          const RTI::FedTime& theTime,
                                          const char *theTag,
                                          RTI::EventRetractionHandle theHandle)
        throw( RTI::ObjectNotKnown,
               RTI::InvalidFederationTime,
               RTI::FederateInternalError )
{
	remove_object_instance( theObject );
        std::cout << "Reflect Object Removed: handle=" << theObject  << std::endl;
}

void cFederateAmbassador::remove_object_instance(RTI::ObjectHandle object_handle)
{
	_p_federate->remove_reflected_object_instance(object_handle);
}
