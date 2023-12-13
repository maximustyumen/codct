/***************************************************************************
BaseSound.cpp  -  Определение звукового класса
-------------------
begin                : 8 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
#include "OutputAudio.h"


OutputAudioClass::OutputAudioClass(Configure* configuration)
{
	//если звук выключен
	std::string notfound_ = "not found";
	SOUND_OFF = false;
	if (configuration->GetRecordString("#MasterVolume") != notfound_)
	{
			float MasterVolume = configuration->GetRecordFloat("#MasterVolume");
			if (MasterVolume == 0) SOUND_OFF = true;
	}
	if (SOUND_OFF == true) return;


	try
	{
	x=0;
	y=0;
	z=0;
	lookupdown=0;
	//
	// Позиция слушателя.
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
	// Скорость слушателя.
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	// Ориентация слушателя. (Первые 3 элемента – направление «на», последние 3 – «вверх»)
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
	// Открываем заданное по умолчанию устройство
	//#ifdef WIN32
	//	pDevice = alcOpenDevice("Generic Software"); //NULL
	//#else
		pDevice = alcOpenDevice(NULL);
	//#endif

	if (!pDevice)
	{
		std::cout << "Default sound device not present.. try 'Generic Software'" << std::endl;
		pDevice = alcOpenDevice("Generic Software");
	}
	if (!pDevice)
	{
		std::cout << "Default sound device not present.. try 'DirectSound Software'" << std::endl;
		pDevice = alcOpenDevice("DirectSound Software");
	}
	
	//if (!pDevice)
	//{
	//	std::cout << "'Generic Software' not present... sound switch off." << std::endl;
	//}

	// Проверка на ошибки
	if (!pDevice)
	{
			throw (ErrorClass (__FILE__, __LINE__ , "OutputAudioClass::OutputAudioClass()", "Default sound device not present. OpenAL alcOpenDevice error." , 4));
	}

	// Создаем контекст рендеринга
	pContext = alcCreateContext(pDevice, NULL);

		if (!CheckALCError())
		{
			throw (ErrorClass (__FILE__, __LINE__ , "OutputAudioClass::OutputAudioClass()", "OpenAL alcCreateContext error." , 4));
		}

	// Делаем контекст текущим
	alcMakeContextCurrent(pContext);

		//информация о типе и версии
		std::cout << "OutputAudioClass constructor.. OpenAL version " << alGetString(AL_VERSION) << std::endl ;
		std::cout << alcGetString(pDevice, ALC_DEVICE_SPECIFIER ) <<  std::endl << alGetString(AL_VENDOR) << std::endl << alGetString(AL_RENDERER)<< std::endl << alGetString(AL_EXTENSIONS)<< std::endl;

		//ALUT
		alutInitWithoutContext(0,0);
		std::cout << "alut version= " << alutGetMajorVersion () <<  "." << alutGetMinorVersion() << std::endl;

	// Устанавливаем параметры слушателя
	// Позиция
	alListenerfv(AL_POSITION,    ListenerPos);
	// Скорость
	alListenerfv(AL_VELOCITY,    ListenerVel);
	// Ориентация
	alListenerfv(AL_ORIENTATION, ListenerOri);
		//Модель затухания
		//defaults AL_INVERSE_DISTANCE_CLAMPED
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
		//Расстояние, на котором сила звука уменьшается вдвое
		//alListenerf(AL_REFERENCE_DISTANCE, 100.0f);

		std::string notfound = "not found";
		if (configuration->GetRecordString("#MasterVolume") != notfound)
	{
			float MasterVolume = configuration->GetRecordFloat("#MasterVolume");
			alListenerf( AL_GAIN, MasterVolume/100.0f );
	}

	// RECORD
	//---------------------------------------------
	char * captureDevices = (char *) alcGetString ( NULL, ALC_CAPTURE_DEVICE_SPECIFIER );
	if ( captureDevices == NULL )
	{
		std::cout << "No capture devices found" << std::endl;
	}
	else
	{
		std::cout <<  "Capture devices: " << captureDevices  << std::endl;
	}

		return;
	}
	catch (ErrorClass error)
	{
		error.print();
	}
}

OutputAudioClass::~OutputAudioClass()
{
	if (SOUND_OFF == true) return;
    std::cout << "Destructor of Sound System" << std::endl;

    //Правильный деструктор
    //Get active context
    ALCcontext*  Context=alcGetCurrentContext();
    //Get device for active context
    ALCdevice* Device=alcGetContextsDevice(Context);
    //Disable context
    alcMakeContextCurrent(NULL);
    //Release context(s)
    alcDestroyContext(Context);
    //Close device
    alcCloseDevice(Device);

    //alutExit();
}

ALboolean OutputAudioClass::CheckALCError()
{

  ALenum ErrCode;
  std::string Err = "ALC error: ";
  if ((ErrCode = alcGetError(pDevice)) != ALC_NO_ERROR)
  {
	Err += (char *)alcGetString(pDevice, ErrCode);
	printf(Err.data());
	//!!!!!!!ExitProgramm(Err.data());
	return AL_FALSE;
  }
  return AL_TRUE;
}

ALboolean OutputAudioClass::CheckALError()
{
  ALenum ErrCode;

  if ((ErrCode = alGetError()) != AL_NO_ERROR)
  {
	std::string Err = "OpenAL error: ";
	Err += (char *)alGetString(ErrCode);
	std::cout << Err.data() << std::endl;
	//!!!!!!!ExitProgramm(Err.data());
	return AL_FALSE;
  }
  return AL_TRUE;
}



void OutputAudioClass::Update(float posx,float posy, float posz, float Yaw, float Pitch, float Roll)
{
	if (SOUND_OFF == true) return;

	static float oldX=posx;
	static float oldY=posy;
	static float oldZ=posz;
	static float oldYaw=Yaw;
	static float oldPitch=Pitch;
	static float oldRoll=Roll;

        /*
	static osg::Timer_t tick1 = osg::Timer::instance()->tick();
	double D_T = osg::Timer::instance()->delta_s(osg::Timer::instance()->tick(), tick1 );
	if (D_T >= 10000) //20
	{
		std::cout << "*";
		tick1 = osg::Timer::instance()->tick();
	}
	else
	{
		return;
	}
        */

        #ifdef WIN32
        if ((oldX!=posx)||(oldY!=posy)||(oldZ!=posz))
        {
                // Позиция слушателя.
                ALfloat ListenerPos[] = { posx, posy, posz };
                // Позиция
                alListenerfv(AL_POSITION,    ListenerPos);
                CheckALError();
        }
        #else //this FIXed BUG in linux
        {
                // Позиция слушателя.
                ALfloat ListenerPos[] = { posx + (rand() % 2 + 1), posy+ (rand() % 2 + 1), posz+ (rand() % 2 + 1) };
                // Позиция
                alListenerfv(AL_POSITION,    ListenerPos);
                CheckALError();
        }
        #endif


        if ((oldYaw!=Yaw)||(oldPitch!=Pitch)||(oldRoll!=Roll))
        {
                // Ориентация
                ALfloat ListenerOri[] = { sin(Yaw * 3.14f / 180.0f), cos(Yaw * 3.14f / 180.0f), 1.0,  0.0, 0.0, 1.0 };
                alListenerfv(AL_ORIENTATION, ListenerOri);
                CheckALError();
        }

	// Скорость слушателя.
	//ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	// Скорость
		//alListenerfv(AL_VELOCITY,    ListenerVel);
	// Ориентация
	//alListenerfv(AL_ORIENTATION, ListenerOri);
	//	CheckALError();

        //
        oldX=posx;
        oldY=posy;
        oldZ=posz;
        oldYaw=Yaw;
        oldPitch=Pitch;
        oldRoll=Roll;
}

 //---------------------------------------------------

				/*
				List input devices:
				alcGetString(Null, ALC_CAPTURE_DEVICE_SPECIFIER)
				Default input device:
				alcGetString(Null, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER)
				Open device:
				alcCaptureOpenDevice(...)
				Start recording:
				alcCaptureStart(...)
				Stop recording.
				alcCaptureStop()
				Get number of captured samples
				alcGetIntegerv(Recorder, ALC_CAPTURE_SAMPLES, 4, Varptr(NumSamples))
				Get samples:
				alcCaptureSamples(...)
				*/
			//PROGRESS+=5;
			//LOG_write("PROGRES:"+intToString(PROGRESS));

			/*
			//std::vector<ALuint> SourcePull;
			for (int i=0; i <50000;i++)
			{
				alGetError(); // clear error code
				ALuint      mSourceID;
						alGenSources(1, &mSourceID);
						//SourcePull.push_back(0);
						//alGenSources(1, &SourcePull.back());
				ALenum ErrCode;
				if ((ErrCode = alGetError()) != AL_NO_ERROR)
				{
						std::string Err = "OpenAL error: ";
						Err += (char *)alGetString(ErrCode);
						std::cout << Err << "i="<<  i << std::endl;
										exit (0);
						break;
				}
			}
			*/
