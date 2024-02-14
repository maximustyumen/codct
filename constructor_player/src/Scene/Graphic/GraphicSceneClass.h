#ifndef GRAPHICSCENECLASS
#define GRAPHICSCENECLASS

#include <stdlib.h>
#include <stdio.h> 
#include <iostream>
#include <osg/Version>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#ifdef WIN32
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#endif

#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osg/ComputeBoundsVisitor>
#include <osgUtil/IntersectVisitor>
#include <osgDB/FileUtils>
#include <osg/ShapeDrawable>
#include <osg/Texture3D>
#include <osg/TexGen>
#include <osg/PolygonMode>
#include <osg/Drawable>
#include <osg/LineWidth>
#include <osg/AutoTransform>
#include <osg/BlendFunc>


#include <osg/KdTree>


#include "../Visitors/FindNodeByName.h"
#include "../Visitors/FindTexture.h"
#include "../Visitors/FindAnimationByName.h"
#include "../Visitors/RenameAllNode.h"
#include "../Visitors/FindImageStreamsVisitor.h"
#include "../Visitors/FindGeometry.h"
#include "../Visitors/FindLightByName.h"
#include "../Visitors/CcalculateBoundingBox.h"
#include "../Visitors/ChangeShaderUniformVisitor.h"

//QT!!!
//#define QTBUG
#ifdef QTBUG
    #include "../Visitors/FindAndUnrefTexture.h"
#endif


//Обработка критических ошибок
#include "../../error/errorclass.h"


//для Volume rendering
#include "./GraphicContext.h"

#include <sstream>
//#include "../../observer.h"
#include "../../observer.h"

#include "GraphicSceneClassObserver.h"

//
//#include "./Helpers/CopyHelpers.h"

//#define SHADOW true

class GraphicSceneClass : public Base::HasObserver<GraphicSceneClassObserver>
{
public:
	GraphicSceneClass(osg::ref_ptr<osg::Group> _root);
	virtual ~GraphicSceneClass();
	//------------------------------3d objects----------------------------------------
	//загрузка OSG файла
	bool LoadGraphicFile(std::string);
	//смещение указанного matrixtransform'a
	bool RSetPosition(std::string , float, float, float);
	//очистка всех загруженных функцией LoadGraphicFile данных ...
	bool Reset(); //scene->LoadGraphicFile("d:\\temp\\1.osg"); //scene->Reset();
	//Функция совмещения точки привязки одного объекта с другим
	bool RSetAt(std::string, std::string);
	//Функция выборочного сброса матриц позиции/поворота/масштаба
	bool RResetObject(std::string,bool,bool,bool);
	//альтернативное  вращение 3D объекта
	bool RSetRotate(std::string , float, float, float);
	//альтернативное cложное вращение объекта
	bool RSetRotate2(std::string ObjectName,float angle1, float x1, float y1, float z1, float angle2, float x2, float y2, float z2,float angle3, float x3, float y3, float z3);
	//альтернативное перемещение относительно другого объекта по абсолютным координатам
	bool ASetAt(std::string, std::string);
	//альтернативное задание видимости объекта
	bool RSetVisible (std::string,bool);
	//альтернативное масштабирование объекта
	bool RSetScale(std::string , float, float, float);
	//альтернативная передача x,y,z объекта
	bool RGetPosition(std::string, float &, float&, float&);
	//альтернативная передача x,y,z объекта - абсолютные координаты
	bool AGetPosition(std::string, float &, float&, float&);
	//альтернативное получение масштаба объекта
	bool RGetScale(std::string, float &, float&, float&);
	//альтернативное получение квантериона вращения объекта
	bool RGetRotate(std::string, float &, float&, float&, float&);
	//возвращает предка (точнее ближайшего предка имеющего имя)
	std::string  GetParent (std::string);
	//--------------------------рисование----------------------------------------------------
	//альтернативное рисование на текстуре объекта
	bool RTextureDraw(std::string, int, int, float,float,float,float,bool,unsigned int UnitNumber=0);
	//альтернативная замена текстуры  объекта
	bool RChangeTexture(std::string, std::string,unsigned int UnitNumber=0);
        // замена текстуры через streamstring
        bool RChangeTexture( const std::string&, std::stringstream&, const std::string&, unsigned int UnitNumber=0 );
	//альтернативная Установка цвета объекта
	bool RSetColor(std::string, float,float,float,float);
	//-----------------------------Иерархия--------------------------------------------------
	bool RConnectObjectTo(std::string, std::string);
	//Клонирование объекта
	bool CloneObject(std::string, std::string, std::string);
	//Удаление объекта из сцены
	bool UnloadObject(std::string);
	//---------------------------Запросы пересечения----------------------------
	//Тест на пересечение 2 объектов в относительных координатах
	bool RTestIntersects(std::string, std::string, float);
        //Тест на пересечение 2 объектов в относительных координатах
        bool RTestIntersects2(std::string, std::string, float);
        //Тест на пересечение линией
	bool RTestLineIntersects(float,float,float,float,float,float, std::string, std::string &, float &, float &, float &, float &, float &, float &);
	//-----------------------------Анимация------------------------------------------
	//установка паузы анимации, снятие паузы
	bool SetPauseAnimation(std::string, bool, bool);
	//Переход на нужное время анимации
	bool GotoAnimation(std::string, float, bool);
	//Установка опорного смещения анимации
	bool SetPivotPointAnimation(std::string, float,float,float);

