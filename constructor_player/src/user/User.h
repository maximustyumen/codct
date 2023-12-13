#ifndef USERCLASS
#define USERCLASS

#include "../Configure/Configure.h"

#include "./output/Output.h"
#include "./input/Input.h"

#include <osgUtil/IntersectVisitor>

#include "UserFoot.h"
#include "UserHands.h"

#include "HandsDataClass.h"


class UserClass
{
public:
	UserClass(Configure* configuration, osg::ref_ptr<osgViewer::Viewer> viewer = 0);
	~UserClass();
	//метод обновления состояния пользователя (позиция, что видит и т.д.) Вызывается постояно в цикле
	void update();
	void setUserFocusLight(float focus = 50.0f);
	void SetUserPositionAndOrientation(float x,float y, float z, float hx, float hy, float hz);
	void setDataToVideoViewer(osg::ref_ptr<osg::Node>, osg::Node* menu_node);
        //setCompileOnNextDraw
        void setCompileOnNextDraw();
        //Полное кеширование сцены (уже после подключения к Viewer'у)
        void FullCompileScene(bool);
        //замена значения текста рядом с курсором
        void SetCursorText(std::string);
        void SetCursorTextSize (int newsize);
        void SetCursorTextFont (std::string filename);
        void SetCursorTextColor (float r,float g ,float b ,float a);

	void frame();
        //
	bool isWork();
	//Выход из тренажера
	void Exit();
	//блокировка выхода (например F10)
	void BlockExit(bool block);
	//screenshot
	void takeScreenshot(std::string);
	//установка режима управления
	//1 - просмотр 2- управление 3- перемещение
	void setInputMode(bool, bool, bool);
	//возврат копии буфера накопленных событий во внутреннем формате (на интерпритацию)...
	InternalEventsClass getHandsEvents();
	InternalEventsClass getFootEvents();
	InternalEventsClass getKeysEvents();
	//Возврат состояния джойстиков
	JoysticksData GetJoysticksData();
	//возврат значений ширины/высоты экрана наблюдателя
	int getViewHeight();
	int getViewWidth();
	//возврат значений по текущему объекту управления
	std::string GetCurrentObjectName();
	osg::Vec3 GetCurrentObjectIntersectsCoordLOCAL();
	osg::Vec3 GetCurrentObjectIntersectsCoordWORLD();

	osg::Camera* get_camera();

	//Изменение текста спец. HUD
        std::string updateHUDText(std::string, bool show=true);
	bool hideHUDText;
	bool developmode;
	//
	//поворот головы персонажа
	float HeadX, HeadY, HeadZ;
	//желаемые координаты персонажа
	float BodyX, BodyY, BodyZ;
	//zoom 1..10
	float BodyZoom;
	//значение фокусного расстояния из файла конфигурации
	float StandartFocus;
	//флаг для отодвигания сцены для ее "прогрузки"
	bool preLoading;
	//staticstic
	double UserInput_T;
	double UserCalc_T;
	double Rendering_T;
	double UpdateAudio_T;
	
private:
	UserOutputClass* UserOutput;
	UserInputClass* UserInput;

	Configure* configuration;
	
	//текущие координаты персонажа
	float oldBodyX, oldBodyY, oldBodyZ;
	//
	HandsData Hand;
	//
	InternalEventsClass FootEvents;
	//
	float HumanHeight;
	//выдерживаемая дистанция до объекта (используется в CheckIntersect)
	float Distance;
	//ноги
	UserFootClass* userFoot;
	//руки
	UserHandsClass* userHands;
	//
	void UpdateCamera();
	//
	bool initialize;
	bool m_oculus_enable;
	//Стартовые позиция наблюдателя
	float StartX,StartY,StartZ, StartHeadX, StartHeadY, StartHeadZ;
	//флаг разрешения режима просмотра (изменения направления взгляда)
	bool viewModeEnable;
	//флаг разрешения режима управления
	bool commandModeEnable;
	//флаг разрешения обработки перемещения пользователя с помощью прокрутки мыши
	bool moveModeEnable;
        //
        //транслятор кодов клавиш
        KeyTranslation *tranlator;
};

#endif

