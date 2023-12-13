// [86open@gmail.com]		01.10.2010
#ifndef PROTO_OPEN_RTI_PROXY_H
#define PROTO_OPEN_RTI_PROXY_H
#pragma once

#include "../error/errorclass.h"
#include <vector>
#include <map>
#include <algorithm>

class SceneClass;

// базовый класс proxy
class cRTIProxy
{
public:
	enum e_proxy_type
	{
		e_proxy_file = 0,
		e_proxy_texture,
		e_proxy_sound,
		// etc..
		e_proxy_type_max
	};

	static e_proxy_type get_type_by_str( const std::string& str_proxy_type );

	struct sDesc
	{
		std::string str_proxy_name;
		std::string str_object_name;
		std::string str_attribute_name;
	};

	void set_desc( const sDesc& desc )	{	_desc = desc;	}
	const sDesc& get_desc() const		{	return _desc;	}	

	virtual void on_obtained( SceneClass* p_scene, const std::string& str_decoded ) = 0;

private:
	sDesc _desc;
};

class cRTIProxyManager
{
public:
	cRTIProxyManager();
	~cRTIProxyManager();

	// очистка
	void destroy();

	// добавить прокси
	bool add_proxy( const std::string& str_proxy_name, const std::string& str_object_name,
		const std::string& str_attribute_name, cRTIProxy::e_proxy_type type,
		const std::vector<std::string>& v_params );

	// удалить прокси
	void remove_proxy( const std::string& str_proxy_name );

	// поиск проксика по имени объекта
	bool find_proxy_by_object( const std::string& str_object_name ) const;

	// поиск проксика по имени объекта и атрибуту
	cRTIProxy* find_proxy( const std::string& str_object_name, const std::string& str_attribute_name );

	// обработать полученные данные
	void process_obtained_proxy( SceneClass* p_scene, cRTIProxy* p_proxy, const std::string& str_encoded_data );

private:
	// поиск проксика по имени
	std::map<std::string, cRTIProxy*>::iterator find_proxy( const std::string& str_proxy_name );

private:	// members
	std::map<std::string, cRTIProxy*> _m_proxis;
        std::map<std::string, std::map<std::string, cRTIProxy*> > _m_proxis_by_obj_attr;
};

#endif
