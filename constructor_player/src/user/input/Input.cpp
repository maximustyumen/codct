/***************************************************************************
Input.cpp  -  Ввод пользователя пользователем
-------------------
begin                : 10 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
// 
// подсистема взаимодействия пользователя
// 
#include "Input.h"



UserInputClass::UserInputClass(osg::ref_ptr<osgViewer::Viewer> _viewer,  Configure* configuration)
{
	std::cout << std::endl << "UserInputClass constructor ... " <<std::endl;

	m_htc_vive_enable = false;

	//опрашиваем все известные нам устройства VR
	emaginZ800 = false;

	#ifdef WIN32
		#if (_MSC_VER < 1700)
		emaginZ800 = EmaginZ800Class::DetectEmaginZ800();
		if (emaginZ800==true)
		{	
			std::cout << "\tactivating Emagin Z800 tracker..."  << std::endl;
			z800Input = new EmaginZ800Class();
		}
		#endif
	#else
		emaginZ800 = false;
	#endif

	//джойстик
	JoyInput = new InputJoystickClass();
	//
	//hydraInput = new InputHydraClass();
	//
	//OculusInput = new OculusClass();
	

	
	//подключаем мышь в любом случае
	if (true) //emaginZ800==false
	{
		//Подключение состояния манипулятора мышь (только значения)
		mouseInput = new MouseEventHandler(configuration);
		_viewer->addEventHandler(mouseInput);
		//разрешаем все действия (просмотр, управление и передвижение)
		mouseInput->setMode(true,true,true);
	}
	//для событий типа нажата клавиша "а"
	keysInput = new KeysEventHandler();
	_viewer->addEventHandler(keysInput);
	//
	//Подключение состояния клавиш (только значения..это уже определенные клавиши для передвижения)
	keyboardInput = new KeyboardEventHandler();
	keyboardInput->BlockExit(false);
	_viewer->addEventHandler(keyboardInput);
	//
	viewer = _viewer;
	//std::cout << (int)osg::DisplaySettings::instance()->getScreenHeight() << std::endl;
	//узнаем размеры экрана
	osgViewer::Viewer::Windows windows;
	viewer->getWindows(windows);
	osgViewer::Viewer::Windows::iterator itr = windows.begin();
	int tempx,tempy;
	(*itr)->getWindowRectangle(tempx, tempy, WINDOW_WIDTH, WINDOW_HEIGHT);
	//
	HeadX=0;
	HeadY=0;
	HeadZ=0;
	//
	MoveVectorX = 0;
	MoveVectorY = 0;
	MoveVectorZ = 0;
	return;
}

//установка ориентации головы
void UserInputClass::SetHeadOrientation(float x, float y, float z)
{
	if (emaginZ800==false)
	{
                mouseInput->SetHeadOrientation (x,y);
	}
}


//установка режима управления
//1 - просмотр 2- управление 3- перемещение
void UserInputClass::setInputMode(bool head, bool commands, bool move)
{
	//обработка просмотр - мышь
	if (emaginZ800==false)
	{
		mouseInput->setMode (head, commands, move);
	}
	//обработка перемещения ног - клавиатура
	keyboardInput->setMode (head, commands, move);

}

//блокировка выхода (например F10)
void UserInputClass::BlockExit(bool block)
{
	//для клавиатуры
	keyboardInput->BlockExit(block);
}


void UserInputClass::update()
{
	//vive
	HtcViveData ViveData = GetHtcViveData() ;

	//oculus
	//OculusInput->update();


	//обработка мыши
	if ((emaginZ800==false)&&(m_htc_vive_enable==false))
	{
		//1. расчет направления взгляда
		HeadData data = mouseInput->GetHeadData();
		HeadX = data.GetYaw();
		HeadY = data.GetPitch();
		HeadZ = data.GetRoll();	
	}
	if (m_htc_vive_enable==true)
	{
		//1. расчет направления взгляда
		
		HeadX = osg::RadiansToDegrees (ViveData.HeadOrientationEuler.x());
		HeadY = -1.0f * osg::RadiansToDegrees (ViveData.HeadOrientationEuler.z());
		HeadZ = 1.0f * osg::RadiansToDegrees (ViveData.HeadOrientationEuler.y());	
		//HeadOrientationQuat = ViveData.HeadOrientationQuat;
	}
	//обработка Z800
	#ifdef WIN32
	#if (_MSC_VER < 1700)
	if (emaginZ800==true)
	{
		HeadData data = z800Input->GetHeadData();
		HeadX = data.GetYaw();
		HeadY = data.GetPitch();
		HeadZ = data.GetRoll();		
	}
	#endif
	#endif
	//обработка клавы
	{
		FootData data = keyboardInput->GetFootData();
		MoveVectorX= data.getVectorX();
		MoveVectorY= data.getVectorY();
		MoveVectorZ= data.getVectorZ();
		Zoom = data.GetZoom();
	}

	if (m_htc_vive_enable==true)
	{
		MoveVectorX += ViveData.RightHand_TrackPad_x;
		MoveVectorY += ViveData.RightHand_TrackPad_y;
	}


	//обработка рук (мыши)
	{
		Hand = mouseInput->GetHandsData();
	}
	//обработка джойстика
	{
		JoyInput->update();
	}
}

//возврат параметров головы
HeadData UserInputClass::GetHeadData()
{
	//+ hydraData.dHeadY

	HeadData data;
	data.SetMode (HeadData::ORIENTATION);
	data.SetPitch (HeadY);
	data.SetYaw (HeadX);
	data.SetRoll(HeadZ);

	//для гидры подправляем значения
	SetHeadOrientation (HeadX, HeadY , HeadZ);
	return data;
}

//возврат состояния "ног"
FootData UserInputClass::GetFootData()
{
	FootData data;
	
	data.SetMode (FootData::VECTORS);
	data.SetVectorX (MoveVectorX); //  hydraData.footX * 3.0f
	data.SetVectorY (MoveVectorY) ; // hydraData.footY * 3.0f
	data.SetVectorZ (MoveVectorZ);
	data.SetZoom (Zoom);
	return data;
}

//возврат состояния "рук"
HandsData UserInputClass::GetHandsData()
{
	return Hand;
}

//возврат нажатых клавиш 
std::vector <KeysOperationsClass> UserInputClass::GetKeys()
{
	return keysInput->GetKeys();
}

//возврат параметров джойстиков
JoysticksData UserInputClass::GetJoysticksData()
{	
	return JoyInput->GetJoysticksData ();
}


void UserInputClass::getEulerFromQuat(osg::Quat q, double& heading, double& attitude, double& bank) 
{ 
	double limit = 0.499999; 
	double sqx = q.x()*q.x(); 
	double sqy = q.y()*q.y(); 
	double sqz = q.z()*q.z(); 
	double t = q.x()*q.y() + q.z()*q.w(); 

	if (t>limit) // gimbal lock ? 
	{ 
		heading = 2 * atan2(q.x(),q.w()); 
		attitude = osg::PI_2; 
		bank = 0; 
	} 
	else if (t<-limit) 
	{ 
		heading = -2 * atan2(q.x(),q.w()); 
		attitude = - osg::PI_2; 
		bank = 0; 
	} 
	else 
	{ 
		heading = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*sqy - 2*sqz); 
		attitude = asin(2*t); 
		bank = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*sqx - 2*sqz); 
	} 
} 


//возврат всего что есть в VIVE
HtcViveData UserInputClass::GetHtcViveData() 
{
	//https://github.com/ValveSoftware/openvr/wiki/VREvent_t

	HtcViveData a;

	if (m_htc_vive_enable==true)
	{

		if (OpenvrDevice->position().size()>=1)
		{
			a.presend = true;

			{
				osg::Vec3 position = OpenvrDevice->position().at(0);
				osg::Quat orientation = OpenvrDevice->orientation().at(0);
				double heading, attitude,  bank;
				getEulerFromQuat (orientation, heading, attitude,  bank);

				a.HeadOrientationEuler = osg::Vec3( heading, attitude, bank);
				a.HeadOrientationQuat = orientation;
				a.HeadPosition = position;

				HeadOrientationQuat = orientation;

				HeadPositionVec = position;
			}
		}
		

		if (OpenvrDevice->position().size()>=3)
		{
			{
				osg::Vec3 position = OpenvrDevice->position().at(1);
				osg::Quat orientation = OpenvrDevice->orientation().at(1);
				double heading, attitude,  bank;
				getEulerFromQuat (orientation, heading, attitude,  bank);

				a.LeftHandOrientationEuler = osg::Vec3( heading, attitude, bank);
				a.LeftHandOrientationQuat = orientation;
				a.LeftHandPosition = position;
			}
			{
				osg::Vec3 position = OpenvrDevice->position().at(2);
				osg::Quat orientation = OpenvrDevice->orientation().at(2);
				double heading, attitude,  bank;
				getEulerFromQuat (orientation, heading, attitude,  bank);

				a.RightHandOrientationEuler = osg::Vec3( heading, attitude, bank);
				a.RightHandOrientationQuat = orientation;
				a.RightHandPosition = position;
			}
			//std::cout << i << " heading= " << heading << " attitude= " << attitude << " bank= " << bank <<  std::endl;
			//std::cout << i << " x= " << position.x() << " y= " << position.y() << " z= " << position.z() <<  std::endl;
			//std::cout << "!!!ok!!!" << std::endl;
		}

		
		if (OpenvrDevice->buttonsAndAnalogs().size()>=2)
		{
			a.RightHand_EButton_ApplicationMenu = OpenvrDevice->buttonsAndAnalogs().at(0).k_EButton_ApplicationMenu;
			a.RightHand_EButton_SteamVR_Trigger = OpenvrDevice->buttonsAndAnalogs().at(0).k_EButton_SteamVR_Trigger;
			a.RightHand_EButton_Grip = OpenvrDevice->buttonsAndAnalogs().at(0).k_EButton_Grip;
			a.RightHand_EButton_SteamVR_Touchpad = OpenvrDevice->buttonsAndAnalogs().at(0).k_EButton_SteamVR_Touchpad;
			a.RightHand_TrackPad_x = OpenvrDevice->buttonsAndAnalogs().at(0).TrackPad_x;
			a.RightHand_TrackPad_y = OpenvrDevice->buttonsAndAnalogs().at(0).TrackPad_y;
			a.RightHand_Trigger_x = OpenvrDevice->buttonsAndAnalogs().at(0).Trigger_x;
			a.RightHand_Trigger_y = OpenvrDevice->buttonsAndAnalogs().at(0).Trigger_y;
			a.RightHand_Joystick_x = OpenvrDevice->buttonsAndAnalogs().at(0).Joystick_x;
			a.RightHand_Joystick_y = OpenvrDevice->buttonsAndAnalogs().at(0).Joystick_y;

			a.LeftHand_EButton_ApplicationMenu = OpenvrDevice->buttonsAndAnalogs().at(1).k_EButton_ApplicationMenu;
			a.LeftHand_EButton_SteamVR_Trigger = OpenvrDevice->buttonsAndAnalogs().at(1).k_EButton_SteamVR_Trigger;
			a.LeftHand_EButton_Grip = OpenvrDevice->buttonsAndAnalogs().at(1).k_EButton_Grip;
			a.LeftHand_EButton_SteamVR_Touchpad = OpenvrDevice->buttonsAndAnalogs().at(1).k_EButton_SteamVR_Touchpad;
			a.LeftHand_TrackPad_x = OpenvrDevice->buttonsAndAnalogs().at(1).TrackPad_x;
			a.LeftHand_TrackPad_y = OpenvrDevice->buttonsAndAnalogs().at(1).TrackPad_y;
			a.LeftHand_Trigger_x = OpenvrDevice->buttonsAndAnalogs().at(1).Trigger_x;
			a.LeftHand_Trigger_y = OpenvrDevice->buttonsAndAnalogs().at(1).Trigger_y;
			a.LeftHand_Joystick_x = OpenvrDevice->buttonsAndAnalogs().at(1).Joystick_x;
			a.LeftHand_Joystick_y = OpenvrDevice->buttonsAndAnalogs().at(1).Joystick_y;
		}
		

	}

	
	return a;
}
