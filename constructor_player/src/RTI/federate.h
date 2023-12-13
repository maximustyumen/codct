// [86open@gmail.com]		01.06.2010
#ifndef PROTO_OPEN_RTI_FEDERATE_H
#define PROTO_OPEN_RTI_FEDERATE_H
#pragma once

#include "../error/errorclass.h"
#include "../RTI/federate_ambassador.h"
#include <vector>
#include <map>

class cFederate
{
public:
	cFederate();
	virtual ~cFederate();

	// создать федерат
    bool create( const char* sz_fed_name, const char* sz_fed_file , std::string federation_name );
	
	// уничтожить федерат
	void destroy();

	// регистрация точки синхронизации
	void register_synchronization_point();

	// запуск федерата. blocking - блокировать тренажер пока все не подключаться.
	void run( bool b_time_regulating, bool b_time_constrained, bool blocking );

	// опубликовать атрибут класса
	bool publish( const char* sz_class_name, const char* sz_attribute_name );

	// подписаться на атрибут класса
	bool subscribe( const char* sz_class_name, const char* sz_attribute_name );

	// добавить экземпляр класса - объект
	bool add_instance_object( const char* sz_class_name, const char* sz_object_name );
	
	// удалить объект
	bool remove_instance_object( const char* sz_object_name );

	// установить значение атрибута
	bool set_attribute_value_by_object_instance( //const char* sz_object_name, const char* sz_attribute_name,
		const std::string& str_object_name, const std::string& str_attribute_name, const char* sz_value );

	// опубликовать класс интеракции
	bool publish_interaction( const char* sz_class_name );

	// подписаться на класс интеракции
	bool subscribe_interaction( const char* sz_class_name );

	// добавить параметр интеракции
	bool add_parameter( const std::string& str_class_name, const char* sz_param_name );

	// отправить интеракцию
	bool send_interaction( const std::string& str_class_name, std::vector<std::string>& v_values );

	void update();

	std::vector<cFederateAmbassador::sReflectObject>* get_reflect_objects();
	//std::vector<cFederateAmbassador::sReflectObject>* get_reflect_objects_with_time();

	std::vector<cFederateAmbassador::sReflectInteraction>* get_reflect_interactions();
	//std::vector<cFederateAmbassador::sReflectInteraction>* get_reflect_interactions_with_time();

	bool is_run() const {   return _b_run;  }

private:
	void enable_time_policy(bool b_time_regulating, bool b_time_constrained);
	void remove_reflected_object_instance(RTI::ObjectHandle object_handle);
	void advance_time(double timestep);
	void publish_and_subscribe();
	void cleanup();

	// members
	RTI::RTIambassador* _rtiamb;
	cFederateAmbassador* _fedamb;

	friend class cFederateAmbassador;

	bool _b_creator_federation_execution;
	bool _b_run;

	// имя федерации
	std::string federationName;
public:
	struct sAttributes
	{
		typedef std::map<std::string, RTI::AttributeHandle> attributes_t;
		
		attributes_t m_attributes;
		RTI::ObjectClassHandle class_handle;

		const std::string* find_by_attribute_handle(RTI::AttributeHandle attribute_handle) const
		{
			for( std::map<std::string, RTI::AttributeHandle>::const_iterator itera =
				m_attributes.begin(), itera_end = m_attributes.end(); itera != itera_end;
				++itera )
			{
				if( itera->second == attribute_handle )
				{
					return &(itera->first);
				}
			}

			ErrorClass error( __FILE__, __LINE__ , "find_by_attribute_handle", "", 0 );
			error.print();

			return NULL;
		}
	};

	typedef std::map<std::string, sAttributes> classes_with_attributes_t;
	
	classes_with_attributes_t _m_classes_with_attributes_to_subscribe;
	classes_with_attributes_t _m_classes_with_attributes_to_publish;

	struct sAttributeWithValue
	{
		sAttributeWithValue()	{}

		sAttributeWithValue( const RTI::AttributeHandle& a_handle,
			const std::string& str_v ): attribute_handle( a_handle ), str_value( str_v )
		{
		}

		RTI::AttributeHandle attribute_handle;
		std::string str_value;
	};

	// хэндл объекта
	struct sObjectInstanceHandle
	{
		RTI::ObjectHandle handle;
		std::map<std::string, sAttributes>::const_iterator class_itera;
	};

	/// получить список удалённых объектов
	typedef std::vector<std::string> removed_objects_t;
	removed_objects_t& get_removed_object_instances();

private:
	// хэш-мап <имя объекта, хэндл объекта>
	typedef std::map<std::string, sObjectInstanceHandle> object_instances_t;
	object_instances_t _m_object_instance_handles;

	object_instances_t _m_reflected_object_instance_handles;
	removed_objects_t m_removed_reflected_object_instances;

	// вектор новых обнаруженных инстансов объектов
	std::vector<std::map<std::string, cFederate::sObjectInstanceHandle>::iterator> _v_discover_object_instances;

	struct sAttributesToUpdate
	{
		std::map<std::string, sAttributeWithValue> m_attributes;
		//RTI::ObjectHandle obj_handle;
		//RTI::ObjectClassHandle class_handle;
		std::map<std::string, sObjectInstanceHandle>::iterator object_instance_handle_itera;
	};

	// атрибуты классы для обновления значений
	std::map<std::string, sAttributesToUpdate> _m_attributes_to_update;

	// список классов интеракций для опубликования/подписки
	typedef std::vector<RTI::InteractionClassHandle> interactions_t;
	interactions_t _v_interaction_classes_to_subscribe;
	interactions_t _v_interaction_classes_to_publish;

	struct sParameter
	{
		std::string str_name;
		RTI::ParameterHandle handle;
	};
	
	struct sParameters
	{
		RTI::InteractionClassHandle interaction_class_handle;
		std::vector<sParameter> v_params;
	};

	// хэш-мап для параметров интеракции
	std::map<std::string, sParameters> _m_parameters;

public:
	// get newly discover instances objects
	std::vector<std::map<std::string, cFederate::sObjectInstanceHandle>::iterator>* get_discover_object_instances();
};

#endif
