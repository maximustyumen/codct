#include "../RTI/federate.h"
using namespace std;

//static std::string s_str_execution_name( "ExampleFederation" );

cFederate::cFederate():
	_rtiamb( NULL ),
	_fedamb( NULL ),
	_b_creator_federation_execution( false ),
	_b_run( false )
{
    //#define RTI_VERSION "CERTI_3.4.1"
    //std::cout << "RTI_VERSION = " << RTI_VERSION << std::endl;
}

cFederate::~cFederate()
{
	cleanup();
}

void cFederate::cleanup()
{
	if( _rtiamb )
	{
		// delete objects
		for(object_instances_t::iterator it=_m_object_instance_handles.begin(); it!=_m_object_instance_handles.end(); ++it)
		{
			_rtiamb->deleteObjectInstance(it->second.handle, "delete object instance");
		}

		// unpublish class
		for(classes_with_attributes_t::iterator it=_m_classes_with_attributes_to_publish.begin(); it!=_m_classes_with_attributes_to_publish.end(); ++it)
		{
			_rtiamb->unpublishObjectClass(it->second.class_handle);
		}

		// unsubscribe class
		for(classes_with_attributes_t::iterator it=_m_classes_with_attributes_to_subscribe.begin(); it!=_m_classes_with_attributes_to_subscribe.end(); ++it)
		{
			_rtiamb->unsubscribeObjectClass(it->second.class_handle);
		}
		
		// unpublish interaction
		for(interactions_t::iterator it=_v_interaction_classes_to_publish.begin(); it!=_v_interaction_classes_to_publish.end(); ++it)
		{
			_rtiamb->unpublishInteractionClass(*it);
		}

		// unsubscribe interaction
		for(interactions_t::iterator it=_v_interaction_classes_to_subscribe.begin(); it!=_v_interaction_classes_to_subscribe.end(); ++it)
		{
			_rtiamb->unsubscribeInteractionClass(*it);
		}

		////////////////////////////////////
		// resign from the federation //
		////////////////////////////////////
		try
		{
			_rtiamb->resignFederationExecution( RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES );
			std::cout << "Resigned from Federation" << std::endl;
		}
		catch(...)
		{
			std::cout << "Error to resigned from federation" << std::endl;
		}

		// try and destroy the federation
		// NOTE: we won't die if we can't do this because other federates
		//       remain. in that case we'll leave it for them to clean up
		//if(_b_creator_federation_execution)
		{
			try
			{
				_rtiamb->destroyFederationExecution( federationName.c_str() );
				std::cout << "Destroyed Federation" << std::endl;
			}
			catch( RTI::FederationExecutionDoesNotExist dne )
			{
				std::cout << "No need to destroy federation, it doesn't exist" << std::endl;
			}
			catch( RTI::FederatesCurrentlyJoined fcj )
			{
				std::cout << "Didn't destroy federation, federates still joined" << std::endl;
			}
			catch( RTI::RTIinternalError error )
			{
				std::cout << "Internal error on destroy federation execution" << std::endl;
			}
		}

		delete _fedamb;
		_fedamb = NULL;

		delete _rtiamb;
		_rtiamb = NULL;
	}

	_m_object_instance_handles.clear();
	_m_reflected_object_instance_handles.clear();
	_v_discover_object_instances.clear();

	_m_classes_with_attributes_to_publish.clear();
	_m_classes_with_attributes_to_subscribe.clear();

	_v_interaction_classes_to_publish.clear();
	_v_interaction_classes_to_subscribe.clear();

	_m_parameters.clear();

	_b_creator_federation_execution = false;
	_b_run = false;
}

