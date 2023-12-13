#ifndef CONFIGCLASS
#define CONFIGCLASS

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <fstream>



class Configure
{
public:
	//Конструктор
	Configure();
	//получить значение параметра
        const std::string GetRecordString(std::string keyName);
        float GetRecordFloat(std::string keyName);
	//изменить
	bool SetRecord(std::string keyName, std::string value);
	//записать файл
	void save();

private:
	//
	std::map <std::string, std::string> records;
        //
        std::string replace(const std::string & sBuffer, const std::string & sSrc, const std::string & sDst);
};

#endif
