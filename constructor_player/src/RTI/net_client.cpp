#include "../RTI/net_client.h"

NetClient::NetClient(lua_State* lua_state,const std::string& federate_name,const std::string& classes_path)
: m_federate(Net::create_federate(federate_name,classes_path))
, m_client_lua(new Net::ClientLua(m_federate.get(),lua_state))
{
	m_federate->set_observer(this);
}
void NetClient::update()
{
	float dt = 0.f;
	if(m_federate.get()) m_federate->update(dt);
	if(m_client_lua) m_client_lua->update(dt);
}
/// сообщение о создании другим федератом объекта
void NetClient::on_discover_object(Net::Object* object)
{
	m_client_lua->process_discover_object(object);
}
/// сообщение об удалении другим федератом объекта
void NetClient::on_remove_object(Net::Object* object)
{
	m_client_lua->process_remove_object(object);
}
/// сообщение об изменении объекта другим федератом
void NetClient::on_change_object(Net::Object* object,const reflected_values_t& reflected_values)
{
	m_client_lua->process_change_object(object,reflected_values);
}