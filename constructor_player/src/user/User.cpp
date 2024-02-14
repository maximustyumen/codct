/***************************************************************************
User.cpp -  Определение класса лакального пользователя 
-------------------
begin                : 2 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
//класс пользователя объеденяет классы ввода и вывода, обработку событий и передачу в модуль интепритации
/*
User---
      |-Input  ------------------
	  |                         |-InputVR
	  |							|-InputKeyboardMouse
	  |
	  |- Output------------------
	                            |-OutputVideo
	  							|-OutputAudio
*/

//подключение заголовочного файла
#include "User.h"
//#include <iomanip>




//конструктор
UserClass::UserClass(Configure* _configuration, osg::ref_ptr<osgViewer::Viewer> _viewer)
{
	configuration = _configuration;
	std::cout << "UserClass constructor ... " <<std::endl;

	preLoading=false;

	UserOutput = new UserOutputClass (configuration, _viewer);
	//создаем обработчики устройств ввода 
	//для указания с какого окна брать клавиши, передаем viewer
	UserInput = new UserInputClass(UserOutput->getViewer(),configuration) ;

	//если нужно включить VIVE
	if (configuration->GetRecordString("#HTC_VIVE_ON") == "ON")
	{
		//нужно передать информацию с VIVE на UserInput
		#ifdef WIN32
		UserInput->SetOpenvrDevice(UserOutput->GetOpenvrDevice());
		#endif
	}

	//
	BodyX = 0; BodyY = 0; BodyZ = 0;
	HeadX = 0; HeadY = 0; HeadZ = 0;
	oldBodyX = 0; oldBodyY = 0; oldBodyZ = 0;
	//
	HumanHeight = 37.0f;
	//
	initialize=false;
	//
	developmode=false;
	//
	hideHUDText = false;
	//
	//значение стандартного фокусного расстояния из файла конфигурации
        StandartFocus = configuration->GetRecordFloat("#Focus");
        //
        tranlator = new KeyTranslation();

	m_oculus_enable = configuration->GetRecordString("#OCULUS_ON") == "ON";
}

//деструктор
UserClass::~UserClass()
{
	std::cout << "UserClass destructor..." << std::endl;
	delete UserOutput;
	delete UserInput;

        delete tranlator;
}

//
void UserClass::setDataToVideoViewer(osg::ref_ptr<osg::Node> _root, osg::Node* menu_node)
{
	initialize=true;
	//получив ссылку на группу сцены, можем создавать userFoot
	userFoot = new UserFootClass(_root, HumanHeight, configuration);
	//получив ссылку на группу сцены, можем создавать UserHands
	userHands = new UserHandsClass(_root, UserOutput->getViewer(), m_oculus_enable);

	//
	UserOutput->setDataToVideoViewer(_root,menu_node,userHands->get_cursor_node());
}


//установка режима управления
//1 - просмотр 2- управление 3- перемещение
void UserClass::setInputMode(bool orient, bool commands, bool move)
{
	//для внутренних обработчиков
	UserInput->setInputMode (orient,commands, move);
	//флаг разрешения режима просмотра (изменения направления взгляда)
	viewModeEnable = orient;
	//флаг разрешения режима управления
	commandModeEnable = commands;
	//флаг разрешения обработки перемещения пользователя с помощью прокрутки мыши
	moveModeEnable=move;
}

//
void UserClass::setUserFocusLight(float focus)
{
	UserOutput->setUserFocusLight(focus);
}

//запрос, работает-ли тренажер
bool UserClass::isWork()
{
	return !UserOutput->getViewer()->done();
}

//Выход из тренажера
void UserClass::Exit()
{

	UserOutput->getViewer()->setDone (true);
}

//блокировка выхода (например F10)
void UserClass::BlockExit(bool block)
{
	//блокировка выхода на всех устройствах ввода
	UserInput->BlockExit(block);
}

//screenshot
void UserClass::takeScreenshot(std::string filaneme)
{
	UserOutput->takeScreenshot(filaneme);
}