bool cFederate::create( const char* sz_fed_name, const char* sz_fed_file, std::string federation_name )
{
	try
	{
        //имя федерации
        federationName =  federation_name;

		try
		{
			assert( NULL == _rtiamb );
			_rtiamb = new RTI::RTIambassador();
		}
		catch(...)
		{
			_rtiamb = NULL;
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::create", "Run RTIG.exe!!!", 1 );
		}

		// create and join to the federation
		// note: some other federate may have already created the federation, in that case, we'll just try and join it
		try
		{
            _rtiamb->createFederationExecution( federationName.c_str(), sz_fed_file );
			std::cout << "Created federation: " << sz_fed_file << std::endl;
			_b_creator_federation_execution = true;
		}
		catch( RTI::FederationExecutionAlreadyExists exists )
		{
			std::cout << "Didn't create federation, it already existed" << std::endl;
		}
		catch( RTI::CouldNotOpenFED exists )
		{
			std::string str_description( "Could not open fed file: " );
			str_description.append( sz_fed_file );
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::create", str_description, 1 );
		}
		catch( RTI::ErrorReadingFED exists )
		{
			std::string str_description( "Error reading fed file: " );
			str_description.append( sz_fed_file );
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::create", str_description, 1 );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::create", "May be need to run RTIG.exe?", 1 );
		}

		// create the federate ambassador and join the federation
		_fedamb = new cFederateAmbassador( this );

		try
		{
			_rtiamb->joinFederationExecution( sz_fed_name, federationName.c_str(), _fedamb );
			std::cout << "Joined federation as " << sz_fed_name << std::endl;
		}
		catch( RTI::FederateAlreadyExecutionMember )
		{
			std::string str_description;
			str_description.append( "Failed to join federation as '" );
			str_description.append( sz_fed_name );
			str_description.append( "', because duplicate name" );
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::create", str_description, 1 );
		}
		catch(...)
		{
			std::string str_description( "Failed to join federation as " );
			str_description.append( sz_fed_name );
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::create", str_description, 1 );
		}
	}
	catch( ErrorClass& e )
	{
		e.print();
		return false;
	}

	return true;
}

void cFederate::destroy()
{
	cleanup();
}

// регистрация точки синхронизации
void cFederate::register_synchronization_point()
{
	// announce a sync point to get everyone on the same page. if the point
	// has already been registered, we'll get a callback saying it failed,
	// but we don't care about that, as long as someone registered it
	try
	{
		if( _b_creator_federation_execution )
		{
			_rtiamb->registerFederationSynchronizationPoint( FED_READY_TO_RUN, "" );
		}
	}
	catch(...)
	{
		std::cout << "internal error in register_synchronization_point()" << std::endl;
	}

	while( !_fedamb->is_announced() )
	{
		_rtiamb->tick();
	}
}

// запуск федерата
void cFederate::run( bool b_time_regulating, bool b_time_constrained, bool blocking )
{
	// achieve the point and wait for synchronization
	// tell the RTI we are ready to move past the sync point and then wait
	// until the federation has synchronized on
	try
	{
		_rtiamb->synchronizationPointAchieved( FED_READY_TO_RUN );
		std::cout << "Achieved sync point: " << FED_READY_TO_RUN << ", waiting for federation..." << std::endl;
	}
	catch(...)
	{
		std::cout << "Fail synchronizationPointAchieved!!!" << std::endl;
	}

	if (blocking)
	{
		while( !_fedamb->is_ready_to_run() )
		{
			_rtiamb->tick();
		}
	}

	// enable time policies
	// in this section we enable/disable all time policies
	// note that this step is optional!
	enable_time_policy( b_time_regulating, b_time_constrained );
	std::cout << "Time Policy Enabled" << std::endl;

	// publish and subscribe
	// in this section we tell the RTI of all the data we are going to
	// produce, and all the data we want to know about
	publish_and_subscribe();
	std::cout << "Published and Subscribed" << std::endl;

	////
	try
	{
		if( _b_creator_federation_execution )
		{
			_rtiamb->registerFederationSynchronizationPoint( FED_READY_TO_RUN, "" );
		}
	}
	catch(...)
	{
		std::cout << "internal error in run()" << std::endl;
	}
	////

	_b_run = true;
}

