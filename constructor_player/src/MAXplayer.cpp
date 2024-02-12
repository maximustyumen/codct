/***************************************************************************
MAXplayer.cpp  -  главный модуль 4х версии. Счастливого Нового Года!
-------------------
begin                : 10 октября 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "MAXplayer.h"

#include "SystemInformation.h"


int main( int argc, char **argv )
{


	
	/*

	 char * MyArray2 = (char *)malloc(0x1ffffffff);

	if (MyArray2)
		printf_s("It worked!");
	else
		printf_s("It didn't work.");
		system ("pause");
	*/
		


	
	#if (_MSC_VER >= 1700 && _MSC_VER < 1800)
   // ... Do VC9/Visual Studio 2008 specific stuff
   #endif

    //
    //печать версии
    std::cout << player_version << std::endl << "Build " << __DATE__ << " " << __TIME__ << std::endl;
    //
   // std::cout << std::endl << "RTI_VERSION = " << RTI_VERSION << std::endl;

	//печать характеристик оборудования и ОС
	PrintAllInfo( argc, argv);
	//
		

    if (std::ifstream("./start.lua").good() == false)
    {
        std::cout << "not found start script file. exit." << std::endl;
        return 0;
    }


    SetConsole();
	
    //загрузка конфигурации
    configuration = new Configure();

    //Консоль
    //if (configuration->GetRecordString("#StandartConsole") == "ON") SetConsole();
	
	//проверка защиты
	bool developmode;

	//Shield* check = new Shield();
	//developmode = check->CheckShield ();
    //delete check;

#ifdef GuardiantShield
	//GuardiantShield a;
	//a.testKey();
#endif


    //инициализация scorm-модуля
    SCORM = new interfaceForScorm();
    SCORM->GetDataFromLMS();

	//инициализация tincan модуля
	TINCAN = new InterfaceForTinCan();

	Init(developmode);
	Start();
	
	
	Bue();
	
	return 0;
}

void Init(bool developmode)
{
	//включить (1) /выкл (0) вертикальную синхронизацию
	setVSync(1);
	//1. Создаем объекты сцены
	scene = new SceneClass(configuration);
	//2. Создаем объект пользователя
	localUser = new UserClass (configuration);
	//3. создаем пул доступных источников звука
	scene->Audio->GenerateSources();
	//4. 
	localUser->developmode = developmode;
	//5. Настраиваем объект сцены
	scene->setViewResolution (localUser->getViewWidth() , localUser->getViewHeight ());
	//6. Отображаем данные сцены пользователю
	localUser->setDataToVideoViewer(scene->GetDataToVideoViewer().get(),scene->HUD->get_menu_node());
	//7. Уточняем фокусное расстояние
        float focus = configuration->GetRecordFloat("#Focus");
	localUser->setUserFocusLight(focus);
	//8. Курсор в центр
	MouseInCenterScreen();
	//9. установка режима управления,
	//1 - просмотр (вертеть головой) 2- управление 3- перемещение
	localUser->setInputMode(true,true,true);
	//координат и головы
	localUser->SetUserPositionAndOrientation(0,0,30,90,0,0); //x,y,z, headX,headY,headZ=0;

    Interpritator::LUAinit1 ();
    Interpritator::LUAinit2 ("./start.lua");
	//на мгновение отодвигаемся от сцены подальше
	localUser->preLoading=false;

}

//главный цикл программы
void Start()
{
	while( localUser->isWork() )
	{
		SimulationStep();
	}
}