//замена значения текста рядом с курсором
void UserClass::SetCursorText(std::string value)
{
    if (initialize==true)
    {
        userHands->SetCursorText(value);
    }
}
void UserClass::SetCursorTextSize (int newsize)
{
    if (initialize==true)
    {
        userHands->SetCursorTextSize(newsize);
    }
}
void UserClass::SetCursorTextFont (std::string filename)
{
    if (initialize==true)
    {
        userHands->SetCursorTextFont(filename);
    }
}
void UserClass::SetCursorTextColor (float r,float g ,float b ,float a)
{
    if (initialize==true)
    {
        userHands->SetCursorTextColor(r,g,b,a);
    }
}

//
void UserClass::update()
{
        //статистика
	osg::Timer_t tick1;
	osg::Timer_t tick2;

    if (preLoading==true)
    {
	//обновляем информацию по всем устройствам ввода
	tick1 = osg::Timer::instance()->tick();
	UserInput->update();
	tick2 = osg::Timer::instance()->tick();
	UserInput_T = osg::Timer::instance()->delta_s(tick1,tick2);

	//все скинуть в одну очередь событий...
	//на интерпритатор

	tick1 = osg::Timer::instance()->tick();
	//0.Руки, т.к. координаты указывались при текущем положении камеры
	//события "рук"
	Hand = UserInput->GetHandsData();

	//HtcVive
	HtcViveData _HtcViveData = UserInput->GetHtcViveData();
	userHands->UpdateVRHands(_HtcViveData,BodyX, BodyY, BodyZ);
	

    userHands->setDataAndCalc (Hand);

	


	//1.Голова
	HeadData headdata = UserInput->GetHeadData();
	HeadX = headdata.GetYaw();
	HeadY = headdata.GetPitch();
	HeadZ = headdata.GetRoll();
	//2.Ноги - передвижение, zoom
	if (moveModeEnable)
	{
		userFoot->setDataAndCalc (UserInput->GetFootData(), UserInput->GetHeadData());
		BodyX = userFoot->GetX();
		BodyY = userFoot->GetY();
		BodyZ = userFoot->GetZ();
		BodyZoom = userFoot->GetZOOM();
	}
	//если упали - восстанавливаемся в начале
	if (BodyZ<10)
	{
		BodyX=StartX;
		BodyY=StartY;
		BodyZ=StartZ;
		HeadX = StartHeadX;
		HeadY = StartHeadY;
		HeadZ = StartHeadZ;
		oldBodyX = BodyX; oldBodyY = BodyY; oldBodyZ = BodyZ;
		//перемещаем ноги
		userFoot->SetNewPosition (BodyX,BodyY,BodyZ);
		//поворачиваем голову
		UserInput->SetHeadOrientation(HeadX,HeadY,HeadZ);
	}
	tick2 = osg::Timer::instance()->tick();
	UserCalc_T = osg::Timer::instance()->delta_s(tick1,tick2);

    }
        //визуализация
	tick1 = osg::Timer::instance()->tick();
	UpdateCamera();
	tick2 = osg::Timer::instance()->tick();
	Rendering_T = osg::Timer::instance()->delta_s(tick1,tick2);


	//обновление звуковой сцены
	tick1 = osg::Timer::instance()->tick();
	UserOutput->UpdateAudio (BodyX,BodyY,BodyZ,HeadX,HeadY,HeadZ);
	tick2 = osg::Timer::instance()->tick();
	UpdateAudio_T = osg::Timer::instance()->delta_s(tick1,tick2);

	//изменяем HUD
	//setHUDText();

	//Модификация звука пользователя (собственные шаги?)
}


