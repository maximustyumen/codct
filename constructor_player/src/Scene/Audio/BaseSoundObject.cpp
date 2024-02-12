/***************************************************************************
BaseSoundObject.h  -  Определение класса звукового источника
-------------------
begin                : 8 марта 2023
begin                : 14 октября 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "BaseSoundObject.h"
// Disable warning messages
//#pragma warning( disable : 4996 )

//список
//std::map <std::string, osg::ref_ptr<BaseSoundObject>> BaseSoundObject::VirtualVoices;

//список имен всех созданных и неудаленных голосов
std::map <std::string, int> BaseSoundObject::VirtualVoices;
//список освободифшихся при удалении id источников...
std::stack <ALuint> BaseSoundObject::FreeOnDeleteSources;

size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource);
int SeekOgg(void *datasource, ogg_int64_t offset, int whence);
long TellOgg(void *datasource);
int CloseOgg(void *datasource);

ALboolean BaseSoundObject::CheckALError()
{
	ALenum ErrCode;
	std::string Err = "OpenAL error: ";
	if ((ErrCode = alGetError()) != AL_NO_ERROR)
	{
		Err += (char *)alGetString(ErrCode);
		std::cout << Err << std::endl;
		return AL_FALSE;
	}
	return AL_TRUE;
}

BaseSoundObject::BaseSoundObject(std::string name)
{
	mVF=0;

	isDebugCreated=false;

	//задаем имя для OSG
	this->setName(name);

	//ShowSoundDebug();

	//вносим в список имен
	VirtualVoices[name]=0;
	//пока источник не назначен...
	isReal = false;

	//инициализируем свойства значениями поумолчанию
	//флаг режима зациклливания
	mLooped = false;
	//громкость
	GainValue=1.0f;
    //частота
    PitchValue = 1.0f;
	//Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
	MaxDistance=10000000.0f; //MAX_FLOAT;
	//Расстояние, на котором сила звука уменьшается вдвое
	ReferenceDistance=1.0f;
	//Минимальное/максимальное значение громкости
	MinGain=0.0f;
	MaxGain=1.0f;
	//Коэфффициент кривизны затухания звука (rolloff фактор)
	RolloffFactor=1.0f;
	//позиция звука в секундах
	OFFSET=0.0f;
	//желаемое->фактическое состояние
	STATE="PAUSE";
		//Приоритет - 50%
	Priority = 0.5f;
	//Priority = 	(float) (rand() % 100) / (float) 100.0f;




}

//деструктор
BaseSoundObject::~BaseSoundObject() 
{
    try
    {
    std::cout << "Destructor BaseSoundObject. name: " << this->getName() ;
    //ибираем из списка имен
    VirtualVoices.erase(this->getName());
    //Заносим id источника в стек "свободных", чтобы менеджер об этом знал!
    //Только если это активный источник, т.е. действующий!
    if (isReal)
    {
        alSourceStop(mSourceID);
        alSourcei(mSourceID, AL_BUFFER, NULL); //+
        FreeOnDeleteSources.push(mSourceID);
        if (!CheckALError())
        {
                throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::~BaseSoundObject()", "CheckALError ", 4));
        }
    }
    //очистка буферов
    if (alIsBuffer (ID))
    {
        //std::cout << "X" << std::endl;
        alDeleteBuffers(1, &ID);
        if (!CheckALError())
        {
                throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::~BaseSoundObject()", "CheckALError", 4));
        }
    }
    /*
    if (!mVF)
    {
            ov_clear(mVF);
            delete mVF;
    }
    */
    std::cout << " .. ok." << std::endl;
    }
    catch (ErrorClass error)
    {
            error.print();
    }
}



//0. Отбирание источника звука, превращение звука в виртуальный...
void BaseSoundObject::TakeSource() throw (ErrorClass)
{
	//std::cout << "BaseSoundObject::TakeSource()" << std::endl;
        //try
        //{
	callback->TakeSource();
	if (isReal)
	{
		alSourceStop(mSourceID);
		alSourcei(mSourceID, AL_BUFFER, NULL); //+
		FreeOnDeleteSources.push(mSourceID);
				if (!CheckALError())
				{
					 throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::TakeSource()", "CheckALError", 4));
				}
	}
	isReal = false;
        //}
        //catch (ErrorClass error)
        //{
        //	error.print();
        //}

}