    //---------------------------------------morph------------------------------------
    osg::MatrixTransform* find_matrix_transform( const std::string& str_obj_name );
    // создание morph-объекта
    void CreateMorph( const std::string& str_morph, const std::string& str_default_state, const std::vector<std::string>& v_str_target_states );
    // переход в нужное morph-состояние
    void GotoMorphState( const std::string& str_morph, const std::vector<float>& v_weights );
    //
	//
	//--------------------------шейдеры2------------------------------
	bool ChangeShaderUniform(std::string,std::string,float);

		
		//---------------------------------------КАНДИДАТЫ/ЭКСПЕРИМЕНТЫ---------------------



	//---------------------------------------ТЕНИ-------------------------------------
	//Создание отдельной группы теней, параметр - имя группы
	bool AddShadowedGroup(std::string);
	//Определение маски Имя объекта, 4-ничего, 1-принимать, 2-бросать, 3 - и то и другое.
	bool SetShadowMask(std::string, int);
	//--------------------------------------------------------------------------------
	//Тестовые функции для Volume rendering
	bool AddVolumeTexture(std::string objectname);
	osg::StateSet* createVolumeTextureState();
	//-------------------------------Вершины---------------------------------------
	//передвигает одну вершину (i) на dx,dy,dz
	bool RSetVertexPosition(std::string ObjectName, unsigned int i, float dx, float dy, float dz);
	//передвигает всю текстуру  на dx,dy
	bool RSetTexturePosition(std::string ObjectName, float dx, float dy, unsigned int UnitNumber=0);
	//передвигает одну координату текстуры (i) на dx,dy
	bool RSetTexturePointPosition(std::string ObjectName, unsigned int i, float dx, float dy);
    //---------------------------------------------------------------------------------
    //"Выделение объекта" для конструктора
    bool SelectObject(std::string, std::string method = "BoundingBox");
    // Получить границы рамки объекта.
    float* GetBoundingBox(std::string ObjectName, bool global);
	void CreateBrowser(std::string ObjectName);

private:
	//Главная группа сцены
	//#define SHADOW = true;
	#ifndef SHADOW
		osg::ref_ptr<osg::Group> root;
	#else
		osgShadow::ShadowedScene* root;
		int ReceivesShadowTraversalMask;
		int CastsShadowTraversalMask;
	#endif
	//osg::StateSet* rootStateSet;
	//загруженные OSG файлы
	//std::vector < osg::ref_ptr<osg::Node> > loadedfiles;

};

#endif