// this method will attempt to enable the various time related properties for the federate
void cFederate::enable_time_policy( bool b_time_regulating, bool b_time_constrained )
{
	////////////////////////////
	// enable time regulation //
	////////////////////////////
	if( b_time_regulating )
	{
		RTIfedTime federateTime = _fedamb->federate_time();
		RTIfedTime lookahead = _fedamb->federate_lookahead();
		_rtiamb->enableTimeRegulation( federateTime, lookahead );

		// tick until we get the callback
		while( !_fedamb->is_regulating() )
		{
			_rtiamb->tick();
		}
	}

	/////////////////////////////
	// enable time constrained //
	/////////////////////////////
	if( b_time_constrained )
	{
		_rtiamb->enableTimeConstrained();

		// tick until we get the callback
		while( !_fedamb->is_constrained() )
		{
			_rtiamb->tick();
		}
	}
}

void cFederate::publish_and_subscribe()
{
	// before we can register instance of the object class and
	// update the values of the various attributes, we need to tell the RTI
	// that we intend to publish this information
	for( std::map<std::string, sAttributes>::iterator class_itera = _m_classes_with_attributes_to_publish.begin(),
		class_itera_end = _m_classes_with_attributes_to_publish.end(); class_itera != class_itera_end; ++class_itera )
	{
		// package the information into a handle set
		RTI::AttributeHandleSet* attributes = RTI::AttributeHandleSetFactory::create( class_itera->second.m_attributes.size() );

		for( std::map<std::string, RTI::AttributeHandle>::iterator attribute_itera = class_itera->second.m_attributes.begin(),
			attribute_itera_end = class_itera->second.m_attributes.end(); attribute_itera != attribute_itera_end; ++attribute_itera )
		{
			attributes->add( attribute_itera->second );
		}

		// do the actual publication
		_rtiamb->publishObjectClass( class_itera->second.class_handle, *attributes );

		// clean up
		delete attributes;
	}

	// subscribe attributes
	for( std::map<std::string, sAttributes>::iterator class_itera = _m_classes_with_attributes_to_subscribe.begin(),
		class_itera_end = _m_classes_with_attributes_to_subscribe.end(); class_itera != class_itera_end; ++class_itera )
	{
		RTI::AttributeHandleSet* attributes = RTI::AttributeHandleSetFactory::create( class_itera->second.m_attributes.size() );

		for( std::map<std::string, RTI::AttributeHandle>::iterator attribute_itera = class_itera->second.m_attributes.begin(),
			attribute_itera_end = class_itera->second.m_attributes.end(); attribute_itera != attribute_itera_end; ++attribute_itera )
		{
			attributes->add( attribute_itera->second );
		}

		// we also want to hear about the same sort of information as it is
		// created and altered in other federates, so we need to subscribe to it
		//if( !_b_creator_federation_execution )
		try
		{
			_rtiamb->subscribeObjectClassAttributes( class_itera->second.class_handle, *attributes );
		}
		catch(RTI::FederateNotExecutionMember)
		{
			std::cout << "FederateNotExecutionMember in subscribe object class handle=" << class_itera->second.class_handle << std::endl;
		}
		catch(...)
		{
			std::cout << "internal error in subscribe object class handle=" << class_itera->second.class_handle << std::endl;
		}

		// clean up
		delete attributes;
	}

	// also publish interaction classes
	for( std::vector<RTI::InteractionClassHandle>::const_iterator itera = _v_interaction_classes_to_publish.begin(),
		itera_end = _v_interaction_classes_to_publish.end(); itera != itera_end; ++itera )
	{
		_rtiamb->publishInteractionClass( *itera );
	}

	// subscribe interaction
	for( std::vector<RTI::InteractionClassHandle>::const_iterator itera = _v_interaction_classes_to_subscribe.begin(),
		itera_end = _v_interaction_classes_to_subscribe.end(); itera != itera_end; ++itera )
	{
		_rtiamb->subscribeInteractionClass( *itera );
	}
}