//0. Возвращение источника звука, превращение звука в реальный...
void BaseSoundObject::RestoreSource(ALuint newSourceID) throw (ErrorClass)
{
	//std::cout << "BaseSoundObject::RestoreSource()" << std::endl;
	mSourceID = newSourceID;
	isReal = true;

    //try
    //{
	//СБРАСЫВАЕМ ПРЕДЫДУЩУЮ ОШИБКУ, если она есть (без выхода)
    if (!CheckALError())
    {
            //throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::RestoreSource", "CheckALError", 4));
    }
	
	//is a legal way to release the current buffer
	alSourcei(mSourceID, AL_BUFFER, AL_NONE);
	CheckALError();

    //2. Ассоциируем буфер с источником
    alSourcei (mSourceID, AL_BUFFER, ID);
    if (!CheckALError())
    {
            throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::RestoreSource", "CheckALError", 4));
    }
    //}
    //catch (ErrorClass error)
    //{
    //         error.print();
    //}


	//1. Восстанавливаем запомненные параметры
	//
	setLoop(mLooped); //mLooped

	SetGain(GainValue);
    SetPitch(PitchValue);
	SetMaxDistance (MaxDistance);
	SetReferenceDistance(ReferenceDistance);
	SetMinMaxGain(MinGain, MaxGain);
	SetRolloffFactor(RolloffFactor);



	if (Head_Relative)
	{
		alSourcei (mSourceID, AL_SOURCE_RELATIVE,    AL_TRUE);
		ALfloat Pos[3] = { 0, 0, 0 };
		alSourcefv(mSourceID, AL_POSITION, Pos);
	}
	else
	{
		alSourcei (mSourceID, AL_SOURCE_RELATIVE,    AL_FALSE);
	}



	//3. Включаем callback
	callback->RestoreSource(mSourceID, Head_Relative);
	//4. "перемотать звук на нужную секунду" (Возможно конец файла)
	SetOFFSET(OFFSET); //OFFSET



	//5. Выйти на необходимое состояние
	if (STATE=="PAUSE") Pause();
	if (STATE=="PLAY") Play();


}

//0. Расчет "виртуальной" игры звука (), изменение состояния виртуального звука и т.д.
void BaseSoundObject::Update()
{

	if (Head_Relative==true)
	{
		//if (HeadRelative) то xyz совпадают с пользователем, иначе взять из callback
		X=0.0f;
		Y=0.0f;
		Z=0.0f;
	}
	else
	{
		X= callback->x;
		Y= callback->y;
		Z= callback->z;
	}


	//Рассчитываем dT
	double deltaT=0;
	{
		static osg::Timer_t old_tick = osg::Timer::instance()->tick();
		osg::Timer_t current_tick = osg::Timer::instance()->tick();
		// за последнюю итерацию прошло
		deltaT = osg::Timer::instance()->delta_s(old_tick,current_tick);
		old_tick=current_tick;
	}



	//3. Считать, даже если звук не играет, то все равно "мотать пленку", чтобы стартовать не с того места где остановили, а где должен был играть.. либо фиксировать конец файла....
	//Рассчет OFFSET, если перешел за конец файла и не loop, то изменяем состояние
	//если loop, то крутим от начала...
	if (GetState()=="PLAYING")
		OFFSET+=deltaT;


	if ((isReal == false) && (mLooped==false) && (OFFSET>=SoundLenght)) STATE="STOP";
	if ((isReal == false) && (mLooped==true) && (OFFSET>=SoundLenght)) OFFSET=0;

	if ((isReal == true) && (mLooped==true) && (GetState()=="PAUSED")) 
	{
		//STATE="STOP";
		try
		{
			TakeSource();
		}
		catch (ErrorClass error)
		{
			error.print();
		}
	}


	//переходим в разряд виртуальных....
	if ((isReal == true) && (GetState() == "STOPPED") ) //&& (STATE != "PLAY")
	{
		STATE="STOP";
                try
                {
                    TakeSource();
                }
                catch (ErrorClass error)
                {
                    error.print();
                }
	}

	//
	if (isDebugCreated==true)
	{
		if (isReal==true)   ShapeDrawable->setColor(osg::Vec4(0,1,0,1));
		if (isReal==false)  ShapeDrawable->setColor(osg::Vec4(1,0,0,1));
	}

}