void UserClass::UpdateCamera()
{
	//рабочий режим
	if (preLoading)
	{          
		//передвигаем камеру в соответствии с координатами пользователя и направлением его взгляда
		//следующие объявления отвечают за создание пользовательской камеры
		osg::Matrixd myCameraMatrix;
		osg::Matrixd cameraRotation;
		osg::Matrixd cameraTrans;

		float xtrans = BodyX;
		float ztrans = BodyY;
		float ytrans = BodyZ;
		float xrotate  = 360.0f - HeadX;
		float yrotate  = HeadY;//UserInput->getYrotate();
		float zrotate  = HeadZ;

		//если Zoom не 1.0 то изменяем фокусное
        static bool fixflag = false;
		if (BodyZoom!=1.0f) 
		{
                    fixflag = true;
                    //узнаем принятое в конфигурации фокусное расстояние
                    setUserFocusLight (StandartFocus/BodyZoom);
		}
        //fix возврата в нормальный зум при низком FPS
        if ((BodyZoom==1.0f)&&(fixflag==true))
        {
            setUserFocusLight (StandartFocus);
            fixflag = false;
        }


		if (UserInput->m_htc_vive_enable == false)
		{
			cameraRotation.makeRotate(
			osg::DegreesToRadians(zrotate), osg::Vec3(0,1,0), // roll
			osg::DegreesToRadians(yrotate), osg::Vec3(1,0,0) , // pitch
			osg::DegreesToRadians(xrotate), osg::Vec3(0,0,1) ); // heading 

			cameraTrans.makeTranslate( xtrans,ztrans,ytrans );
			myCameraMatrix = cameraRotation * cameraTrans;

			

			osg::Matrixd i = myCameraMatrix.inverse(myCameraMatrix);
			UserOutput->getViewer()->getCamera()->setViewMatrix(i* osg::Matrixd::rotate( -3.14f / 2.0, 1, 0, 0 ));

			osg::Matrixd Hud3DMatrix = UserOutput->getViewer()->getCamera()->getInverseViewMatrix();
			userHands->update3DHUD(Hud3DMatrix);
		}
		else
		{
			osg::Quat z = UserInput->HeadOrientationQuat;
			osg::Quat x (-z.x(), z.z(), -z.y(), z.w());
			cameraRotation.makeRotate(x); // heading 
			//

			osg::Vec3 XYZ = UserInput->HeadPositionVec;
			//cameraTrans.makeTranslate( xtrans ,ztrans,ytrans );
		    cameraTrans.makeTranslate( xtrans + XYZ.x() * 25,ztrans - XYZ.z()*25,ytrans + XYZ.y()*25 - 15.0f);
			myCameraMatrix = cameraRotation * cameraTrans;


			osg::Matrixd i = myCameraMatrix.inverse(myCameraMatrix);
			UserOutput->getViewer()->getCamera()->setViewMatrix(i* osg::Matrixd::rotate( -3.14f / 2.0, 1, 0, 0 ));

			osg::Matrixd Hud3DMatrix = UserOutput->getViewer()->getCamera()->getInverseViewMatrix();
			userHands->update3DHUD(Hud3DMatrix);

			//osg::Matrixd Hud3DMatrix =UserOutput->getViewer()->getCamera()->getInverseViewMatrix();
			//Hud3DMatrix.preMultRotate(UserInput->HeadOrientationQuat.inverse());
			//userHands->update3DHUD(Hud3DMatrix);
		}
	}
	//режим загрузки
	else
	{
		preLoading = true;
		//передвигаем камеру в 
		osg::Matrixd myCameraMatrix;
		osg::Matrixd cameraRotation;
		osg::Matrixd cameraTrans;

		float xtrans = 0;
		float ztrans = -1000;
		float ytrans = 0;
		float xrotate  = 360.0f;
		float yrotate  = 0;//UserInput->getYrotate();
		float zrotate  = 0;

		cameraRotation.makeRotate(
		osg::DegreesToRadians(zrotate), osg::Vec3(0,1,0), // roll
		osg::DegreesToRadians(yrotate), osg::Vec3(1,0,0) , // pitch
		osg::DegreesToRadians(xrotate), osg::Vec3(0,0,1) ); // heading 

		cameraTrans.makeTranslate( xtrans,ztrans,ytrans );
		myCameraMatrix = cameraRotation * cameraTrans;

		osg::Matrixd i = myCameraMatrix.inverse(myCameraMatrix);
		UserOutput->getViewer()->getCamera()->setViewMatrix((osg::Matrixd(i.ptr()))* osg::Matrixd::rotate( -3.14f / 2.0, 1, 0, 0 ));
	}
	//
	osgViewer::Viewer* viewer = UserOutput->getViewer().get();
	const osg::Matrixd& view = viewer->getCamera()->getViewMatrix();
	if(viewer->getSceneData())
	{
		viewer->getSceneData()->getOrCreateStateSet()->addUniform(new osg::Uniform("matrix_view0", osg::Vec4(view(0,0),view(0,1),view(0,2),view(0,3)) ));
		viewer->getSceneData()->getOrCreateStateSet()->addUniform(new osg::Uniform("matrix_view1", osg::Vec4(view(1,0),view(1,1),view(1,2),view(1,3)) ));
		viewer->getSceneData()->getOrCreateStateSet()->addUniform(new osg::Uniform("matrix_view2", osg::Vec4(view(2,0),view(2,1),view(2,2),view(2,3)) ));
		viewer->getSceneData()->getOrCreateStateSet()->addUniform(new osg::Uniform("matrix_view3", osg::Vec4(view(3,0),view(3,1),view(3,2),view(3,3)) ));
	}

	// fire off the cull and draw traversals of the scene.
	//UserOutput->getViewer()->frame();
}