void cFederate::update()
{
	if( !_b_run )
		return;

	///////////////////////////////////////////////
	// create the necessary container and values //
	///////////////////////////////////////////////
	// create the collection to store the values in, as you can see
	// this is quite a lot of work
	for( std::map<std::string, sAttributesToUpdate>::iterator obj_itera = _m_attributes_to_update.begin(),
		obj_itera_end = _m_attributes_to_update.end(); obj_itera != obj_itera_end; ++obj_itera )
	{
		RTI::AttributeHandleValuePairSet* attributes =
			RTI::AttributeSetFactory::create( obj_itera->second.m_attributes.size() );

                for( std::map<std::string, sAttributeWithValue>::iterator attr_itera =
			obj_itera->second.m_attributes.begin(), attr_itera_end = obj_itera->second.m_attributes.end();
			attr_itera != attr_itera_end; ++attr_itera )
		{
			attributes->add( attr_itera->second.attribute_handle, attr_itera->second.str_value.c_str(),
				(RTI::ULong)attr_itera->second.str_value.size()+1 );
		}

		try
		{
			try
			{
				//////////////////////////
				// do the actual update //
				//////////////////////////
				//_rtiamb->updateAttributeValues( obj_itera->second.obj_handle, *attributes, "no_time_shtamp" );
				// _rtiamb->updateAttributeValues( obj_itera->second.object_instance_handle_itera->second.handle,
				//	*attributes, "no_time_shtamp" );

				// note that if you want to associate a particular timestamp with the
				// update. here we send another update, this time with a timestamp:
				RTIfedTime time = _fedamb->federate_time() + _fedamb->federate_lookahead();
				//_rtiamb->updateAttributeValues( obj_itera->second.obj_handle, *attributes, time, "with_time_shtamp" );
				_rtiamb->updateAttributeValues( obj_itera->second.object_instance_handle_itera->second.handle,
					*attributes, "no_time_shtamp" );
			}
			catch(...)
			{
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::update", "updateAttributeValues", 1 );
			}
		}
		catch( ErrorClass& e )
		{
			e.print();
			return;
		}

		// clean up
		delete attributes;
	}

	_m_attributes_to_update.clear();

	// request a time advance and wait until we get it
	advance_time( 1.0 );
	//std::cout << "Time Advanced to " << _fedamb->federate_time() << std::endl;
}

// опубликовать атрибут класса
bool cFederate::publish( const char* sz_class_name, const char* sz_attribute_name )
{
	std::string str_class_name( sz_class_name );

	std::map<std::string, sAttributes>::iterator class_itera = _m_classes_with_attributes_to_publish.find( str_class_name );
	if( class_itera == _m_classes_with_attributes_to_publish.end() )
	{
		RTI::ObjectClassHandle class_handle = 0;
		try
		{
			try
			{
				class_handle = _rtiamb->getObjectClassHandle( sz_class_name );
			}
			catch( RTI::NameNotFound )
			{
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::publish", "class not found", 1 );
			}
			catch(...)
			{
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::publish", "internal error", 1 );
			}
		}
		catch( ErrorClass error)
		{
			error.print();
			return false;
		}

		std::pair<std::map<std::string, sAttributes>::iterator, bool> ins_res =
			_m_classes_with_attributes_to_publish.insert( std::pair<std::string, sAttributes>( str_class_name, sAttributes() ) );
		class_itera = ins_res.first;
		class_itera->second.class_handle = class_handle;
	}

	RTI::AttributeHandle attribute_handle = 0;
	try
	{
		try
		{
			attribute_handle = _rtiamb->getAttributeHandle( sz_attribute_name, class_itera->second.class_handle );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::publish", "error to find attribute", 0 );
		}
	}
	catch( ErrorClass error )
	{
		error.print();
		return false;
	}


	std::pair<std::map<std::string, RTI::AttributeHandle>::iterator, bool> res_ins =
		class_itera->second.m_attributes.insert( std::pair<std::string, RTI::AttributeHandle>
		( std::string( sz_attribute_name ), attribute_handle ) );

	if( !res_ins.second )
	{
		std::cout << "Attribute is duplicated in the publishing: " << sz_attribute_name << std::endl;
	}

	return true;
}