//открытие файла, создание источника звука
bool BaseSoundObject::Open(const std::string Filename, bool Looped, bool Streamed) throw (ErrorClass)
{
	//запоминаем имя файла
	filename = Filename;

	//проверяем расширение
	std::string ext = filename.substr(filename.length()-4,4);
	std::transform(ext.begin(), ext.end(), ext.begin(), (int (*)(int))std::toupper);
	//если это vorbis или wav
	bool ok;
	if (ext==".OGG")
	{
				//std::cout << "its OGG!" << std::endl;
		mStreamed = Streamed;
		ok = LoadOggFile(Filename, Streamed);
	}
	else
	{
		ok = LoadWavFile(Filename);
	}


	//сейчас можно использовать Callback...
	callback =  new MyTransformCallback();
	setUpdateCallback(callback );

	return ok;
}

//2. воспроизведение
void BaseSoundObject::Play() throw (ErrorClass)
{
	//запоминаем желаемое состояние
	STATE = "PLAY";
	//если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcePlay(mSourceID);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::Play()", "CheckALError", 4));
		}
	}

	if ((isReal == false) && (mLooped==false) && (OFFSET>=SoundLenght))
	{
		OFFSET=0;
	}
}

//3. установка режима повтора
void BaseSoundObject::setLoop(bool Looped) throw (ErrorClass)
{
	//запоминаем режим
	mLooped = Looped;
	//и если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcei (mSourceID, AL_LOOPING, mLooped);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::setLoop()", "CheckALError", 4));
		}
	}
}

//4. поставить на паузу
void BaseSoundObject::Pause() throw (ErrorClass)
{
	//запоминаем желаемое состояние
	STATE = "PAUSE";
	//если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcePause(mSourceID);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::Pause()", "CheckALError", 4));
		}
	}
}

//5. получить состояние объекта (играет, на паузе, воспроизведение закончено и т.д)
std::string BaseSoundObject::GetState() throw (ErrorClass)
{
	//и если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		ALint       iState;
		alGetSourcei(mSourceID , AL_SOURCE_STATE, &iState);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::GetState()", "CheckALError", 4));
		}
		switch( iState )
		{
			case AL_INITIAL:
				return "INITIAL"; break;
			case AL_PLAYING:
				return "PLAYING"; break;
			case AL_PAUSED:
				return "PAUSED"; break;
			case AL_STOPPED:
				return "STOPPED"; break;
		}
	}
	//если источник виртуальный, мы должны выдать предпологаемое или желаемое состояние
	else
	{
		if ( STATE == "PAUSE") return "PAUSED";
		if ( STATE == "PLAY") return "PLAYING";
		if ( STATE == "STOP") return "STOPPED";
	}
		throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::GetState()", "unknown state", 4));
}

