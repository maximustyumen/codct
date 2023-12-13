/***************************************************************************
env.h  -  Платформонезависимая функция определения директории пользователя
-------------------
begin                : 29 января 2010
copyright            : (C) 2005-2010 by Гаммер Максим Дмитриевич (maximum2000)
email                : MaxGammer@gmail.com
***************************************************************************/


#ifndef ENV_H
#define ENV_H

#include <stdlib.h>
#include <iostream>
#include <string>

static std::string _GetUserHomePath ()
{
    #ifdef WIN32
        char * homepath = getenv("homepath");
        char * homedrive = getenv("homedrive");
        if ((homepath == NULL)||(homedrive == NULL))
        {
            std::cout << "error get homepath or homedrive" << std::endl;
        }
        std::string a = homedrive ;
        std::string b = homepath;
        std::string value = a+b;
        return value;
    #else
        char * homepath = getenv("HOME");
        if (homepath == NULL)
        {
            std::cout << "error get HOME" << std::endl;
        }
        std::string value = homepath ;
        return value;
    #endif
}


static std::string _GetUserTempDir ()
{
    #ifdef WIN32
        std::string temp (getenv ("TEMP"));
    #else
        std::string temp ("/tmp");
    #endif
    return temp;
}



#endif // ENV_H