// подписаться на атриббут класса
bool cFederate::subscribe( const char* sz_class_name, const char* sz_attribute_name )
{
	std::string str_class_name( sz_class_name );

	std::map<std::string, sAttributes>::iterator class_itera = _m_classes_with_attributes_to_subscribe.find( str_class_name );
	if( class_itera == _m_classes_with_attributes_to_subscribe.end() )
	{
		RTI::ObjectClassHandle class_handle = 0;
		try
		{
			try
			{
				class_handle = _rtiamb->getObjectClassHandle( sz_class_name );
			}
			catch( RTI::NameNotFound )
			{
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::subscribe", "class not found", 0 );
			}
			catch(...)
			{
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::subscribe", "internal error", 0 );
			}
		}
		catch( ErrorClass error)
		{
			error.print();
			return false;
		}

		std::pair<std::map<std::string, sAttributes>::iterator, bool> ins_res =
			_m_classes_with_attributes_to_subscribe.insert( std::pair<std::string, sAttributes>( str_class_name, sAttributes() ) );

		class_itera = ins_res.first;
		class_itera->second.class_handle = class_handle;
	}

	RTI::AttributeHandle attribute_handle = 0;
	try
	{
		try
		{
			attribute_handle = _rtiamb->getAttributeHandle( sz_attribute_name, class_itera->second.class_handle );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::subscribe", "error to find attribute", 0 );
		}
	}
	catch( ErrorClass error )
	{
		error.print();
		return false;
	}


	std::pair<std::map<std::string, RTI::AttributeHandle>::iterator, bool> res_ins =
		class_itera->second.m_attributes.insert( std::pair<std::string, RTI::AttributeHandle>
		( std::string( sz_attribute_name ), attribute_handle ) );

	if( !res_ins.second )
		std::cout << "Attribute is duplicated in the subscribing: " << sz_attribute_name << std::endl;

	return true;
}

bool cFederate::add_instance_object( const char* sz_class_name, const char* sz_object_name )
{
	RTI::ObjectClassHandle class_handle;

	try
	{
		try
		{
			class_handle = _rtiamb->getObjectClassHandle( sz_class_name );
		}
		catch( RTI::NameNotFound )
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::add_instance_object", "error to find class handle", 0 );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::add_instance_object", "getObjectClassHandle", 0 );
		}

		sObjectInstanceHandle object_instance_handle;
		try
		{			
			object_instance_handle.handle = _rtiamb->registerObjectInstance( class_handle, sz_object_name );
		}
		catch( RTI::ObjectAlreadyRegistered )
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::add_instance_object", "ObjectAlreadyRegistered", 0 );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::add_instance_object", "registerObjectInstance", 0 );
		}

		object_instance_handle.class_itera = _m_classes_with_attributes_to_publish.find( sz_class_name );
		if( _m_classes_with_attributes_to_publish.end() == object_instance_handle.class_itera )
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::add_instance_object", "error to find class itera", 0 );
		}

		_m_object_instance_handles.insert( std::pair<std::string, sObjectInstanceHandle>( sz_object_name, object_instance_handle ) );
	}
	catch( ErrorClass& e )
	{
		e.print();
		return false;
	}

	std::cout << "Add instance object: " << sz_object_name << std::endl;

	return true;
}

