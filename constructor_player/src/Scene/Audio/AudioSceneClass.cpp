/***************************************************************************
BaseSoundObject.h  -  Определение класса аудио-сцены
-------------------
begin                : 8 марта 2023
begin                : 14 ноября 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "AudioSceneClass.h"


//конструктор
AudioSceneClass::AudioSceneClass(osg::ref_ptr<osg::Group> mainRoot, bool sound_off)
{
	SOUND_OFF = sound_off;

	if (SOUND_OFF == false)
	{
		std::cout << "AudioSceneClass constructor ... ";
		root = mainRoot;
		std::cout << " ok." << std::endl;
	}
}

AudioSceneClass::~AudioSceneClass()
{
	if (SOUND_OFF == false)
	{
		std::cout << "Destructor AudioSceneClass."  << std::endl;
		Reset();

		for(std::map<ALuint, bool>::iterator hitr2=SourcesPull.begin(); hitr2!=SourcesPull.end();	++hitr2)
		{
			alDeleteSources(1, &hitr2->first);
		}
		SourcesPull.clear();
		alGetError();
	}
}

//Обнеовление звуковой сцены
void AudioSceneClass::Update(float userX, float userY, float userZ)
{
	if (SOUND_OFF == true) return;
	
	//это здесь!!!!
	//оперировать только координатами пользователя, для оптимизации нельзя .. т.к. источники звука тоже двигаются
	//return;

	//Сортировка
	std::sort(AllSounds.begin(), AllSounds.end(), SoundSort(userX, userY, userZ));



	// 1. Помечаем "свободен" все источники, которые были активными при удаленнии объектов из сцены или просто останове звука по достижению конца файла.
	while (BaseSoundObject::FreeOnDeleteSources.size())
	{
		ALuint mSourceID = BaseSoundObject::FreeOnDeleteSources.top();
		SourcesPull[mSourceID] = true;
		BaseSoundObject::FreeOnDeleteSources.pop();
	}



	//2. Обход всех голосов, поиск "остановившихся" источников (!AL_PLAYING  !AL_PAUSED)
	// Уже выполняется при Update голоса

	//3. Построение отсортированного списка "желающих" и "активных" голосов в порядке "важности", подсчет сколько необходимо выделить источников для "головы состава",
	//   забирание (если не хватает) источников с "хвоста" и отдача их "голове" .... вот и все....
	//if (HeadRelative) то xyz совпадают с пользователем, иначе взять из callback


        for(std::vector < osg::ref_ptr<BaseSoundObject> >::iterator hitr=AllSounds.begin(); hitr!=AllSounds.end();	++hitr)
        {
                //std::cout << "Priority = " << (*hitr)->GetPriority() << std::endl;
                (*hitr)->Update();
                if (((*hitr)->isReal==false)&&((*hitr)->GetState()=="PLAYING"))
                {
                        for(std::map<ALuint, bool>::iterator hitr2=SourcesPull.begin(); hitr2!=SourcesPull.end();	++hitr2)
                        {
                                if (hitr2->second==true)
                                {
                                    try
                                    {
                                        (*hitr)->RestoreSource(hitr2->first);
                                        hitr2->second = false;
                                        break;
                                    }
                                    catch (ErrorClass error)
                                    {
                                        error.print();
                                    }

                                }
                        }
                }
        }



        //std::cout << "..ok" << std::endl;




}

//генерируеет пул доступных источников звука
void AudioSceneClass::GenerateSources()
{
	if (SOUND_OFF == true) return;


	std::cout << "AudioSceneClass::GenerateSources()";

	int max = 128;
	int i;
	for (i=0; i < max;i++)
	{
		alGetError(); // clear error code
		ALuint      mSourceID;
		alGenSources(1, &mSourceID);
		//
		ALenum ErrCode;
		if ((ErrCode = alGetError()) != AL_NO_ERROR)
		{
			break;
		}
		else
		{
			SourcesPull[mSourceID] = true;
		}
	}
	std::cout << " genegated "<<  i << " sources." << std::endl;
}

//возвращает количество задействованных элементов пула источников
int AudioSceneClass::GetActiveSources()
{

	int k=0;
	for(std::map<ALuint, bool>::iterator hitr2=SourcesPull.begin(); hitr2!=SourcesPull.end();	++hitr2)
	{
		if (hitr2->second==false)
		{
			k++;
		}
	}
	return k;
}


//проверка кеша на отсутствие "открепленных" со сцены объектов
void AudioSceneClass::CheckCache()
{
	if (SOUND_OFF == true) return;

	 std::vector < osg::ref_ptr<BaseSoundObject> >::iterator hitr=AllSounds.begin();
	 while(hitr != AllSounds.end())
	 {
		//Не пора-бы выкинуть объект из кеша....
		if ((*hitr)->referenceCount() <= 1 )
		{
			//std::cout << "delete cash sound ... " << (*hitr)->referenceCount()  << std::endl;
			hitr = AllSounds.erase (hitr);
			continue;
		}
		else
		{
			hitr++;
		}
	 }
}
//------------------------------


//Добавление нового объекта
void AudioSceneClass::NewObject(std::string Name, bool _value) throw (ErrorClass)
{
	if (SOUND_OFF == true) return;

	//проверка на совпадение имен с уже имеющимися
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(Name);
		root->accept(*fnnv.get());
	if (fnnv->_foundNodes.empty()!=true)
	{
			throw (ErrorClass (__FILE__, __LINE__ , "AudioSceneClass::NewObject", "Add new sound: Name already use: " + Name, 3));
	}

	//добавляем в сцену
	osg::ref_ptr<BaseSoundObject> newSound = new BaseSoundObject(Name);
	newSound->Head_Relative = _value;
	//связь с OSG сценой
	//newSound->setName(Name);
	root->addChild(newSound);

	AllSounds.push_back (newSound);


}

//Сброс всей сцены
void AudioSceneClass::Reset()
{
	if (SOUND_OFF == true) return;

		AllSounds.clear();
		CheckCache();
		std::cout << "AudioSceneClass::Reset()" << std::endl;
}

//--------------------------------------------------------------------------------------------------------
//сервисная функция
osg::ref_ptr<BaseSoundObject> AudioSceneClass::FindSound(std::string name)  throw (ErrorClass)
{

	//ищем в графе сцены
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(name);
		root->accept(*fnnv.get());

	//
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << name << std::endl;
		//попытка преобразовать в BaseSoundObject
				if ( BaseSoundObject* _foundSound = dynamic_cast<BaseSoundObject*>( fnnv->_foundNodes.front().get() ) )
				{
						osg::ref_ptr<BaseSoundObject> foundSound =  _foundSound;
						return foundSound.get();
				}
	}

		//генерируем ошибку
		throw (ErrorClass (__FILE__, __LINE__ , "AudioSceneClass::FindSound", "Object " + name + " not found", 3));
}
//--------------------------------------------------------------------------------------------------------
void AudioSceneClass::Open(std::string name, const std::string Filename, bool Looped, bool Streamed)
{
	if (SOUND_OFF == true) return;

	try
	{
	FindSound(name)->Open(Filename, Looped, Streamed);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}

void AudioSceneClass::Play(std::string name)
{
	if (SOUND_OFF == true) return;

	try
	{
	FindSound(name)->Play();
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}

void AudioSceneClass::setLoop(std::string name, bool Looped)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->setLoop(Looped);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}

void AudioSceneClass::Pause(std::string name)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->Pause();
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}

std::string AudioSceneClass::GetState(std::string name)
{
	if (SOUND_OFF == true) return "";

	try
	{
		return FindSound(name)->GetState();
	}
	catch (ErrorClass a)
	{
		a.print();
	}
	return "";
}

//Создает геометрический объект, для того чтобы видеть источник звука
void AudioSceneClass::ShowSoundDebug(std::string name)
{
	if (SOUND_OFF == true) return;

	try
	{
	FindSound(name)->ShowSoundDebug();
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}

//установить громкость
void AudioSceneClass::SetGain(std::string name, float value)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->SetGain (value);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}
//Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
void AudioSceneClass::SetMaxDistance(std::string name, float value)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->SetMaxDistance (value);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}
//Расстояние, на котором сила звука уменьшается вдвое (до воздействия параметра rolloff
void AudioSceneClass::SetReferenceDistance(std::string name, float value)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->SetReferenceDistance (value);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}
//Минимальное/максимальное значение громкости
void AudioSceneClass::SetMinMaxGain(std::string name, float min, float max)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->SetMinMaxGain (min, max);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}
//Перемотка звука на нужную секунду
void AudioSceneClass::SetOFFSET(std::string name, float sec)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->SetOFFSET(sec);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}
//Коэфффициент кривизны затухания звука (rolloff фактор)
void AudioSceneClass::SetRolloffFactor(std::string name, float value)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->SetRolloffFactor(value);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}
//12. установить приоритет
void AudioSceneClass::SetPriority (std::string name, float value)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->SetPriority(value);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}



//установить Pitch (0..10x) - ускорение/замедление звука
void AudioSceneClass::SetPitch(std::string name, float value)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->SetPitch(value);
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}

//add reverb
void AudioSceneClass::AddReverb(std::string name)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->AddReverb();
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}

void AudioSceneClass::DeleteReverb(std::string name)
{
	if (SOUND_OFF == true) return;

	try
	{
		FindSound(name)->DeleteReverb();
	}
	catch (ErrorClass a)
	{
		a.print();
	}
}