void UserClass::frame()
{
	UserOutput->getViewer()->frame();
}


void UserClass::SetUserPositionAndOrientation(float x,float y, float z, float hx, float hy, float hz)
{
	//Запоминаем координаты, вдруг нужно будет вспомнить
	StartX=x;
	StartY=y;
	StartZ=z;
	StartHeadX = hx;
	StartHeadY = hy;
	StartHeadZ = hz;
	//
	BodyX = x; BodyY = y; BodyZ = z;
	BodyZoom = 1.0f;
	HeadX = hx; HeadY = hy; HeadZ = hz;
	oldBodyX = x; oldBodyY = y; oldBodyZ = z;
	//перемещаем ноги
	userFoot->SetNewPosition (x,y,z);
	//поворачиваем голову
	UserInput->SetHeadOrientation(hx,hy,hz);
}

//возврат копии буфера накопленных событий во внутреннем формате (на интерпритацию LUA)...
InternalEventsClass UserClass::getHandsEvents()
{
	return userHands->getHandsEvents();
}

//возврат копии буфера накопленных событий во внутреннем формате (на интерпритацию LUA)...
InternalEventsClass UserClass::getFootEvents()
{
	return  userFoot->getFootEvents();
}

//возврат копии буфера накопленных событий во внутреннем формате (на интерпритацию LUA)...
InternalEventsClass UserClass::getKeysEvents()
{
	InternalEventsClass data;
	std::vector <KeysOperationsClass> keys = UserInput->GetKeys();
	for (unsigned int i=0; i < keys.size() ;i++)
	{
		//std::cout << keysOperations.at(i).operation << " " << keysOperations.at(i).keyCode << std::endl;
		InternalEventsClass::OneInternalEventClass oneevent;
		if (keys.at(i).operation==KeysOperationsClass::PUSH)
		{
			oneevent.Event="KEY_PUSH";
                        if (tranlator->GetConvertKey( keys.at(i).keyRawCode)==69)
			{
				InternalEventsClass::OneInternalEventClass useevent;
				useevent.Event = userHands->GetCurrentObjectName()+"_USE";
				data.addEvent(useevent);
			}
			//Клавиша отключения/включения статистики при "Develop Mode" ()
			if ((keys.at(i).keyCode==osgGA::GUIEventAdapter::KEY_Pause)&&(keys.at(i).SHIFT==true))
			{
				hideHUDText = !hideHUDText;
			}
		}
		else
		{
			oneevent.Event="KEY_RELEASE";
		}
                oneevent.number_param.push_back(tranlator->GetConvertKey( keys.at(i).keyRawCode));
		oneevent.number_param.push_back(keys.at(i).ALT );
		oneevent.number_param.push_back(keys.at(i).CTRL );
		oneevent.number_param.push_back(keys.at(i).SHIFT);
                oneevent.number_param.push_back(keys.at(i).keyRawCode);
                //std::cout << "**key=" << keys.at(i).keyRawCode <<std::endl;
		data.addEvent(oneevent);
	}
	return data;
}

