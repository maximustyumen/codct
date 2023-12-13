/***************************************************************************
Configure.cpp -  Модуль конфигурации
-------------------
begin                : 23 марта 2023 
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./Configure.h"


//получить значение параметра
const std::string Configure::GetRecordString(std::string keyName)
{
	if (records.count(keyName)==0)
	{
		std::cout << "Warning: Configure record '" << keyName << "' not found!" << std::endl;
		return "not found";
	}
	else
	{
		return records[keyName];
	}
}

float Configure::GetRecordFloat(std::string keyName)
{
    //текущий разделитель из системных настроек:
    struct lconv *loc = localeconv();
    //этот самый separator и будет искать atof в строке
    std::string separator =  loc->decimal_point;
    std::string myseparator =  ".";


    std::string temp = GetRecordString (keyName);
    std::string result = replace(temp, myseparator, separator);

     float num = (float)atof(result.c_str());
     return num;
}


std::string Configure::replace(const std::string & sBuffer, const std::string & sSrc, const std::string & sDst)
{
    std::string sResult = sBuffer;
    std::string::size_type uPos = 0;
    while ((uPos = sResult.find(sSrc, uPos)) != sResult.npos)
    {
        sResult = sResult.replace(uPos, sSrc.size(), sDst);
        uPos += sDst.size();
    }
    return sResult;
}


//изменить значение параметра
bool Configure::SetRecord(std::string keyName, std::string value)
{
	if (records.count(keyName)==0)
	{
		std::cout << "Warning: Configure record '" << keyName << "' not found!" << std::endl;
		return false;
	}
	else
	{
		records[keyName] = value;
		return true;
	}
}


//Конструктор
Configure::Configure()
{
/*
структура файла configure.cfg
#Stereo
OFF
#DisplayType
MONITOR
#SpecialDisplayType
NO
#StereoMode
HORIZONTAL_SPLIT
#EyeSeparation 
0.05
#ScreenWidth
0.325
#ScreenHeight
0.26
#ScreenDistance
0.5
#MouseBlur
10
#Focus
60
#MouseSens
0.4
#ThreadingModel
SingleThread
#StandartConsole
OFF
#MasterVolume
50
#ArduinoPort
COM3
#ArduinoSpeed
115200
#PPU
OFF
#Shaders
OFF
*/
	records["#Stereo"]  = "OFF";
	records["#DisplayType"]  = "MONITOR";
	records["#SpecialDisplayType"]  = "NO";
	records["#StereoMode"]  = "HORIZONTAL_SPLIT";
	records["#EyeSeparation"]  = "0.05";
	records["#ScreenWidth"]  = "0.325";
	records["#ScreenHeight"]  = "0.26";
	records["#ScreenDistance"]  = "0.5";
	
	records["#Focus"]  = "60";
	records["#ThreadingModel"]  = "SingleThread";

	records["#MasterVolume"]  = "50";

	records["#MouseSens"]  = "0.4";
	records["#MouseBlur"]  = "10";

	records["#TextZoom"]  = "0";
	records["#ArduinoPort"]  = "COM3"; // Порт, по которому работает Arduino.

	records["#OCULUS_ON"]  = "OFF";

	records["#PPU"]  = "OFF";
	records["#Shaders"]  = "OFF";

    records["#windowed"]  = "0";
    records["#window.left"]  = "0";
    records["#window.top"]  = "0";
    records["#window.width"]  = "800";
    records["#window.height"]  = "600";

	records["#pseudofullscreen"]  = "OFF";
	records["#alternativemousecontrol"]  = "OFF";

	records["#HTC_VIVE_ON"]  = "OFF";


	std::string filename = "./configure.cfg";
	std::cout << "Read configuration in file (" << filename << ") .. ";
	std::ifstream  File1(filename.c_str());
    if(!File1)
    {
            std::cout << " error. "<< std::endl;

			//получаем директорию пользователя
			#ifdef WIN32
				std::string homedir (getenv ("USERPROFILE"));
				filename = homedir + "/./configure.cfg";
			#else
				std::string homedir (getenv ("HOME"));
				filename = homedir + "/./configure.cfg";
			#endif
			std::cout << "Read configuration in file (" << filename << ") .. ";
    }

    std::ifstream  File(filename.c_str());
    if(!File)
    {
            std::cout << " error. "<< std::endl;
    }
    else
    {
        std::cout << std::endl;
        while (!File.eof())
        {
            std::string keyName;
            std::string keyValue;

            File >> keyName;
            File >> keyValue;

            if ((keyName!="") && (keyValue !=""))
            {
                    std::cout << "\tread new Record: " << keyName << " = " << keyValue << std::endl;
                    records[keyName] = keyValue;
            }
        }
        File.close();
        std::cout << "done. "<< std::endl;

		//records["#OCULUS_ON"]  = "OFF";
    }


}

//записать файл
void Configure::save()
{
    //получаем директорию пользователя
	#ifdef WIN32
		//getenv("homepath");
		std::string homedir (getenv ("USERPROFILE"));
		std::string filename = homedir + "/./configure.cfg";
	#else
		std::string homedir (getenv ("HOME"));
		std::string filename = homedir + "/./configure.cfg";
	#endif

	std::cout << "Write configuration in file " << filename <<  " .. ";
	std::ofstream outClientFile(filename.c_str(), std::ios:: out);
	if(!outClientFile)
	{
		std::cout << " error. "<< std::endl;
	}
	else
	{
		for(std::map<std::string, std::string>::iterator hitr=records.begin();hitr!=records.end();	++hitr)
		{
			std::string z1 = hitr->first;
			std::string z2 = hitr->second;

			outClientFile << z1 << std::endl << z2 << std::endl;
		}
		outClientFile.close();
		std::cout << "done. "<< std::endl;
	}
	
}

 