//1 шаг имитации
void SimulationStep()
{
	//Проверяем, не нужно-ли подгрузить другой сценарий
        if (Interpritator::ReloadScript==true)
        {
                Interpritator::ReloadScript=false;
                Interpritator::ReloadScript2=true;
                scene->Texts->CreateText ("__ReloadText","Please wait");
                scene->Graphic->RConnectObjectTo("__ReloadText", "HUD");
                scene->Texts->SetTextFont ("__ReloadText","arial.ttf");
                scene->Texts->SetTextSize ("__ReloadText",200.0f * 1600.0f / (float)localUser->getViewWidth());
                scene->Texts->SetTextColor ("__ReloadText",1,1,1,1);
                scene->Graphic->RSetPosition ("__ReloadText",700* 1600.0f / (float)localUser->getViewWidth(),600* 1200.0f / (float)localUser->getViewHeight(),0);
                //для прорисовки
                localUser->update();
                //убираем надпись "Loading..."
                scene->Graphic->UnloadObject ("__ReloadText");
        }
	if (Interpritator::ReloadScript2==true)
	{
		Interpritator::ReloadScript2=false;
		lua_close(Interpritator::L);
                Interpritator::LUAinit1 ();
                Interpritator::LUAinit2 (Interpritator::NewFileNameScript);
		//1 - просмотр (вертеть головой) 2- управление 3- перемещение
		//localUser->setInputMode(true,true,true);
		std::cout << "Reload script...ok" << std::endl;
		//на мгновение отодвигаемся от сцены подальше
		localUser->preLoading=false;
        }

	//
	//статистика
	osg::Timer_t tick1;
	osg::Timer_t tick2;

	localUser->update();
	localUser->frame();
	//сбор внутренней статистики
	double UserInput_T = localUser->UserInput_T;
	double UserCalc_T = localUser->UserCalc_T;
	double Rendering_T = localUser->Rendering_T;
	double UpdateAudio_T = localUser->UpdateAudio_T;

	//физика и т.д.
	tick1 = osg::Timer::instance()->tick();
	scene->Update(localUser->BodyX, localUser->BodyY, localUser->BodyZ);

	//localUser->get_camera()->getViewMatrix()
	//scene->HUD3D->update(localUser->get_camera());


	tick2 = osg::Timer::instance()->tick();
	double scene_T = osg::Timer::instance()->delta_s(tick1,tick2);

	tick1 = osg::Timer::instance()->tick();
	//1. события рук 
	Interpritator::Interpretation(localUser->getHandsEvents());
	//2. события ног
	Interpritator::Interpretation(localUser->getFootEvents());
	//нажатые клавиши
	Interpritator::Interpretation(localUser->getKeysEvents());
	//
	tick2 = osg::Timer::instance()->tick();
	double UserLuaInterpretation_T = osg::Timer::instance()->delta_s(tick1,tick2);

	//3.RTI
        //#ifdef RTI_ON
	tick1 = osg::Timer::instance()->tick();
	//Обновление RTI

        //rti_update();
	tick2 = osg::Timer::instance()->tick();
	double RTI_T = osg::Timer::instance()->delta_s(tick1,tick2);


	tick1 = osg::Timer::instance()->tick();
	//4. передаем CurrentObject, коорд. пересеч. и т.д. в функцию SetObjectsInfo
	{
		lua_getglobal(Interpritator::L, "UserData");

		//объект на который нацелен курсор
		lua_pushstring(Interpritator::L, "CurrentObjectName" );
		lua_pushstring(Interpritator::L, localUser->GetCurrentObjectName().c_str());
		lua_settable(Interpritator::L, -3);
		//мировые коорд. пересечения
		lua_pushstring(Interpritator::L, "CurrentObjectIntersectsCoordWORLD_X" );
		lua_pushnumber(Interpritator::L, localUser->GetCurrentObjectIntersectsCoordWORLD().x());
		lua_settable(Interpritator::L, -3); 
		lua_pushstring(Interpritator::L, "CurrentObjectIntersectsCoordWORLD_Y" );
		lua_pushnumber(Interpritator::L, localUser->GetCurrentObjectIntersectsCoordWORLD().y());
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "CurrentObjectIntersectsCoordWORLD_Z" );
		lua_pushnumber(Interpritator::L, localUser->GetCurrentObjectIntersectsCoordWORLD().z());
		lua_settable(Interpritator::L, -3);
		//локальные координаты пересечения
		lua_pushstring(Interpritator::L, "CurrentObjectIntersectsCoordLOCAL_X" );
		lua_pushnumber(Interpritator::L, localUser->GetCurrentObjectIntersectsCoordLOCAL().x());
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "CurrentObjectIntersectsCoordLOCAL_Y" );
		lua_pushnumber(Interpritator::L, localUser->GetCurrentObjectIntersectsCoordLOCAL().y());
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "CurrentObjectIntersectsCoordLOCAL_Z" );
		lua_pushnumber(Interpritator::L, localUser->GetCurrentObjectIntersectsCoordLOCAL().z());
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "HeadYaw" );
		lua_pushnumber(Interpritator::L, localUser->HeadX);
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "HeadPitch" );
		lua_pushnumber(Interpritator::L, localUser->HeadY);
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "HeadRoll" );
		lua_pushnumber(Interpritator::L, localUser->HeadZ);
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "X" );
		lua_pushnumber(Interpritator::L, localUser->BodyX);
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "Y" );
		lua_pushnumber(Interpritator::L, localUser->BodyY);
		lua_settable(Interpritator::L, -3);
		lua_pushstring(Interpritator::L, "Z" );
		lua_pushnumber(Interpritator::L, localUser->BodyZ);
		lua_settable(Interpritator::L, -3);
		//
		lua_pop(Interpritator::L,1); // вытолкнуть массив из стека
		
	}
	tick2 = osg::Timer::instance()->tick();
	double SetObjectsInfo_T = osg::Timer::instance()->delta_s(tick1,tick2);

	tick1 = osg::Timer::instance()->tick();
	//5. вызов таймеров
	//Таймеры всех объектов в LUA
	if (Interpritator::IsLuaObject("LocalTimers"))
	{
		lua_getglobal(Interpritator::L,"LocalTimers");
		if (lua_pcall(Interpritator::L,0,0,0))
		{
			std::cout << "Error run script" << std::endl;
			std::cout << lua_tostring(Interpritator::L,-1) << std::endl;
			lua_pop(Interpritator::L,1);
			exit(3);
		}
	}
	//главный таймер в LUA
	if (Interpritator::IsLuaObject("MainTimer"))
	{
		lua_getglobal(Interpritator::L,"MainTimer");
		if (lua_pcall(Interpritator::L,0,0,0))
		{
			std::cout << "Error run script" << std::endl;
			std::cout << lua_tostring(Interpritator::L,-1) << std::endl;
			lua_pop(Interpritator::L,1);
			exit(3);
		}
	}
	//таймер STDlib
	if (Interpritator::IsLuaObject("STDLib_Timer"))
	{
		lua_getglobal(Interpritator::L,"STDLib_Timer");
		if (lua_pcall(Interpritator::L,0,0,0))
		{
			std::cout << "Error run script" << std::endl;
			std::cout << lua_tostring(Interpritator::L,-1) << std::endl;
			lua_pop(Interpritator::L,1);
			exit(3);
		}
	}
	tick2 = osg::Timer::instance()->tick();
	double Timers_T = osg::Timer::instance()->delta_s(tick1,tick2);

	//А вот тут собипаем статистику за итерацию....
	//Запрашиваем у всех систем/подситем....
	//изменяем HUD
	double Summ_T = (UserInput_T + UserCalc_T + Rendering_T + UpdateAudio_T) + scene_T + UserLuaInterpretation_T + RTI_T + SetObjectsInfo_T + Timers_T;
	std::ostringstream AddStatistic;
	AddStatistic.setf(std::ios::fixed | std::ios::showpoint);
	AddStatistic.precision(3);
	AddStatistic.clear();
        AddStatistic << "CASHE size=      : " << std::setw(8) << std::setfill('0') << FindNamedNodeVisitor::getCacheSize() << std::endl;
		AddStatistic << "VirtualVoices=   : " << std::setw(8) << std::setfill('0') << BaseSoundObject::VirtualVoices.size() << " ("<<scene->Audio->GetActiveSources() << ")" <<  std::endl	;
        //
	AddStatistic << "UserInput_T      : " << std::setw(8) << std::setfill('0') << UserInput_T/Summ_T * 100.0 << std::endl;
	AddStatistic << "UserCalc_T       : " << std::setw(8) << std::setfill('0') << UserCalc_T/Summ_T * 100.0 << std::endl;
	AddStatistic << "Rendering_T      : " << std::setw(8) << std::setfill('0') << Rendering_T/Summ_T * 100.0 << std::endl;
	AddStatistic << "UpdateAudio_T    : " << std::setw(8) << std::setfill('0') << UpdateAudio_T/Summ_T * 100.0 << std::endl;
	AddStatistic << "scene_T          : " << std::setw(8) << std::setfill('0') << scene_T/Summ_T*100.0 << std::endl ;
	AddStatistic << "UserLuaEvents_T  : " << std::setw(8) << std::setfill('0') << UserLuaInterpretation_T/Summ_T*100.0 << std::endl	;
	AddStatistic << "RTI_T            : " << std::setw(8) << std::setfill('0') << RTI_T/Summ_T * 100.0 << std::endl	;
	//AddStatistic << "SetObjectsInfo_T : " << std::setw(8) << std::setfill('0') << SetObjectsInfo_T/Summ_T *100.0 << std::endl	;
	AddStatistic << "Timers_T         : " << std::setw(8) << std::setfill('0') << Timers_T/Summ_T *100.0  << std::endl;
		
        localUser->updateHUDText(AddStatistic.str(), true);
}


