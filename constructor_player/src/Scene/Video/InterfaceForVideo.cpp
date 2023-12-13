/***************************************************************************
InterfaceForHUD.cpp  -  интерфейс для работы с видео файлами/потоками (quicktime)
-------------------
begin                : 31 марта 2023
modify               : 25 апреля 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./InterfaceForVideo.h"

//конструктор
InterfaceForVideo::InterfaceForVideo(osg::ref_ptr<osg::Group> _root)
{
	std::cout << "InterfaceForVideo constructor ... " ;

	 //#ifndef WIN32
		// support XINE for LINUX
		std::cout << " use ffmpeg .. " << std::endl;
		osgDB::Registry::instance()->addFileExtensionAlias("","ffmpeg"); //xine
		osgDB::Registry::instance()->addFileExtensionAlias("mov","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("avi","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("mpg","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("flv","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("mpv","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("dv","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("mp4","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("m4v","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("m4v","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("/dev/video0","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("video0","ffmpeg");
		osgDB::Registry::instance()->addFileExtensionAlias("live","ffmpeg");
		// /dev/video0
/*
	#else
		// support QuickTime for Windows/MacOS X
		std::cout << " use QUICKTIME .. " << std::endl;
		osgDB::Registry::instance()->addFileExtensionAlias("live","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("mov","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("avi","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("mpg","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("flv","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("mpv","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("dv","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("mp4","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("m4v","qt");
		osgDB::Registry::instance()->addFileExtensionAlias("m4v","qt");
	#endif
	*/


	//osgDB::readImageFile("devices.live");



	root = _root;
}
//
InterfaceForVideo::~InterfaceForVideo()
{
	std::cout << "Destructor InterfaceForVideo."  << std::endl;
	Reset();
}

//сброс всего, что на Video
bool InterfaceForVideo::Reset()
{
	return true ;
}



//прикрепляет к объекту видеофайл (QuickTime MOV)
bool InterfaceForVideo::CreateVideoPlane(std::string Name, std::string MovieFileName)
{
	try
	{
	//проверка на совпадение имен с уже имеющимися
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(Name);
		root->accept(*fnnv.get());
	if (fnnv->_foundNodes.empty()!=true)
	{
			 throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForVideo::CreateVideoPlane()", "Add new Video: Name already use:  " + Name, 3));
	}
	osg::ref_ptr<BaseVideoObject> newVideo = new BaseVideoObject(Name, MovieFileName);
	root->addChild(newVideo);
	return true;
	}
	catch (ErrorClass error)
	{
		error.print();
	}
}

//1. Играть
void InterfaceForVideo::Play(std::string name)
{
	FindVideoObject (name)->Play();
}

//2. Пауза
void InterfaceForVideo::Pause(std::string name)
{
	FindVideoObject (name)->Pause();
}

//3. Длинна файла в секундах
double InterfaceForVideo::getLength(std::string name)
{
	return FindVideoObject (name)->getLength();
}

//4. Перемотать на секунду ...
void InterfaceForVideo::setReferenceTime(std::string name, double time)
{
	FindVideoObject (name)->setReferenceTime(time);
}

//5. Позиция в секундах..
double InterfaceForVideo::getReferenceTime(std::string name)
{
	return FindVideoObject (name)->getReferenceTime();
}

//6. Установить громкость
void InterfaceForVideo::setVolume(std::string name, float volume)
{
	FindVideoObject (name)->setVolume(volume);
}

//7. Узнать громкость
float InterfaceForVideo::getVolume(std::string name)
{
	return FindVideoObject (name)->getVolume();
}

//8. Режим повтора
void InterfaceForVideo::setLoopingMode(std::string name, bool loop)
{
	FindVideoObject (name)->setLoopingMode(loop);
}

//--------------------------------------------------------------------------------------------------------
//сервисная функция
osg::ref_ptr<BaseVideoObject> InterfaceForVideo::FindVideoObject(std::string name)
{
	try
	{
	//ищем в графе сцены
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(name);
		root->accept(*fnnv.get());
	//
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << name << std::endl;
		//попытка преобразовать в BaseSoundObject
		if ( BaseVideoObject* _foundVideoObject = dynamic_cast<BaseVideoObject*>( fnnv->_foundNodes.front().get() ) )
		{
			osg::ref_ptr<BaseVideoObject> foundVideoObject =  _foundVideoObject;
			return foundVideoObject;
		}
				throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForVideo::FindVideoObject()", "Object  - NOT video! " + name, 3));
	}
	else
	{
			throw (ErrorClass (__FILE__, __LINE__ , "InterfaceForVideo::FindVideoObject()", "Find objects in Root - FALSE!  " + name, 3));
	}
	}
	catch (ErrorClass error)
	{
		error.print();
	}
}
//--------------------------------------------------------------------------------------------------------
