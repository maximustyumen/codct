#ifndef USERHANDSCLASS
#define USERHANDSCLASS

#include <osgUtil/IntersectVisitor>
#include <osgViewer/Viewer>
#include <osg/AutoTransform>
#include <osg/Math>

#include "FootDataClass.h"
#include "HeadDataClass.h"
#include "HandsDataClass.h"
#include "HtcViveDataClass.h"

#include "./events/InternalEventClass.h"

#include "../Scene/Visitors/FindNodeByName.h"

//Обработка критических ошибок
#include "../error/errorclass.h"

#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osgText/Font>
#include <osgText/Text>



class UserHandsClass
{
public:
	//конструктор
	UserHandsClass(osg::ref_ptr<osg::Node> _root, osg::ref_ptr<osgViewer::Viewer>, bool oculus_enable);
	//процедура расчета положение рук (курсора) и генерация соотв. событий
	//в параметрах данные по состоянию рук, т.к. координаты указывались при текущем положении камеры, текущие координаты пользователя и его ориентация
        void setDataAndCalc(HandsData);
	//возврат копии буфера накопленных событий во внутреннем формате (на интерпритацию)...
	InternalEventsClass getHandsEvents();
	//инфо о текущем объекте управления
	std::string GetCurrentObjectName();
	std::string GetParentObjectName();
	osg::Vec3 GetCurrentObjectIntersectsCoordLOCAL();
	osg::Vec3 GetCurrentObjectIntersectsCoordWORLD();

    //замена значения текста рядом с курсором
    void SetCursorText(std::string value);
    void SetCursorTextSize (int newsize);
    void SetCursorTextFont (std::string filename);
    void SetCursorTextColor (float r,float g ,float b ,float a);

	void UpdateVRHands(HtcViveData data, float x, float y, float z);
	/// обновить положение 3D худа
	void update3DHUD(osg::Matrixd m);

	osg::Node* get_cursor_node() {	return cursorForHud.get();	}

private:

	void CreateHydraHands();
	float hydra_temp_x, hydra_temp_y;
	int hydra_temp_command;
	int old_hydra_temp_command;
	int hydra_temp_command2;
	bool hydra_presend;
	void RotateFinger (std::string name, float angle);
	
	osg::ref_ptr<osg::MatrixTransform> cursorHand;
	osg::ref_ptr<osg::MatrixTransform> cursorHand2;
	osg::ref_ptr<osg::MatrixTransform> cursorHandPalez;

	//указатель на группу сцены
	osg::ref_ptr<osg::Node> root;
	//события, происходящие при рукоблудии)
	std::vector <InternalEventsClass::OneInternalEventClass> events;
	//
	void AddUserHandsEvent(std::string val,float a1,float a2, float a3, float a4);
	//функция изменяет значения переменных CurrentObjectname, ParentObjectName, координыты пересечения и т.д.
	void GetObjectInfoFromXY(float x, float y);
	osg::ref_ptr<osgViewer::Viewer> viewer;
	std::string CurrentObjectName, ParentObjectName, oldCurrentObjectName;
	//координаты пересечения курсора в локальных координатах объекта
	osg::Vec3 CurrentObjectIntersectsCoordLOCAL;
	//координаты пересечения курсора в глобальных координатах мира
	osg::Vec3 CurrentObjectIntersectsCoordWORLD;
	//нормали пересечения курсора в глобальных координатах мира
	osg::Vec3 CurrentObjectIntersectsNormalWORLD;
	//
        osg::ref_ptr<osg::MatrixTransform> cursor;
		
        osg::ref_ptr<osg::MatrixTransform> cursorForHud;
        osg::ref_ptr<osgText::Text> updateText;
        osg::ref_ptr<osg::MatrixTransform> textForCursor;
	//прозрачный объект
	std::string TransparentObjectsName;
	//
	bool HUDCursorEnabled;
	//
	float WINDOW_WIDTH;
	float WINDOW_HEIGHT;
	//
        float START_WINDOW_WIDTH;
        float START_WINDOW_HEIGHT;
        //
	std::string GetName(osgUtil::LineSegmentIntersector::Intersections::iterator hitr);

        //создает камеру (hud) и размещает там курсор (для Hud) и "курсорный текст"
        void CreateHudCursorAndText();
        osg::ref_ptr<osg::Node> CreateCursorShape();
        //FIX для horizontal split mode
        bool HSmode;

		osg::Camera* createHUDCamera( double left, double right, double bottom, double top );


	osg::ref_ptr<osg::Camera> m_camera;


	//3d hud menu
	osg::ref_ptr<osg::MatrixTransform> hud3D;
};

#endif