//Обновление RTI
void rti_update()
{
	/*
    if( g_federate.is_run() )
    {
        // отправляем изменения в федерацию и получаем изменения из федерации
        g_federate.update();

		cFederate::removed_objects_t& removed = g_federate.get_removed_object_instances();
		for(cFederate::removed_objects_t::const_iterator it=removed.begin();it!=removed.end();++it)
		{
			Interpritator::RTI_Removed(*it);
		}
		removed.clear();

        // 1. при необходимости сообщаем о создании новых инстансов 'RTI_Discover'
        std::vector<std::map<std::string, cFederate::sObjectInstanceHandle>::iterator>* pv_discovers =
                g_federate.get_discover_object_instances();
        for( std::vector<std::map<std::string, cFederate::sObjectInstanceHandle>::iterator>::iterator itera =
                pv_discovers->begin(), itera_end = pv_discovers->end(); itera != itera_end; ++itera )
        {
                // (*itera)->first - instance object name	// (*itera)->second.class_itera->first - class name
                Interpritator::RTI_Discover( (*itera)->first, (*itera)->second.class_itera->first );
        }
        pv_discovers->clear();

        // 2. при необходимости сообщаем об изменении атрибутов 'RTI_Change'
		
        std::vector<cFederateAmbassador::sReflectObject>* pv_reflect_objects = g_federate.get_reflect_objects();
        for( std::vector<cFederateAmbassador::sReflectObject>::const_iterator itera_obj = pv_reflect_objects->begin(),
                itera_obj_end = pv_reflect_objects->end(); itera_obj != itera_obj_end; ++itera_obj )
        {
                Interpritator::RTI_Change( itera_obj->str_name, itera_obj->v_attributes, itera_obj->str_tag, itera_obj->time );
        }
        pv_reflect_objects->clear();

        // 3. при необходимости сообщаем о пришедших интеракциях 'RTI_Interaction'
        std::vector<cFederateAmbassador::sReflectInteraction>* pv_reflect_interactions =
                g_federate.get_reflect_interactions();
        for( std::vector<cFederateAmbassador::sReflectInteraction>::const_iterator itera =
                pv_reflect_interactions->begin(), itera_end = pv_reflect_interactions->end(); itera != itera_end;
                        ++itera )
        {
                Interpritator::RTI_Interaction( itera->str_name, itera->v_params, itera->str_tag );
        }
        pv_reflect_interactions->clear();
    }

	if(g_net_client)
		g_net_client->update();
		*/

}