bool cFederate::remove_instance_object( const char* sz_object_name )
{
	try
	{
		// ищём хэндл объекта
		std::map<std::string, sObjectInstanceHandle>::iterator find_itera =
			_m_object_instance_handles.find( sz_object_name );
		if( _m_object_instance_handles.end() == find_itera )
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::remove_instance_object", "error find in hash-map", 0 );
		}

		// запоминаем
		RTI::ObjectHandle obj_handle = find_itera->second.handle;

		// дополнительно, удалим объект, если он находится в списке на обновление
		for( std::map<std::string, sAttributesToUpdate>::iterator obj_itera = _m_attributes_to_update.begin(),
			obj_itera_end = _m_attributes_to_update.end(); obj_itera != obj_itera_end; ++obj_itera )
		{
			if( obj_itera->second.object_instance_handle_itera->second.handle == obj_handle )
			{
				_m_attributes_to_update.erase( obj_itera );
				break;
			}
		}

		// наконец, удаляем
		try
		{
			_rtiamb->deleteObjectInstance( obj_handle, "delete object instance" );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::remove_instance_object", "deleteObjectInstance", 0 );
		}
		_m_object_instance_handles.erase(find_itera);
	}
	catch( ErrorClass& e )
	{
		e.print();
		return false;
	}

	std::cout << "Remove instance object: " << sz_object_name << std::endl;

	return true;

	/*RTI::ObjectHandle object_handle;

	try
	{
		try
		{
			object_handle = _rtiamb->getObjectInstanceHandle( sz_object_name );
		}
		catch( RTI::ObjectNotKnown )
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::remove_instance_object", "getObjectInstanceHandle::ObjectNotKnown", 0 );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::remove_instance_object", "getObjectInstanceHandle::internal error", 0 );
		}

		try
		{
			_rtiamb->deleteObjectInstance( object_handle, "" );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::remove_instance_object", "deleteObjectInstance", 0 );
		}
	}
	catch( ErrorClass& e )
	{
		e.print();
		return false;
	}

	std::cout << "Remove instance object: " << sz_object_name << std::endl;

	return true;*/
}

bool cFederate::set_attribute_value_by_object_instance( const std::string& str_object_name,
													   const std::string& str_attribute_name,
													   const char* sz_value )
{
	if( !_b_run )
		return false;

	//return false;

	//std::string str_object_name( sz_object_name );

	std::map<std::string, sAttributesToUpdate>::iterator obj_itera =
		_m_attributes_to_update.find( str_object_name );

	if( obj_itera == _m_attributes_to_update.end() )
	{
		std::pair<std::map<std::string, sAttributesToUpdate>::iterator, bool> ins_res =
			_m_attributes_to_update.insert( std::pair<std::string, sAttributesToUpdate>( str_object_name, sAttributesToUpdate() ) );

		obj_itera = ins_res.first;

		try
		{
			/*try
			{
				obj_itera->second.obj_handle = _rtiamb->getObjectInstanceHandle( sz_object_name );
			}
			catch( RTI::ObjectNotKnown )
			{
				std::string str_desc_error( "Object handle not found: " );
				str_desc_error.append( sz_object_name );
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::set_attribute_value_by_object_instance", str_desc_error, 0 );
			}
			catch(...)
			{
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::set_attribute_value_by_object_instance", "", 0 );
			}*/

			obj_itera->second.object_instance_handle_itera = _m_object_instance_handles.find( str_object_name );

			if( _m_object_instance_handles.end() == obj_itera->second.object_instance_handle_itera )
			{
				//obj_itera->second.object_instance_handle_itera = _m_reflected_object_instance_handles.find(str_object_name);

				//if(_m_reflected_object_instance_handles.end() == obj_itera->second.object_instance_handle_itera)
				{
					std::string str_desc_error( "Object handle not found: " );
					str_desc_error.append( str_object_name );
					throw ErrorClass( __FILE__, __LINE__ , "cFederate::set_attribute_value_by_object_instance", str_desc_error, 0 );
				}

			}

			//obj_itera->second.obj_handle = itera_object_instance_handle->second.handle;
			//obj_itera->second.class_handle = itera_object_instance_handle->second.class_itera->second.class_handle;



			/*try
			{
				obj_itera->second.class_handle = _rtiamb->getObjectClass( obj_itera->second.obj_handle );
			}
			catch( RTI::ObjectNotKnown )
			{
				std::string str_desc_error( "Class handle not found: " );
				str_desc_error.append( sz_object_name );
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::set_attribute_value_by_object_instance", str_desc_error, 0 );
			}
			catch(...)
			{
				throw ErrorClass( __FILE__, __LINE__ , "cFederate::set_attribute_value_by_object_instance", "", 0 );
			}*/
		}
		catch( ErrorClass& e )
		{
			e.print();
			return false;
		}
	}


	const std::map<std::string, RTI::AttributeHandle>& m_attributes =
		obj_itera->second.object_instance_handle_itera->second.class_itera->second.m_attributes;

	std::map<std::string, RTI::AttributeHandle>::const_iterator find_itera = m_attributes.find( str_attribute_name );
	if( m_attributes.end() == find_itera )
	{
		ErrorClass error( __FILE__, __LINE__ , "cFederate::set_attribute_value_by_object_instance", "error to find attribute", 0 );
		error.print();
		return false;
	}

	RTI::AttributeHandle attribute_handle = find_itera->second;

	/*RTI::AttributeHandle attribute_handle = 0;
	try
	{
		//attribute_handle = _rtiamb->getAttributeHandle( sz_attribute_name, obj_itera->second.class_handle );

		std::map<std::string, RTI::AttributeHandle>& m_attributes =
			obj_itera->second.object_instance_handle_itera->second.class_itera->second.m_attributes;

		std::map<std::string, RTI::AttributeHandle>::iterator find_itera = m_attributes.find( sz_attribute_name );
		if( m_attributes.end() == find_itera )
		{

		}
		attribute_handle = find_itera->second;
	}
	catch(...)
	{
		ErrorClass error( __FILE__, __LINE__ , "cFederate::set_attribute_value_by_object_instance", "error to find attribute", 0 );
		error.print();
		return false;
	}*/

        std::pair<std::map<std::string, sAttributeWithValue>::iterator, bool> res_ins =
		obj_itera->second.m_attributes.insert( std::pair<std::string, sAttributeWithValue>
			( std::string( str_attribute_name ), sAttributeWithValue( /*attribute_handle, sz_value*/ ) ) );

	res_ins.first->second.attribute_handle = attribute_handle;
	res_ins.first->second.str_value.assign( sz_value );

	return true;
}

