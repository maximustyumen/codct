/***************************************************************************
Unterpretator.h  -  процедурный модуль интерпритатора
-------------------
begin                : 2 декабря 2005
refactoring          : 1 декабря 2007
copyright            : (C) 2005-2007 by Гаммер Максим Дмитриевич (maximum2000)
email                : GammerMax@yandex.ru
***************************************************************************/

#include "../BuildConfig.h"


//Lua
#ifdef WIN32
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#else
extern "C" {
#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
}

#endif

//Модуль вызова функций из внешних библиотек
#include "./SupportExternLibs.h"
//Своя реализация os.clock
#include "./os_clock.h"
//реализация функции определения директории пользователя HOME\HomePath ....
#include "./env.h"
// Функция для работы с Arduino
#include "../Arduino/SerialClass.h"

//класс описания ошибок
#include "../error/errorclass.h"

//#include "../RTI/Uuencode/uuencode.h"

#include <fstream>
#include <iostream>

// Disable warning messages.
//#pragma warning( disable : 4800 )

namespace Interpritator
{

/*
//ВРЕМЕННО
//данные по импортируемому в луа классу
#include "./SampleClass.h"
//Статическая инициализация
const char JoystickLuaClass::className[] = "JoystickClass"; //можно и другое))
Lunar<JoystickLuaClass>::RegType JoystickLuaClass::methods[] = {
  method(JoystickLuaClass, GetAxesState),
  method(JoystickLuaClass, GetHatState),
  method(JoystickLuaClass, GetButtonState),
  {0,0}
};
//+ далее по коду Lunar<SampleLuaClass>::Register(L);
//конец...
*/

bool ReloadScript=false;
bool ReloadScript2=false;
std::string NewFileNameScript="";

//LUA
//Инициализация LUA
void LUAinit1();
bool LUAinit2(std::string filename);
//Регистрация функций для LUA
void LUAregistration();
//интерпретация комманд в LUA
void Interpretation(InternalEventsClass events);
//проверка, есть ли обработчик события в LUA или нет
bool IsLuaObject(std::string Name);
//Объявление LUA
lua_State *L;

//--------------------------test--------------------------------
static int test_createCustomGeometry(lua_State *L);
//полное кеширование уже подключенных к viewer'у данных
static int FullCompileScene(lua_State *L);
//--------------------------SCORM-------------------------------
//-------------------------Выставление оценки точности (правильности) выполнения
static int ScormSetScore(lua_State *L);
//-------------------------Выставление оценки объема (количества) выполненных заданий
static int ScormSetProgress(lua_State *L);
//-------------------------Возврат значения из LMS по имени поля (например cmi.learner_name)
static int ScormGetValue(lua_State *L);
//-------------------------Запись "сырых данных" пары ключ \ значение
static int ScormSetValue(lua_State *L);

//---------------------------TINCAN-----------------------------
//-------------------------создание JSON записи
static int TinCanAddRecord(lua_State *L);
//-------------------------отправка всех записей в LRS
static int TinCanPostToLRS(lua_State *L);
//отправка всех записей в файл
static int TinCanPostToFile(lua_State *L);


//
//------------------------- функция для для рабты с потоком------
static int SerialPortInit(lua_State *L);

//
//-------------------------функция для определения "директории пользователя" HOME/HOMEPATH------
static int GetUserHomeDir(lua_State *L);
static int GetUserTempDir(lua_State *L);
//-------------------------Выход из тренажера +- блокировка выхода по F10
static int Exit(lua_State *L);
//-------------------------ScreenShot---------------------------
static int ScreenShot(lua_State *L);
//-------------------------Перезагрузка другого сценария--------
static int NextLevel(lua_State *L);
//-------------------------ПОЛЬЗОВАТЕЛЬ-------------------------
static int player_SetUserPositionAndRotation(lua_State *L);
static int player_SetUserHold(lua_State *L);
static int player_SetFocus(lua_State *L);
//-------------------------текст в около текстового курсора------------------
static int player_SetCursorText(lua_State *L);
static int player_SetCursorTextSize(lua_State *L);
static int player_SetCursorTextFont(lua_State *L);
static int player_SetCursorTextColor(lua_State *L);
//-------------------------Джойстики----------------------------
//функция возвращает число джойстиков (нет аргументов)
static int joystick_JoyGetNum(lua_State *L);
//функция возвращает состояние указанной кнопки указанного джойстика
static int joystick_JoyGetButtonsState(lua_State *L);
//функция возвращает состояние указанного "Hats"а указанного джойстика
static int joystick_JoyGetHatsState(lua_State *L);
//функция возвращает состояние указанной оси вращения указанного джойстика
static int joystick_JoyGetAxisState(lua_State *L);


// --------------------------- физика --------------------------- //
#ifdef PHYSX
#if (_MSC_VER < 1700)
// добавить в сцену статическую плоскость
static int phAddStaticPlane( lua_State* pLuaState );
// добавить твёрдое тело
static int phAddRigidBody( lua_State* pLuaState );
// пирнудительно поставить физический объект в позицию (глоб.коорд)
static int phSetPosition( lua_State* pLuaState );
// описатель сил, вращений
struct sPhysicForceDesc
{
	std::string strName;
	cPhysicObject::eForceMode mode;
	NxVec3 vForce;
	bool bGlobal;
};
// вспомогательные функции
static void parseToPhysicForceDesc( lua_State* pLuaState, sPhysicForceDesc* pOut );
static void parseD6flag( lua_State* pLuaState, int index, NxD6JointMotion* pMotion );

// добавить силу к телу
static int phAddForce( lua_State* pLuaState );
// добавить вращение к телу
static int phAddTorque( lua_State* pLuaState );
// установить центр масс внутри объекта
static int phSetShapeLocalPosition( lua_State* pLuaState );
// добавить на сцену соединение
static int phAddJoint( lua_State* pLuaState );
// создать разрушаемое соединение
static int phSetBreakableJoint( lua_State* pLuaState );
// установить глобальную ось для соединения
static int phSetGlobalAxisAtJoint( lua_State* pLuaState );
// установить глобальную точку соединения
static int phSetGlobalAnchorAtJoint( lua_State* pLuaState );
// установить ограничение в виде точки
static int phSetLimitPointAtJoint( lua_State* pLuaState );
// установить ограничение плоскостью
static int phAddLimitPlaneAtJoint( lua_State* pLuaState );
// установить свойства "мотора" на revolute соединение
static int phSetMotorAtRevoluteJoint( lua_State* pLuaState );

// new
static int phActorWakeUp( lua_State* pLuaState );
static int phActorSetCMassOffsetLocalPosition( lua_State* pLuaState );

static int phMaterialCreate( lua_State* pLuaState );
static int phMaterialDestroy( lua_State* pLuaState );

static int phActorGetNumShapes( lua_State* pLuaState );
static int phActorSetMaterialForShape( lua_State* pLuaState );
static int phActorSetLinearVelocity( lua_State* pLuaState );
static int phActorSetAngularVelocity( lua_State* pLuaState );
static int phActorGetLinearVelocity( lua_State* pLuaState );

static int phActorGetGlobalOrientationQuat( lua_State* pLuaState );
static int phActorSetGlobalOrientationQuat( lua_State* pLuaState );


static int phWheelCreate( lua_State* pLuaState );
static int phWheelGetPos( lua_State* pLuaState );
static int phWheelSetAngle( lua_State* pLuaState );
static int phWheelIsGroundContact( lua_State* pLuaState );
static int phWheelUpdate( lua_State* pLuaState );

static int phRaycast( lua_State* pLuaState );

//
#endif
#endif

//
static int object_Select( lua_State *L );
static int object_GetBoundingBox( lua_State *L );

//-------------------------3D объекты---------------------------
//Установка значения шейдера
static int object_ChangeShaderUniform(lua_State *L);
//Функция для загрузки 3D объектов
static int object_Load3DObject(lua_State *L);
//Функция для перемещения объекта (3d)
static int object_RSetPosition(lua_State *L);
//Функция очистки загруженных в сцену данных (Load3DObject), кроме курсора и т.д.
static int object_RReset(lua_State *L);
//Функция совмещения точки привязки одного объекта с другим
static int object_RSetAt(lua_State *L);
//Функция выборочного сброса матриц позиции/поворота/масштаба
static int object_RResetObject(lua_State *L);
//альтернативное вращение 3D объекта
static int object_RSetRotate(lua_State *L);
//альтернативное сложное вращение 3D объекта
static int object_RSetRotate2(lua_State *L);
//альтернативное перемещение относительно другого объекта по абсолютным координатам
static int object_ASetAt(lua_State *L);
//альтернативное задание видимости объекта
static int object_RSetVisible(lua_State *L);
//альтернативное масштабирование объекта
static int object_RSetScale(lua_State *L);
//альтернативная передача x,y,z объекта
static int object_RGetPosition(lua_State *L);
//альтернативная передача x,y,z объекта - абсолютные координаты
static int object_AGetPosition(lua_State *L);
//альтернативное получение масштаба объекта
static int object_RGetScale(lua_State *L);
//альтернативное получение квантериона вращения объекта
static int object_RGetRotate(lua_State *L);
//--------------------------рисование--------------------------------
//альтернативное рисование на текстуре объекта
static int object_RTextureDraw(lua_State *L);
//альтернативная замена текстуры  объекта
static int object_RChangeTexture(lua_State *L);
//альтернативная Установка цвета объекта
static int object_RSetColor(lua_State *L);
//-------------------------------------------------------------------
//Изменение иерархии объектов (изменение подчинения объектов))
static int object_RConnectObjectTo(lua_State *L);
//-------------------------------------------------------------------
//Тест на пересечение 2 объектов в относительных координатах
static int object_TestIntersects(lua_State *L);
//Тест на пересечение 2 объектов в относительных координатах
static int object_TestIntersects2(lua_State *L);
//Тест на пересечение линией
static int object_TestLineIntersects(lua_State *L);
//-------------------------------------------------------------------
//Запрос предка объекта
static int object_GetParent(lua_State *L);
//-------------------------------------------------------------------
//Функция подключает шейдер к объекту
static int object_SetShader(lua_State *L);
//--------------------------------------------------------------------
//ПАУЗА/СТАРТ анимации
static int object_SetPauseAnimation(lua_State *L);
//Переход на нужное время анимации
static int object_GotoAnimation(lua_State *L);
//Установка опорного смещения анимации
static int object_SetPivotPointAnimation(lua_State *L);
//-------------------------------------------------------------------
// создание morph-объекта
static int CreateMorph( lua_State* p_lua_state );
// переход на нужное время анимации morph-объекта
static int GotoMorphState( lua_State* p_lua_state );
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//Создание отдельной группы теней, параметр - имя группы
static int object_AddShadowedGroup(lua_State *L);
//Определение маски Имя объекта, 4-ничего, 1-принимать, 2-бросать, 3 - и то и другое.
static int object_SetShadowMask(lua_State *L);
//---------------------RTI-----------------------------------------
/*
static int RTI_CreateFederate( lua_State* pLuaState );
static int RTI_DestroyFederate( lua_State* pLuaState );
static int RTI_RegisterSynchronizationPoint( lua_State* pLuaState );
static int RTI_RunFederate( lua_State* pLuaState );
static int RTI_Publish( lua_State* pLuaState );
static int RTI_Subscribe( lua_State* pLuaState );
static int RTI_AddObject( lua_State* pLuaState );
static int RTI_RemoveObject( lua_State* pLuaState );
static int RTI_SetValue( lua_State* pLuaState );

void RTI_Change( const std::string& str_object_name, const std::vector<cFederateAmbassador::sReflectAttribute>& v_attributes,
        const std::string& str_tag, double time  );

void RTI_Discover( const std::string& str_object_name, const std::string& str_class_name );
void RTI_Removed(const std::string& object_name);

static int RTI_PublishInteraction( lua_State* pLuaState );
static int RTI_SubscribeInteraction( lua_State* pLuaState );

static int RTI_AddParameter( lua_State* pLuaState );
static int RTI_SendInteraction( lua_State* pLuaState );

void RTI_Interaction( const std::string& str_class_name, const std::vector<cFederateAmbassador::sReflectParameter>& v_params,
        const std::string& str_tag );

static int RTI_AddProxy( lua_State* pLuaState );
static int RTI_RemoveProxy( lua_State* pLuaState );


static int net_create(lua_State* lua_state);
*/
//---------------------text-----------------------------------------
//Создание текстового объекта
static int text_AddTextObject(lua_State *L);
//Задание значения текстового поля
static int text_SetText(lua_State *L);
//Задание цвета текста
static int text_SetColor(lua_State *L);
//Задание размера текста
static int text_SetSize(lua_State *L);
//Задание шрифта текста
static int text_SetFont(lua_State *L);
//задание равнения текста
static int text_SetAlignment(lua_State *L);
//задание ориентации текста
static int text_SetLayout(lua_State *L);
//задание ориентации текста к экрану 
static int text_SetAxisAlignment(lua_State *L);



//------------------------звук---------------------------------------
//Функция для создания Sound объектов
static int sound_NewSoundObject(lua_State *L);
//Функция для загрузки звукового файла
static int sound_LoadSound(lua_State *L);
//Функция для начала воспроизведения звукового файла
static int sound_PlaySound(lua_State *L);
//Функция для паузы звукового файла
static int sound_PauseSound(lua_State *L);
//Функция для задания режима LOOP звукового файла
static int sound_LoopSound(lua_State *L);
//Функция для задания позиции источника звука
static int sound_PositionSound(lua_State *L);
//получить состояние объекта (играет, на паузе, воспроизведение закончено и т.д)
static int sound_GetState(lua_State *L);
//Создает геометрический объект, для того чтобы видеть источник звука
static int sound_ShowDebug(lua_State *L);
//установить громкость
static int sound_SetGain(lua_State *L);
//Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
static int sound_SetMaxDistance(lua_State *L);
//Расстояние, на котором сила звука уменьшается вдвое (до воздействия параметра rolloff
static int sound_SetReferenceDistance(lua_State *L);
//Минимальное/максимальное значение громкости
static int sound_SetMinMaxGain(lua_State *L);
//"перемотка звука на нужную секунду"
static int sound_SetOFFSET(lua_State *L);
//Коэфффициент кривизны затухания звука (rolloff фактор)
static int sound_SetRolloffFactor(lua_State *L);
//установка приоритета звука [0..1]
static int sound_SetPriority(lua_State *L);
//установить Pitch (0..10)
static int sound_SetPitch(lua_State *L);
//добавить эффект Reverb
static int sound_AddReverb(lua_State *L);

//-------------------------человеки/персонажи-------------------------
//Функция для загрузки и создания персонажа
static int character_Load(lua_State *L);
//установка анимации персонажа
static int character_SetAction(lua_State *L);
//установка позиции персонажа
static int character_SetPosition(lua_State *L);
//установка места-назначения персонажа
static int character_GoTo(lua_State *L);
//Прикрепляет объект к кости
static int character_ConnectObjectToBone(lua_State *L);


//---------------------------Skin Animation ----------------------------------------
static int LoadSkinAnimation(lua_State *L);
static int PlaySkinAnimation(lua_State *L);
static int StopSkinAnimation(lua_State *L);
static int GotoTimeSkinAnimation(lua_State *L);
static int RotateSkinBone(lua_State* L);

//--------------------------свет----------------------------------------
//устанавливает Diffuse-цвет указанного светового источника
static int SetLightColor(lua_State *L);
//включает/выключает свет
static int SetLightEnable(lua_State *L);
//Включает/выключает учет освещения объектом
//изменять атрибуты, например "объект освещается или нет" (stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);)
static int SetLightingEnable(lua_State *L);
//-------------------------Клонирование/удаление---------------------------------
//клонирует объект-оригинал и цепляет его к предку оригинала (т.е. у них общий родитель сохраняется)
static int object_Clone(lua_State *L);
//Удаление объекта из сцены
static int object_UnloadObject(lua_State *L);
//-------------------------------video---------------------------------------------
//создает объект-видео
static int object_CreateVideoPlane(lua_State *L);
//1. Играть
static int object_PlayVideo(lua_State *L);
//2. Пауза
static int object_PauseVideo(lua_State *L);
//3. Длинна файла в секундах
static int object_getLengthVideo(lua_State *L);
//4. Перемотать на секунду ...
static int object_setReferenceTimeVideo(lua_State *L);
//5. Позиция в секундах..
static int object_getReferenceTimeVideo(lua_State *L);
//6. Установить громкость
static int object_setVolumeVideo(lua_State *L);
//7. Узнать громкость
static int object_getVolumeVideo(lua_State *L);
//8. Режим повтора
static int object_setLoopingModeVideo(lua_State *L);
//-------------------------------------вершины-------------------------------------
static int object_RSetVertexPosition(lua_State *L);
static int object_RSetTexturePointPosition(lua_State *L);
static int object_RSetTexturePosition(lua_State *L);
//------------------------------частицы--------------------------------------------
//Создание частиц
static int particle_Add(lua_State *L);
//время жизни частицы
static int particle_ParticleSetLifeTime(lua_State *L);
//разброс размеров
static int particle_ParticleSetSizeRange(lua_State *L);
//разброс прозрачности
static int particle_ParticleSetAlphaRange(lua_State *L);
//разброс цвета
static int particle_ParticleSetColorRange(lua_State *L);
//первоначальный радиус
static int particle_ParticleSetRadius(lua_State *L);
//масса
static int particle_ParticleSetMass(lua_State *L);
//угол поворота проскостей
static int particle_ParticleSetAngle(lua_State *L);
//текструра,  emissive? , освещаются или нет, текстурный юнит=0
static int particle_ParticleSetDefaultAttributes(lua_State *L);
//Разброс количества генерируемых частиц в секунду
static int particle_ParticleSetRateRange(lua_State *L);
//Указание углов разброса по двум плоскастям (замечание Ассель)
static int particle_ParticleSetEmitterAngle(lua_State *L);
//Указание радиуса сегмента, из которого появляются частицы (замечание Ассель)
static int particle_ParticleSetEmitterRadius(lua_State *L);
//время скорость частицы
static int particle_ParticleSetSpeed(lua_State *L);
//--------------------------------------volume render-----------------------------------------
static int SetVolumeTexture(lua_State *L);

//------------------------------------------------BASE3DOBJECT-------------------------------------
//Функция для создания 3D объектов
static int player_New3DObject(lua_State *L);
//Функция задает координаты точки привязки объекта, для дальнейшей анимации
static int object_SetCenterPosition(lua_State *L);
//Функция задает координаты объекта
static int object_SetPosition(lua_State *L);
//Функция задает видимость объекта
static int object_SetVisible(lua_State *L);
//Функция задает масштабирование объекта
static int object_SetScale(lua_State *L);
//Функция задает простое вращение объекта 3D объекта
static int object_SetRotate(lua_State *L);
//Функция задает сложное перемещение  3D объекта
static int object_SetRotate2(lua_State *L);




//проверка, есть ли обработчик события в LUA или нет
bool IsLuaObject(std::string Name)
{
	lua_getglobal(Interpritator::L, Name.c_str());
	/*
  lua_pushstring( L, Name.c_str() );
  lua_gettable( L, LUA_GLOBALSINDEX);
*/
  bool object_nil = lua_isnil( L, -1 );
  lua_pop( L, 1 );

  return object_nil == false;
}

//Инициализация LUA
void LUAinit1 ()
{
	std::cout << "Init  ... ";
	#ifdef LUA_RELEASE
	std::cout << LUA_RELEASE << std::endl;
	#endif
	#ifdef LUA_COPYRIGHT
	std::cout << "\t" << LUA_COPYRIGHT << std::endl;
	#endif
	//Инициализация LUA
	L = luaL_newstate();// open();   /* opens Lua */
	luaL_openlibs (L);
	//в обработчик ошибок...
	ErrorClass::Lua = L;
}

bool LUAinit2 (std::string filename)
{
	//ВРЕМЕННО
	//Регистрация класса
	//Lunar<JoystickLuaClass>::Register(L);
	//Регистрация закончена


	//Регистрация функций для LUA
	LUAregistration();
	//patch для os.clock
	luaL_dostring(L, "os.clock = os_clock");

	//открываем файл, файл выполняется ...
	try
	{
		if (luaL_loadfile(L, filename.c_str())||lua_pcall(L,0,0,0))
		{
			std::cout << "Error run start script" << std::endl;
			std::cout << lua_tostring(L,-1) << std::endl;
			
			lua_pop(L,1);
			throw (ErrorClass (__FILE__, __LINE__ , "LUAinit2", "Error run start script" , 3));
		}
		//Передаем переменные среды в таблицу UserData
		{
			static int __Width =  localUser->getViewWidth();
			static int __Height =  localUser->getViewHeight();

			/*
			lua_pushstring(L, "UserData");
			lua_gettable(L, LUA_GLOBALSINDEX);
			*/
			lua_getglobal(L, "UserData");
			lua_pushstring(L, "WINDOW_WIDTH" );
			lua_pushnumber(L, __Width);
			lua_settable(L, -3);
			lua_pushstring(L, "WINDOW_HEIGHT" );
			lua_pushnumber(L, __Height);
			lua_settable(L, -3);
			lua_pop(L,1);
		}
		// Инициализация STDLib
		{
			std::string _COMMAND = "STDLIB_INIT";
			lua_getglobal(L,_COMMAND.c_str());
			if (!lua_isnil(L, -1) && lua_pcall(L,0,0,0))
			{
				std::cout << "Error run start STDLIB_INIT script" << std::endl;
				std::cout << lua_tostring(L,-1) << std::endl;
				lua_pop(L,1);
				throw (ErrorClass (__FILE__, __LINE__ , "LUAinit2", "Error run start STDLIB_INIT script" , 3));
			}
		}
		//Глобальная инициализация
		{
			std::string _COMMAND = "INIT";
			lua_getglobal(L,_COMMAND.c_str());
			if (!lua_isnil(L, -1) && lua_pcall(L,0,0,0))
			{
				std::cout << "Error run start INIT script" << std::endl;
				std::cout << lua_tostring(L,-1) << std::endl;
				lua_pop(L,1);
				throw (ErrorClass (__FILE__, __LINE__ , "LUAinit2", "Error run start INIT script" , 3));
			}
		}

		//Локальная инициализация (LocalInits)
		{
			std::string _COMMAND = "LocalInits";
			lua_getglobal(L,_COMMAND.c_str());
			if (lua_pcall(L,0,0,0))
			{
				std::cout << "Error run start LocalInits script" << std::endl;
				std::cout << lua_tostring(L,-1) << std::endl;
				lua_pop(L,1);
				throw (ErrorClass (__FILE__, __LINE__ , "LUAinit2", "Error run start LocalInits script" , 3));
			}
		}
	}
	catch (ErrorClass error)
	{
		error.print();
		return false;
	}
	return true;
}

//Регистрация функций для LUA
void LUAregistration()
{
	//--------------------------patch os.clock------------------------------------------------------
	lua_pushcfunction(L, os_clock);
	lua_setglobal (L,"os_clock");
	//-------------------------функция для вызова внешних подпрограмм из DLL/SO-------------------
	lua_pushcfunction(L, externcall);
	lua_setglobal (L,"ExternCall");

	
		//------------------------- функция для для рабты с потоком------
		lua_pushcfunction(L, SerialPortInit);
        lua_setglobal (L,"SerialPortInit");

        //-------------------------функция для определения "директории пользователя" HOME/HOMEPATH------
        lua_pushcfunction(L, GetUserHomeDir);
        lua_setglobal (L,"GetUserHomeDir");
        //------------------------temp dir--------------------------------------------------------------
        lua_pushcfunction(L, GetUserTempDir);
        lua_setglobal (L,"GetUserTempDir");
        //
        //--------------------------SCORM-------------------------------
        //-------------------------Выставление оценки точности (правильности) выполнения заданий
        lua_pushcfunction(L, ScormSetScore);
        lua_setglobal (L,"ScormSetScore");
        //-------------------------Выставление оценки объема (колтчества)) выполненных заданий
        lua_pushcfunction(L, ScormSetProgress);
        lua_setglobal (L,"ScormSetProgress");
        //-------------------------Возврат значения из LMS по имени поля (например cmi.learner_name)
        lua_pushcfunction(L, ScormGetValue);
        lua_setglobal (L,"ScormGetValue");
        //-------------------------Запись сырых данных типа "ключ\значение"
        lua_pushcfunction(L, ScormSetValue);
        lua_setglobal (L,"ScormSetValue");

		//------------------------TINCAN-----------------------------------
		//-------------------------создание JSON записи
		lua_pushcfunction(L, TinCanAddRecord);
        lua_setglobal (L,"TinCanAddRecord");
		//-------------------------отправка всех записей в LRS
		lua_pushcfunction(L, TinCanPostToLRS);
        lua_setglobal (L,"TinCanPostToLRS");
		//-------------------------отправка всех записей в файл
		lua_pushcfunction(L, TinCanPostToFile);
        lua_setglobal (L,"TinCanPostToFile");
		








	//-------------------------test---------------------------------
	lua_pushcfunction(L, test_createCustomGeometry);
	lua_setglobal (L,"createCustomGeometry");
         //полное кеширование уже подключенных к viewer'у данных
        lua_pushcfunction(L, FullCompileScene);
        lua_setglobal (L,"FullCompileScene");
	//-------------------------Выход из тренажера +- блокировка выхода по F10
	lua_pushcfunction(L, Exit);
	lua_setglobal (L,"Exit");
	//-------------------------ScreenShot---------------------------
	lua_pushcfunction(L, ScreenShot);
	lua_setglobal (L,"ScreenShot");
	//-------------------------Перезагрузка другого сценария--------
	lua_pushcfunction(L, NextLevel);
	lua_setglobal (L,"NextLevel");//+doc
	//-----------------------MAIN USER--------------------------------
	//Функция установки нового положения пользователя в пространстве и уго направления взгляда
	lua_pushcfunction(L, player_SetUserPositionAndRotation);
	lua_setglobal (L,"SetUserPositionAndRotation");//+doc
	//Функция установки флага заморозки передвижения пользователя/взгляда/управления
	lua_pushcfunction(L, player_SetUserHold);
	lua_setglobal (L,"SetUserHold");//+doc
	//Функция установки фокусного расстояния
	lua_pushcfunction(L, player_SetFocus);
	lua_setglobal (L,"SetUserFocus");//-doc
        //----------------------текст около курсора---------------------------------
        lua_pushcfunction(L, player_SetCursorText);
        lua_setglobal (L,"SetUserCursorText");
        //
        lua_pushcfunction(L, player_SetCursorTextSize);
        lua_setglobal (L,"SetUserCursorTextSize");
        //
        lua_pushcfunction(L, player_SetCursorTextFont);
        lua_setglobal (L,"SetUserCursorTextFont");
        //
        lua_pushcfunction(L, player_SetCursorTextColor);
        lua_setglobal (L,"SetUserCursorTextColor");
        //
	//----------------------Joystick---------------------------------------------
	//функция возвращает число джойстиков (нет аргументов)
	lua_pushcfunction(L, joystick_JoyGetNum);
	lua_setglobal (L,"JoyGetNum");
	//функция возвращает состояние указанной кнопки указанного джойстика
	lua_pushcfunction(L, joystick_JoyGetButtonsState);
	lua_setglobal (L,"JoyGetButtonsState");
	//функция возвращает состояние указанного "Hats"а указанного джойстика
	lua_pushcfunction(L, joystick_JoyGetHatsState);
	lua_setglobal (L,"JoyGetHatsState");
	//функция возвращает состояние указанной оси вращения указанного джойстика
	lua_pushcfunction(L, joystick_JoyGetAxisState);
	lua_setglobal (L,"JoyGetAxisState");
	//------------------------------------Тестовая физика------------------------
#ifdef PHYSX
	#if (_MSC_VER < 1700)
	// physic
	lua_pushcfunction( L, phAddStaticPlane );
	lua_setglobal( L, "phAddStaticPlane" );
	//
	lua_pushcfunction( L, phAddRigidBody );
	lua_setglobal( L, "phAddRigidBody" );
	//
	lua_pushcfunction( L, phSetPosition );
	lua_setglobal( L, "phSetPosition" );
	//
	lua_pushcfunction( L, phAddForce );
	lua_setglobal( L, "phAddForce" );
	//
	lua_pushcfunction( L, phAddTorque );
	lua_setglobal( L, "phAddTorque" );
	//
	lua_pushcfunction( L, phSetShapeLocalPosition );
	lua_setglobal( L, "phSetShapeLocalPosition" );
	//
	lua_pushcfunction( L, phAddJoint );
	lua_setglobal( L, "phAddJoint" );


	// test!

	lua_pushcfunction( L, phActorWakeUp );
	lua_setglobal( L, "phActorWakeUp" );

	lua_pushcfunction( L, phActorSetCMassOffsetLocalPosition );
	lua_setglobal( L, "phActorSetCMassOffsetLocalPosition" );

	lua_pushcfunction( L, phMaterialCreate );
	lua_setglobal( L, "phMaterialCreate" );

	lua_pushcfunction( L, phMaterialDestroy );
	lua_setglobal( L, "phMaterialDestroy" );

	lua_pushcfunction( L, phActorGetNumShapes );
	lua_setglobal( L, "phActorGetNumShapes" );

	lua_pushcfunction( L, phActorSetMaterialForShape );
	lua_setglobal( L, "phActorSetMaterialForShape" );

	lua_pushcfunction( L, phActorSetLinearVelocity );
	lua_setglobal( L, "phActorSetLinearVelocity" );

	lua_pushcfunction( L, phActorSetAngularVelocity );
	lua_setglobal( L, "phActorSetAngularVelocity" );

	lua_pushcfunction( L, phActorGetLinearVelocity );
	lua_setglobal( L, "phActorGetLinearVelocity" );

	lua_pushcfunction( L, phActorSetGlobalOrientationQuat );
	lua_setglobal( L, "phActorSetGlobalOrientationQuat" );

	lua_pushcfunction( L, phActorGetGlobalOrientationQuat );
	lua_setglobal( L, "phActorGetGlobalOrientationQuat" );

	lua_pushcfunction( L, phWheelCreate );
	lua_setglobal( L, "phWheelCreate" );

	lua_pushcfunction( L, phWheelGetPos );
	lua_setglobal( L, "phWheelGetPos" );

	lua_pushcfunction( L, phWheelSetAngle );
	lua_setglobal( L, "phWheelSetAngle" );

	lua_pushcfunction( L, phWheelIsGroundContact );
	lua_setglobal( L, "phWheelIsGroundContact" );

	lua_pushcfunction( L, phWheelUpdate );
	lua_setglobal( L, "phWheelUpdate" );

	lua_pushcfunction( L, phRaycast );
	lua_setglobal( L, "phRaycast" );	


	//
	lua_pushcfunction( L, phSetBreakableJoint );
	lua_setglobal( L, "phSetBreakableJoint" );
	//
	lua_pushcfunction( L, phSetGlobalAxisAtJoint );
	lua_setglobal( L, "phSetGlobalAxisAtJoint" );
	//
	lua_pushcfunction( L, phSetGlobalAnchorAtJoint );
	lua_setglobal( L, "phSetGlobalAnchorAtJoint" );
	//
	lua_pushcfunction( L, phSetLimitPointAtJoint );
	lua_setglobal( L, "phSetLimitPointAtJoint" );
	//
	lua_pushcfunction( L, phAddLimitPlaneAtJoint );
	lua_setglobal( L, "phAddLimitPlaneAtJoint" );
	//
	lua_pushcfunction( L, phSetMotorAtRevoluteJoint );
	lua_setglobal( L, "phSetMotorAtRevoluteJoint" );
#endif
	#endif
	//------------------------------------alternative 3D-------------------------
	//Установка значения шейдера
	lua_pushcfunction(L, object_ChangeShaderUniform);
	lua_setglobal (L,"ChangeShaderUniform"); //+doc
	//Регистрация внутренней функции создания 3D объектов
	lua_pushcfunction(L, object_Load3DObject);
	lua_setglobal (L,"Add3DObject"); //+doc
	//Регистрация внутренней функции очистки загруженных в сцену данных (Load3DObject), кроме курсора и т.д.
	lua_pushcfunction(L, object_RReset);
	lua_setglobal (L,"RReset"); //+doc
	//Регистрация внутренней функции перемещения 3D объектов
	lua_pushcfunction(L, object_RSetPosition);
	lua_setglobal (L,"RSetPosition"); //+doc
	//Регистрация Функции совмещения точки привязки одного объекта с другим
	lua_pushcfunction(L, object_RSetAt);
	lua_setglobal (L,"RSetAt"); //+doc
	//Функция выборочного сброса матриц позиции/поворота/масштаба
	lua_pushcfunction(L, object_RResetObject);
	lua_setglobal (L,"RResetObject"); //+doc
	//Функция относительного поворота объекта
	lua_pushcfunction(L, object_RSetRotate);
	lua_setglobal (L,"RSetRotate"); //+doc
	//альтернативное вращение сложное перемещение  3D объекта
	lua_pushcfunction(L, object_RSetRotate2);
	lua_setglobal (L,"RSetRotate2"); //+doc
	//альтернативное перемещение относительно другого объекта по абсолютным координатам
	lua_pushcfunction(L, object_ASetAt);
	lua_setglobal (L,"ASetAt"); //+doc
	//альтернативное задание видимости объекта
	lua_pushcfunction(L, object_RSetVisible);
	lua_setglobal (L,"RSetVisible"); //+doc
	//альтернативное масштабирование объекта
	lua_pushcfunction(L, object_RSetScale);
	lua_setglobal (L,"RSetScale"); //+doc
	//альтернативная передача x,y,z объекта
	lua_pushcfunction(L, object_RGetPosition);
	lua_setglobal (L,"RGetPosition"); //+doc
	//альтернативная передача x,y,z объекта - абсолютные координаты
	lua_pushcfunction(L, object_AGetPosition);
	lua_setglobal (L,"AGetPosition"); //+doc
	//альтернативное получение масштаба объекта
	lua_pushcfunction(L, object_RGetScale);
	lua_setglobal (L,"RGetScale"); //+doc
	//альтернативное получение квантериона вращения объекта
	lua_pushcfunction(L, object_RGetRotate);
	lua_setglobal (L,"RGetRotate"); //+doc
	//--------------------------запросы-----------------------------------
	//Запрос предка объекта
	lua_pushcfunction(L, object_GetParent);
	lua_setglobal (L,"GetParent"); //+doc
	//-------------------------рисование----------------------------------
	//альтернативное рисование на текстуре объекта
	lua_pushcfunction(L, object_RTextureDraw);
	lua_setglobal (L,"RTextureDraw");  //+-doc + 1 arg
	//альтернативная замена текстуры  объекта
	lua_pushcfunction(L, object_RChangeTexture);
	lua_setglobal (L,"RChangeTexture"); //+-doc + 1 arg
	//установка цвета материала
	lua_pushcfunction(L, object_RSetColor);
	lua_setglobal (L,"RSetColor"); //+doc
	//------------------------alternative HIERARHION----------------------
	//Изменение иерархии объектов (изменение подчинения объектов))
	lua_pushcfunction(L, object_RConnectObjectTo);
	lua_setglobal (L,"RConnectObjectTo"); //+doc
	//-------------------------Тесты на пересеения------------------------
	//Тест на пересечение 2 объектов в относительных координатах
	lua_pushcfunction(L, object_TestIntersects);
	lua_setglobal (L,"RTestIntersects"); //+doc
        //Тест на пересечение 2 объектов в относительных координатах
        lua_pushcfunction(L, object_TestIntersects2);
        lua_setglobal (L,"RTestIntersects2"); //+doc
	//Тест на пересечение линией
	lua_pushcfunction(L, object_TestLineIntersects);
	lua_setglobal (L,"RTestLineIntersects"); //+doc
	//----------------------------шейдеры---------------------------------
	//Функция подключает шейдер к объекту
	lua_pushcfunction(L, object_SetShader);
	lua_setglobal (L,"SetShader");	 //+doc
	//------------------------------------alternative animations-------------------------
	//установка паузы на анимации/снятие с паузы
	lua_pushcfunction(L, object_SetPauseAnimation);
	lua_setglobal (L,"SetPauseAnimation"); //+doc
	//переход на заданное время анимации
	lua_pushcfunction(L, object_GotoAnimation);
	lua_setglobal (L,"GotoAnimation"); //+doc
	//Установка опорного смещения анимации
	lua_pushcfunction(L, object_SetPivotPointAnimation);
	lua_setglobal (L,"SetPivotPointAnimation"); //+doc
        //----------------------------------------------------------------------------
        lua_pushcfunction(L, CreateMorph);
        lua_setglobal (L, "CreateMorph");
        //
        lua_pushcfunction(L, GotoMorphState);
        lua_setglobal (L, "GotoMorphState");
	//-----------------------------------Тени--------------------------------------------
	//Создание отдельной группы теней, параметр - имя группы
	lua_pushcfunction(L, object_AddShadowedGroup);
	lua_setglobal (L,"AddShadowedGroup");
	//Определение маски Имя объекта, 4-ничего, 1-принимать, 2-бросать, 3 - и то и другое.
	lua_pushcfunction(L, object_SetShadowMask);
	lua_setglobal (L,"SetShadowMask");

        // RTI
	/*
        lua_pushcfunction( L, RTI_CreateFederate );
		lua_setglobal( L, "RTI_CreateFederate" );
		
        lua_pushcfunction( L, RTI_DestroyFederate );
        lua_setglobal( L, "RTI_DestroyFederate" );

        lua_pushcfunction( L, RTI_RegisterSynchronizationPoint );
        lua_setglobal( L, "RTI_RegisterSynchronizationPoint" );

        lua_pushcfunction( L, RTI_RunFederate );
        lua_setglobal( L, "RTI_RunFederate" );

        lua_pushcfunction( L, RTI_Publish );
        lua_setglobal( L, "RTI_Publish" );

        lua_pushcfunction( L, RTI_Subscribe );
        lua_setglobal( L, "RTI_Subscribe" );

        lua_pushcfunction( L, RTI_AddObject );
        lua_setglobal( L, "RTI_AddObject" );

        lua_pushcfunction( L, RTI_RemoveObject );
        lua_setglobal( L, "RTI_RemoveObject" );

        lua_pushcfunction( L, RTI_SetValue );
        lua_setglobal( L, "RTI_SetValue" );

        lua_pushcfunction( L, RTI_PublishInteraction );
        lua_setglobal( L, "RTI_PublishInteraction" );

        lua_pushcfunction( L, RTI_SubscribeInteraction );
        lua_setglobal( L, "RTI_SubscribeInteraction" );

        lua_pushcfunction( L, RTI_AddParameter );
        lua_setglobal( L, "RTI_AddParameter" );

        lua_pushcfunction( L, RTI_SendInteraction );
        lua_setglobal( L, "RTI_SendInteraction" );

        lua_pushcfunction( L, RTI_AddProxy );
        lua_setglobal( L, "RTI_AddProxy" );

        lua_pushcfunction( L, RTI_RemoveProxy );
        lua_setglobal( L, "RTI_RemoveProxy" );
		
		
		
	lua_pushcfunction(L,net_create);
	lua_setglobal(L,"net_create");
	*/

	//------------------------------------Text-------------------------------------------
	//Создание текстового объекта
	lua_pushcfunction(L, text_AddTextObject);
	lua_setglobal (L,"AddTextObject");  //+doc
	//Задание значения текстового поля
	lua_pushcfunction(L, text_SetText);
	lua_setglobal (L,"SetText");  //+doc
	//Задание цвета текста
	lua_pushcfunction(L, text_SetColor);
	lua_setglobal (L,"SetTextColor"); //+doc
	//Задание размера текста
	lua_pushcfunction(L, text_SetSize);
	lua_setglobal (L,"SetTextSize"); //+doc
	//Задание шрифта текста
	lua_pushcfunction(L, text_SetFont);
	lua_setglobal (L,"SetTextFont");  //+doc
	//задание равнения текста
	lua_pushcfunction(L, text_SetAlignment);
	lua_setglobal (L,"SetTextAlignment");  //+doc
	//задание ориентации текста
	lua_pushcfunction(L, text_SetLayout);
	lua_setglobal (L,"SetTextLayout"); //+doc
	//задание ориентации текста к экрану 
	lua_pushcfunction(L, text_SetAxisAlignment);
	lua_setglobal (L,"SetTextAxisAlignment"); //+doc
	//------------------------звук---------------------------------------
	//Регистрация внутренней функции создания звуковых объектов
	lua_pushcfunction(L, sound_NewSoundObject);
	lua_setglobal (L,"AddSoundObject");
	//Регистрация внутренней функции загрузки звуковых файлов
	lua_pushcfunction(L, sound_LoadSound);
	lua_setglobal (L,"LoadSound");
	//Регистрация внутренней функции проигрывания звукового файла
	lua_pushcfunction(L, sound_PlaySound);
	lua_setglobal (L,"PlaySound");
	//Регистрация внутренней функции паузы звукового файла
	lua_pushcfunction(L, sound_PauseSound);
	lua_setglobal (L,"PauseSound");
	//Регистрация внутренней функции задания режима LOOP звукового файла
	lua_pushcfunction(L, sound_LoopSound);
	lua_setglobal (L,"LoopSound");
	//Регистрация внутренней функции задания позиции
	lua_pushcfunction(L, sound_PositionSound);
	lua_setglobal (L,"PositionSound");
	//Регистрация функции получить состояние объекта (играет, на паузе, воспроизведение закончено и т.д)
	lua_pushcfunction(L, sound_GetState);
	lua_setglobal (L,"GetSoundState");
	//Регистрация функции, которая создает геометрический объект, для того чтобы видеть источник звука
	lua_pushcfunction(L, sound_ShowDebug);
	lua_setglobal (L,"ShowSoundDebug");
		//Регистрация функции, установить громкость
		lua_pushcfunction(L, sound_SetGain);
		lua_setglobal (L,"SetSoundGain");
		//Регистрация функции, установить Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
		lua_pushcfunction(L, sound_SetMaxDistance);
		lua_setglobal (L,"SetSoundMaxDistance");
		//Регистрация функции, установить Расстояние, на котором сила звука уменьшается вдвое (до воздействия параметра rolloff
		lua_pushcfunction(L, sound_SetReferenceDistance);
		lua_setglobal (L,"SetSoundReferenceDistance");
		//Регистрация функции, установить Минимальное/максимальное значение громкости
		lua_pushcfunction(L, sound_SetMinMaxGain);
		lua_setglobal (L,"SetSoundMinMaxGain");
		//"перемотка звука на нужную секунду" [0,any]
		lua_pushcfunction(L, sound_SetOFFSET);
		lua_setglobal (L,"SetSoundOFFSET");
		//Коэфффициент кривизны затухания звука (rolloff фактор)
		lua_pushcfunction(L, sound_SetRolloffFactor);
		lua_setglobal (L,"SetSoundRolloffFactor");
		//установка приоритета звука [0..1]
		lua_pushcfunction(L, sound_SetPriority);
		lua_setglobal (L,"SetSoundPriority");

		//Регистрация функции установить Pitch (0..10)
	lua_pushcfunction(L, sound_SetPitch);
	lua_setglobal (L,"SetSoundPitch");
	//Регистрация внутренней функции добавить эффект Reverb
	lua_pushcfunction(L, sound_AddReverb);
	lua_setglobal (L,"AddReverbSound");

	//--------------------------------персонажи---------------------------
	//Регистрация внутренней функции для загрузки и создания персонажа
	lua_pushcfunction(L, character_Load);
	lua_setglobal (L,"LoadCharacter");
	//Регистрация внутренней функции установки анимации персонажа
	lua_pushcfunction(L, character_SetAction);
	lua_setglobal (L,"SetCharacterAction");
	//Регистрация внутренней функции установки позиции персонажа
	lua_pushcfunction(L, character_SetPosition);
	lua_setglobal (L,"SetCharacterPosition");
	//Регистрация внутренней функции установки места-назначения персонажа
	lua_pushcfunction(L, character_GoTo);
	lua_setglobal (L,"SetCharacterGoTo");
	//Прикрепляет объект к кости
	lua_pushcfunction(L, character_ConnectObjectToBone);
	lua_setglobal (L,"ConnectObjectToBone");


	//-----------------------------------Skin Animation
	lua_pushcfunction(L, LoadSkinAnimation);
	lua_setglobal (L,"LoadSkinAnimation");
	lua_pushcfunction(L, PlaySkinAnimation);
	lua_setglobal (L,"PlaySkinAnimation");
	lua_pushcfunction(L, StopSkinAnimation);
	lua_setglobal (L,"StopSkinAnimation");
	lua_pushcfunction(L, GotoTimeSkinAnimation);
	lua_setglobal (L,"GotoTimeSkinAnimation");
	lua_pushcfunction(L, RotateSkinBone);
	lua_setglobal (L,"RotateSkinBone");

	//-----------------------------------свет-----------------------------
	//Регистрация внутренней функции устанавки Diffuse-цвет указанного светового источника
	lua_pushcfunction(L, SetLightColor);
	lua_setglobal (L,"SetLightColor"); //+doc
	//включает/выключает свет
	lua_pushcfunction(L, SetLightEnable);
	lua_setglobal (L,"SetLightEnable"); //+doc
	//Включает/выключает учет освещения объектом
	//изменять атрибуты, например "объект освещается или нет" (stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);)
	lua_pushcfunction(L, SetLightingEnable);
	lua_setglobal (L,"SetLightingEnable"); //+doc
	//---------------------------------clone/del----------------------------------
	//клонирует объект-оригинал и цепляет его к предку оригинала (т.е. у них общий родитель сохраняется)
	lua_pushcfunction(L, object_Clone);
	lua_setglobal (L,"Clone"); //+doc
	//Удаление объекта из сцены
	lua_pushcfunction(L, object_UnloadObject);
	lua_setglobal (L,"UnloadObject"); //+doc
	//------------------------------------------------------------------------------

	lua_pushcfunction(L, object_CreateVideoPlane);
	lua_setglobal (L,"CreateVideoPlane"); //+doc
	//1. Играть
	lua_pushcfunction(L, object_PlayVideo);
	lua_setglobal (L,"PlayVideo"); //+doc
	//2. Пауза
	lua_pushcfunction(L, object_PauseVideo);
	lua_setglobal (L,"PauseVideo"); //+doc
	//3. Длинна файла в секундах
	lua_pushcfunction(L, object_getLengthVideo);
	lua_setglobal (L,"GetLengthVideo"); //+doc
	//4. Перемотать на секунду ...
	lua_pushcfunction(L, object_setReferenceTimeVideo);
	lua_setglobal (L,"SetTimeVideo"); //+doc
	//5. Позиция в секундах..
	lua_pushcfunction(L, object_getReferenceTimeVideo);
	lua_setglobal (L,"GetTimeVideo"); //+doc
	//6. Установить громкость
	lua_pushcfunction(L, object_setVolumeVideo);
	lua_setglobal (L,"SetVolumeVideo"); //+doc
	//7. Узнать громкость
	lua_pushcfunction(L, object_getVolumeVideo);
	lua_setglobal (L,"GetVolumeVideo"); //+doc
	//8. Режим повтора
	lua_pushcfunction(L, object_setLoopingModeVideo);
	lua_setglobal (L,"SetLoopingVideo"); //+doc
	//-------------------------------------вершины-------------------------------------
	//передвигает одну вершину (i) на dx,dy,dz
	lua_pushcfunction(L, object_RSetVertexPosition);
	lua_setglobal (L,"RSetVertexPosition"); //
	//передвигает одну координату текстуры (i) на dx,dy
	lua_pushcfunction(L, object_RSetTexturePointPosition);
	lua_setglobal (L,"RSetTexturePointPosition"); //
	//передвигает всю текстуру  на dx,dy
	lua_pushcfunction(L, object_RSetTexturePosition);
	lua_setglobal (L,"RSetTexturePosition"); //

	//------------------------------частицы--------------------------------------------
	//Создание частиц
	lua_pushcfunction(L, particle_Add);
	lua_setglobal (L,"AddParticle");
	//время жизни частицы
	lua_pushcfunction(L, particle_ParticleSetLifeTime);
	lua_setglobal (L,"ParticleSetLifeTime");
	//разброс размеров
	lua_pushcfunction(L, particle_ParticleSetSizeRange);
	lua_setglobal (L,"ParticleSetSizeRange");
	//разброс прозрачности
	lua_pushcfunction(L, particle_ParticleSetAlphaRange);
	lua_setglobal (L,"ParticleSetAlphaRange");
	//разброс цвета
	lua_pushcfunction(L, particle_ParticleSetColorRange);
	lua_setglobal (L,"ParticleSetColorRange");
	//первоначальный радиус
	lua_pushcfunction(L, particle_ParticleSetRadius);
	lua_setglobal (L,"ParticleSetRadius");
	//масса
	lua_pushcfunction(L, particle_ParticleSetMass);
	lua_setglobal (L,"ParticleSetMass");
	//угол поворота проскостей
	lua_pushcfunction(L, particle_ParticleSetAngle);
	lua_setglobal (L,"ParticleSetAngle");
	//текструра,  emissive? , освещаются или нет, текстурный юнит=0
	lua_pushcfunction(L, particle_ParticleSetDefaultAttributes);
	lua_setglobal (L,"ParticleSetDefaultAttributes");
	//Разброс количества генерируемых частиц в секунду
	lua_pushcfunction(L, particle_ParticleSetRateRange);
	lua_setglobal (L,"ParticleSetRateRange");
	//Указание углов разброса по двум плоскастям (замечание Ассель)
	lua_pushcfunction(L, particle_ParticleSetEmitterAngle);
	lua_setglobal (L,"ParticleSetEmitterAngle");
	//Указание радиуса сегмента, из которого появляются частицы (замечание Ассель)
	lua_pushcfunction(L, particle_ParticleSetEmitterRadius);
	lua_setglobal (L,"ParticleSetEmitterRadius");

	lua_pushcfunction(L, particle_ParticleSetSpeed);
	lua_setglobal (L,"ParticleSetSpeed");

	
	//----------------------------------------------------------------------
	lua_pushcfunction(L, SetVolumeTexture);
	lua_setglobal (L,"SetVolumeTexture");


	//-----------------------
	//строит BoundingBox
	lua_pushcfunction(L, object_Select);
	lua_setglobal (L,"SelectObject"); //

	//строит BoundingBox
	lua_pushcfunction(L, object_GetBoundingBox);
	lua_setglobal (L,"GetBoundingBox"); //

	//----------------------------------функции, которые поддерживаются-----------
	//Функция для создания 3D объектов
	lua_pushcfunction(L, player_New3DObject);
	lua_setglobal (L,"NewOld3DObject");
	//Функция задает координаты объекта
	lua_pushcfunction(L, object_SetPosition);
	lua_setglobal (L,"SetPosition");
	//Функция задает простое вращение объекта 3D объекта
	lua_pushcfunction(L, object_SetRotate);
	lua_setglobal (L,"SetRotate");
	//Функция задает сложное перемещение  3D объекта
	lua_pushcfunction(L, object_SetRotate2);
	lua_setglobal (L,"SetRotate2");
	//Функция задает видимость объекта
	lua_pushcfunction(L, object_SetVisible);
	lua_setglobal (L,"SetVisible");
	//Функция задает масштабирование объекта
	lua_pushcfunction(L, object_SetScale);
	lua_setglobal (L,"SetScale");
	//Функция задает координаты точки привязки объекта, для дальнейшей анимации
	lua_pushcfunction(L, object_SetCenterPosition);
	lua_setglobal (L,"SetCenterPosition");
	//
	lua_pushcfunction(L, object_RChangeTexture);
	lua_setglobal (L,"ChangeTexture");
	lua_pushcfunction(L, object_RSetPosition);
	lua_setglobal (L,"SetTextPosition");
	lua_pushcfunction(L, object_RSetVisible);
	lua_setglobal (L,"SetTextVisible");
	lua_pushcfunction(L, object_RSetRotate2);
	lua_setglobal (L,"SetTextRotate");
	lua_pushcfunction(L, object_RSetRotate2);
	lua_setglobal (L,"SetTextRotate2");
}

//интерпретация комманд в LUA (для плеера)
//ВЕРСИЯ ДЛЯ ПЛЕЕРА! ВСЕ ИЗМЕНЕНИЯ ДЛЯ ПЛАГИНА ДУБЛИРОВАТЬ В PlayerWidget::Interpretation(InternalEventsClass events)
void Interpretation(InternalEventsClass events)
{
	try
	{
		for (unsigned int i=0;i<events.getEvents().size();i++)
		{
			std::string COMMAND = events.getEvents().at(i).Event;
			//std::cout << "to lua: " << COMMAND;
			if (IsLuaObject(COMMAND))
			{
				//std::cout << " ok" << std::endl;
				lua_getglobal(L,COMMAND.c_str());
				//
				int arg_cound=0;
				if (events.getEvents().at(i).number_param.size()!=0)
				{
					for (unsigned int a=0; a< events.getEvents().at(i).number_param.size();a++)
					{
						lua_pushnumber( L, events.getEvents().at(i).number_param.at(a));
					}
					arg_cound = events.getEvents().at(i).number_param.size();
				}
				if (lua_pcall(L,arg_cound,0,0))
				{
					std::cout << "Error run '" << COMMAND << "' script" << std::endl;
					std::cout << lua_tostring(L,-1) << std::endl;
					lua_pop(L,1);
					throw (ErrorClass (__FILE__, __LINE__ , "Interpretation()", "Error run '" + COMMAND + "' script" , 3));
				}
			}
			else
			{
				//std::cout << " not find" << std::endl;
			}
		}
		events.getEvents().clear();
	}
	catch (ErrorClass error)
	{
		error.print();
	}
}

//------------------------- функция для для рабты с потоком------
static int SerialPortInit(lua_State *L)
{
	std::string port = lua_tostring(L,1);  // Имя COM порта
    //если имя порта не указано (""), используем заданный порт в настройках
	if (port == "")
	{
		port = configuration->GetRecordString("#ArduinoPort");
	}

    Serial* serial;
    //если указана скорость, то....
    unsigned int portBoundrate =0;
    if (lua_type(L,2)!=LUA_TNONE)
    {
        portBoundrate = lua_tonumber(L,2);
        serial = new Serial(port.c_str(), portBoundrate);
    }
    else
    {
        serial = new Serial(port.c_str());
    }

    //результат....
	if (serial->IsConnected() == true)
	{
		lua_pushstring(L, port.c_str());
	}
	else
	{
		lua_pushstring(L, "");

		throw (ErrorClass (__FILE__, __LINE__ , "SerialPortInit", "Error opening COM port " + port, 3));
	}
	delete serial;
	return 1;
}



//-------------------------функция для определения "директории пользователя" HOME/HOMEPATH------
static int GetUserHomeDir (lua_State *L)
{
    std::string _homedir = _GetUserHomePath();
	lua_pushstring(L, _homedir.c_str() );
	return 1;
}
//
static int GetUserTempDir (lua_State *L)
{
        std::string _tempdir = _GetUserTempDir();
        lua_pushstring(L, _tempdir.c_str() );
        return 1;
}



//-------------------------SCORM--------------------------------------------------------------
//-------------------------Выставление оценки точности (правильности) выполнения заданий
static int ScormSetScore(lua_State *L)
{
    float min = lua_tonumber(L,1);  //граница - минимальная оценка
    float max = lua_tonumber(L,2);  //граница - максимальная оценка
    float raw = lua_tonumber(L,3);  //абсолютная оценка (ненормализованная)
    SCORM->SetValue("cmi.score.min", SCORM->floatToStr (min));
    SCORM->SetValue("cmi.score.max", SCORM->floatToStr (max));
    SCORM->SetValue("cmi.score.raw", SCORM->floatToStr (raw));
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}
//-------------------------Выставление оценки объема (количества) выполненных заданий
static int ScormSetProgress(lua_State *L)
{
    float progress_measure = lua_tonumber(L,1);     //количество
    SCORM->SetValue("cmi.progress_measure", SCORM->floatToStr (progress_measure));
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}
//-------------------------Возврат значения из LMS по имени поля (например cmi.learner_name)
static int ScormGetValue(lua_State *L)
{
    std::string key( lua_tostring( L, 1 ) );
    std::string value = SCORM->GetValue(key);
    //возврат значения
    lua_pushstring(L, value.c_str() );
    return 1;
}
//------------------------Запись сырых данных типа ключ\значение
static int ScormSetValue(lua_State *L)
{
    std::string key  ( lua_tostring( L, 1 ) );
    std::string value( lua_tostring( L, 2 ) );
    SCORM->SetValue (key, value);
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}


//----------------------TIN CAN----------------------------
//-------------------------создание JSON записи
static int TinCanAddRecord(lua_State *L)
{
    std::string record  ( lua_tostring( L, 1 ) );
	std::string type  ( lua_tostring( L, 2 ) );
    TINCAN->AddTinCanRecord (record, type);
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}
//-------------------------отправка всех записей в LRS
static int TinCanPostToLRS(lua_State *L)
{
	std::string host   ( lua_tostring( L, 1 ) );
	std::string login  ( lua_tostring( L, 2 ) );
	std::string pass   ( lua_tostring( L, 3 ) );

    TINCAN->PostToLRS (host, login, pass);
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}
//------------------------отправка всех записей в файл
static int TinCanPostToFile(lua_State *L)
{
	std::string filename  ( lua_tostring( L, 1 ) );
    TINCAN->PostToFile (filename);
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}


//----------------------------------------тест-------------------------------------------------------
//Тест
static int test_createCustomGeometry(lua_State *L)
{
	scene->TESTTEST();
	//возврат значения
	lua_pushnumber(L,333);
	return 1;
}


//Полное кеширование сцены (уже после подключения данных к Viewer'у)
static int FullCompileScene(lua_State *L)
{
    bool full = lua_toboolean( L, 1 );
    localUser->FullCompileScene(full);

	//возврат значения
	lua_pushnumber(L,333);
	return 1;
}

//----------------------------------------определение функции установки пользователя-----------------
//Функция установки нового положения пользователя в пространстве и уго направления взгляда
static int player_SetUserPositionAndRotation(lua_State *L)
{
	//получаем необходимые параметры
	float X = lua_tonumber(L,1);
	float Y = lua_tonumber(L,2);
	float Z = lua_tonumber(L,3);
	float HEADX = lua_tonumber(L,4);
	float HEADY = lua_tonumber(L,5);
	float HEADZ = lua_tonumber(L,6);
	//позиция наблюдателя
	localUser->SetUserPositionAndOrientation (X,Y,Z,HEADY,HEADX,HEADZ);
	/*
	//СТАРТОВЫЕ КООРДИНАТЫ
	*/
	//возврат значения
	lua_pushnumber(L,333);
	return 1;
}

//Функция установки флага заморозки передвижения пользователя и/или заморозки взгляда
static int player_SetUserHold(lua_State *L)
{
	//получаем необходимые параметры
	bool holdstep = !lua_toboolean(L,1); //Блокировать передвигаться
	bool holdeye = !lua_toboolean(L,2);  //Блокировать изменять направление взгляда
	bool holdcommand = !lua_toboolean(L,3);  //Блокировать переходить в режим управления
	//Выставляем флаги
	//1 - просмотр 2- управление 3- перемещение
	localUser->setInputMode (holdeye,holdcommand,holdstep);
	//возврат значения
	lua_pushnumber(L,333);
	return 1;
}

static int player_SetFocus(lua_State *L)
{
	//получаем необходимые параметры
	float X = lua_tonumber(L,1);

	//позиция наблюдателя
	localUser->setUserFocusLight(X);
	//возврат значения
	lua_pushnumber(L,333);
	return 1;
}

//----------------------текст рядом с курсором--------------------------
static int player_SetCursorText(lua_State *L)
{
    std::string strText( lua_tostring( L, 1 ) );
    //установка нового значения текста
    localUser->SetCursorText(strText);
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}
//
static int player_SetCursorTextSize(lua_State *L)
{
    float newsize = lua_tonumber(L,1);
    //
    localUser->SetCursorTextSize(newsize);
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}
//
static int player_SetCursorTextFont(lua_State *L)
{
    std::string newfont( lua_tostring( L, 1 ) );
    //
    localUser->SetCursorTextFont(newfont);
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}
//
static int player_SetCursorTextColor(lua_State *L)
{
    float r = lua_tonumber(L,1);
    float g = lua_tonumber(L,2);
    float b = lua_tonumber(L,3);
    float a = lua_tonumber(L,4);
    //установка нового значения текста
    localUser->SetCursorTextColor(r,g,b,a);
    //возврат значения
    lua_pushnumber(L,333);
    return 1;
}

//----------------------------JOYSTICK-----------------------------------------
//функция возвращает число джойстиков
static int joystick_JoyGetNum(lua_State *L)
{
	//возврат значения
	lua_pushnumber(L,localUser->GetJoysticksData().JoysticksNum);
	return 1;
}
//функция возвращает состояние указанной кнопки указанного джойстика
static int joystick_JoyGetButtonsState(lua_State *L)
{
	const int JoystickNum = lua_tonumber(L,1);
	const int ButtonNum = lua_tonumber(L,2);
	bool buttonState = localUser->GetJoysticksData().getButtonState (JoystickNum, ButtonNum);
	if (buttonState==false)
	{
		lua_pushboolean(L,0);
	}
	else
	{
		lua_pushboolean(L,1);
	}
	lua_pushnumber(L,333);
	return 2;
}
//функция возвращает состояние указанного "Hats"а указанного джойстика
static int joystick_JoyGetHatsState(lua_State *L)
{
	int JoystickNum = lua_tonumber(L,1);
	int HatNum = lua_tonumber(L,2);
	std::string  HatState = localUser->GetJoysticksData().getHatState (JoystickNum, HatNum);
	lua_pushstring(L, HatState.c_str() );
	lua_pushnumber(L,333);
	return 2;
}
//функция возвращает состояние указанной оси вращения указанного джойстика
static int joystick_JoyGetAxisState(lua_State *L)
{
	int JoystickNum = lua_tonumber(L,1);
	int HatNum = lua_tonumber(L,2);
	int  AxesState = localUser->GetJoysticksData().getAxesState (JoystickNum, HatNum);
	lua_pushnumber(L,AxesState);
	lua_pushnumber(L,333);
	return 2;
}

//---------------------------------------Физика ----------------------------------
// --------------------------------------------------------------------------------------------
#ifdef PHYSX
#if (_MSC_VER < 1700)
int phAddStaticPlane( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );

