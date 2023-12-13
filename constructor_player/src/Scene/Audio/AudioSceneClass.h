#ifndef AUDIOSCENECLASS
#define AUDIOSCENECLASS

#include "BaseSoundObject.h"
#include "../Visitors/FindNodeByName.h"
#include "../../error/errorclass.h"

class AudioSceneClass
{
	
public:
	//конструктор
	AudioSceneClass(osg::ref_ptr<osg::Group> mainRoot, bool sound_off);
	~AudioSceneClass();
	//добавление нового объекта
        void NewObject(std::string Name, bool _value) throw (ErrorClass);
	//Сброс всех объектов
	void Reset();
	//Обнеовление звуковой сцены
	void Update(float userX, float userY, float userZ);
	//генерируеет пул доступных источников звука
	void GenerateSources();
	//возвращает количество задействованных элементов пула источников
	int GetActiveSources();

        //проверка кеша на отсутствие "открепленных" со сцены объектов
        void CheckCache();


	//загрузить звуковой файл
	void Open(std::string name, const std::string Filename, bool Looped, bool Streamed);
	//начать воспроизведение
	void Play(std::string name);
	//поставить на паузу
	void Pause(std::string name);
	//Установить режим зацикливания
    void setLoop(std::string name, bool Looped);
	//получить состояние объекта (играет, на паузе, воспроизведение закончено и т.д)
	std::string GetState(std::string name);
	//Создает геометрический объект, для того чтобы видеть источник звука
	void ShowSoundDebug(std::string name);
        //установить громкость
        void SetGain(std::string name, float);
        //Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
        void SetMaxDistance(std::string name, float);
        //Расстояние, на котором сила звука уменьшается вдвое (до воздействия параметра rolloff
        void SetReferenceDistance(std::string name, float);
        //Минимальное/максимальное значение громкости
        void SetMinMaxGain(std::string name, float, float);
        //Перемотка звука на нужную секунду
        void SetOFFSET(std::string name, float);
        //Коэфффициент кривизны затухания звука (rolloff фактор)
        void SetRolloffFactor(std::string name, float);
        //12. установить приоритет
        void  SetPriority (std::string name, float priopity = 0.5f);
		
	
	
	//установить Pitch (0..10)
	void SetPitch(std::string name, float value);
	//+- reverb
	void AddReverb(std::string name);
	void DeleteReverb(std::string name);



private:
	//
	bool SOUND_OFF;
	//
	osg::ref_ptr<osg::Group> root;
	//сервисная функция
        osg::ref_ptr<BaseSoundObject> FindSound(std::string name) throw (ErrorClass);

	//пул sources <id, свободен>
	std::map <ALuint, bool> SourcesPull; 

    //
    std::vector < osg::ref_ptr<BaseSoundObject> > AllSounds;

	class SoundSort
	{
	public:
                SoundSort(float _userX, float _userY, float _userZ)
		{
			userX = _userX;
			userY = _userY;
			userZ = _userZ;
		}


		bool operator()(osg::ref_ptr<BaseSoundObject> rpStart, osg::ref_ptr<BaseSoundObject> rpEnd)
		{
			// head-relative и расстояния до пользователя
			float L1;
			if (rpStart->Head_Relative==true) 
			{
				L1=0;
			}
			else
			{
				L1 = sqrt ( mySquare(userX - rpStart->X) + mySquare(userY - rpStart->Y) + mySquare(userZ - rpStart->Z) );
			}
			float L2;
			if (rpEnd->Head_Relative==true) 
			{
				L2=0;
			}
			else
			{
				L2 = sqrt ( mySquare(userX - rpEnd->X) + mySquare(userY - rpEnd->Y) + mySquare(userZ - rpEnd->Z) );
			}
			//учет приоритета
			float A1 = L1 * (1.0f-rpStart->GetPriority());
			float A2 = L2 * (1.0f-rpEnd->GetPriority());
			//Делаем вывод...
			return A1 > A2;
		}

	private:
		float userX;
		float userY;
		float userZ;

		
		float mySquare(float arg)
		{
			return arg*arg;
		}
		
	};

};


#endif
