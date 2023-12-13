/***************************************************************************
interfaceforscorm.cpp  - Модуль взаимодействия с LMS
-------------------
begin                : 29 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "interfaceforscorm.h"

interfaceForScorm::interfaceForScorm()
{
    std::cout << "SCORM interface init." << std::endl;
    //Засекаем время запуска
    tick1 = osg::Timer::instance()->tick();
}


//Добавление новой записи
void interfaceForScorm::SetValue (std::string key, std::string value)
{
	bool ok=false;
	for (int i=0; i < DataToLMT.size();i++)
	{
		if (DataToLMT.at(i).key == key)
		{
			DataToLMT.at(i).value = value;
			ok= true;
			break;
		}
	}

	if (ok==false)
	{
		keyValue newValue;
		newValue.key = key;
		newValue.value=value;
		DataToLMT.push_back(newValue);
	}

    //а это для того, чтобы пользователь мог получить только-что записанные данные
    DataFromLMT[key] = value;
}

//Возврат значения из LMS по имени поля (например cmi.learner_name)
std::string interfaceForScorm::GetValue(std::string key)
{
    //if (map.count(key))
    if (DataFromLMT.find(key) == DataFromLMT.end())
    {
        return "not found";
    }
	else
		return DataFromLMT[key];    
}

void interfaceForScorm::GetDataFromLMS()
{
    //получаем директорию пользователя
    std::string _homedir =  _GetUserHomePath();
    std::string filename = _homedir + "/SCORM.input";

    std::cout << "\tRead SCORM.input file (" << filename << ") .. ";

    std::ifstream  File(filename.c_str());
    if(!File)
    {
        std::cout << " error. "<< std::endl;
        return;
    }

    std::cout << std::endl;
    while (!File.eof())
    {
        std::string keyName;
        std::string keyValue;

        std::getline(File, keyName);
        std::getline(File, keyValue);

        if (keyName!="")
        {
            std::cout << "\tread new Record: " << keyName << " = " << keyValue << std::endl;
            DataFromLMT[keyName] = keyValue;
        }
    }
    File.close();
    std::cout << "\tdone. "<< std::endl;

}

bool interfaceForScorm::find (std::string key)
{
	for (int i=0; i < DataToLMT.size();i++)
	{
		if (DataToLMT.at(i).key == key)
		{
			return true;
		}
	}
	return false;
}

void interfaceForScorm::SetDataToFile(std::string filename)
{
	std::cout << "\tWrite SCORM.output file (" << filename << ") .. ";
    std::ofstream outClientFile(filename.c_str(), std::ios:: out);
    if(!outClientFile)
    {
            std::cout << " error. "<< std::endl;
            return;
    }

    //время сессии, если не задано, значит считаем сами
    if (find("cmi.session_time") == false)
    {
        osg::Timer_t tick2 = osg::Timer::instance()->tick();
        double D_T = osg::Timer::instance()->delta_s(tick1, tick2);
        int session_time = (int)D_T;
        SetValue ("cmi.session_time", intToStr (session_time) );
    }

    //exit status, если не задано, значит задаем сами
    if (find("cmi.exit") == false)
    {
        SetValue ("cmi.exit", "normal" );
    }

    //cmi.score.scaled, если не задано, значит задаем сами
    if (find("cmi.score.scaled") == false)
    {
        if ((find("cmi.score.max")) && (find("cmi.score.min")) && (find("cmi.score.raw")))
        {
            float cmi_score_max = strToFloat (GetValue("cmi.score.max"));
            float cmi_score_min = strToFloat (GetValue("cmi.score.min"));
            float cmi_score_raw = strToFloat (GetValue("cmi.score.raw"));
            float cmi_score_scaled = 0;

            if (cmi_score_raw > cmi_score_max) cmi_score_raw = cmi_score_max;
            if (cmi_score_raw < cmi_score_min) cmi_score_raw = cmi_score_min;
            if (cmi_score_min!=cmi_score_max)
            {
                //-1..1 но у нас 0..1
                cmi_score_scaled = ((float)cmi_score_raw / ((float)cmi_score_max - (float)cmi_score_min)); //0..1;
            }
            else
            {
                std::cout  << std::endl << "SCORM warning cmi.scaled is not correct" << std::endl;
                cmi_score_scaled = 0.0f;
            }
            SetValue ("cmi.score.scaled", floatToStr(cmi_score_scaled));
        }
    }
	
    //показатель выполнения по точности (правильности)
    //cmi.success_status, если не задано, значит задаем сами
    if (find("cmi.success_status") == false)
    {
        //если заданы cmi.score.scaled и cmi.scaled_passing_score
        if ( find("cmi.score.scaled") && find("cmi.scaled_passing_score") )
        {
            float cmi_score_scaled = strToFloat (GetValue("cmi.score.scaled"));
            float cmi_scaled_passing_score = strToFloat (GetValue("cmi.scaled_passing_score"));
            if (cmi_score_scaled >= cmi_scaled_passing_score)
            {
                SetValue ("cmi.success_status", "passed");
            }
            else
            {
                SetValue ("cmi.success_status", "failed");
            }
        }
        else
        {
            SetValue ("cmi.success_status", "unknown");
        }

    }

	//количество (или процент от максимального количества) выполненных заданий или действий в рамках SCO
    //cmi.success_status, если не задано, значит задаем сами
    //Возможны четыре значения: “completed” - успешное выполнение, “incomplete” - недостаточное количество выполненных заданий; “not attempted” - обучаемый не выполнял задания; “unknown” - отсутствие утверждения о статусе выполнения SCO.
    //если не задано, значит задаем сами
    if (find("cmi.completion_status") == false)
    {
        //если заданы cmi.progress_measure и cmi.completion_threshold
        if (find("cmi.progress_measure") && find("cmi.completion_threshold") )
        {
            float cmi_progress_measure = strToFloat (GetValue("cmi.progress_measure"));
            float cmi_completion_threshold = strToFloat (GetValue("cmi.completion_threshold"));

            if (cmi_progress_measure >= cmi_completion_threshold)
            {
                SetValue ("cmi.completion_status", "completed");
            }
            else if (cmi_progress_measure < cmi_completion_threshold)
            {
                SetValue ("cmi.completion_status", "incomplete");
            }
            else if (cmi_progress_measure == 0)
            {
                SetValue ("cmi.completion_status", "not attempted");
            }
        }
        else
        {
            SetValue ("cmi.completion_status", "unknown");
        }
    }

    //записываем все
    for (int i=0; i < DataToLMT.size();i++)
    {
        outClientFile << DataToLMT.at(i).key << std::endl;
        outClientFile << DataToLMT.at(i).value  << std::endl;
    }

    outClientFile.close();

    std::cout << "\tdone. "<< std::endl;
}

void interfaceForScorm::SetDataToLMS()
{
    //получаем директорию пользователя
    std::string _homedir =  _GetUserHomePath() + "/Desktop";
    std::string filename = _homedir + "/SCORM.output";
    
	SetDataToFile(filename);
}



int interfaceForScorm::strToInt(std::string strIntString)
{
    int iConvertToMe;
    std::stringstream strStream(strIntString);
    strStream >> iConvertToMe;
    return iConvertToMe;
}

std::string interfaceForScorm::intToStr(int value)
{
    std::string str;
    std::string str2;
    std::stringstream strStream (str);
    strStream << value;
    strStream >> str2;
    return str2;
}

std::string interfaceForScorm::floatToStr(float value)
{
    std::string str;
    std::string str2;
    std::stringstream strStream (str);
    strStream << value;
    strStream >> str2;
    return str2;
}

float interfaceForScorm::strToFloat(std::string strIntString)
{
    float iConvertToMe;
    std::stringstream strStream(strIntString);
    strStream >> iConvertToMe;
    return iConvertToMe;
}


