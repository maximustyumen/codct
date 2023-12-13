#ifndef BASEVIDEOOBJECTCLASS
#define BASEVIDEOOBJECTCLASS

#include <iostream>

#include <osgViewer/Viewer>
#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
//#include <osg/Options>


#include "../Visitors/FindImageStreamsVisitor.h"

//Обработка критических ошибок
#include "../../error/errorclass.h"


class BaseVideoObject: public osg::MatrixTransform
{
public:
	//конструктор
	BaseVideoObject(std::string Name, std::string MovieFileName);
	//деструктор
	~BaseVideoObject();

	//1. Играть
	void Play();
	//2. Пауза
	void Pause();
	//3. Длинна файла в секундах
	double getLength();
	//4. Перемотать на секунду ...
	void setReferenceTime(double time);
	//5. Позиция в секундах..
	double getReferenceTime() ;
	//6. Установить громкость
	void setVolume(float volume);
	//7. Узнать громкость
	float getVolume();
	//8. Режим повтора
	void setLoopingMode(bool loop);
	
	/*
        virtual void setTimeMultiplier(double) {}
        virtual double getTimeMultiplier() const { return 0.0; }
	*/


private:
	//создает поверхность для Video
	osg::ref_ptr <osg::Geometry> myCreateTexturedQuadGeometry(const osg::Vec3& pos, osg::ref_ptr <osg::Image> image);
	osg::ref_ptr <osg::ImageStream> ImageStreamTV;
	osg::ref_ptr <osg::Image> ImageTV;

	std::string MovieFileName;
};


#endif
