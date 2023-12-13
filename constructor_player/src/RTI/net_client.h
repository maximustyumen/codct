#pragma once
#include "net/precomp.h"
#include "net/client_lua.h"
#include "net/federate_observer.h"

class NetClient : public Net::FederateObserver
{
private:
	Net::UFederate m_federate;
	std::unique_ptr<Net::ClientLua> m_client_lua;

public:
	NetClient(lua_State* lua_state,const std::string& federate_name,const std::string& classes_path);
	void update();

private:
	/// сообщение о создании другим федератом объекта
	virtual void on_discover_object(Net::Object* object);
	/// сообщение об удалении другим федератом объекта
	virtual void on_remove_object(Net::Object* object);
	/// сообщение об изменении объекта другим федератом
	virtual void on_change_object(Net::Object* object,const reflected_values_t& reflected_values);
};