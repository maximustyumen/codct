#include "../RTI/proxy.h"
#include "../RTI/Uuencode/uuencode.h"
#include "../lua/env.h"
#include <fstream>
#include "../Scene/SceneClass.h"


static std::string s_m_str_proxy_types[cRTIProxy::e_proxy_type_max] =
{
	std::string( "file" ),
	std::string( "texture" ),
	std::string( "sound" ),
};

cRTIProxy::e_proxy_type cRTIProxy::get_type_by_str( const std::string& str_proxy_type )
{
	for( int i=0; i<cRTIProxy::e_proxy_type_max; i++ )
	{
		if( str_proxy_type == s_m_str_proxy_types[i] )
		{
			return ( cRTIProxy::e_proxy_type )i;
		}
	}

	std::string str_desc_error( "error to find proxy type: " );
	str_desc_error.append( str_proxy_type );
	ErrorClass error( __FILE__, __LINE__ , " cRTIProxy::get_type_by_str", str_desc_error.c_str(), 1 );
	error.print();

	return e_proxy_type_max;
}


// ------------------------------------------------------------------------------
class cRTIFileProxy: public cRTIProxy
{
public:
	cRTIFileProxy( const std::vector<std::string>& v_params );
	virtual void on_obtained( SceneClass* p_scene, const std::string& str_decoded );

private:
	std::string _str_file_name;
	std::string _str_file_path;
};

cRTIFileProxy::cRTIFileProxy( const std::vector<std::string>& v_params )
{
	_str_file_name = v_params.at(0);
        _str_file_path = _GetUserHomePath();
	_str_file_path.append( "/" );
	_str_file_path.append( _str_file_name );
}

void cRTIFileProxy::on_obtained( SceneClass* p_scene, const std::string& str_decoded )
{
	std::ofstream file( _str_file_path.c_str(), std::ios::out | std::ios::binary );
	file.write( str_decoded.c_str(), str_decoded.length() );
	file.close();
}


// ------------------------------------------------------------------------------
class cRTITextureProxy: public cRTIProxy
{
public:
	cRTITextureProxy( const std::vector<std::string>& v_params );
	virtual void on_obtained( SceneClass* p_scene, const std::string& str_decoded );

private:
	std::string _str_object_name;
	osgDB::ReaderWriter* _plugin;
	std::string _str_extension;
};

cRTITextureProxy::cRTITextureProxy( const std::vector<std::string>& v_params )
{
	_str_extension = v_params.at(0);
	_str_object_name = v_params.at(1);
}


void cRTITextureProxy::on_obtained( SceneClass* p_scene, const std::string& str_decoded )
{
	std::stringstream ss;
	ss << str_decoded;
	p_scene->Graphic->RChangeTexture( _str_object_name, ss, _str_extension );
}


// ------------------------------------------------------------------------------
cRTIProxyManager::cRTIProxyManager()
{
}

cRTIProxyManager::~cRTIProxyManager()
{
	destroy();
}

struct s_pred_delete_m_proxis
{
	void operator() ( const std::pair<std::string, cRTIProxy*>& elem )
	{
		delete elem.second;
	}
};

void cRTIProxyManager::destroy()
{
	std::for_each( _m_proxis.begin(), _m_proxis.end(), s_pred_delete_m_proxis() );
	_m_proxis.clear();
	_m_proxis_by_obj_attr.clear();
}