//6.установить громкость
void BaseSoundObject::SetGain(float value) throw (ErrorClass)
{
	//запоминаем громкость
	GainValue = value;
	//и если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcef (mSourceID, AL_GAIN, GainValue);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::SetGain()", "CheckALError", 4));
		}
	}
}
//7. Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
void BaseSoundObject::SetMaxDistance(float value) throw (ErrorClass)
{
	//запоминаем расстояние
	MaxDistance = value;
	//и если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcef (mSourceID, AL_MAX_DISTANCE, value);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::SetMaxDistance()", "CheckALError", 4));
		}
	}
}
//8. Расстояние, на котором сила звука уменьшается вдвое (до воздействия параметра rolloff)
void BaseSoundObject::SetReferenceDistance(float value) throw (ErrorClass)
{
	//запоминаем расстояние
	ReferenceDistance = value;
	//и если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcef (mSourceID, AL_REFERENCE_DISTANCE, value);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::SetReferenceDistance()", "CheckALError", 4));
		}
	}
}
//9. Минимальное/максимальное значение громкости
void BaseSoundObject::SetMinMaxGain(float min, float max) throw (ErrorClass)
{
	//запоминаем
	MinGain = min;
	MaxGain = max;
	//и если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcef (mSourceID, AL_MIN_GAIN, min);
		alSourcef (mSourceID, AL_MAX_GAIN, max);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::SetMinMaxGain()", "CheckALError", 4));
		}
	}
}
//10. "перемотать звук на нужную секунду"
void BaseSoundObject::SetOFFSET(float offset) throw (ErrorClass)
{
	//запоминаем
	OFFSET = offset;
	//если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcef(mSourceID, AL_SEC_OFFSET, offset);
		if (!CheckALError())
		{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::SetOFFSET()", "CheckALError", 4));
		}
	}
}
//11. Коэфффициент кривизны затухания звука (rolloff фактор)
void BaseSoundObject::SetRolloffFactor(float value) throw (ErrorClass)
{
	//запоминаем
	RolloffFactor = value;
	//и если мы владеем реальный источник, то ...
	if (isReal==true)
	{
		alSourcef (mSourceID, AL_ROLLOFF_FACTOR, value);
				if (!CheckALError())
				{
					throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::SetRolloffFactor()", "CheckALError", 4));
				}
	}
}

//12. установить приоритет
void BaseSoundObject::SetPriority (float value)
{
	Priority = value;
}
float BaseSoundObject::GetPriority ()
{
	return Priority;
}


//-----------------------------------------------------------------------------------
BaseSoundObject::MyTransformCallback::MyTransformCallback()
{
	isReal = false;
}
//
void BaseSoundObject::MyTransformCallback::RestoreSource (ALuint newSourceID, bool _Head_Relative)
{
	mSourceID = newSourceID;
	isReal = true;

	Head_Relative = _Head_Relative;
	//
	oldX =-1;
	oldY =-1;
	oldZ =-1;

}
//
void BaseSoundObject::MyTransformCallback::TakeSource ()
{
	isReal = false;
}
//
void BaseSoundObject::MyTransformCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	//static osg::Timer_t tick1 = osg::Timer::instance()->tick();
	//double D_T = osg::Timer::instance()->delta_s(osg::Timer::instance()->tick(), tick1);
	//if (D_T >= 1000 + rand()*3000) //20
	{
		//tick1 = osg::Timer::instance()->tick();
		//std::cout << tick1 << std::endl;
		if ( node )
		{
			osg::NodePath& fullNodePath = nv->getNodePath();
			osg::Matrix localCoordMat = osg::computeLocalToWorld( fullNodePath );
			//позиция
			//std::cout << "x= " << localCoordMat.getTrans().x() << "y= " << localCoordMat.getTrans().y() << "z= " << localCoordMat.getTrans().z() << std::endl;
			x =localCoordMat.getTrans().x();
			y =localCoordMat.getTrans().y();
			z =localCoordMat.getTrans().z();
			//Если это реальный голос, и не "head relative", то
			//Оптимизация
			if ((oldX!=x) || (oldY!=y) || (oldZ!=z))
			{
				if ((isReal == true) && (Head_Relative == false))
				{
					oldX = x;
					oldY = y;
					oldZ = z;
					ALfloat Pos[3] = { x, y, z };
					//std::cout << "is real=" << isReal << "   head=" << Head_Relative << std::endl;
                                        alSourcefv(mSourceID, AL_POSITION, Pos);
					// std::cout << x << ";" << y << ";" << z << std::endl;
				}
			}
			//можно выставить скорость)
		}
	}
	traverse(node, nv);
}
//--------------------------------------------------------------------------------

bool BaseSoundObject::LoadWavFile(const std::string filename) throw (ErrorClass)
{
		// Ищем, а нет ли уже существующего буфера с данным звуком?

        // Создаём  (ID = alutCreateBufferFromFile)
        //alGenBuffers(1, &ID);
        //if (!CheckALError()) return false;

		// Загружаем данные из wav файла
		alutGetError();
                //ID = alutCreateBufferFromFile((ALbyte *)filename.data());
                ID = alutCreateBufferFromFile((char *)filename.data());
		if (ID ==  AL_NONE)
		{
			ALenum bar = alutGetError();
			std::string error = alutGetErrorString(bar);
			//генерируем ошибку
			throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::LoadWavFile", "alutCreateBufferFromFile error: " + error + " (" + filename + ")", 4));
		}

	//Длинна звука в ФАЙЛе
	SoundLenght=15;

		if (!CheckALError())
		{
			throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::LoadWavFile()", "CheckALError", 4));
		}

	return true;
}

