/***************************************************************************
errorclass.cpp  -  класс определения обоаботчика критических ошибок
-------------------
begin                : 7 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "errorclass.h"

//SingletonExample *SingletonExample::m_instance = 0;
lua_State *ErrorClass::Lua;


ErrorClass:: ErrorClass(std::string _file,  unsigned int _line,  std::string _function, std::string _description,  unsigned int _return_code)
{
    file = _file;
    function = _function;
    line = _line;
    return_code = _return_code;
    description = _description;


}

void ErrorClass::print()
{
    std::cout << "----------------error detected------------------------" << std::endl;
    std::cout << "description: " << description << std::endl;
    std::cout << "function: " << function << std::endl;
    std::cout << "file: " << file << std::endl;
    std::cout << "line: " << line << std::endl;
    std::cout << "return code: " << return_code << std::endl;
    std::cout << "----------------error end report----------------------" << std::endl;

    #ifdef THIS_PLUGIN
        std::string usercode = "Exit (true,false)";
        luaL_loadbuffer(Lua, usercode.c_str(), strlen (usercode.c_str()), "line") || lua_pcall(Lua, 0, 0, 0);
        //lua_close(Lua);
        //lua_settop (Lua, 0);
        lua_yield (Lua,0);

        QMessageBox::critical(0, QObject::tr("critical error detected"), QString::fromStdString(description));
    #else
        exit(return_code);
    #endif
}