bool cRTIProxyManager::add_proxy( const std::string& str_proxy_name, const std::string& str_object_name,
									const std::string& str_attribute_name, cRTIProxy::e_proxy_type type,
									const std::vector<std::string>& v_params )
{
	cRTIProxy* p_proxy = NULL;

	switch( type )
	{
		case cRTIProxy::e_proxy_file:
		{
			p_proxy = new cRTIFileProxy( v_params );
			break;
		}
		case cRTIProxy::e_proxy_texture:
		{
			p_proxy = new cRTITextureProxy( v_params );
			break;
		}
		default:
		{
			exit(2);
		}
	}

	cRTIProxy::sDesc desc;
	desc.str_proxy_name = str_proxy_name;
	desc.str_object_name = str_object_name;
	desc.str_attribute_name = str_attribute_name;
	p_proxy->set_desc( desc );

	_m_proxis.insert( std::pair<std::string, cRTIProxy*>( str_proxy_name, p_proxy ) );

        std::map<std::string, std::map<std::string, cRTIProxy*> >::iterator itera_obj =
		_m_proxis_by_obj_attr.find( str_object_name );
	if( _m_proxis_by_obj_attr.end() == itera_obj )
	{
                std::pair<std::map<std::string, std::map<std::string, cRTIProxy*> >::iterator, bool> res =
                _m_proxis_by_obj_attr.insert( std::pair<std::string, std::map<std::string, cRTIProxy*> >( str_object_name,
			std::map<std::string, cRTIProxy*>() ) );
		itera_obj = res.first;
	}

	itera_obj->second.insert( std::pair<std::string, cRTIProxy*>( str_attribute_name, p_proxy ) );
	std::cout << "Add proxy: " << str_proxy_name << std::endl;
	return true;
}

std::map<std::string, cRTIProxy*>::iterator cRTIProxyManager::find_proxy( const std::string& str_proxy_name )
{
	std::map<std::string, cRTIProxy*>::iterator find_itera = _m_proxis.find( str_proxy_name );
	if( _m_proxis.end() == find_itera )
	{
		std::string str_desc_error( "error to find proxy: " );
		str_desc_error.append( str_proxy_name );
		ErrorClass error( __FILE__, __LINE__ , "cRTIProxyManager::find_proxy", str_desc_error.c_str(), 1 );
		error.print();
	}
	return find_itera;
}

void cRTIProxyManager::remove_proxy( const std::string& str_proxy_name )
{
	std::map<std::string, cRTIProxy*>::iterator proxy_itera = find_proxy( str_proxy_name );
	cRTIProxy* p_proxy = proxy_itera->second;
	_m_proxis.erase( proxy_itera );

	const cRTIProxy::sDesc& desc = p_proxy->get_desc();

        std::map<std::string, std::map<std::string, cRTIProxy*> >::iterator obj_itera =
		_m_proxis_by_obj_attr.find( desc.str_object_name );
	if( _m_proxis_by_obj_attr.end() != obj_itera )
	{
		std::map<std::string, cRTIProxy*>::iterator attr_itera = obj_itera->second.find( desc.str_attribute_name );
		if( obj_itera->second.end() != attr_itera )
		{
			obj_itera->second.erase( attr_itera );
			if( obj_itera->second.empty() )
			{
				_m_proxis_by_obj_attr.erase( obj_itera );
			}
		}
		else
		{
			std::string str_desc_error( "error to find proxy by attribute name: " );
			str_desc_error.append( desc.str_attribute_name );
			ErrorClass error( __FILE__, __LINE__ , "cRTIProxyManager::remove_proxy", str_desc_error.c_str(), 1 );
			error.print();
		}
	}
	else
	{
		std::string str_desc_error( "error to find proxy by object name: " );
		str_desc_error.append( desc.str_object_name );
		ErrorClass error( __FILE__, __LINE__ , "cRTIProxyManager::remove_proxy", str_desc_error.c_str(), 1 );
		error.print();
	}

	delete p_proxy;

	std::cout << "Remove proxy: " << str_proxy_name << std::endl;
}

/*const cRTIProxy* cRTIProxyManager::send_proxy( const std::string& str_proxy_name,
						const std::vector<std::string>& v_params, std::string* p_str_uuencoded_out )
{
	std::map<std::string, cRTIProxy*>::iterator find_itera = find_proxy( str_proxy_name );
	
	std::string str_original;
	cRTIProxy* p_proxy = find_itera->second;
	p_proxy->on_send( v_params, &str_original );

	// добавляем параметры
	std::string str_to_encode;
	convert_params_to_str( v_params, str_to_encode );

	str_to_encode.append( str_original );

	*p_str_uuencoded_out = Uuencode::base64_encode( reinterpret_cast<const unsigned char*>(
		str_to_encode.c_str() ), str_to_encode.length() );

	std::cout << "Send proxy: " << str_proxy_name << " (size: " <<
		p_str_uuencoded_out->length() << " bytes)" << std::endl;
	return p_proxy;
}*/

