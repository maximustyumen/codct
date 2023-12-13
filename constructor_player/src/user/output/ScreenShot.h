// Взыто из файла viewer.cpp
#include <osgDB/WriteFile>

class SnapImageDrawCallback : public osg::Camera::DrawCallback
{
public:

	SnapImageDrawCallback()
	{
		_snapImageOnNextFrame = false;
	}

	void setFileName(const std::string& filename) { _filename = filename; }
	const std::string& getFileName() const { return _filename; }

	void setSnapImageOnNextFrame(bool flag) { _snapImageOnNextFrame = flag; }
	bool getSnapImageOnNextFrame() const { return _snapImageOnNextFrame; }

	virtual void operator () (const osg::Camera& camera) const
	{
		//osg::notify(osg::NOTICE) << "Saved screen image to `"<<_filename<<"`"<< std::endl;
		if (!_snapImageOnNextFrame) return;
        
		int x,y,width,height;
		x = camera.getViewport()->x();
		y = camera.getViewport()->y();
		width = camera.getViewport()->width();
		height = camera.getViewport()->height();

		osg::ref_ptr<osg::Image> image = new osg::Image;
		image->readPixels(x,y,width,height,GL_RGB,GL_UNSIGNED_BYTE);

		if (osgDB::writeImageFile(*image,_filename))
		{
			std::cout  << "Saved screen image to `"<<_filename<<"`"<< std::endl;
		}

		_snapImageOnNextFrame = false;
	}

protected:
    
	std::string _filename;
	mutable bool        _snapImageOnNextFrame;

    
};