/*
 * This method will request a time advance to the current time, plus the given
 * timestep. It will then wait until a notification of the time advance grant
 * has been received.
 */
void cFederate::advance_time( double timestep )
{
	// request the advance
//_fedamb->set_advancing( true );
//RTIfedTime newTime = ( _fedamb->federate_time() + timestep );
//_rtiamb->timeAdvanceRequest( newTime );

	// wait for the time advance to be granted. ticking will tell the
	// LRC to start delivering callbacks to the federate
//while( _fedamb->is_advancing() )
	{
		try
		{
			//rtiamb->tick2(); //
			_rtiamb->tick();
			//_rtiamb->tick( 0.001, 0.02 );
		}
		catch( RTI::Exception &e )
		{
			std::cout << "Exception tick2 : " << &e << std::endl;
		}
	}
}

std::vector<cFederateAmbassador::sReflectObject>* cFederate::get_reflect_objects()
{
	return &_fedamb->_v_reflect_objects;
}

// std::vector<cFederateAmbassador::sReflectObject>* cFederate::get_reflect_objects_with_time()
// {
// 	return &_fedamb->_v_reflect_objects_with_time;
// }

bool cFederate::publish_interaction( const char* sz_class_name )
{
	try
	{
		try
		{
			_v_interaction_classes_to_publish.push_back( _rtiamb->getInteractionClassHandle( sz_class_name ) );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::subscribe_interaction", "getInteractionClassHandle", 1 );
		}
	}
	catch( ErrorClass& e )
	{
		e.print();
		return false;
	}

	return true;
}

bool cFederate::subscribe_interaction( const char* sz_class_name )
{
	try
	{
		try
		{
			_v_interaction_classes_to_subscribe.push_back( _rtiamb->getInteractionClassHandle( sz_class_name ) );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::subscribe_interaction", "getInteractionClassHandle", 1 );
		}
	}
	catch( ErrorClass& e )
	{
		e.print();
		return false;
	}

	return true;
}