	std::string strName( lua_tostring( pLuaState, 1 ) );
	NxVec3 vNormal( lua_tonumber( pLuaState, 2 ), lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 3 ) );
	float fD = lua_tonumber( pLuaState, 5 );

	scene->physic()->addStaticPlane( strName, vNormal, fD );
	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phAddRigidBody( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );

	std::string strName( lua_tostring( pLuaState, 1 ) );

	// режим аппроксимации
	std::string strMode = lua_tostring( pLuaState, 2 );
	std::transform( strMode.begin(), strMode.end(), strMode.begin(), ::tolower );

	if( "mesh" == strMode )
	{
		scene->physic()->addStaticMesh( strName );
	}
	else if( "decomposition" == strMode )
	{
		std::string strRenderName( lua_tostring( pLuaState, 3 ) );
		float fMassa = lua_tonumber( pLuaState, 4 );
		int iDepth = lua_tointeger( pLuaState, 5 );
		scene->physic()->addDecompositionBody( strName, strRenderName, fMassa, iDepth );
	}
	else if( "sphere" == strMode )
	{
		float fMassa = lua_tonumber( pLuaState, 3 );
		float fRadius = lua_tonumber( pLuaState, 4 );
		scene->physic()->addSphereBody( strName, fMassa, fRadius );
	}
	else if( "box" == strMode )
	{
		assert( numParams == 6 );
		float fMassa = lua_tonumber( pLuaState, 3 );

		NxVec3 vDimension( lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 6 ), lua_tonumber( pLuaState, 5 ) );
		scene->physic()->addBoxBody( strName, fMassa, vDimension );
	}
	else if( "plane" == strMode )
	{
		assert( numParams == 6 );
		NxVec3 vNormal( lua_tonumber( pLuaState, 3 ), lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 4 ) );
		float fD = lua_tonumber( pLuaState, 6 );

		scene->physic()->addStaticPlane( strName, vNormal, fD );
	}

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phSetPosition( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( 4 == numParams );

	std::string strName( lua_tostring( pLuaState, 1 ) );

	NxVec3 vPos;
	vPos.x = lua_tonumber( pLuaState, 2 );
	vPos.y = lua_tonumber( pLuaState, 4 );
	vPos.z = lua_tonumber( pLuaState, 3 );

	scene->physic()->setPosition( strName, vPos );

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

static int phSetShapeLocalPosition( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( 4 == numParams );

	std::string strName( lua_tostring( pLuaState, 1 ) );

	NxVec3 vPos;
	vPos.x = lua_tonumber( pLuaState, 2 );
	vPos.y = lua_tonumber( pLuaState, 4 );
	vPos.z = lua_tonumber( pLuaState, 3 );

	scene->physic()->setShapeLocalPosition( strName, vPos );

	lua_pushnumber( pLuaState, 1 );
	return 1;
}




void parseToPhysicForceDesc( lua_State* pLuaState, sPhysicForceDesc* pOut )
{
	// name
	assert( 6 == lua_gettop( pLuaState ) );
	pOut->strName.assign( lua_tostring( pLuaState, 1 ) );

	// mode
	pOut->mode = cPhysicObject::e_FORCE;
	std::string strMode( lua_tostring( pLuaState, 2 ) );
	if( "force" == strMode )
	{
		pOut->mode = cPhysicObject::e_FORCE;
	}
	else if( "impulse" == strMode )
	{
		pOut->mode = cPhysicObject::e_IMPULSE;
	}

	// local or global?
	std::string strCoordSystem( lua_tostring( pLuaState, 3 ) );
	if( "global" == strCoordSystem )
	{
		pOut->bGlobal = true;
	}
	else if( "local" == strCoordSystem )
	{
		pOut->bGlobal = false;
	}

	// vector force or torque
	pOut->vForce.set( lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 6 ), lua_tonumber( pLuaState, 5 ) );
}