void Bue()
{
	std::cout << "Destructor ManagerClass" << std::endl;
	//выход из LUA
	lua_close(Interpritator::L);
	// Удаление федерации и всех ее федератов. Нужно вызывать обязательно.

        //g_proxy_manager.destroy();

	//вызываем деструкторы
	delete scene;
	delete localUser;
	delete configuration;
        SCORM->SetDataToLMS();
        delete  SCORM;
	//прощаемся
	std::cout << "bye." << std::endl;
	//debug memoryleaks
	#ifdef memoryleaks
		_CrtDumpMemoryLeaks();
	#endif
}

//мышь в центр
void MouseInCenterScreen()
{
	#ifdef WIN32
		SetCursorPos(localUser->getViewWidth()/2,localUser->getViewHeight()/2);
	#endif
}

//включить/выкл вертикальную синхронизацию
void setVSync(int interval)
{
	#ifdef WIN32
	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = NULL;

	const unsigned char *extensions = glGetString( GL_EXTENSIONS );
	if( extensions == 0 || strstr( (const char *)extensions,
	"WGL_EXT_swap_control" ) == 0 ) return;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress(
	"wglSwapIntervalEXT" );
	if( wglSwapIntervalEXT ) wglSwapIntervalEXT(interval);
	#endif
} 





/*
//#include "Manager.h"



int main( int argc, char **argv )
{
	ManagerClass* manager = new ManagerClass;
	manager->Start();
	//посылаем всем федерациям привеееетттттт...)
	delete manager;
	exit(0);
}
*/

























