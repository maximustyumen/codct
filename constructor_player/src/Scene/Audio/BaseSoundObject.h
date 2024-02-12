#ifndef BASESOUNDOBJECTCLASS
#define BASESOUNDOBJECTCLASS

#include "../../error/errorclass.h"

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osg/Texture3D>
#include <osg/TexGen>
#include <osg/PolygonMode>
#include <osg/Drawable>
#include <osg/LineWidth>
#include <osg/AutoTransform>
#include <osg/Timer>

	#include <AL/al.h>
	#include <AL/alc.h>

#include <AL/alut.h>

//http://www.gamedev.ru/articles/?id=100102&page=2
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <algorithm>
#include <cctype>
#include <istream>

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <stack>


class BaseSoundObject: public osg::MatrixTransform
{
public:
	//конструктор
	BaseSoundObject(std::string name);
	//деструктор
        ~BaseSoundObject() ;
	
	//список имен всех созданных и неудаленных голосов
	static std::map <std::string, int> VirtualVoices;
	//список освободифшихся при удалении id источников...
	static std::stack <ALuint> FreeOnDeleteSources;
	
	//0. Отбирание источника звука, превращение звука в виртуальный...
        void TakeSource() throw (ErrorClass);;
	//0. Возвращение источника звука, превращение звука в реальный...
        void RestoreSource(ALuint) throw (ErrorClass);;
	//0. Расчет "виртуальной" игры звука (), изменение состояния виртуального звука и т.д.
	void Update();

	//принадлежит ли этому голосу реальный источник (source)
	bool isReal;

	//координаты звука в мировых координатах...
	float X;
	float Y;
	float Z;



	//1. открытие файла, создание источника звука
        bool Open(const std::string Filename, bool Looped, bool Streamed) throw (ErrorClass);
	//2. воспроизведение
        void Play() throw (ErrorClass);
	//3. установка режима повтора
        void setLoop(bool Looped) throw (ErrorClass);
	//4. поставить на паузу
        void Pause() throw (ErrorClass);
	//5.получить состояние объекта (играет, на паузе, воспроизведение закончено и т.д)
        std::string GetState() throw (ErrorClass);
        //6.установить громкость
        void SetGain(float) throw (ErrorClass);
        //7. Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
        void SetMaxDistance(float) throw (ErrorClass);
        //8. Расстояние, на котором сила звука уменьшается вдвое (до воздействия параметра rolloff
        void SetReferenceDistance(float) throw (ErrorClass);
        //9. Минимальное/максимальное значение громкости
        void SetMinMaxGain(float, float) throw (ErrorClass);
        //10. "перемотать звук на нужную секунду" [0;any]
        void SetOFFSET(float) throw (ErrorClass);
	//11. Коэфффициент кривизны затухания звука (rolloff фактор)
        void SetRolloffFactor(float) throw (ErrorClass);
        //12. установить приоритет
        void SetPriority (float priopity = 0.5f);
        //13. запросить приоритет
        float GetPriority ();


	
	//флаг режима "звук в голове"
	bool Head_Relative;


	//6. Создает геометрический объект, для того чтобы видеть источник звука
	void ShowSoundDebug();
	//Эксперементы//
	//установить Pitch (0..10x) - ускорение/замедление звука
	void SetPitch(float value);
	//+- reverb
	void AddReverb();
	void DeleteReverb();

private:
	class MyTransformCallback : public osg::NodeCallback
	{
		public:
			MyTransformCallback();
			void RestoreSource (ALuint newSourceID, bool _Head_Relative);
			void TakeSource ();
			//координаты
			float x;
			float y;
			float z;
		
		private:
			virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
			//
			// Идентификатор источника
			ALuint mSourceID;
			//принадлежит ли этому голосу реальный источник (source)
			bool isReal;
			//флаг
			bool Head_Relative;
			//
			float oldX;
			float oldY;
			float oldZ;
    };

	//ПАРАМЕТРЫ
	//скорость
	ALfloat mVel[3];
	//позиция
	ALfloat mPos[3];
	//флаг режима зациклливания
	bool  mLooped;
	//громкость
	float GainValue;
    //частота
    float PitchValue;
	//Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
	float MaxDistance;
	//Расстояние, на котором сила звука уменьшается вдвое
	float ReferenceDistance;
	//Минимальное/максимальное значение громкости
	float MinGain;
	float MaxGain;
	//Коэфффициент кривизны затухания звука (rolloff фактор)
	float RolloffFactor;
	//позиция звука в секундах
	float OFFSET;
	//желаемое->фактическое состояние
	std::string STATE;
        //Длинна файла/звука в секундах
        double SoundLenght;
        //"важность звука"/приоритет
        float Priority;


        //debug
        osg::ref_ptr<osg::ShapeDrawable>  ShapeDrawable;
        bool isDebugCreated;
	
	//EAX
	ALuint uiEffectSlot;
	ALuint uiEffect;
	//
	// Идентификатор источника
	ALuint      mSourceID;
	// Идентификатор буфера
	ALuint  ID;
	
	
	
	//-----------------WAV---------------------
	//загрузка файлов с данными
        bool LoadWavFile (const std::string Filename) throw (ErrorClass);


	ALboolean CheckALError();
	std::string filename;
	
	//-------------VORBIS------------------------------------
	unsigned int	Rate;
	unsigned int	Format;
	// Главная структура описания файла
	OggVorbis_File  *mVF;
	// Структура комментариев к файлу
	vorbis_comment  *mComment;
	// Информация о файле
	vorbis_info    *mInfo;
	// Файловый поток содержащий наш ogg файл
	//std::ifstream    OggFile;
	bool      mStreamed;
	// Функция чтения блока из файла в буфер
        bool ReadOggBlock(size_t Size);
	// Функция открытия и инициализации OGG файла
        bool LoadOggFile (const std::string &Filename, bool Streamed) throw (ErrorClass);
	

public:
	//
	osg::ref_ptr <MyTransformCallback> callback;
	
};

#endif