int phAddForce( lua_State* pLuaState )
{
	sPhysicForceDesc desc;
	parseToPhysicForceDesc( pLuaState, &desc );

	if( desc.bGlobal )
	{
		scene->physic()->addForce( desc.strName, desc.vForce, desc.mode );
	}
	else
	{
		scene->physic()->addLocalForce( desc.strName, desc.vForce, desc.mode );
	}

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phAddTorque( lua_State* pLuaState )
{
	sPhysicForceDesc desc;
	parseToPhysicForceDesc( pLuaState, &desc );

	if( desc.bGlobal )
	{
		assert( false && "not available!!!" );
		//scene->physic()->addTorque( desc.strName, desc.vForce, desc.mode );
	}
	else
	{
		scene->physic()->addLocalTorque( desc.strName, desc.vForce, desc.mode );
	}

	//lua_pushnumber( pLuaState, 1 );
	return 0;
}

static NxCombineMode parse_combine_mode( const char* sz_value )
{
	if( _strcmpi( sz_value, "average" ) == 0 )
		return NX_CM_AVERAGE;
	else if( _strcmpi( sz_value, "min" ) == 0 )
		return NX_CM_MIN;
	else if( _strcmpi( sz_value, "multiply" ) == 0 )
		return NX_CM_MULTIPLY;
	else if( _strcmpi( sz_value, "max" ) == 0 )
		return NX_CM_MAX;
	else if( _strcmpi( sz_value, "n_values" ) == 0 )
		return NX_CM_N_VALUES;

	std::cout << "error parse combine mode" << std::endl;
	return NX_CM_AVERAGE;	
}

static NxVec3 parse_vector3( lua_State* pLuaState )
{
	NxVec3 v( 0.f, 0.f, 0.f );
	lua_pushnil( pLuaState );
	while( lua_next( pLuaState, -2 ) != 0 )
	{
		const char* sz_type = lua_tostring( pLuaState, -2 );
		if( _strcmpi( sz_type, "x" ) == 0 )
			v.x = lua_tonumber( pLuaState, -1 );
		else if( _strcmpi( sz_type, "y" ) == 0 )
			v.y = lua_tonumber( pLuaState, -1 );
		else if( _strcmpi( sz_type, "z" ) == 0 )
			v.z = lua_tonumber( pLuaState, -1 );

		lua_pop( pLuaState, 1 );
	}
	return v;
}

static NxQuat parse_quat( lua_State* pLuaState )
{
	NxQuat q;
	lua_pushnil( pLuaState );
	while( lua_next( pLuaState, -2 ) != 0 )
	{
		const char* sz_type = lua_tostring( pLuaState, -2 );
		if( _strcmpi( sz_type, "x" ) == 0 )
			q.x = lua_tonumber( pLuaState, -1 );
		else if( _strcmpi( sz_type, "y" ) == 0 )
			q.y = lua_tonumber( pLuaState, -1 );
		else if( _strcmpi( sz_type, "z" ) == 0 )
			q.z = lua_tonumber( pLuaState, -1 );
		else if( _strcmpi( sz_type, "w" ) == 0 )
			q.w = lua_tonumber( pLuaState, -1 );

		lua_pop( pLuaState, 1 );
	}
	return q;
}

int phActorWakeUp( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	float wake_counter = lua_tonumber( pLuaState, 2 );
	scene->physic()->actor_wakeup( str_actor_name, wake_counter );
	return 0;
}

int phActorSetCMassOffsetLocalPosition( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	NxVec3 v_cmass_offset( lua_tonumber( pLuaState, 2 ), lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 3 ) );
	scene->physic()->physic_object( str_actor_name )->actor()->setCMassOffsetLocalPosition( v_cmass_offset );
	return 0;
}

