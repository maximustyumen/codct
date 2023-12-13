#ifndef MAXPLAYER_OPEN_POST_PROCESSING
#define MAXPLAYER_OPEN_POST_PROCESSING

#include <osgPPU/Processor.h>
#include <memory>

namespace osgViewer { class Viewer; }
namespace osg
{
	class Texture2D;
	class Camera;
	class Node;
}

namespace osgGA
{
	class GUIEventHandler;
}

//=====================================================================================//
//                                class PostProcessing                                 //
//=====================================================================================//
class PostProcessing
{
private:
	osg::ref_ptr<osgPPU::Processor> m_processor;
	osgViewer::Viewer* m_viewer;
	osg::ref_ptr<osgGA::GUIEventHandler> m_event_resize;
	osg::Camera::RenderTargetImplementation m_rt_impl;
	osg::Group* m_root;

public:
	PostProcessing(const std::string& ppu_filename, osgViewer::Viewer* viewer, osg::Node* node);
	virtual ~PostProcessing();

public:
	/// обновить
	void update();

private:
	/// создать рендер таргет
	osg::ref_ptr<osg::Texture2D> createRenderTarget(int width, int height);
	/// создать текстуру глубины
	osg::ref_ptr<osg::Texture2D> createDepthTarget(int width, int height);
	/// настроить камеру
	void setupCamera(osg::Camera* camera);
	/// обновить глобальные переменные
	void updateGlobalUniforms();
	
};

#endif