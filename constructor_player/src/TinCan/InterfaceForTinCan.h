#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

#include <curl/curl.h>

//using namespace std;

#ifdef WIN32
	#include "./cJSON.h"
#else
	#include "./cJSON.h"
#endif

class InterfaceForTinCan
{
public:
	InterfaceForTinCan();
	void AddTinCanRecord(std::string str, std::string type);
	void PostToLRS(std::string host, std::string login, std::string password);
	void PostFileToLRS(std::string filename);
	void PostToFile (std::string filename);
	

private:
	std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty = true ) ;

	//созданные объекты
	std::map <std::string, cJSON *> OBJECTS;
	//
	cJSON *top;

	std::string LRS_host;
	std::string LRS_login;
	std::string LRS_password;



	void PostStringToLRS(std::string zzz);

};