int phMaterialCreate( lua_State* pLuaState )
{
	bool b_table = lua_istable( pLuaState, 1 );
	if( b_table )
	{
		NxMaterialDesc material_desc;

		lua_pushnil( pLuaState );  /* first key */
		while( lua_next( pLuaState, -2 ) != 0 )
		{
			if( lua_isstring( pLuaState, -2 ) )
			{
				const char* sz_key_name = lua_tostring( pLuaState, -2 );
				int value_type = lua_type( pLuaState, -1 );
				if( LUA_TSTRING == value_type )
				{
					const char* sz_value = lua_tostring( pLuaState, -1 );
					if( _strcmpi( sz_key_name, "friction_combine_mode" ) == 0 )
					{
						material_desc.frictionCombineMode = parse_combine_mode( sz_value );
					}
					else if( _strcmpi( sz_key_name, "restitution_combine_mode" ) == 0 )
					{
						material_desc.restitutionCombineMode = parse_combine_mode( sz_value );
					}

				}
				else if( LUA_TNUMBER == value_type )
				{
					float f_value = lua_tonumber( pLuaState, -1 );
					if( _strcmpi( sz_key_name, "dynamic_friction" ) == 0 )
						material_desc.dynamicFriction = f_value;
					else if( _strcmpi( sz_key_name, "static_friction" ) == 0 )
						material_desc.staticFriction = f_value;
					else if( _strcmpi( sz_key_name, "restitution" ) == 0 )
						material_desc.restitution = f_value;
					else if( _strcmpi( sz_key_name, "dynamic_friction_v" ) == 0 )
						material_desc.dynamicFrictionV = f_value;
					else if( _strcmpi( sz_key_name, "static_friction_v" ) == 0 )
						material_desc.staticFrictionV = f_value;
				}
				else if( LUA_TBOOLEAN == value_type )
				{
					if( lua_toboolean( pLuaState, -1 ) )
					{
						if( _strcmpi( sz_key_name, "b_anisotropic" ) == 0 )
							material_desc.flags |= NX_MF_ANISOTROPIC;
						else if( _strcmpi( sz_key_name, "b_disable_friction" ) == 0 )
							material_desc.flags |= NX_MF_DISABLE_FRICTION;
						if( _strcmpi( sz_key_name, "b_disable_strong_friction" ) == 0 )
							material_desc.flags |= NX_MF_DISABLE_STRONG_FRICTION;
					}
				}
				else if( LUA_TTABLE == value_type )
				{
					if( _strcmpi( sz_key_name, "dir_of_anisotropy" ) == 0 )
						material_desc.dirOfAnisotropy = parse_vector3( pLuaState );
				}
			}

			/* removes 'value'; keeps 'key' for next iteration */
			lua_pop( pLuaState, 1 );
		}
		

		int material_index = scene->physic()->create_material( material_desc );
		lua_pushnumber( pLuaState, material_index );
		return 1;
	}
	else
	{
		std::cout << "must be a lua table" << std::endl;
		lua_pushnumber( pLuaState, 0 );
		return 1;
	}
}