cRTIProxy* cRTIProxyManager::find_proxy( const std::string& str_object_name,
										const std::string& str_attribute_name )
{
        std::map<std::string, std::map<std::string, cRTIProxy*> >::iterator obj_itera =
		_m_proxis_by_obj_attr.find( str_object_name );

	if( _m_proxis_by_obj_attr.end() != obj_itera )
	{
		std::map<std::string, cRTIProxy*>::iterator attr_itera = obj_itera->second.find( str_attribute_name );
		if( obj_itera->second.end() != attr_itera )
		{
			return attr_itera->second;
		}
	}

	return NULL;
}

static std::string s_uucode = "Uuencode";
void cRTIProxyManager::process_obtained_proxy( SceneClass* p_scene, cRTIProxy* p_proxy,
                                        const std::string& str_encoded_data_with_title )
{
	std::string::size_type ok = str_encoded_data_with_title.find( s_uucode, 0 );
	if( ok != std::string::npos )
	{
		std::string str_encoded = str_encoded_data_with_title.substr( s_uucode.length(),
			str_encoded_data_with_title.length() - s_uucode.length() );
		std::string str_decoded = Uuencode::base64_decode( str_encoded );

		p_proxy->on_obtained( p_scene, str_decoded );

                //std::cout << "Proxy obtained: " << p_proxy->get_desc().str_proxy_name << " (" <<
                //	str_encoded.length() << " bytes)" << std::endl;
	}
	else
	{
		std::string str_desc_error( "Proxy object with data encoded strings do not have a title 'UUencode': " );
		str_desc_error.append( p_proxy->get_desc().str_proxy_name );
		ErrorClass error( __FILE__, __LINE__ , "cRTIProxyManager::process_obtained_proxy", str_desc_error.c_str(), 1 );
		error.print();
	}
}

bool cRTIProxyManager::find_proxy_by_object( const std::string& str_object_name ) const
{
        std::map<std::string, std::map<std::string, cRTIProxy*> >::const_iterator obj_itera =
		_m_proxis_by_obj_attr.find( str_object_name );

	return _m_proxis_by_obj_attr.end() != obj_itera;
}
/*void cRTIProxyManager::convert_params_to_str( const std::vector<std::string>& v_params, std::string& str_out )
{
	// пишем кол-во параметров	>_<
	char sz_params_size[64];
	sprintf_s( sz_params_size, 64, "%d", v_params.size() );
	size_t params_size_len = strnlen_s( sz_params_size, 64 );

	size_t size = params_size_len + v_params.size();
	++size;

	for( std::vector<std::string>::const_iterator itera = v_params.begin(), itera_end = v_params.end();
		itera != itera_end; ++itera )
	{
		size += itera->length();
	}

	// устанавливаем размер строки
	str_out.resize( size );

	// заполняем строку
	char* p_buf = &str_out.at( 0 );
	memcpy( p_buf, sz_params_size, params_size_len );
	p_buf[params_size_len] = '\0';
	p_buf += params_size_len;
	++p_buf;

	for( std::vector<std::string>::const_iterator itera = v_params.begin(), itera_end = v_params.end();
		itera != itera_end; ++itera )
	{
		memcpy( p_buf, itera->c_str(), itera->length() );
		p_buf += itera->length();
		*p_buf = '\0';
		++p_buf;
	}
}

size_t cRTIProxyManager::convert_str_to_params( std::string& str_in, std::vector<std::string>& v_params_out )
{
	char* p_buf_begin = &str_in.at( 0 );
	char* p_buf = p_buf_begin;

	// читаем кол-во параметров
	size_t num_params = atol( p_buf );
	v_params_out.reserve( num_params );

	p_buf += strlen( p_buf );
	++p_buf;

	// читаем сами строки
	for( size_t i=0; i<num_params; i++ )
	{
		std::string str( p_buf );
		v_params_out.push_back( str );

		p_buf += strlen( p_buf );
		++p_buf;
	}

	return p_buf - p_buf_begin;
}*/