//Возврат состояния джойстиков
JoysticksData UserClass::GetJoysticksData()
{
	return UserInput->GetJoysticksData();
}

//возврат значений ширины/высоты экрана наблюдателя
int UserClass::getViewHeight()
{
	//узнаем размеры экрана
	osgViewer::Viewer::Windows windows;
	UserOutput->getViewer()->getWindows(windows);
	osgViewer::Viewer::Windows::iterator itr = windows.begin();
	int tempx,tempy,WINDOW_WIDTH,WINDOW_HEIGHT;
	(*itr)->getWindowRectangle(tempx, tempy, WINDOW_WIDTH, WINDOW_HEIGHT);
	return WINDOW_HEIGHT;
}
int UserClass::getViewWidth()
{
	//узнаем размеры экрана
	osgViewer::Viewer::Windows windows;
	UserOutput->getViewer()->getWindows(windows);
	osgViewer::Viewer::Windows::iterator itr = windows.begin();
	int tempx,tempy,WINDOW_WIDTH,WINDOW_HEIGHT;
	(*itr)->getWindowRectangle(tempx, tempy, WINDOW_WIDTH, WINDOW_HEIGHT);
	return WINDOW_WIDTH;
}

//Изменение текста спец. HUD
std::string UserClass::updateHUDText(std::string statistic, bool show)
{
	static double deltaT=0;
	//Расчет FPS
	static osg::Timer_t old_tick = osg::Timer::instance()->tick();
	osg::Timer_t current_tick = osg::Timer::instance()->tick();
	// за последнюю итерацию прошло   
	deltaT = osg::Timer::instance()->delta_s(old_tick,current_tick);
	old_tick=current_tick;
	//а таких итераций в секунде будет
	float FPS = 1.0f/deltaT;

	std::ostringstream hud;
	hud.clear();
        hud.imbue( std::locale::classic());

	hud<<std::endl<<std::endl<<"DEVELOP MODE"<<std::endl<<std::endl;
        hud<<"fps: "<< (int)FPS<<std::endl;
        //hud.setf (std::ios::dec , std::ios::basefield);
        //hud.fill('0') ;

        hud<<"x: " <<  (int)userFoot->GetX() << " y: " << (int)userFoot->GetY() << " z: " << (int)userFoot->GetZ() << std::endl;
        hud<<"HeadX: " <<  (int)HeadX << " HeadY: " << (int)HeadY << " HeadZ: " << (int)HeadZ << std::endl;
	hud<<"object: "<< userHands->GetCurrentObjectName() << std::endl;
	hud<<"parent: "<<userHands->GetParentObjectName() << std::endl;;
	hud<<statistic;

        std::string dump = "";
	//Выводим HUD только если DevelopMode!
	if ((developmode==true)&&(hideHUDText==false))
	{
                //В виджите не выводить
                if (show==true)
                {
                    UserOutput->SetHUDText (hud.str());
                }
                else
                {
                    UserOutput->SetHUDText    (dump);
                }
                return hud.str();
	}
	else
	{
		UserOutput->SetHUDText (dump);
                return dump;
	}
}

//возврат значений по текущему объекту управления
std::string UserClass::GetCurrentObjectName()
{
	return userHands->GetCurrentObjectName();
}
osg::Vec3 UserClass::GetCurrentObjectIntersectsCoordLOCAL()
{
	return userHands->GetCurrentObjectIntersectsCoordLOCAL();
}
osg::Vec3 UserClass::GetCurrentObjectIntersectsCoordWORLD()
{
	return userHands->GetCurrentObjectIntersectsCoordWORLD();
}

//setCompileOnNextDraw
void UserClass::setCompileOnNextDraw()
{
    UserOutput->setCompileOnNextDraw();
}

//Полное кеширование сцены (уже после подключения данных к Viewer'у)
void UserClass::FullCompileScene(bool full)
{
    UserOutput->FullCompileScene(full);
}

osg::Camera* UserClass::get_camera()
{
	return UserOutput->getViewer()->getCamera();
}