//--------------------------------------------------------------------------------

bool BaseSoundObject::LoadOggFile(const std::string &Filename, bool Streamed) throw (ErrorClass)
{
	// Файловый поток содержащий наш ogg файл
	std::ifstream    OggFile;

	//!!!!!!!!!
	Streamed = false;
	//
		int BlockSize;
	// Структура с функциями обратного вызова.
	ov_callbacks  cb;

	// Заполняем структуру cb
	cb.close_func = CloseOgg;
	cb.read_func  = ReadOgg;
	cb.seek_func  = SeekOgg;
	cb.tell_func  = TellOgg;

	// Создаем структуру OggVorbis_File
	mVF = new OggVorbis_File;

	// Открываем OGG файл как бинарный
	OggFile.open(Filename.c_str(), std::ios_base::in | std::ios_base::binary);

	// Инициализируем файл средствами vorbisfile
	//
	//int ok_open = ov_open_callbacks(&OggFile, mVF, NULL, -1, cb)
	int ok_open = ov_open_callbacks(&OggFile, mVF, NULL, 0, cb);
	if (ok_open < 0)
	{
		switch(ok_open)
		{
			case OV_EREAD:
				std::cout << "OV_EREAD = A read from media returned an error " << std::endl;
				break;
			case OV_ENOTVORBIS:
				std::cout << "OV_ENOTVORBIS = Bitstream is not Vorbis data " << std::endl;
				break;
			case OV_EVERSION:
				std::cout << "OV_EVERSION = Vorbis version mismatch " << std::endl;
				break;
			case OV_EBADHEADER:
				std::cout << "OV_EBADHEADER = Invalid Vorbis bitstream header " << std::endl;
				break;
			case OV_EFAULT:
				std::cout << "OV_EFAULT = Internal logic fault; indicates a bug or heap/stack corruption " << std::endl;
				break;
			default:
				std::cout << " ??? " << std::endl;
				break;
		}
	

		
		// Если ошибка, то открываемый файл не является OGG
		throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::LoadOggFile", std::string("its not OGG file error: ") +   Filename, 4));
	}

	// Начальные установки в зависимости от того потоковое ли проигрывание
	// затребовано
	if (!Streamed)
	{
	// Размер блока – весь файл
	BlockSize = ov_pcm_total(mVF, -1) * 4;
	}


	// Получаем комментарии и информацию о файле
	mComment    = ov_comment(mVF, -1);
	mInfo      = ov_info(mVF, -1);

	//Длинна звука в ФАЙЛе
	SoundLenght=ov_time_total(mVF, -1);
	//std::cout << "SoundLenght (OGG) from " << Filename << " = " << SoundLenght << " sec." <<std::endl;

	// Заполняем SndInfo структуру данными
	Rate    = mInfo->rate;
	//buffer.Filename  = Filename;
	Format = (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	// Если потоковое проигрывание, или буфер со звуком не найден то

	// Создаем буфер
	alGenBuffers(1, &ID);
		if (!CheckALError())
		{
			throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::LoadOggFile()", "CheckALError", 4));
		}
	// Считываем блок данных
		ReadOggBlock( BlockSize);

		if (!CheckALError())
		{
			throw (ErrorClass (__FILE__, __LINE__ , "BaseSoundObject::LoadOggFile()", "CheckALError", 4));
		}

	/*
	if (Streamed) // Помещаем буфер в очередь.
	  {
		alSourceQueueBuffers(mSourceID, 1, &buffer.ID);
		if (!CheckALError()) return false;
	  }
	  else
	*/

	ov_clear(mVF);
	delete mVF;
	// Ассоциируем буфер с источником
	//alSourcei(mSourceID, AL_BUFFER, ID);

	OggFile.close();


	return true;
}

//-----------------------------------------------------------
//    ov_callback specific functions
//-----------------------------------------------------------

size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	std::istream *File = reinterpret_cast<std::istream*>(datasource);
	File->read((char *)ptr, size * nmemb);
	return File->gcount();
}

