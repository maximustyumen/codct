/***************************************************************************
emagin_z800.cpp  -  стандартный интерфейс для описания шлема VR EMaginZ800
-------------------
begin                : 10 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "emagin_z800.h"

#ifdef WIN32

float EmaginZ800Class::Yaw =0.0f;
float EmaginZ800Class::Pitch =0.0f;
float EmaginZ800Class::Roll =0.0f;

//конструктор
EmaginZ800Class::EmaginZ800Class()
{
	std::cout << "\tEmaginZ800 constructor ... " <<std::endl;

	Z800=NULL;
	emaTracker = NULL;

	InitializeHeadset();
	//толку от аксилометра НИКАКОГО!
	//if (useEmagin) InitializeGestureAnalyzer();
	if (useEmagin) InitializeTracker();
	
}


void EmaginZ800Class::DeInitializeTracker() 
{ 
    // destroy tracker 
    if ( NULL != emaTracker ) 
    { 
        if ( NULL != Z800 )
        {
            Z800->GoSleep();
        }
        delete emaTracker; 
    } 
} 

//возврат параметров головы
HeadData EmaginZ800Class::GetHeadData()
{
	HeadData data;
	data.SetMode (HeadData::ORIENTATION);
	data.SetPitch (-Pitch*100.0f);
	data.SetYaw (-Yaw * 60.0f);
	data.SetRoll(Roll*46.0f);
	return data;
}


void EmaginZ800Class::DeInitializeHeadset ( ) 
{ 
    // destroy device 
    if ( NULL != Z800 ) 
    { 
        delete Z800 ; 
    } 
} 

HRESULT CALLBACK EmaginZ800Class::TrackProc(EMADeviceData* data, void* param)
{
    if ( NULL == data )
    {
        return EMA_INVALID_PARAMETER;
    }


	
    // make a copy of the command so we can use it later on
    // when we draw the data on the screen
	Yaw = data->Yaw;
    Pitch = data->Pitch;
    Roll = data->Roll;

	//проверяем связанные с этим пользовательские события (то-есть пользователь что-то УВИДЕЛ)
	//UserEYEActionFunc();

	//std::cout << Yaw << " " << Pitch << " " << Roll << std::endl;

    return S_OK;
}




void EmaginZ800Class::InitializeHeadset ( ) 
{ 
	Z800 = new EMADevice () ;
	if ( NULL == Z800 ) 
	{ 
		HRESULT hr = EMADevice::GetLastError (  ) ; 
		std::cout << "Could not find EMAGIN Z800" << std::endl;
        //MessageBoxA ( NULL , "Could not connect to headset" , "eMagin SDK - MAXlab play" , MB_OK ) ; 
		DeInitializeHeadset ( ) ;
		useEmagin = false;
		return; 
	} 
    // Get device information 
    Z800->GetDeviceInfo ( & Z800Info ) ; 
    // Turn on the headset if it was sleeping 
    Z800->KeepAlive ( ) ;
	//notebooks
	Z800->EnableExternalMonitor ( true ) ; 
	std::cout << "EMAGIN Z800 Headset detect!  "  << Z800Info.ScreenX << " " << Z800Info.ScreenY << std::endl;
	useEmagin = true;
}


void EmaginZ800Class::InitializeTracker() 
{ 
	emaTracker = new EMATracker(NULL, EMA_NONE); //Z800 / NULL
	if ( NULL == emaTracker ) 
	{ 
		HRESULT hr = EMATracker::GetLastError(); 
        std::cout << "Could not find EMAGIN Z800 tracker" << std::endl;
		useEmaginTracker = false;
		return; 
	} 
    
    if ( SUCCEEDED(emaTracker->GetEMADevice(&Z800)) )
    {
        Z800->KeepAlive();
    }

    emaTracker->SetTrackCallback(TrackProc, NULL);
    emaTracker->EnablePolling(true);
    emaTracker->EnableTracking(true);
	emaTracker->SetPollingPeriod(30);

	useEmaginTracker = true;
	std::cout << "EMAGIN Z800 Headset tracer detect !" << std::endl;
} 


#endif


int EmaginZ800Class::EmaginZ800detected = 0;

bool EmaginZ800Class::DetectEmaginZ800()
{
	std::cout << "\tfindind EMAGIN Z800 ... ";
	//если тестирование уже проводилось, выдаем ранее полученный результат, иначе это еще время теряется
	if (EmaginZ800detected ==2) return true;
	if (EmaginZ800detected ==1) 
	{
		std::cout << "not find (emagin agent closed?)" << std::endl;
		return false;
	}

	//если тестирование не проводилось, проводим
	//если линух, то не имеет смысла искать
	#ifndef WIN32
		std::cout << "EMAGIN Z800 not work in Linux(" << std::endl;
		EmaginZ800detected =1;
		return false;
	#endif
	//пробуем найти
	#ifdef WIN32 
		bool useEmagin=false;
		bool useEmaginTracker=false;
		EMADevice *Z800 = new EMADevice () ;
		if ( NULL == Z800 ) 
		{ 
			HRESULT hr = EMADevice::GetLastError (  ) ; 
			std::cout << "not find (emagin agent closed?)" << std::endl;
			useEmagin = false;
			EmaginZ800detected =1;
		} 
		else
		{
			EMADeviceInfo Z800Info;
			// Get device information 
			Z800->GetDeviceInfo ( & Z800Info ) ; 
			// Turn on the headset if it was sleeping 
			Z800->KeepAlive () ; 
			std::cout << "detect !" << std::endl << Z800Info.ScreenX << " " << Z800Info.ScreenY << std::endl;
			EmaginZ800detected =2;
			useEmagin = true;
		}
		// initialize the tracker interface
		EMATracker *emaTracker;
		if (useEmagin) 
		{
			emaTracker = new EMATracker(NULL, EMA_NONE); //Z800 / NULL
			if ( NULL == emaTracker ) 
			{ 
				HRESULT hr = EMATracker::GetLastError(); 
				std::cout << "Could not find EMAGIN Z800 tracker" << std::endl;
				useEmaginTracker = false;
				EmaginZ800detected =1;
			} 
			else
			{
				useEmaginTracker = true;
				EmaginZ800detected =2;
			}
		}
		
		//
		if ( NULL != emaTracker ) 
		{ 
			Z800->GoSleep();
			//delete emaTracker; 
		} 
		// destroy device 
		if ( NULL != Z800 ) 
		{ 
			delete Z800 ; 
		} 
		



		//окончательный вердикт
		if ((useEmagin==true)&&(useEmaginTracker==true))
		{
			EmaginZ800detected =2;
			return true;
		}
		else
		{
			EmaginZ800detected =1;
			return false;
		}
	#endif
}



/*
HRESULT CALLBACK EmaginZ800Class::GestureProc(EMAGestureCommand* command, void* param)
{
	if ( NULL == command )
    {
        return EMA_INVALID_PARAMETER;
    }
	//
	//	EMA_JUMP,
	//	EMA_CROUCH,
	//	EMA_DODGE_LEFT,
	//	EMA_DODGE_RIGHT,
	//	EMA_MOVE_FORWARD,
	//	EMA_MOVE_BACKWARD,
	//	EMA_NO_GESTURE,
	//	EMA_ALL_GESTURES
	//
	if (command->Gesture == EMA_MOVE_FORWARD) 
	{
		std::cout << "^";
	}
	if (command->Gesture == EMA_MOVE_BACKWARD) 
	{
		std::cout << "v";
	}
	if (command->Gesture == EMA_DODGE_LEFT) 
	{
		std::cout << "<";
	}
	if (command->Gesture == EMA_DODGE_LEFT) 
	{
		std::cout << ">";
	}


	if (command->Degree == EMA_SMALL) 
	{
		//std::cout << "small";
	}
	if (command->Degree == EMA_MEDIUM) 
	{
		//std::cout << "medium";
	}
	if (command->Degree == EMA_LARGE) 
	{
		//std::cout << "large";
	}
	//command->Degree    
	
	//	EMA_SMALL,
	//	EMA_MEDIUM,
	//	EMA_LARGE
	
	return S_OK;
}
/*

/*
void EmaginZ800Class::InitializeGestureAnalyzer() 
{
	emaGestureAnalyzer = new EMAGestureAnalyzer(Z800);
	if ( NULL == emaGestureAnalyzer ) 
	{ 
		HRESULT hr = EMAGestureAnalyzer::GetLastError(); 
        std::cout << "Could not find EMAGIN Z800 Gesture Analyzer" << std::endl;
		useGestureAnalyzer = false;
		return; 
	} 
	emaGestureAnalyzer->SetTrackerCallback(TrackProc, NULL);
	emaGestureAnalyzer->EnableTracker(true);
	emaGestureAnalyzer->EnableGestureAnalyzer(true);

	emaGestureAnalyzer->SetGestureCallback(GestureProc, NULL);

	EMAGestureThresholds z;
	z.threshold = 230.0f;//230.0f;
	z.timeInterval = 1000;//300;
	z.settleTime =  150;//150;
	emaGestureAnalyzer->SetGestureThresholds(EMAGestureDirection(EMA_ALL_DIRECTIONS) , &z);

	useGestureAnalyzer = true;
	std::cout << "EMAGIN Z800 Gesture Analyzer detect !" << std::endl;


}
*/



