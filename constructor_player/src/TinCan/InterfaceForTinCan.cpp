#include "InterfaceForTinCan.h"



InterfaceForTinCan::InterfaceForTinCan()
{
	top=cJSON_CreateObject();
}




void InterfaceForTinCan::AddTinCanRecord(std::string str, std::string type)
{
	//1. преобразуем страку в вектор через разделитель (::: или @@@)
	std::vector<std::string> words = split(str, ":::");
	//последние 2 элемента это поле=значение
	int numOfObject = words.size();

	//
	std::string z =  words [0];
	if( OBJECTS.end() != OBJECTS.find(z))
	{
		//ключ присутствует
	} 
	else
	{
		//создаем
		OBJECTS[z] =cJSON_CreateObject();
		//клеим к root
		cJSON_AddItemToObject(top,z.c_str(), OBJECTS[z]);
	}

	for (int i=1; i < numOfObject -2; i++)
	{
		std::string oldz = z;
		z = z + ":::" + words [i];

		if( OBJECTS.end() != OBJECTS.find(z))
		{
			//ключ присутствует
		} 
		else
		{
			//создаем
			OBJECTS[z] =cJSON_CreateObject();
			//клеим к 
			cJSON_AddItemToObject(OBJECTS[oldz], words [i].c_str(), OBJECTS[z]);
		}
	}

	
	std::string value = words [numOfObject-1];
	if (type=="string")
	{
		cJSON_AddStringToObject(OBJECTS[z], words [numOfObject-2].c_str(), value.c_str());
	}
	else if  (type=="number")
	{
		cJSON_AddNumberToObject(OBJECTS[z], words [numOfObject-2].c_str(), std::stod(value));
	}
	else if  (type=="bool")
	{
		bool val = false;
		if ((value=="true")||(value=="TRUE")) val = true;
		cJSON_AddBoolToObject(OBJECTS[z], words [numOfObject-2].c_str(), val);
	}


	// ["actor"]
	// ["actor::discription"]
	// ["actor::discription::discription"]

	//рекурсивная функция по созданию ТОЛЬКО объектов, начинает с первого элемента до последнего, после создания первого вызывает саму себя со следующими
}


void InterfaceForTinCan::PostToLRS(std::string host, std::string login, std::string password)
{
	char* out=cJSON_Print(top);	
	std::string zzz = out;
	cJSON_Delete(top);
	OBJECTS.clear();

	printf("%s\n",out);
	free(out);
	top=cJSON_CreateObject();


	LRS_host = host;
	LRS_login = login;
	LRS_password = password;

	PostStringToLRS(zzz);
}

void InterfaceForTinCan::PostFileToLRS(std::string filename)
{
	std::string zzz;
	std::string line;
	std::ifstream myfile (filename.c_str());
    if (myfile.is_open())
    {
		while ( myfile.good() )
		{
			getline (myfile,line);
			zzz = zzz + line;
		}
		myfile.close();
    }
	//
	PostStringToLRS(zzz);
}

void InterfaceForTinCan::PostToFile(std::string filename)
{
	char* out=cJSON_Print(top);	
	std::string zzz = out;
	cJSON_Delete(top);
	OBJECTS.clear();
	
	std::ofstream myfile;
	myfile.open (filename);
	myfile << zzz;
	myfile.close();

	free(out);
	top=cJSON_CreateObject();
}



void InterfaceForTinCan::PostStringToLRS(std::string zzz)
{
	std::string URL = LRS_host; //"https://cloud.scorm.com/ScormEngineInterface/TCAPI/public/statements";
	std::string loginpassword = LRS_login + ":" + LRS_password; //"test:test" 

	CURL *curl;
	struct curl_slist *headers=NULL; 

    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append( headers, "Content-Type: application/json");
    headers = curl_slist_append( headers, "X-Experience-API-Version:1.0.0");
    headers = curl_slist_append( headers, "charsets: utf-8");

	curl = curl_easy_init(); 

    if (curl)
    {
        /* enable verbose for easier tracing */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST"); //PUT
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		//
        curl_easy_setopt( curl, CURLOPT_USERPWD, loginpassword.c_str() ); //"test:test"
		// With the curl command line tool, you disable this with -k/--insecure.
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, zzz.c_str());

        std::cout<< "..." << std::endl;
		CURLcode res = curl_easy_perform(curl);
        std::cout<<   std::endl << "..." << std::endl;

		/* Check for errors */ 
        if(res != CURLE_OK)
        {
            std::cout<< "error:" << std::endl;
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            std::cout << std::endl;
        }

		curl_easy_cleanup(curl);
	}
	else
	{
		std::cout << "false" << std::endl;
	}
}













std::vector<std::string> InterfaceForTinCan::split(const std::string& s, const std::string& delim, const bool keep_empty) 
{
	std::vector <std::string> result;
	if (delim.empty()) 
	{
		result.push_back(s);
		return result;
	}
    std::string::const_iterator substart = s.begin(), subend;
    while (true) 
	{
        subend = search(substart, s.end(), delim.begin(), delim.end());
        std::string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}











