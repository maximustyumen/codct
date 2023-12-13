/***************************************************************************
SupportExternLibs.h  -  Модуль вызова функций из внешних библиотек
-------------------
begin                : 23 апреля 2009
copyright            : (C) 2005-2009 by Курылев Евгений Викторович
					 : (C) 2005-2009 by Гаммер Максим Дмитриевич (maximum2000)
email                : GammerMax@yandex.ru
***************************************************************************/

#ifndef SUPPORTEXTERNLIBS
#define SUPPORTEXTERNLIBS

#ifdef WIN32
	#define NOMINMAX
	#include <windows.h>
	typedef char* (*MYPROC)(int, const char**); // Все функции в dll выглядят так.
	//
	int externcall(lua_State *lua)
	{
		// Общее количество параметров.
		int cnt = lua_gettop(lua);
		// Имя библиотеки.
		const char* libName = lua_tostring(lua, 1);
		// Имя метода.
		const char* funcName = lua_tostring(lua, 2);
		
		// Параметры.
		const char** argv = new const char*[cnt - 2];
		for (int i = 3; i <= cnt; i++)
		{
			const char* arg = lua_tostring(lua, i);
			argv[i - 3] = arg;
		}

		// Загрузка dll.
		// Нужно подставить путь для вызова dll.
		HMODULE mod = LoadLibrary((LPCTSTR)libName);
		// Получение адреса функции.
		MYPROC proc = (MYPROC)GetProcAddress(mod, funcName);

		// Вызов функции.
		char* res = proc(cnt - 2, argv);
		// Вернем результат в lua.
		lua_pushstring(lua, res);
		// Выгрузка библиотеки.
		FreeLibrary(mod);

		return 1; // Вернули 1 значение.
	}
#else
	int externcall(lua_State *lua)
	{
                // Имя библиотеки.
                const char* libName = lua_tostring(lua, 1);
                // Имя метода.
                const char* funcName = lua_tostring(lua, 2);

                std::cout << libName << funcName << std::endl;

                std::cout << "ExternCall not supported for Unix/Linux/" << std::endl;
		exit(0);
		return 0;
	}
#endif


#endif