int SeekOgg(void *datasource, ogg_int64_t offset, int whence)
{
	std::istream *File = reinterpret_cast<std::istream*>(datasource);
	std::ios_base::seekdir Dir;
	File->clear();
	switch (whence)
	{
		case SEEK_SET: Dir = std::ios::beg;  break;
		case SEEK_CUR: Dir = std::ios::cur;  break;
		case SEEK_END: Dir = std::ios::end;  break;
		default: return -1;
	}
	File->seekg((std::streamoff)offset, Dir);
	return (File->fail() ? -1 : 0);
}

long TellOgg(void *datasource)
{
	std::istream *File = reinterpret_cast<std::istream*>(datasource);
	return File->tellg();
}

int CloseOgg(void *datasource)
{

	return 0;
}

bool BaseSoundObject::ReadOggBlock(size_t Size)
{
	// vars
		int current_section;
		long TotalRet = 0, ret=0;
		char *PCM;

	if (Size < 1) return false;
	PCM = new char[Size];

	// Read loop
	while (TotalRet < Size)
	{
		ret = ov_read(mVF, PCM + TotalRet, Size - TotalRet, 0, 2, 1, &current_section);

		// if end of file or read limit exceeded
		if (ret == 0) break;
		else if (ret < 0) 		// Error in bitstream
		{
			//
		}
		else
		{
			TotalRet += ret;
		}
	}
	if (TotalRet > 0)
	{
		alBufferData(ID, Format, (void *)PCM,
					 TotalRet, Rate);
		CheckALError();
	}
	delete [] PCM;
	return (ret > 0);
}


//---------------------------------------------------------------------------------------

//Эксперементы
//установить Pitch (0..10x) - ускорение/замедление звука
void BaseSoundObject::SetPitch(float value)
{
    //запоминаем громкость
    PitchValue = value;
    //и если мы владеем реальный источник, то ...
    if (isReal==true)
    {
        alSourcef (mSourceID, AL_PITCH,  value);
        if (!CheckALError())
        {
            std::cout << "alSourcef(AL_PITCH) error" << std::endl;
        }
    }
}

//add reverb
void BaseSoundObject::AddReverb()
{
	/*
	#ifdef WIN32
	if (ALFWIsEFXSupported()==false)
	{
		std::cout << "EAX not supported" << std::endl;
		return;
	}
	//проверка на наличие такого имени
	if (objects.count(name)==0)
	{
		std::cout << "Warning: Sound '" << name << "' not exsist!" << std::endl;
		return;
	}

	alSourceStop(objects[name]->mSourceID);
	ALint       state;
	while( true  )
	{
		// Get state
		alGetSourcei( objects[name]->mSourceID, AL_SOURCE_STATE, &state);
		if( state != AL_PLAYING )
			break;
	}

	std::cout << "Source played through an auxiliary reverb without filtering" << std::endl;
	alGenAuxiliaryEffectSlots( 1, &objects[name]->uiEffectSlot );
	// Create an effect to be loaded in the auxiliary effect slot
	alGenEffects( 1, &objects[name]->uiEffect );
	// Configure effect to be Reverb
	if( alIsEffect( objects[name]->uiEffect ) )
	{
		alEffecti( objects[name]->uiEffect, AL_EFFECT_TYPE, AL_EFFECT_REVERB );

		// Load effect into slot
		if( alIsAuxiliaryEffectSlot( objects[name]->uiEffectSlot ) )
		{
			alAuxiliaryEffectSloti( objects[name]->uiEffectSlot, AL_EFFECTSLOT_EFFECT, objects[name]->uiEffect );
		}
	}
	{
	}

	// Enable Send 0 from the Source to the Auxiliary Effect Slot without filtering
	alSource3i(objects[name]->mSourceID, AL_AUXILIARY_SEND_FILTER, objects[name]->uiEffectSlot, 0, AL_FILTER_NULL);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cout << "Error add REVERB" << std::endl;
		return;
	}
	//Play(objects[name]->mSourceID / uiSource);
	#endif
	*/
}