int phMaterialDestroy( lua_State* pLuaState )
{
	int material_index = lua_tointeger( pLuaState, 1 );
	scene->physic()->destroy_material( material_index );
	//lua_pushnumber( pLuaState, 1 );
	return 0;
}

int phActorGetNumShapes( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	unsigned int num_shapes = scene->physic()->actor_get_num_shapes( str_actor_name );
	lua_pushnumber( pLuaState, num_shapes );
	return 1;
}

int phActorSetMaterialForShape( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	unsigned int index_shape = lua_tointeger( pLuaState, 2 );
	int index_material = lua_tointeger( pLuaState, 3 );
	scene->physic()->actor_set_material_for_shape( str_actor_name, index_shape, index_material );
	//lua_pushnumber( pLuaState, 1 );
	return 0;
}

int phActorSetLinearVelocity( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	NxVec3 vVelocity( lua_tonumber( pLuaState, 2 ), lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 3 ) );
	scene->physic()->actor_set_linear_velocity( str_actor_name, vVelocity );
	//lua_pushnumber( pLuaState, 1 );
	return 0;
}

int phActorSetAngularVelocity( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	NxVec3 vVelocity( lua_tonumber( pLuaState, 2 ), lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 3 ) );
	scene->physic()->actor_set_angular_velocity( str_actor_name, vVelocity );
	//lua_pushnumber( pLuaState, 1 );
	return 0;
}

int phActorGetLinearVelocity( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	NxVec3 v_relative_velocity = scene->physic()->actor_get_linear_velocity( str_actor_name );
	lua_pushnumber( pLuaState, v_relative_velocity.x );
	lua_pushnumber( pLuaState, v_relative_velocity.z );
	lua_pushnumber( pLuaState, v_relative_velocity.y );
	return 3;
}

int phActorGetGlobalOrientationQuat( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	NxQuat q_rotate = scene->physic()->physic_object( str_actor_name )->actor()->getGlobalOrientationQuat();
	lua_pushnumber( pLuaState, q_rotate.x );
	lua_pushnumber( pLuaState, q_rotate.y );
	lua_pushnumber( pLuaState, q_rotate.z );
	lua_pushnumber( pLuaState, q_rotate.w );
	return 4;
}

int phActorSetGlobalOrientationQuat( lua_State* pLuaState )
{
	std::string str_actor_name = lua_tostring( pLuaState, 1 );
	NxQuat q_rotate = parse_quat( pLuaState );
	scene->physic()->physic_object( str_actor_name )->actor()->setGlobalOrientationQuat( q_rotate );
	return 0;
}

int phWheelCreate( lua_State* pLuaState )
{
	std::string str_car_body_name( lua_tostring( pLuaState, 1 ) );
	//lua_pop( pLuaState, 1 );

	bool b_table = lua_istable( pLuaState, 2 );
	if( b_table )
	{
		cWheelDesc wheel_desc;

		lua_pushnil( pLuaState );  /* first key */
		while( lua_next( pLuaState, -2 ) != 0 )
		{
			if( lua_isstring( pLuaState, -2 ) )
			{
				const char* sz_key_name = lua_tostring( pLuaState, -2 );
				int value_type = lua_type( pLuaState, -1 );
				if( LUA_TNUMBER == value_type )
				{
					float f_value = lua_tonumber( pLuaState, -1 );
					if( _strcmpi( sz_key_name, "wheel_approximation" ) == 0 )
						wheel_desc.wheelApproximation = (NxU32)f_value;
					else if( _strcmpi( sz_key_name, "wheel_radius" ) == 0 )
						wheel_desc.wheelRadius = f_value;
					else if( _strcmpi( sz_key_name, "wheel_width" ) == 0 )
						wheel_desc.wheelWidth = f_value;
					else if( _strcmpi( sz_key_name, "wheel_suspension" ) == 0 )
						wheel_desc.wheelSuspension = f_value;
					else if( _strcmpi( sz_key_name, "spring_restitution" ) == 0 )
						wheel_desc.springRestitution = f_value;
					else if( _strcmpi( sz_key_name, "spring_damping" ) == 0 )
						wheel_desc.springDamping = f_value;
					else if( _strcmpi( sz_key_name, "spring_bias" ) == 0 )
						wheel_desc.springBias = f_value;
					else if( _strcmpi( sz_key_name, "max_brake_force" ) == 0 )
						wheel_desc.maxBrakeForce = f_value;
					else if( _strcmpi( sz_key_name, "friction_to_front" ) == 0 )
						wheel_desc.frictionToFront = f_value;
					else if( _strcmpi( sz_key_name, "friction_to_side" ) == 0 )
						wheel_desc.frictionToSide = f_value;
					else if( _strcmpi( sz_key_name, "inverse_mass" ) == 0 )
						wheel_desc.inverseWheelMass = f_value;
				}
				else if( LUA_TSTRING == value_type )
				{
					const char* sz_value = lua_tostring( pLuaState, -1 );
					if( _strcmpi( sz_key_name, "name" ) == 0 )
						wheel_desc.str_wheel_name.assign( sz_value );
				}
				else if( LUA_TTABLE == value_type )
				{
					if( _strcmpi( sz_key_name, "position" ) == 0 )
						wheel_desc.position = parse_vector3( pLuaState );
					else if( _strcmpi( sz_key_name, "flags" ) == 0 )
					{
						lua_pushnil( pLuaState );
						while( lua_next( pLuaState, -2 ) != 0 )
						{
							const char* sz_flag_name = lua_tostring( pLuaState, -2 );
							if( lua_toboolean( pLuaState, -1 ) )
							{
								if( _strcmpi( sz_flag_name, "steerable_input" ) == 0 )
									wheel_desc.wheelFlags |= WF_STEERABLE_INPUT;
								else if( _strcmpi( sz_flag_name, "accelerated" ) == 0 )
									wheel_desc.wheelFlags |= WF_ACCELERATED;
								else if( _strcmpi( sz_flag_name, "affected_by_handbrake" ) == 0 )
									wheel_desc.wheelFlags |= WF_AFFECTED_BY_HANDBRAKE;
							}

							lua_pop( pLuaState, 1 );
						}

					}
				}
				/*else if( LUA_TBOOLEAN == value_type )
				{
					
				}*/
			}

			/* removes 'value'; keeps 'key' for next iteration */
			lua_pop( pLuaState, 1 );
		}

		scene->physic()->create_wheel( str_car_body_name, wheel_desc );
	}
	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phWheelGetPos( lua_State* pLuaState )
{
	std::string str_wheel_name( lua_tostring( pLuaState, 1 ) );
	NxVec3 v_wheel_pos = scene->physic()->wheel( str_wheel_name )->getWheelPos();

	lua_pushnumber( pLuaState, v_wheel_pos.x );
	lua_pushnumber( pLuaState, v_wheel_pos.y );
	lua_pushnumber( pLuaState, v_wheel_pos.z );
	//lua_pushnumber( pLuaState, 1 );
	return 3;
}

int phWheelSetAngle( lua_State* pLuaState )
{
	std::string str_wheel_name( lua_tostring( pLuaState, 1 ) );
	float f_angle = lua_tonumber( pLuaState, 2 );
	scene->physic()->wheel( str_wheel_name )->setAngle( f_angle );

	//lua_pushnumber( pLuaState, 1 );
	return 0;
}

int phWheelIsGroundContact( lua_State* pLuaState )
{
	std::string str_wheel_name( lua_tostring( pLuaState, 1 ) );
	bool b_contact = scene->physic()->wheel( str_wheel_name )->hasGroundContact();
	
	lua_pushboolean( pLuaState, b_contact );
	//lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phWheelUpdate( lua_State* pLuaState )
{
	std::string str_wheel_name( lua_tostring( pLuaState, 1 ) );
	float motor_torque = lua_tonumber( pLuaState, 2 );
	float brake_pedal = lua_tonumber( pLuaState, 3 );
	bool b_handbrake = lua_toboolean( pLuaState, 4 );
	float dt = lua_tonumber( pLuaState, 5 );

	scene->physic()->wheel( str_wheel_name )->tick( b_handbrake, motor_torque, brake_pedal, dt );
	return 0;
}

int phRaycast( lua_State* pLuaState )
{
	std::string str_render_name( lua_tostring( pLuaState, 1 ) );

	float f_distance = 0.f;
	if( scene->physic()->raycast( str_render_name, &f_distance ) )
	{
		lua_pushboolean( pLuaState, true );
		lua_pushnumber( pLuaState, f_distance );
		return 2;
	}

	lua_pushboolean( pLuaState, false );
	return 1;
}





/*int phCreateVehicle( lua_State* pLuaState )
{
	scene->physic()->create_vehicle();
	lua_pushnumber( pLuaState, 1 );
	return 1;
}


int phControlVehicle( lua_State* pLuaState )
{
	//unsigned char keyCode = lua_tonumber( pLuaState, 1 );
	bool forward = lua_toboolean( pLuaState, 1 );
	bool backward = lua_toboolean( pLuaState, 2 );
	bool left = lua_toboolean( pLuaState, 3 );
	bool right = lua_toboolean( pLuaState, 4 );
	bool handbrake = lua_toboolean( pLuaState, 5 );
	bool b_stand_up = lua_toboolean( pLuaState, 6 );

	scene->physic()->control_vehicle( forward, backward, left, right, handbrake, b_stand_up );
	lua_pushnumber( pLuaState, 1 );
	return 1;
}*/





int phAddJoint( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( numParams >= 4 );

	sJointNameDesc jointNameDesc;
	jointNameDesc.strJointName.assign( lua_tostring( pLuaState, 1 ) );
	jointNameDesc.strActorName[0].assign( lua_tostring( pLuaState, 2 ) );
	jointNameDesc.strActorName[1].assign( lua_tostring( pLuaState, 3 ) );

	std::string strType( lua_tostring( pLuaState, 4 ) );

	if( "distance" == strType )
	{
		assert( 6 == lua_gettop( pLuaState ) );
		float fMinDistance = lua_tonumber( pLuaState, 5 );
		float fMaxDistance = lua_tonumber( pLuaState, 6 );
		scene->physic()->addDistanceJoint( jointNameDesc, fMinDistance, fMaxDistance );
	}
	else if( "fixed" == strType )
	{
		if( 4 == numParams )
		{
			scene->physic()->addFixedJoint( jointNameDesc );
		}
	}
	else if( "revolute" == strType )
	{
		sJointLocalDesc jointLocalDesc;
		switch( numParams )
		{
			case 4:
			{
				break;
			}
			case 10:
			{
				jointLocalDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 7 ), lua_tonumber( pLuaState, 6 ) );
				jointLocalDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 8 ), lua_tonumber( pLuaState, 10 ), lua_tonumber( pLuaState, 9 ) );
				break;
			}
			case 16:
			{
				jointLocalDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 7 ), lua_tonumber( pLuaState, 6 ) );
				jointLocalDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 8 ), lua_tonumber( pLuaState, 10 ), lua_tonumber( pLuaState, 9 ) );
				jointLocalDesc.vLocalAxis[0].set( lua_tonumber( pLuaState, 11 ), lua_tonumber( pLuaState, 13 ), lua_tonumber( pLuaState, 12 ) );
				jointLocalDesc.vLocalAxis[1].set( lua_tonumber( pLuaState, 14 ), lua_tonumber( pLuaState, 16 ), lua_tonumber( pLuaState, 15 ) );
				break;
			}
			case 22:
			{
				jointLocalDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 7 ), lua_tonumber( pLuaState, 6 ) );
				jointLocalDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 8 ), lua_tonumber( pLuaState, 10 ), lua_tonumber( pLuaState, 9 ) );
				jointLocalDesc.vLocalAxis[0].set( lua_tonumber( pLuaState, 11 ), lua_tonumber( pLuaState, 13 ), lua_tonumber( pLuaState, 12 ) );
				jointLocalDesc.vLocalAxis[1].set( lua_tonumber( pLuaState, 14 ), lua_tonumber( pLuaState, 16 ), lua_tonumber( pLuaState, 15 ) );
				jointLocalDesc.vLocalNormal[0].set( lua_tonumber( pLuaState, 17 ), lua_tonumber( pLuaState, 19 ), lua_tonumber( pLuaState, 18 ) );
				jointLocalDesc.vLocalNormal[1].set( lua_tonumber( pLuaState, 20 ), lua_tonumber( pLuaState, 22 ), lua_tonumber( pLuaState, 21 ) );
				break;
			}
			default:
			{
				std::cout << "Error num params for revolute joint" << std::endl;
				lua_pushnumber( pLuaState, 1 );
				return 1;
			}
		}

		// good :)))
		scene->physic()->addRevoluteJoint( jointNameDesc, jointLocalDesc );
	}
	else if( "prismatic" == strType )
	{
		sJointLocalDesc jointLocalDesc;
		switch( numParams )
		{
		case 4:
			{
				break;
			}
		case 10:
			{
				jointLocalDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 7 ), lua_tonumber( pLuaState, 6 ) );
				jointLocalDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 8 ), lua_tonumber( pLuaState, 10 ), lua_tonumber( pLuaState, 9 ) );
				break;
			}
		default:
			{
				std::cout << "Error num params for prismatic joint " << jointNameDesc.strJointName << std::endl;
				lua_pushnumber( pLuaState, 1 );
				return 1;
			}
		}
		scene->physic()->addPrismaticJoint( jointNameDesc, jointLocalDesc );
	}
	else if( "cylindrical" == strType )
	{
		sJointLocalDesc jointLocalDesc;
		switch( numParams )
		{
		case 4:
			{
				break;
			}
		case 10:
			{
				jointLocalDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 7 ), lua_tonumber( pLuaState, 6 ) );
				jointLocalDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 8 ), lua_tonumber( pLuaState, 10 ), lua_tonumber( pLuaState, 9 ) );
				break;
			}
		default:
			{
				std::cout << "Error num params for cylindrical joint " << jointNameDesc.strJointName << std::endl;
				lua_pushnumber( pLuaState, 1 );
				return 1;
			}
		}
		scene->physic()->addCylindricalJoint( jointNameDesc, jointLocalDesc );
	}
	else if( "pointInPlane" == strType )
	{
		sJointLocalDesc jointLocalDesc;
		switch( numParams )
		{
		case 4:
			{
				break;
			}
		case 10:
			{
				jointLocalDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 7 ), lua_tonumber( pLuaState, 6 ) );
				jointLocalDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 8 ), lua_tonumber( pLuaState, 10 ), lua_tonumber( pLuaState, 9 ) );
				break;
			}
		default:
			{
				std::cout << "Error num params for pointInPlane joint " << jointNameDesc.strJointName << std::endl;
				lua_pushnumber( pLuaState, 1 );
				return 1;
			}
		}
		scene->physic()->addPointInPlaneJoint( jointNameDesc, jointLocalDesc );
	}
	else if( "pointOnLine" == strType )
	{
		sJointLocalDesc jointLocalDesc;
		switch( numParams )
		{
		case 4:
			{
				break;
			}
		case 10:
			{
				jointLocalDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 7 ), lua_tonumber( pLuaState, 6 ) );
				jointLocalDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 8 ), lua_tonumber( pLuaState, 10 ), lua_tonumber( pLuaState, 9 ) );
				break;
			}
		default:
			{
				std::cout << "Error num params for pointOnLine joint " << jointNameDesc.strJointName << std::endl;
				lua_pushnumber( pLuaState, 1 );
				return 1;
			}
		}
		scene->physic()->addPointOnLineJoint( jointNameDesc, jointLocalDesc );
	}
	else if( "spherical" == strType )
	{
		sSphericalJointDesc desc;
		desc.nameDesc = jointNameDesc;

		desc.bTwist = lua_toboolean( pLuaState, 5 );
		desc.fLowTwist = lua_tonumber( pLuaState, 6 );
		desc.fHighTwist = lua_tonumber( pLuaState, 7 );

		desc.bSwing = lua_toboolean( pLuaState, 8 );
		desc.fSwingValue = lua_tonumber( pLuaState, 9 );
		desc.vSwingAxis.set( lua_tonumber( pLuaState, 10 ), lua_tonumber( pLuaState, 12 ), lua_tonumber( pLuaState, 11 ) );

		desc.localDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 13 ), lua_tonumber( pLuaState, 15 ), lua_tonumber( pLuaState, 14 ) );
		desc.localDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 16 ), lua_tonumber( pLuaState, 18 ), lua_tonumber( pLuaState, 17 ) );
		scene->physic()->addSphericalJoint( desc );
	}
	else if( "d6" == strType )
	{
		sD6JointDesc desc;
		desc.nameDesc = jointNameDesc;

		parseD6flag( pLuaState, 5, &desc.bXMotion );
		parseD6flag( pLuaState, 6, &desc.bZMotion );
		parseD6flag( pLuaState, 7, &desc.bYMotion );
		parseD6flag( pLuaState, 8, &desc.bTwistMotion );
		parseD6flag( pLuaState, 9, &desc.bSwing2Motion );
		parseD6flag( pLuaState, 10, &desc.bSwing1Motion );

		desc.fLinearLimit = lua_tonumber( pLuaState, 11 );
		desc.fTwistLimitLow = lua_tonumber( pLuaState, 12 );
		desc.fTwistLimitHigh = lua_tonumber( pLuaState, 13 );

		desc.fSwing2Limit = lua_tonumber( pLuaState, 14 );
		desc.fSwing1Limit = lua_tonumber( pLuaState, 15 );

		desc.localDesc.vLocalAnchor[0].set( lua_tonumber( pLuaState, 16 ), lua_tonumber( pLuaState, 18 ), lua_tonumber( pLuaState, 17 ) );
		desc.localDesc.vLocalAnchor[1].set( lua_tonumber( pLuaState, 19 ), lua_tonumber( pLuaState, 21 ), lua_tonumber( pLuaState, 20 ) );
		scene->physic()->addD6Joint( desc );
	}

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

