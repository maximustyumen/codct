/***************************************************************************
os_clock.h  -  Модуль вызова функций из внешних библиотек
-------------------
begin                : 23 апреля 2009
copyright            : (C) 2005-2009 by Гаммер Максим Дмитриевич (maximum2000)
email                : GammerMax@yandex.ru
***************************************************************************/

#ifndef OSCLOCKPATCH
#define OSCLOCKPATCH

#ifdef WIN32
	const DWORD _os_clock_start = GetTickCount();
	int os_clock(lua_State *lua)
	{
		lua_pushnumber(lua,(GetTickCount()-_os_clock_start)/1000.0f);	
		return 1;	
	}
#else
	#include <sys/time.h> 
	unsigned long GetTickCount()
	{
		struct timeval tv;
		gettimeofday(&tv,NULL);
		return (tv.tv_sec*1000+tv.tv_usec/1000);
	} 
	const unsigned long _os_clock_start = GetTickCount();
	int os_clock(lua_State *lua)
	{
		lua_pushnumber(lua,(GetTickCount()-_os_clock_start)/1000.0f);	
		return 1;	
	}
#endif

#endif