void BaseSoundObject::DeleteReverb()
{
	/*
	#ifdef WIN32
	//проверка на наличие такого имени
	if (objects.count(name)==0)
	{
		std::cout << "Warning: Sound '" << name << "' not exsist!" << std::endl;
		return;
	}
	// Cleanup
	alSource3i(objects[name]->mSourceID, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
	alDeleteAuxiliaryEffectSlots( 1, &objects[name]->uiEffectSlot );
	alDeleteEffects( 1, &objects[name]->uiEffect );
	#endif
	*/
}







//6. Создает геометрический объект, для того чтобы видеть источник звука
void BaseSoundObject::ShowSoundDebug()
{
		isDebugCreated = true;
		//osg::ref_ptr<osg::Node> Cursor = osgDB::readNodeFile("./Cursor.osg");

		//обертка курсора
		osg::ref_ptr<osg::MatrixTransform> SoundMatrix = new osg::MatrixTransform;
		//геометрия курсора
		osg::ref_ptr<osg::Geode>  SoundgGeode = new osg::Geode;
		//
		//osg::ref_ptr<osg::ShapeDrawable>  ShapeDrawable;

		SoundMatrix->setName (getName() + "_DEBUG_AIR");


		//сфера
		{
			float radius = 4.0f + (float) (rand() % 100) / (float) 50.0f;
			//float height = 1.0f;
			osg::TessellationHints* hints = new osg::TessellationHints;
			hints->setDetailRatio(0.25f);
			ShapeDrawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),radius),hints);

			SoundgGeode->addDrawable(ShapeDrawable);

			//ShapeDrawable->setColor(osg::Vec4(1,1,1,1));

		}

/*
		//точность
		const int aaaa=40;
		//считать до отдоления...
		float dL = 200;


		//Школа громкости - 100% и "0"
		{
			osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
			osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(6*2);
			(*vertices)[0].set(0, 0,0);
			(*vertices)[1].set(0, 0, 50 );
			//
			(*vertices)[2].set(0, 0,0);
			(*vertices)[3].set(dL, 0, 0 );
			//
			linesGeom->setVertexArray(vertices);
			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
			linesGeom->setColorArray(colors);
			linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
			linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,4));
			//
			SoundgGeode->addDrawable(linesGeom);
		}

		{

			osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
			osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(3*aaaa);


			//The default attenuation model is AL_INVERSE_DISTANCE_CLAMPED.
			//Inverse Distance Rolloff Model
			//gain = AL_REFERENCE_DISTANCE / (AL_REFERENCE_DISTANCE +AL_ROLLOFF_FACTOR *(distance – AL_REFERENCE_DISTANCE));

			float _AL_ROLLOFF_FACTOR = 0.1f; //1.0f
			float _AL_REFERENCE_DISTANCE = 1.0f;
			float distance=0;


			for (int i=0;i < aaaa; i++)
			{
				float gain = _AL_REFERENCE_DISTANCE / (_AL_REFERENCE_DISTANCE +_AL_ROLLOFF_FACTOR * (distance - _AL_REFERENCE_DISTANCE));
				if (gain<0) gain = 1;
				if (gain>2) gain = 2;
				(*vertices)[i].set(distance, 0, gain*50);
				distance = distance + dL/aaaa;
			}

			//
			linesGeom->setVertexArray(vertices);
			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			colors->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
			linesGeom->setColorArray(colors);
			linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
			linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,aaaa));
			//
			SoundgGeode->addDrawable(linesGeom);
		}
*/

		//Wireframe + lighting off

		{
			osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;

			osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
			polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
			stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

			osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
			linewidth->setWidth(2.0f);
			stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
			stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			//применяем к жертве)
			setStateSet(stateset);
		}



		if (true)
		{
			SoundMatrix->addChild(SoundgGeode);
		}
		else
		{
			osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
			at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN); //osg::AutoTransform::ROTATE_TO_SCREEN ROTATE_TO_CAMERA
			at->addChild   (SoundgGeode);
			SoundMatrix->addChild(at);
		}

		addChild(SoundMatrix);
}