bool cFederate::add_parameter( const std::string& str_class_name, const char* sz_param_name )
{
        std::map<std::string, sParameters>::iterator itera_class = _m_parameters.find( str_class_name );
	if( _m_parameters.end() == itera_class )
	{
                std::pair<std::map<std::string, sParameters>::iterator, bool> ins_res =
			_m_parameters.insert( std::pair<std::string, sParameters>( str_class_name, sParameters() ) );

		itera_class = ins_res.first;

		try
		{
			itera_class->second.interaction_class_handle = _rtiamb->getInteractionClassHandle( str_class_name.c_str() );
		}
		catch(...)
		{
			ErrorClass e( __FILE__, __LINE__ , "cFederate::add_parameter", "getInteractionClassHandle", 1 );
			e.print();
			return false;
		}
	}

	try
	{
		sParameter parameter;
		parameter.handle = _rtiamb->getParameterHandle( sz_param_name, itera_class->second.interaction_class_handle );
		parameter.str_name.assign( sz_param_name );
		itera_class->second.v_params.push_back( parameter );
	}
	catch(...)
	{
		ErrorClass e( __FILE__, __LINE__ , "cFederate::add_parameter", "getParameterHandle", 1 );
		e.print();
		return false;
	}

	return true;
}

bool cFederate::send_interaction( const std::string& str_class_name, std::vector<std::string>& v_values )
{
	if( !_b_run )
		return false;

	//return false;

	try
	{
		std::map<std::string, sParameters>::iterator class_itera = _m_parameters.find( str_class_name );
		if( _m_parameters.end() == class_itera )
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::send_interaction", "error to find interaction class", 1 );
		}

		// проверим кол-во параметров
		if( v_values.size() != class_itera->second.v_params.size() )
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::send_interaction",
				"number of values and parameters of interaction are not the same", 1 );
		}

		RTI::ParameterHandleValuePairSet* parameters = RTI::ParameterSetFactory::create( v_values.size() );

		for( size_t i=0, end_i=v_values.size(); i<end_i; i++ )
		{
			parameters->add( class_itera->second.v_params[i].handle, v_values[i].c_str(), v_values[i].size()+1 );
		}

		try
		{
			//_rtiamb->sendInteraction( class_itera->second.interaction_class_handle, *parameters, "hi!" );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::send_interaction",
				"sendInteraction", 1 );
		}

		try
		{
			RTIfedTime time = _fedamb->federate_time() + _fedamb->federate_lookahead();
			_rtiamb->sendInteraction( class_itera->second.interaction_class_handle, *parameters, "hi!" );
		}
		catch(...)
		{
			throw ErrorClass( __FILE__, __LINE__ , "cFederate::send_interaction",
				"sendInteraction with timestamp", 1 );
		}

		delete parameters;
	}
	catch( ErrorClass& e )
	{
		e.print();
		return false;
	}

	return true;
}

std::vector<cFederateAmbassador::sReflectInteraction>* cFederate::get_reflect_interactions()
{
	return &_fedamb->_v_reflect_interactions;
}

std::vector<std::map<std::string, cFederate::sObjectInstanceHandle>::iterator>* cFederate::get_discover_object_instances()
{
	return &_v_discover_object_instances;
}
/// удалить отражение объекта
void cFederate::remove_reflected_object_instance(RTI::ObjectHandle object_handle)
{
	for(object_instances_t::iterator it=_m_reflected_object_instance_handles.begin(); it!=_m_reflected_object_instance_handles.end(); ++it)
	{
		if(it->second.handle == object_handle)
		{
			// сохраняем имя удалённого инстанса объекта
			m_removed_reflected_object_instances.push_back(it->first);
			_m_reflected_object_instance_handles.erase(it);
			break;
		}
	}
}
/// получить список удалённых объектов
cFederate::removed_objects_t& cFederate::get_removed_object_instances()
{
	return m_removed_reflected_object_instances;
}