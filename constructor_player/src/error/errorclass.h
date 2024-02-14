#ifndef ERRORCLASS_H
#define ERRORCLASS_H

#include <string>
#include <string.h>
#include <iostream>
#include <stdlib.h>

//Lua
#ifdef WIN32
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#else
extern "C" {
#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
}

#endif

//ЗАКОММЕНТИРОВАТЬ ДЛЯ ПЛЕЕРА
//#define THIS_PLUGIN true

#ifdef THIS_PLUGIN
	#include <QMessageBox>
	#include <QString>
#endif

class ErrorClass
{
public:
	ErrorClass(std::string file,  unsigned int line,  std::string function, std::string description,  unsigned int return_code );
	void print();

	//Объявление LUA
	static lua_State *Lua;

private:
	std::string file;
	std::string function;
	unsigned int line;
	unsigned int return_code;
	std::string description;

};



#endif // ERRORCLASS_H