void parseD6flag( lua_State* pLuaState, int index, NxD6JointMotion* pMotion )
{
	const char* pStr = lua_tostring( pLuaState, index );
	if( _strcmpi( pStr, "free" ) == 0 )
	{
		*pMotion = NX_D6JOINT_MOTION_FREE;
	}
	else if( _strcmpi( pStr, "locked" ) == 0 )
	{
		*pMotion = NX_D6JOINT_MOTION_LOCKED;
	}
	else if( _strcmpi( pStr, "limited" ) == 0 )
	{
		*pMotion = NX_D6JOINT_MOTION_LIMITED;
	}
	else
	{
		assert( 0 );
	}
}

int phSetBreakableJoint( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( numParams == 3 );

	std::string strJointName( lua_tostring( pLuaState, 1 ) );

	float fMaxForce = lua_tonumber( pLuaState, 2 );
	float fMaxTorque = lua_tonumber( pLuaState, 3 );

	scene->physic()->setBreakableJoint( strJointName, fMaxForce, fMaxTorque );

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phSetGlobalAxisAtJoint( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( numParams == 4 );

	std::string strJointName( lua_tostring( pLuaState, 1 ) );

	NxVec3 vGlobalAxis( lua_tonumber( pLuaState, 2 ), lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 3 ) );
	scene->physic()->setGlobalAxisAtJoint( strJointName, vGlobalAxis );

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phSetGlobalAnchorAtJoint( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( numParams == 4 );

	std::string strJointName( lua_tostring( pLuaState, 1 ) );

	NxVec3 vGlobalAnchor( lua_tonumber( pLuaState, 2 ), lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 3 ) );
	scene->physic()->setGlobalAnchorAtJoint( strJointName, vGlobalAnchor );

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phSetLimitPointAtJoint( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( numParams == 5 );

	std::string strJointName( lua_tostring( pLuaState, 1 ) );

	NxVec3 vPoint( lua_tonumber( pLuaState, 2 ), lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 3 ) );
	bool bPointIsOnActor2 = lua_toboolean( pLuaState, 5 );
	scene->physic()->setLimitPointAtJoint( strJointName, vPoint, bPointIsOnActor2 );

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phAddLimitPlaneAtJoint( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( numParams == 7 );

	std::string strJointName( lua_tostring( pLuaState, 1 ) );

	NxVec3 vNormal( lua_tonumber( pLuaState, 2 ), lua_tonumber( pLuaState, 4 ), lua_tonumber( pLuaState, 3 ) );
	NxVec3 vPointInPlane( lua_tonumber( pLuaState, 5 ), lua_tonumber( pLuaState, 7 ), lua_tonumber( pLuaState, 6 ) );
	scene->physic()->addLimitPlaneAtJoint( strJointName, vNormal, vPointInPlane );

	lua_pushnumber( pLuaState, 1 );
	return 1;
}

int phSetMotorAtRevoluteJoint( lua_State* pLuaState )
{
	int numParams = lua_gettop( pLuaState );
	assert( numParams == 4 );

	std::string strJointName( lua_tostring( pLuaState, 1 ) );
	float fVelTarget = lua_tonumber( pLuaState, 2 );
	float fMaxForce = lua_tonumber( pLuaState, 3 );
	bool bFreeSpin = lua_toboolean( pLuaState, 4 );

	scene->physic()->setMotorAtRevoluteJoint( strJointName, fVelTarget, fMaxForce, bFreeSpin );

	lua_pushnumber( pLuaState, 1 );
	return 1;
}
#endif
#endif
//----------------------------------------определение функции 3D объектов-----------------

//Установка значения шейдера
int object_ChangeShaderUniform(lua_State *L)
{
	//получаем необходимые параметры
	std::string GroupName = lua_tostring(L,1);
	std::string UniformName = lua_tostring(L,2);
	float newValue = lua_tonumber(L,3);
	scene->Graphic->ChangeShaderUniform(GroupName,UniformName,newValue);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция для создания 3D объектов
int object_Load3DObject(lua_State *L)
{
	//получаем необходимые параметры
	std::string FileName = lua_tostring(L,1);
	std::cout << "Player: load 3d object ... " << FileName << std::endl;
	scene->Graphic->LoadGraphicFile(FileName);
		//viewer.getCamera()->getRenderer()->setCompileOnNextDraw(true);
		//osgViewer::Renderer::setCompileOnNextDraw(true);
		localUser->setCompileOnNextDraw();
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция для перемещения объекта (3d)
int object_RSetPosition(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float x = lua_tonumber(L,2);
	float y = lua_tonumber(L,3);
	float z = lua_tonumber(L,4);
	scene->Graphic->RSetPosition(ObjectName,x,y,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция для удаления загруженных с помощью функции Load3DObject объектов (3d)
int object_RReset(lua_State *L)
{
	scene->Reset();
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция совмещения точки привязки одного объекта с другим
int object_RSetAt(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string ObjectName2 = lua_tostring(L,2);
	scene->Graphic->RSetAt (ObjectName, ObjectName2);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция выборочного сброса матриц позиции/поворота/масштаба
int object_RResetObject(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	bool position = lua_toboolean(L,2); //Сброс матрицы позиции
	bool rotate = lua_toboolean(L,3);   //Сброс матрицы поворота
	bool scale = lua_toboolean(L,4);    //Сброс матрицы масштаба
	scene->Graphic->RResetObject(ObjectName, position, rotate, scale);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//альтернативное вращение 3D объекта
int object_RSetRotate(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float angle1 = lua_tonumber(L,2);
	float angle2 = lua_tonumber(L,3);
	float angle3 = lua_tonumber(L,4);
	scene->Graphic->RSetRotate (ObjectName, angle1, angle2, angle3);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//альтернативное вращение сложное перемещение  3D объекта
static int object_RSetRotate2(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
		float angle1 = lua_tonumber(L,2);
	float dx1 = lua_tonumber(L,3);
	float dy1 = lua_tonumber(L,4);
	float dz1 = lua_tonumber(L,5);
		float angle2 = lua_tonumber(L,6);
	float dx2 = lua_tonumber(L,7);
	float dy2 = lua_tonumber(L,8);
	float dz2 = lua_tonumber(L,9);
		float angle3 = lua_tonumber(L,10);
	float dx3 = lua_tonumber(L,11);
	float dy3 = lua_tonumber(L,12);
	float dz3 = lua_tonumber(L,13);

	scene->Graphic->RSetRotate2 (ObjectName,angle1,dx1,dy1,dz1,angle2,dx2,dy2,dz2,angle3,dx3,dy3,dz3);

	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//альтернативное перемещение относительно другого объекта по абсолютным координатам
int object_ASetAt(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string ObjectName2 = lua_tostring(L,2);
	scene->Graphic->ASetAt(ObjectName,ObjectName2);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//альтернативное задание видимости объекта
int object_RSetVisible(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	bool visible = lua_toboolean(L,2);
	scene->Graphic->RSetVisible (ObjectName, visible);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//альтернативное масштабирование объекта
int object_RSetScale(lua_State *L)
{
	std::string ObjectName = lua_tostring(L,1);
	float dx = lua_tonumber(L,2);
	float dy = lua_tonumber(L,3);
	float dz = lua_tonumber(L,4);
	scene->Graphic->RSetScale(ObjectName, dx, dy, dz);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//альтернативная передача x,y,z объекта
int object_RGetPosition(lua_State *L)
{
	std::string ObjectName = lua_tostring(L,1);
	float x,y,z;
	scene->Graphic->RGetPosition(ObjectName,x,y,z);
	lua_pushnumber(L,x);
	lua_pushnumber(L,y);
	lua_pushnumber(L,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 4;
}

//альтернативная передача x,y,z объекта - абсолютные координаты
int object_AGetPosition(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float x,y,z;
	scene->Graphic->AGetPosition(ObjectName,x,y,z);
	lua_pushnumber(L,x);
	lua_pushnumber(L,y);
	lua_pushnumber(L,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 4;
}

//альтернативное получение масштаба объекта
int object_RGetScale(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float x,y,z;
	scene->Graphic->RGetScale(ObjectName,x,y,z);
	lua_pushnumber(L,x);
	lua_pushnumber(L,y);
	lua_pushnumber(L,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 4;
}

//альтернативное получение квантериона вращения объекта
int object_RGetRotate(lua_State *L)
{
	std::string ObjectName = lua_tostring(L,1);
	float x,y,z,w;
	scene->Graphic->RGetRotate(ObjectName,x,y,z,w);
	lua_pushnumber(L,x);
	lua_pushnumber(L,y);
	lua_pushnumber(L,z);
	lua_pushnumber(L,w);
	//возврат параметра
	lua_pushnumber(L,333);
	return 5;
}

//альтернативное рисование на текстуре объекта
int object_RTextureDraw(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	//координаты точки
	int x = lua_tonumber(L,2);
	int y = lua_tonumber(L,3);
	float r = lua_tonumber(L,4);
	float g = lua_tonumber(L,5);
	float b = lua_tonumber(L,6);
	float a = lua_tonumber(L,7);
	bool flush = lua_toboolean(L,8);

	unsigned int UnitNum =0;
	if (lua_type(L,9)!=LUA_TNONE) UnitNum = lua_tonumber(L,9);
	scene->Graphic->RTextureDraw (ObjectName,x,y,r,g,b,a,flush,UnitNum);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//альтернативная замена текстуры  объекта
int object_RChangeTexture(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string filename = lua_tostring(L,2);

	unsigned int UnitNum =0;
	if (lua_type(L,3)!=LUA_TNONE) UnitNum = lua_tonumber(L,3);

	scene->Graphic->RChangeTexture (ObjectName, filename, UnitNum);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//альтернативная Установка цвета объекта
int object_RSetColor(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float r = lua_tonumber(L,2);
	float g = lua_tonumber(L,3);
	float b = lua_tonumber(L,4);
	float a = lua_tonumber(L,5);
	scene->Graphic->RSetColor(ObjectName,r,g,b,a);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Изменение иерархии объектов (изменение подчинения объектов)
int object_RConnectObjectTo(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string ObjectName2 = lua_tostring(L,2);
	scene->Graphic->RConnectObjectTo (ObjectName,ObjectName2);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Тест на пересечение 2 объектов в относительных координатах
int object_TestIntersects(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string ObjectName2 = lua_tostring(L,2);
	float koeff = lua_tonumber(L,3);
	//
	bool test = scene->Graphic->RTestIntersects (ObjectName,ObjectName2,koeff);
	//
	if (test==false)
	{
		lua_pushboolean(L,0);
	}
	else
	{
		lua_pushboolean(L,1);
	}
	//возврат параметра
	lua_pushnumber(L,333);
	return 2;
}

//Тест на пересечение 2 объектов в относительных координатах
int object_TestIntersects2(lua_State *L)
{
        //получаем необходимые параметры
        std::string ObjectName = lua_tostring(L,1);
        std::string ObjectName2 = lua_tostring(L,2);
        float koeff = lua_tonumber(L,3);
        //
        bool test = scene->Graphic->RTestIntersects2 (ObjectName,ObjectName2,koeff);
        //
        if (test==false)
        {
                lua_pushboolean(L,0);
        }
        else
        {
                lua_pushboolean(L,1);
        }
        //возврат параметра
        lua_pushnumber(L,333);
        return 2;
}

//Тест на пересечение линией
int object_TestLineIntersects(lua_State *L)
{
	float x1 = lua_tonumber(L,1);
	float y1 = lua_tonumber(L,2);
	float z1 = lua_tonumber(L,3);
	float x2 = lua_tonumber(L,4);
	float y2 = lua_tonumber(L,5);
	float z2 = lua_tonumber(L,6);
	std::string proxyObjectName = lua_tostring(L,7);
	//
	std::string Name;
	float gx,gy,gz,lx,ly,lz;
	bool check = scene->Graphic->RTestLineIntersects(x1,y1,z1,x2,y2,z2,proxyObjectName, Name,gx,gy,gz,lx,ly,lz);
	//
	if (check==true)
	{
		//да. пересекает
		lua_pushboolean(L,1);
	}
	else
	{
		//не пересекает
		lua_pushboolean(L,0);
	}
	//название
	lua_pushstring(L, Name.c_str() );
	//Глобальные коорд пересечения
	lua_pushnumber(L,gx);
	lua_pushnumber(L,gy);
	lua_pushnumber(L,gz);
	//Локальные коорд пересечения
	lua_pushnumber(L,lx);
	lua_pushnumber(L,ly);
	lua_pushnumber(L,lz);
	return 8;
}

//Функция подключает шейдер к объекту
static int object_SetShader(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string ShaderName = lua_tostring(L,2);
	if (ShaderName=="BUMP")
	{
		std::string TextureName = lua_tostring(L,3);
		scene->Shaders->ConnectBumpShader(ObjectName,TextureName);
	}
	if (ShaderName=="SKY")
	{
		std::string TextureName = lua_tostring(L,3);
		scene->Shaders->ConnectSkyShader(ObjectName,TextureName);
	}
	if (ShaderName=="CERAMIC")
	{
		std::string TextureName1 = lua_tostring(L,3);
		scene->Shaders->ConnectCeramicShader(ObjectName,TextureName1);
	}
	/*
	if (ShaderName=="FLAG")
	{
		ConnectFlagShader(Scene.Objects3D.GetObject(ObjectName)._Model);
	}

	*/
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//----------------------------------------------------------------
//ПАУЗА/СТАРТ анимации
int object_SetPauseAnimation(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	bool pause = lua_toboolean(L,2);
	bool all = lua_toboolean(L,3);
	scene->Graphic->SetPauseAnimation (ObjectName, pause, all);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Переход на нужное время анимации
int object_GotoAnimation(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float new_t = lua_tonumber(L,2);
	bool all = lua_toboolean(L,3);
	scene->Graphic->GotoAnimation (ObjectName, new_t,all);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Установка опорного смещения анимации
int object_SetPivotPointAnimation(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float new_x = lua_tonumber(L,2);
	float new_y = lua_tonumber(L,3);
	float new_z = lua_tonumber(L,4);
	scene->Graphic->SetPivotPointAnimation (ObjectName,new_x,new_y,new_z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//------------------------------------------------morph--------------------------------
// создание morph-объекта
int CreateMorph( lua_State* p_lua_state )
{
        std::string str_morph( lua_tostring( p_lua_state, 1 ) );
        std::string str_default_state( lua_tostring( p_lua_state, 2 ) );
        std::vector<std::string> v_str_target_states;
        for( int i=3, end_i=lua_gettop( p_lua_state )+1; i<end_i; i++ )
        {
                v_str_target_states.push_back( lua_tostring( p_lua_state, i ) );
        }

        scene->Graphic->CreateMorph( str_morph, str_default_state, v_str_target_states );
        return 0;
}

// переход на указанное время morph-анимации
int GotoMorphState( lua_State* p_lua_state )
{
        std::string str_morph( lua_tostring( p_lua_state, 1 ) );
        std::vector<float> v_weights;
        for( int i=2, end_i=lua_gettop( p_lua_state )+1; i<end_i; i++ )
        {
                v_weights.push_back( lua_tonumber( p_lua_state, i ) );
        }

        scene->Graphic->GotoMorphState( str_morph, v_weights );
        return 0;
}
//-----------------------------------------------Тени------------------------------------------------------------

//Создание отдельной группы теней, параметр - имя группы
int object_AddShadowedGroup(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	scene->Graphic->AddShadowedGroup (ObjectName);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Определение маски Имя объекта, 4-ничего, 1-принимать, 2-бросать, 3 - и то и другое.
int object_SetShadowMask(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	int new_mask = (int)lua_tonumber(L,2);
	scene->Graphic->SetShadowMask (ObjectName, new_mask);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//-----------------------------------------------text-------------------------------------------------------------
//создание текстового объекта
int text_AddTextObject(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string Text = lua_tostring(L,2);
	bool Menu = lua_toboolean(L,3);
	scene->Texts->CreateText (ObjectName,Text);
	if (Menu==true)
	{
		scene->Graphic->RConnectObjectTo(ObjectName, "HUD");
	}
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Задание значения текстового поля
int text_SetText(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string value = lua_tostring(L,2);
		//если задано декодирование
		bool utf8encoding = false;
		if (lua_type(L,3)!=LUA_TNONE)  utf8encoding  = lua_toboolean(L,3);
		//
		scene->Texts->SetText (ObjectName,value, utf8encoding);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Задание цвета текста
int text_SetColor(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float r = lua_tonumber(L,2);
	float g = lua_tonumber(L,3);
	float b = lua_tonumber(L,4);
	float a = lua_tonumber(L,5);
	scene->Texts->SetTextColor(ObjectName,r,g,b,a);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Задание размера текста
int text_SetSize(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float size = lua_tonumber(L,2);
	scene->Texts->SetTextSize (ObjectName,size);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Задание шрифта текста
int text_SetFont(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string FontName = lua_tostring(L,2);
	scene->Texts->SetTextFont (ObjectName,FontName);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//задание равнения текста 0-слева 1-справа
int text_SetAlignment(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	int align = (int)lua_tonumber(L,2);
	scene->Texts->SetTextAlignment (ObjectName,align);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//задание ориентации текста
int text_SetLayout(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	int layout = (int)lua_tonumber(L,2);
	scene->Texts->SetTextLayout (ObjectName, layout);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

int text_SetAxisAlignment(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	int axe = (int)lua_tonumber(L,2);
	scene->Texts->SetAxisAlignment (ObjectName, axe);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//----------------------------------sound----------------------------------------------
//Функция для создания Sound объектов
int sound_NewSoundObject(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	bool HeadRelativeMode  = lua_toboolean(L,2);
		try
		{
			scene->Audio->NewObject (ObjectName,HeadRelativeMode);
		}
		catch (ErrorClass a)
		{
			//вывод ошибки и выход
			a.print();
		}

	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция для загрузки звукового файла
int sound_LoadSound(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string filename = lua_tostring(L,2);
	scene->Audio->Open(ObjectName,filename,false,true);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция для начала воспроизведения звукового файла
int sound_PlaySound(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	scene->Audio->Play(ObjectName);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция для паузы звукового файла
int sound_PauseSound(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	scene->Audio->Pause (ObjectName);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция для задания режима LOOP звукового файла
int sound_LoopSound(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	bool loop = lua_toboolean(L,2);
	scene->Audio->setLoop (ObjectName, loop);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция для задания позиции источника звука
static int sound_PositionSound(lua_State *L)
{
	std::cout << "PositionSound = RSetPosition. use RSetPosition." << std::endl;
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float x = lua_tonumber(L,2);
	float y = lua_tonumber(L,3);
	float z = lua_tonumber(L,4);
	scene->Graphic->RSetPosition(ObjectName,x,y,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Регистрация функции получить состояние объекта (играет, на паузе, воспроизведение закончено и т.д)
static int sound_GetState(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	//запрос
	std::string state = scene->Audio->GetState(ObjectName);
	//возврат параметра
	lua_pushstring(L,state.c_str());
	return 1;
}

//Создает геометрический объект, для того чтобы видеть источник звука
static int sound_ShowDebug(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	scene->Audio->ShowSoundDebug (ObjectName);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//установить громкость
static int sound_SetGain(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float value = lua_tonumber(L,2);
	scene->Audio->SetGain(ObjectName,value);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Максимальное расстояние, далее него уже не будет происходить дальнейшего  ослабления звука
static int sound_SetMaxDistance(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float value = lua_tonumber(L,2);
	scene->Audio->SetMaxDistance(ObjectName,value);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Расстояние, на котором сила звука уменьшается вдвое (до воздействия параметра rolloff
static int sound_SetReferenceDistance(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float value = lua_tonumber(L,2);
	scene->Audio->SetReferenceDistance(ObjectName,value);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Минимальное/максимальное значение громкости
static int sound_SetMinMaxGain(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float min = lua_tonumber(L,2);
	float max = lua_tonumber(L,3);
	scene->Audio->SetMinMaxGain(ObjectName,min,max);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//"перемотка звука на нужную секунду" [0,any]
static int sound_SetOFFSET(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float sec = lua_tonumber(L,2);
	scene->Audio->SetOFFSET(ObjectName,sec);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Коэфффициент кривизны затухания звука (rolloff фактор)
static int sound_SetRolloffFactor(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float rolloff = lua_tonumber(L,2);
	scene->Audio->SetRolloffFactor(ObjectName,rolloff);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Приоритет звука
static int sound_SetPriority(lua_State *L)
{
		//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float priority = lua_tonumber(L,2);
	scene->Audio->SetPriority(ObjectName,priority);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}



//установить Pitch (0..10)
static int sound_SetPitch(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
    float value = lua_tonumber(L,2);
	scene->Audio->SetPitch(ObjectName, value);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//добавить эффект Reverb
static int sound_AddReverb(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	scene->Audio->AddReverb (ObjectName);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//---------------------RTI-----------------------------------------
/*
static int RTI_CreateFederate( lua_State* pLuaState )
{
    //имя федерата
    //имя fed-файла
    //имя федерации (по умолчанию ExampleFederation)
    std::string FederateName = lua_tostring( pLuaState, 1 );
    std::string FedFileName =  lua_tostring( pLuaState, 2 );
    std::string FederationName = "ExampleFederation";
    if (lua_type(L,3)!=LUA_TNONE) FederationName = lua_tostring(pLuaState,3);

    bool b_res = g_federate.create( FederateName.c_str(), FedFileName.c_str(), FederationName );
    lua_pushboolean( pLuaState, b_res );
    return 1;
}

static int RTI_DestroyFederate( lua_State* pLuaState )
{
        g_federate.destroy();
        return 0;
}

static int RTI_RegisterSynchronizationPoint( lua_State* pLuaState )
{
        g_federate.register_synchronization_point();
        return 0;
}

static int RTI_RunFederate( lua_State* pLuaState )
{
        bool b_time_regulating = false;
        bool b_time_constrained = false;
		bool b_blocking = false;

        int numParams = lua_gettop( pLuaState );
        if( numParams > 0 )
                b_time_regulating = lua_toboolean( pLuaState, 1 );
        if( numParams > 1 )
                b_time_constrained = lua_toboolean( pLuaState, 2 );
        if( numParams > 2 )
                b_blocking = lua_toboolean( pLuaState, 3 );

		g_federate.run( b_time_regulating, b_time_constrained, b_blocking);
        return 0;
}

static int RTI_Publish( lua_State* pLuaState )
{
        bool b_res = g_federate.publish( lua_tostring( pLuaState, 1 ), lua_tostring( pLuaState, 2 ) );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

static int RTI_Subscribe( lua_State* pLuaState )
{
        bool b_res = g_federate.subscribe( lua_tostring( pLuaState, 1 ), lua_tostring( pLuaState, 2 ) );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

static int RTI_AddObject( lua_State* pLuaState )
{
        bool b_res = g_federate.add_instance_object( lua_tostring( pLuaState, 1 ), lua_tostring( pLuaState, 2 ) );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

static int RTI_RemoveObject( lua_State* pLuaState )
{
        bool b_res = g_federate.remove_instance_object( lua_tostring( pLuaState, 1 ) );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

static int RTI_SetValue( lua_State* pLuaState )
{
        bool b_res = g_federate.set_attribute_value_by_object_instance( lua_tostring( pLuaState, 1 ),
                lua_tostring( pLuaState, 2 ), lua_tostring( pLuaState, 3 ) );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

void RTI_Change( const std::string& str_object_name,
                                const std::vector<cFederateAmbassador::sReflectAttribute>& v_attributes,
                                const std::string& str_tag, double time )
{
        // флаг отправки команды в луа об изменении атрибутов (если false, то отправки не будет - все объекты прокси!)
        bool b_send_command_RTI_Change = false;

        // вектор прокси-объектов для последующей обработки
        std::vector<std::pair<cRTIProxy*, std::vector<cFederateAmbassador::sReflectAttribute>::const_iterator> >
                v_proxis;

        // флаг необходимости поиска прокси по аттрибуту
        bool b_need_find_proxy = g_proxy_manager.find_proxy_by_object( str_object_name );

        for( std::vector<cFederateAmbassador::sReflectAttribute>::const_iterator itera = v_attributes.begin(),
                itera_end = v_attributes.end(); itera != itera_end; ++itera )
        {
                cRTIProxy* p_proxy = NULL;
                // поиск проксика по имени объекта и атрибуту
                if( b_need_find_proxy && (p_proxy = g_proxy_manager.find_proxy( str_object_name, itera->str_name )) )
                {
                        // при отсутствии временной метки пропускаем сообщение об изменении прокси-объекта
                        // (чтобы не было дублирования)
                        if( time == 0. )
                                continue;

                        // накапливаем прокси-объекты для дальнейшей обработки
                        v_proxis.push_back( std::pair<cRTIProxy*, std::vector<cFederateAmbassador::sReflectAttribute>::const_iterator>( p_proxy, itera ) );
                }
                else
                {
                        // если не прокси-объект, то отправляем в луа функцию 'RTI_Change', имя объекта, ...
                        if( !b_send_command_RTI_Change )
                        {
                                b_send_command_RTI_Change = true;

                                char sz_command[] = "RTI_Change";
                                lua_getglobal( L, sz_command );
                                lua_pushstring( L, str_object_name.c_str() );

                                lua_newtable( L );
                        }

                        // .. и атрибуты
                        lua_pushstring( L, itera->str_name.c_str() );
                        lua_pushstring( L, itera->str_value.c_str() );
                        lua_settable( L, -3 );
                }
        }

        // вызываем в луа функцию 'RTI_Change'
        if( b_send_command_RTI_Change )
        {
                lua_pushstring( L, str_tag.c_str() );
                if( lua_pcall( L, 3, 0, 0 ) )
                {
                        lua_pop( L, 1 );
                }
        }

        // финальный блок обработки прокси-объектов
        if( b_need_find_proxy )
        {
                for( std::vector<std::pair<cRTIProxy*, std::vector<cFederateAmbassador::sReflectAttribute>::const_iterator> >::iterator
                        itera = v_proxis.begin(), itera_end = v_proxis.end(); itera != itera_end; ++itera )
                {
                        cRTIProxy* p_proxy = itera->first;

                        g_proxy_manager.process_obtained_proxy( scene, p_proxy, itera->second->str_value );

                        static char sz_command[] = "RTI_ProxyObtained";
                        lua_getglobal( L, sz_command );
                        lua_pushstring( L, p_proxy->get_desc().str_proxy_name.c_str() );

                        // сообщаем луа об обработке прокси-объекта вызовом функции 'RTI_ProxyObtained'
                        if( lua_pcall( L, 1, 0, 0 ) )
                        {
                                lua_pop( L, 1 );
                        }
                }
        }
}


static int RTI_PublishInteraction( lua_State* pLuaState )
{
        bool b_res = g_federate.publish_interaction( lua_tostring( pLuaState, 1 ) );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

static int RTI_SubscribeInteraction( lua_State* pLuaState )
{
        bool b_res = g_federate.subscribe_interaction( lua_tostring( pLuaState, 1 ) );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

static int RTI_AddParameter( lua_State* pLuaState )
{
        bool b_res = g_federate.add_parameter( lua_tostring( pLuaState, 1 ), lua_tostring( pLuaState, 2 ) );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

static int RTI_SendInteraction( lua_State* pLuaState )
{
        std::string str_class_name( lua_tostring( pLuaState, 1 ) );
        int num_values = lua_gettop( pLuaState ) - 1;

        std::vector<std::string> v_values;
        v_values.reserve( num_values );
        for( int i=0; i<num_values; i++ )
        {
                v_values.push_back( lua_tostring( pLuaState, i+2 ) );
        }

        bool b_res = g_federate.send_interaction( str_class_name, v_values );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

void RTI_Interaction( const std::string& str_class_name, const std::vector<cFederateAmbassador::sReflectParameter>& v_params,
                                         const std::string& str_tag )
{
        char sz_command[] = "RTI_Interaction";
        lua_getglobal( L, sz_command );
        lua_pushstring( L, str_class_name.c_str() );

        lua_newtable( L );
        for( std::vector<cFederateAmbassador::sReflectParameter>::const_iterator itera = v_params.begin(),
                itera_end = v_params.end(); itera != itera_end; ++itera )
        {
                lua_pushstring( L, itera->str_name.c_str() );
                lua_pushstring( L, itera->str_value.c_str() );
                lua_settable( L, -3 );
        }

        lua_pushstring( L, str_tag.c_str() );
        if( lua_pcall( L, 3, 0, 0 ) )
        {
                //std::cout << lua_tostring( L, -1 ) << std::endl;
                lua_pop( L, 1 );
        }
}

// сообщение о создании инстанса объекта
void RTI_Discover( const std::string& str_object_name, const std::string& str_class_name )
{
        static char sz_command[] = "RTI_Discover";
        lua_getglobal( L, sz_command );
        lua_pushstring( L, str_object_name.c_str() );
        lua_pushstring( L, str_class_name.c_str() );

        if( lua_pcall( L, 2, 0, 0 ) )
        {
                lua_pop( L, 1 );
        }
}

// сообщение об удалении инстанса объекта
void RTI_Removed(const std::string& object_name)
{
	static char cmd[] = "RTI_Removed";
	lua_getglobal(L, cmd);
	lua_pushstring(L, object_name.c_str());
	if(lua_pcall(L, 1, 0, 0))	lua_pop(L, 1);
}

static int RTI_AddProxy( lua_State* pLuaState )
{
        std::string str_proxy_name( lua_tostring( pLuaState, 1 ) );
        std::string str_object_name( lua_tostring( pLuaState, 2 ) );
        std::string str_attribute_name( lua_tostring( pLuaState, 3 ) );
        std::string str_proxy_type( lua_tostring( pLuaState, 4 ) );

        const int num_to_skip = 5;
        int lua_top = lua_gettop( pLuaState ) + 1;
        std::vector<std::string> v_params;
        v_params.reserve( lua_top - num_to_skip );
        for( int i=num_to_skip; i<lua_top; i++ )
        {
                v_params.push_back( lua_tostring( pLuaState, i ) );
                //std::cout << lua_tostring( pLuaState, i ) << "  " << lua_top - num_to_skip << std::endl;
        }

        bool b_res = g_proxy_manager.add_proxy( str_proxy_name, str_object_name, str_attribute_name,
                cRTIProxy::get_type_by_str( str_proxy_type ), v_params );
        lua_pushboolean( pLuaState, b_res );
        return 1;
}

static int RTI_RemoveProxy( lua_State* pLuaState )
{
        std::string str_proxy_name( lua_tostring( pLuaState, 1 ) );
        g_proxy_manager.remove_proxy( str_proxy_name );
        return 0;
}
// -----------------------------------------------------------------------------



static int net_create(lua_State* lua_state)
{
	if(!g_net_client)
	{
		std::string federate_name = lua_tostring(lua_state,1);
		std::string classes_path = lua_tostring(lua_state,2);
		g_net_client.reset(new NetClient(lua_state,federate_name,classes_path));
		lua_pushboolean(lua_state, true);
	}
	else
		lua_pushboolean(lua_state, false);
	return 1;
}
*/

//---------------------------персонажи---------------------------------
//Функция для загрузки и создания персонажа
static int character_Load(lua_State *L)
{
	//получаем необходимые параметры
	std::string characterName = lua_tostring(L,1);
	std::string fileName = lua_tostring(L,2);
	//scene->Characters->CreateCharacter(characterName, fileName); //"./characters/characters/character1.char"
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;

}
//установка анимации персонажа
static int character_SetAction(lua_State *L)
{
	//получаем необходимые параметры
	std::string characterName = lua_tostring(L,1);
	int ActNum = lua_tonumber(L,2);
	//scene->Characters->SetCharacterAction (characterName,ActNum); //"ACT_RUN"
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//установка позиции персонажа
static int character_SetPosition(lua_State *L)
{
	//получаем необходимые параметры
	std::string characterName = lua_tostring(L,1);
	float x = lua_tonumber(L,2);
	float y = lua_tonumber(L,3);
	float z = lua_tonumber(L,4);
	//scene->Characters->SetCharacterPosition (characterName,x,y,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//установка места-назначения персонажа
static int character_GoTo(lua_State *L)
{
	//получаем необходимые параметры
	std::string characterName = lua_tostring(L,1);
	float x = lua_tonumber(L,2);
	float y = lua_tonumber(L,3);
	float z = lua_tonumber(L,4);
	//scene->Characters->SetCharacterGoTo (characterName,x,y,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Прикрепляет объект к кости
static int character_ConnectObjectToBone(lua_State *L)
{
	//получаем необходимые параметры
	std::string nameCharacter = lua_tostring(L,1);
	std::string ObjectName = lua_tostring(L,2);
	int boneID = lua_tonumber(L,3);
	//scene->Characters->ConnectObjectToBone(nameCharacter, ObjectName, boneID);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}


//--------------------------Skin Animtaion --------------------------------
static int LoadSkinAnimation(lua_State* L)
{
	std::string object_name = lua_tostring(L,1);
	std::string filename = lua_tostring(L,2);
	bool result = scene->skin_animation->load_skin(object_name,filename);
	lua_pushnumber(L,result ? 1 : 0);
	return 1;
}
static int PlaySkinAnimation(lua_State* L)
{
	std::string object_name = lua_tostring(L,1);
	std::string animation_name = lua_tostring(L,2);

	int priority = 0;
	float weight = 1.f;

	int lua_top = lua_gettop(L);
	if(lua_top > 2)
	{
		if(lua_type(L,3) == LUA_TNUMBER)	priority = lua_tointeger(L,3);
		if(lua_type(L,4) == LUA_TNUMBER)	weight = lua_tonumber(L,4);
	}
	
	scene->skin_animation->play_animation(object_name,animation_name,priority,weight);
	lua_pushnumber(L,333);
	return 1;
}
static int StopSkinAnimation(lua_State* L)
{
	std::string object_name = lua_tostring(L,1);
	std::string animation_name = lua_tostring(L,2);
	scene->skin_animation->stop_animation(object_name,animation_name);
	lua_pushnumber(L,333);
	return 1;
}
static int GotoTimeSkinAnimation(lua_State* L)
{
	std::string object_name = lua_tostring(L,1);
	std::string animation_name = lua_tostring(L,2);
	float time = lua_tonumber(L,3);
	float weight = 1.f;
	if(lua_type(L,4) == LUA_TNUMBER)	weight = lua_tonumber(L,4);
	scene->skin_animation->goto_time_animation(object_name,animation_name,time,weight);
	lua_pushnumber(L,333);
	return 1;
}
static int RotateSkinBone(lua_State* L)
{
	std::string object_name = lua_tostring(L,1);
	std::string bone_name = lua_tostring(L,2);
	std::string rotate_name = lua_tostring(L,3);
	osg::Vec3 axis(lua_tonumber(L,4),lua_tonumber(L,5),lua_tonumber(L,6));
	float angle = lua_tonumber(L,7);
	scene->skin_animation->rotate_bone(object_name,bone_name,rotate_name,axis,angle);
	lua_pushnumber(L,333);
	return 1;
}
//--------------------------свет----------------------------------------
//устанавливает Diffuse-цвет указанного светового источника
static int SetLightColor(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	float r = lua_tonumber(L,2);
	float g = lua_tonumber(L,3);
	float b = lua_tonumber(L,4);
	scene->Light->SetLightColor (Name,r,g,b);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//включает/выключает свет
static int SetLightEnable(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	bool enabled = lua_toboolean(L,2);
	scene->Light->SetLightEnabled (Name, enabled);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Включает/выключает учет освещения объектом
//изменять атрибуты, например "объект освещается или нет" (stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);)
static int SetLightingEnable(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	bool enabled = lua_toboolean(L,2);
	scene->Light->SetLightingEnable (Name, enabled);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//--------------------------------Запросы--------------------------------------
//Запрос предка объекта
static int object_GetParent(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	std::string parentname = scene->Graphic->GetParent (Name);
	//возврат параметра
	lua_pushstring( L, parentname.c_str() );
	return 1;
}
//----------------------------------clone/del---------------------------------------
//клонирует объект-оригинал и цепляет его к предку оригинала (т.е. у них общий родитель сохраняется)
static int object_Clone(lua_State *L)
{
	//получаем необходимые параметры
	std::string OriginalName = lua_tostring(L,1);
	std::string CloneName = lua_tostring(L,2);
	std::string postfix = lua_tostring(L,3);
	scene->Graphic->CloneObject (OriginalName, CloneName, postfix);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Удаление объекта из сцены
static int object_UnloadObject(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	scene->UnloadObject (Name);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//----------------------------------------------------------------------------------------------------

static int object_CreateVideoPlane(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	std::string FileName = lua_tostring(L,2);
	scene->Video->CreateVideoPlane (Name, FileName);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//1. Играть
static int object_PlayVideo(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	scene->Video->Play (Name);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//2. Пауза
static int object_PauseVideo(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	scene->Video->Pause (Name);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//3. Длинна файла в секундах
static int object_getLengthVideo(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	double Length = scene->Video->getLength (Name);
	//возврат параметра
	lua_pushnumber(L,Length);
	return 1;
}
//4. Перемотать на секунду ...
static int object_setReferenceTimeVideo(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	float ReferenceTime = lua_tonumber(L,2);
	scene->Video->setReferenceTime (Name, ReferenceTime);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//5. Позиция в секундах..
static int object_getReferenceTimeVideo(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	double ReferenceTime = scene->Video->getReferenceTime (Name);
	//возврат параметра
	lua_pushnumber(L,ReferenceTime);
	return 1;
}
//6. Установить громкость
static int object_setVolumeVideo(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	float volume = lua_tonumber(L,2);
	scene->Video->setVolume (Name, volume);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//7. Узнать громкость
static int object_getVolumeVideo(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	float Volume = scene->Video->getVolume (Name);
	//возврат параметра
	lua_pushnumber(L,Volume);
	return 1;
}
//8. Режим повтора
static int object_setLoopingModeVideo(lua_State *L)
{
	//получаем необходимые параметры
	std::string Name = lua_tostring(L,1);
	bool looping = lua_toboolean(L,2);
	scene->Video->setLoopingMode (Name, looping);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//-------------------------Перезагрузка другого сценария--------------------------------------------
static int NextLevel(lua_State *L)
{
	//получаем необходимые параметры
	std::string FileName = lua_tostring(L,1);
	ReloadScript=true;
	NewFileNameScript=FileName;
	return 0;
}
//-----------Выход / блокировка F10--------------------------------------------------------------
static int Exit(lua_State *L)
{
	//получаем необходимые параметры
	bool Abort = lua_toboolean(L,1);
	bool BlockF10 = lua_toboolean(L,2);

	localUser->BlockExit (BlockF10);
	if (Abort)
	{
		localUser->Exit();
	}
	return 0;
}
//-----------ScreenShot-------------------------------------------------------------
static int ScreenShot(lua_State *L)
{
	//получаем необходимые параметры
	std::string FileName = lua_tostring(L,1);
	localUser->takeScreenshot (FileName);
	return 0;
}

//-------------------------------------вершины-------------------------------------
static int object_RSetVertexPosition(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float num = lua_tonumber(L,2);
	float x = lua_tonumber(L,3);
	float y = lua_tonumber(L,4);
	float z = lua_tonumber(L,5);
	scene->Graphic->RSetVertexPosition (ObjectName,num,x,y,z);
	return 0;
}
static int object_RSetTexturePosition(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float dx = lua_tonumber(L,2);
	float dy = lua_tonumber(L,3);
	unsigned int UnitNum =0;
	if (lua_type(L,4)!=LUA_TNONE) UnitNum = lua_tonumber(L,4);
	scene->Graphic->RSetTexturePosition (ObjectName,dx,dy,UnitNum);
	return 0;
}
static int object_RSetTexturePointPosition(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float num = lua_tonumber(L,2);
	float dx = lua_tonumber(L,3);
	float dy = lua_tonumber(L,4);
	scene->Graphic->RSetTexturePointPosition (ObjectName,num,dx,dy);
	return 0;
}

//-----------------------------------Частицы-------------------------------------------
static int particle_Add(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	scene->Particles->AddParticle (ObjectName);
	return 0;
}

//время жизни частицы
static int particle_ParticleSetLifeTime(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float time = lua_tonumber(L,2);
	scene->Particles->ParticleSetLifeTime  (ObjectName,time);
	return 0;
}

//разброс размеров
static int particle_ParticleSetSizeRange(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float min = lua_tonumber(L,2);
	float max = lua_tonumber(L,3);
	scene->Particles->ParticleSetSizeRange (ObjectName,min,max);
	return 0;
}

//разброс прозрачности
static int particle_ParticleSetAlphaRange(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float min = lua_tonumber(L,2);
	float max = lua_tonumber(L,3);
	scene->Particles->ParticleSetAlphaRange  (ObjectName,min,max);
	return 0;
}


//разброс цвета
static int particle_ParticleSetColorRange(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float x1 = lua_tonumber(L,2);
	float r1 = lua_tonumber(L,3);
	float g1 = lua_tonumber(L,4);
	float b1 = lua_tonumber(L,5);
	float x2 = lua_tonumber(L,6);
	float r2 = lua_tonumber(L,7);
	float g2 = lua_tonumber(L,8);
	float b2 = lua_tonumber(L,9);
		scene->Particles->ParticleSetColorRange   (ObjectName,x1,r1,g1,b1,x2,r2,g2,b2);
	return 0;
}

//первоначальный радиус
static int particle_ParticleSetRadius(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float value = lua_tonumber(L,2);
	scene->Particles->ParticleSetRadius (ObjectName, value);
	return 0;
}

//масса
static int particle_ParticleSetMass(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float value = lua_tonumber(L,2);
	scene->Particles->ParticleSetMass (ObjectName, value);
	return 0;
}

//угол поворота проскостей
static int particle_ParticleSetAngle(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float x = lua_tonumber(L,2);
	float y = lua_tonumber(L,3);
	float z = lua_tonumber(L,4);
	scene->Particles->ParticleSetAngle  (ObjectName, x,y,z);
	return 0;
}

//текструра,  emissive? , освещаются или нет, текстурный юнит=0
static int particle_ParticleSetDefaultAttributes(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string FileName = lua_tostring(L,2);
	bool Emissive = lua_toboolean(L,3);
	bool Lighting = lua_toboolean(L,4);
	scene->Particles->ParticleSetDefaultAttributes   (ObjectName, FileName,Emissive,Lighting);
	return 0;
}

//Разброс количества генерируемых частиц в секунду
static int particle_ParticleSetRateRange(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float min = lua_tonumber(L,2);
	float max = lua_tonumber(L,3);
	scene->Particles->ParticleSetRateRange (ObjectName, min,max);
	return 0;
}

//Указание углов разброса по двум плоскастям (замечание Ассель)
static int particle_ParticleSetEmitterAngle(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float alpha1 = lua_tonumber(L,2);
	float alpha2 = lua_tonumber(L,3);
	float beta1 = lua_tonumber(L,4);
	float beta2 = lua_tonumber(L,5);
	scene->Particles->ParticleSetEmitterAngle (ObjectName,alpha1,alpha2,beta1,beta2);
	return 0;
}

//Указание радиуса сегмента, из которого появляются частицы (замечание Ассель)
static int particle_ParticleSetEmitterRadius(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float r1 = lua_tonumber(L,2);
	float r2 = lua_tonumber(L,3);
	scene->Particles->ParticleSetEmitterRadius (ObjectName , r1, r2);
	return 0;
}

static int particle_ParticleSetSpeed(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float r1 = lua_tonumber(L,2);
	float r2 = lua_tonumber(L,3);
	
	scene->Particles->ParticleSetVelocity (ObjectName , r1, r2);
	return 0;
}



//---------------------------------------------------------------------
static int SetVolumeTexture(lua_State *L)
{
	std::string ObjectName = lua_tostring(L,1);
	scene->Graphic->AddVolumeTexture (ObjectName);
	return 0;
}
//---------------------------------------------------------------------

//строит BoundingBox
static int object_Select(lua_State *L)
{
	std::string ObjectName = lua_tostring(L,1);
		std::string MethodName = lua_tostring(L,2);
		scene->Graphic->SelectObject(ObjectName, MethodName);
	return 0;
}

// Возвращает координаты рамки объекта (minx, miny, minz, maxx, maxy, maxz).
static int object_GetBoundingBox(lua_State *L)
{
	std::string ObjectName = lua_tostring(L,1);
	bool global = lua_toboolean(L,2);
	float* res = scene->Graphic->GetBoundingBox(ObjectName, global);
	int count = 9;
	for (int i = 0; i < count; i++)
		lua_pushnumber(L, res[i]);
	
	return count;
}

//------------------------------------------------BASE3DOBJECT-------------------------------------
//Функция для создания 3D объектов
static int player_New3DObject(lua_State *L)
{
	std::cout << "Player: create OLD 3d object" << std::endl;
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	std::string FileName = lua_tostring(L,2);
	//
	scene->OLD->NewObject (ObjectName,FileName);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

//Функция задает координаты точки привязки объекта, для дальнейшей анимации
static int object_SetCenterPosition(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float x = lua_tonumber(L,2);
	float y = lua_tonumber(L,3);
	float z = lua_tonumber(L,4);
	scene->OLD->objects[ObjectName]->SetCenterPosition(x,y,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Функция задает координаты объекта
static int object_SetPosition(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float x = lua_tonumber(L,2);
	float y = lua_tonumber(L,3);
	float z = lua_tonumber(L,4);
	scene->OLD->objects[ObjectName]->SetPosition(x,y,z);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Функция задает видимость объекта
static int object_SetVisible(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	bool val = lua_toboolean(L,2);
	scene->OLD->objects[ObjectName]->SetVisible(val);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Функция задает масштабирование объекта
static int object_SetScale(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float dx = lua_tonumber(L,2);
	float dy = lua_tonumber(L,3);
	float dz = lua_tonumber(L,4);
	scene->OLD->objects[ObjectName]->SetScale(dx,dy,dz);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Функция задает простое вращение объекта 3D объекта
static int object_SetRotate(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
	float angle = lua_tonumber(L,2);
	float dx = lua_tonumber(L,3);
	float dy = lua_tonumber(L,4);
	float dz = lua_tonumber(L,5);
	scene->OLD->objects[ObjectName]->SetRotate(angle,dx,dy,dz);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}
//Функция задает сложное перемещение  3D объекта
static int object_SetRotate2(lua_State *L)
{
	//получаем необходимые параметры
	std::string ObjectName = lua_tostring(L,1);
		float angle1 = lua_tonumber(L,2);
	float dx1 = lua_tonumber(L,3);
	float dy1 = lua_tonumber(L,4);
	float dz1 = lua_tonumber(L,5);
		float angle2 = lua_tonumber(L,6);
	float dx2 = lua_tonumber(L,7);
	float dy2 = lua_tonumber(L,8);
	float dz2 = lua_tonumber(L,9);
		float angle3 = lua_tonumber(L,10);
	float dx3 = lua_tonumber(L,11);
	float dy3 = lua_tonumber(L,12);
	float dz3 = lua_tonumber(L,13);
	scene->OLD->objects[ObjectName]->SetRotate2(angle1,dx1,dy1,dz1,angle2,dx2,dy2,dz2,angle3,dx3,dy3,dz3);
	//возврат параметра
	lua_pushnumber(L,333);
	return 1;
}

}
