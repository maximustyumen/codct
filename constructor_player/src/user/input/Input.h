#ifndef INPUTCLASS
#define INPUTCLASS

#include "../../Configure/Configure.h"

#include "./InputKeyboard.h"
#include "./InputKeys.h"
#include "./InputMouse.h"
#include "./Joystick/InputJoystick.h"



#if (_MSC_VER < 1700)
#include "./vr/emagin_z800/emagin_z800.h"
#endif


//#include "./vr/oculus/oculus.h"

#include <osgViewer/Viewer>
#include <stdlib.h>
#include <stdio.h> 
#include <sstream>
#include <iostream>

#include "../HeadDataClass.h"
#include "../FootDataClass.h"
#include "../HandsDataClass.h"
#include "../HtcViveDataClass.h"


#include "../../VR/osgOpenVR/openvrviewer.h"

class UserInputClass
{
public:
	UserInputClass(osg::ref_ptr<osgViewer::Viewer>  _viewer,  Configure* configuration);
	//обновление и рачет данных устройств ввода
	void update();
	//установка режима управления
	//1 - просмотр 2- управление 3- перемещение
	void setInputMode(bool, bool, bool);
	//возврат параметров головы
	HeadData GetHeadData();
	//установка углов обзора головы
	void SetHeadOrientation(float,float,float);
	
	//возврат состояния "ног"
	FootData GetFootData();
	//возврат состояния "рук"
	HandsData GetHandsData();
	//возврат нажатых клавиш 
	std::vector <KeysOperationsClass> GetKeys();
	//возврат параметров джойстиков
	JoysticksData GetJoysticksData();
	//блокировка выхода (например F10)
	void BlockExit(bool block);

	//передача созданного в VideoOutput шлема VIVE
	void SetOpenvrDevice(osg::ref_ptr<OpenVRDevice> _OpenvrDevice) 
	{
		OpenvrDevice = _OpenvrDevice;
		m_htc_vive_enable=true;
	}
	//возврат всего что есть в VIVE
	HtcViveData GetHtcViveData() ;
	bool m_htc_vive_enable;
	osg::Quat HeadOrientationQuat;
	osg::Vec3 HeadPositionVec;

private:
	KeyboardEventHandler* keyboardInput;
	KeysEventHandler* keysInput;
	void KeyboardMoveCalculation();
	MouseEventHandler* mouseInput;
	#ifdef WIN32 
	#if (_MSC_VER < 1700)
		EmaginZ800Class* z800Input; 
	#endif
	#endif
	InputJoystickClass* JoyInput;


	//OculusClass* OculusInput;

	//
	osg::ref_ptr<osgViewer::Viewer> viewer;
	//значения углов головы 
	float HeadX, HeadY, HeadZ ;
	
	//вектора направления движения
	float MoveVectorX, MoveVectorY, MoveVectorZ;
	//zoom
	bool Zoom;
	//ширина и высота экрана
	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;
	//
	HandsData Hand;
	//
	bool emaginZ800;

	//передача созданного в VideoOutput шлема VIVE
	osg::ref_ptr<OpenVRDevice> OpenvrDevice;

	//
	void getEulerFromQuat(osg::Quat q, double& heading, double& attitude, double& bank) ;
	
};

#endif

