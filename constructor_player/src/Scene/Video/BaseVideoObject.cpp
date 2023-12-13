/***************************************************************************
BaseVideoObject.cpp  -  описание класса видео объекта
-------------------
begin                : 29 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./BaseVideoObject.h"

//конструктор
BaseVideoObject::BaseVideoObject(std::string Name, std::string _MovieFileName)
{

    //тест
    //std::cout << "*** VT start:" << _MovieFileName << ":" << std::endl;


    try
    {
	//запоминаем имя файла
	MovieFileName = _MovieFileName;

        /*
        //osgDB::ReaderWriter::Options * options = new ReaderWriter::Options;
        >  options->setPluginStringData("captureWantedWidth", "800");
        > options->setPluginStringData("captureWantedHeight", "600");
        > options->setPluginStringData("captureWantedFps", "30");
        > options->setPluginStringData("captureVideoDevice", "USB Video Device" );
        > options->setPluginStringData("captureSoundDevice", "");
        > then
        > osgDB::readImageFile("capture.directshow", options)
        */

        //FIX странный баг, если файл не найден
        if (std::ifstream(MovieFileName.c_str()).good() == false)
        {
            throw (ErrorClass (__FILE__, __LINE__ , "BaseVideoObject::BaseVideoObject()", "File not found: " + MovieFileName, 3));
        }

        ImageTV = osgDB::readImageFile(MovieFileName);
        //ImageTV = osgDB::readImageFile("/home/maximum2000/LABS/SuperNewTG/video/utz9.mov");


	if (!ImageTV->valid())
	{
            throw (ErrorClass (__FILE__, __LINE__ , "BaseVideoObject::BaseVideoObject()", "File not load: " + MovieFileName, 3));
	}


        ImageStreamTV = dynamic_cast<osg::ImageStream*>(ImageTV.get());
        if (ImageStreamTV->valid())
        {
            //установить режим
            ImageStreamTV->setLoopingMode(osg::ImageStream::NO_LOOPING);
        }


        if (ImageTV->valid())
        {
            osg::ref_ptr<osg::Geode> geode = new osg::Geode;
            osg::StateSet* stateset = geode->getOrCreateStateSet();
            stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            osg::Vec3 pos(0.0f,0.0f,0.0f);
            geode->addDrawable(myCreateTexturedQuadGeometry(pos,ImageTV));
            //
            addChild(geode.get());
            setName (Name);
        }


    }
    catch (ErrorClass error)
    {
        error.print();
    }



}

//деструктор
BaseVideoObject::~BaseVideoObject()
{
	std::cout << "Destructor BaseVideoObject: " << getName() << " file: " << MovieFileName <<  std::endl;
}

//1. Играть
void BaseVideoObject::Play()
{
	if (ImageStreamTV->valid()) 
	{
		ImageStreamTV->play();
	}
}

//2. Пауза
void BaseVideoObject::Pause()
{
	if (ImageStreamTV->valid()) 
	{
		ImageStreamTV->pause();
	}
}

//3. Длинна файла в секундах
double BaseVideoObject::getLength()
{
	if (ImageStreamTV->valid()) 
	{
		return ImageStreamTV->getLength();
	}
	return 0;
}

//4. Перемотать на секунду ...
void BaseVideoObject::setReferenceTime(double time)
{
	if (ImageStreamTV->valid()) 
	{
		ImageStreamTV->setReferenceTime (time);
	}
}

//5. Позиция в секундах..
double BaseVideoObject::getReferenceTime() 
{
	if (ImageStreamTV->valid()) 
	{
		return ImageStreamTV->getReferenceTime();
	}
	return 0;
}

//6. Установить громкость
void BaseVideoObject::setVolume(float volume)
{
	if (ImageStreamTV->valid()) 
	{
		ImageStreamTV->setVolume(volume);
	}
}

//7. Узнать громкость
float BaseVideoObject::getVolume()
{
	if (ImageStreamTV->valid()) 
	{
		return ImageStreamTV->getVolume();
	}
	return 0;
}

//8. Режим повтора
void BaseVideoObject::setLoopingMode(bool loop)
{
	if (ImageStreamTV->valid()) 
	{
		if (loop==false)
		{
			ImageStreamTV->setLoopingMode(osg::ImageStream::NO_LOOPING);
		}
		else
		{
			ImageStreamTV->setLoopingMode(osg::ImageStream::LOOPING);
		}
	}
}

//----------------------------------
osg::ref_ptr<osg::Geometry> BaseVideoObject::myCreateTexturedQuadGeometry(const osg::Vec3& pos, osg::ref_ptr<osg::Image> image)
{
    bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
	osg::ref_ptr<osg::Geometry> pictureQuad = osg::createTexturedQuadGeometry(pos,
                                       osg::Vec3(image->s(),0.0f,0.0f),
                                       osg::Vec3(0.0f,0.0f,image->t()),
                                       0.0f, flip ? image->t() : 0.0, image->s(), flip ? 0.0 : image->t());

    osg::ref_ptr<osg::TextureRectangle> texture = new osg::TextureRectangle(image);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
    return pictureQuad.get();
}
