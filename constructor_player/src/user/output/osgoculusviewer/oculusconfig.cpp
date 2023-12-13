#include "oculusconfig.h"
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/Program>
#include <osg/Shader>

OculusConfig::OculusConfig(osgViewer::Viewer* viewer,osg::Node* scene_node,osg::Node* menu_node,osg::Node* cursor2d_node)
: m_shader_program(create_shader())
, m_dev(new OculusDevice)
, m_near_clip(0.3f)
, m_far_clip(5000.f)
, m_order_value(10)
{
	m_dev->setNearClip(m_near_clip);
	m_dev->setFarClip(m_far_clip);
	m_dev->setSensorPredictionEnabled(true);
	m_dev->setSensorPredictionDelta(0.03f);

	unsigned int height = m_dev->vScreenResolution();
	unsigned int width = m_dev->hScreenResolution();

	osg::Camera* camera = viewer->getCamera();

	osg::ref_ptr<osg::GraphicsContext> gc = create_gc(width,height);
	configure_main_camera(camera,gc,width,height);

	osg::Camera* l_camera = configure_eye(viewer,OculusDevice::LEFT_EYE,gc,scene_node,menu_node,cursor2d_node,0,width,height);
	osg::Camera* r_camera = configure_eye(viewer,OculusDevice::RIGHT_EYE,gc,scene_node,menu_node,cursor2d_node,width/2,width,height);

	camera->setDataVariance(osg::Object::DYNAMIC);
	camera->setUpdateCallback(new OculusViewConfigOrientationCallback(l_camera, r_camera, m_dev));
}
OculusConfig::~OculusConfig()
{
}
/// настроить вывод
osg::Camera* OculusConfig::configure_eye(osgViewer::Viewer* viewer,OculusDevice::EyeSide eye,osg::ref_ptr<osg::GraphicsContext> gc,
	osg::Node* scene_node,osg::ref_ptr<osg::Node> hud_node,osg::Node* cursor2d_node,unsigned int x_viewport,unsigned int width,unsigned int height)
{
	/// 1) create render target
	const int rtt_width  = m_dev->scaleFactor() * width/2;
	const int rtt_height = m_dev->scaleFactor() * height;

	osg::ref_ptr<osg::Texture2D> rtt = create_rtt_texture(rtt_width,rtt_height,osg::Texture2D::LINEAR);

	/// 2) create camera to render main scene
	osg::ref_ptr<osg::Camera> scene_camera = create_scene_camera(gc,rtt);
	scene_camera->addChild(scene_node);

	/// 3) create camera to render ortho 2d hud
	//osg::ref_ptr<osg::Camera> hud_camera = create_hud_camera(gc,rtt,width,height);
	//hud_camera->addChild(hud_node);

	//osg::ref_ptr<osg::Camera> cursor2d_camera = create_hud_camera(gc,rtt,width,height);
	//cursor2d_camera->addChild(cursor2d_node);

	/// 4) create post processing quad and camera
	osg::ref_ptr<osg::Camera> post_camera = create_post_camera(gc);
	osg::ref_ptr<osg::Geode> post_quad = create_post_quad();
	post_camera->addChild(post_quad);
	post_camera->setViewport(new osg::Viewport(x_viewport,0,width/2,height));

	/// 5) connect post processing shader
	connect_shader(post_quad,post_camera,rtt,eye);

	/// 6) add cameras as slave
	viewer->addSlave(scene_camera,m_dev->projectionOffsetMatrix(eye), m_dev->viewMatrix(eye),false);
	//viewer->addSlave(hud_camera,false);
	//viewer->addSlave(cursor2d_camera,false);
	viewer->addSlave(post_camera,false);
	return scene_camera;
}
/// создать контекст рендера
osg::ref_ptr<osg::GraphicsContext> OculusConfig::create_gc(int width, int height) const
{
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowDecoration = false;
	traits->x = 0;
	traits->y = 0;
	traits->width = width;
	traits->height = height;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->sampleBuffers = true;
	traits->samples = 4;
	traits->vsync = true;
	return osg::GraphicsContext::createGraphicsContext(traits);
}
/// настроить главную камеру
void OculusConfig::configure_main_camera(osg::Camera* camera,osg::GraphicsContext* gc,int width, int height) const
{
	camera->setProjectionMatrix(m_dev->projectionCenterMatrix());
	camera->setViewport(0,0,width,height);
	camera->setGraphicsContext(gc);
}
/// создать рендер таргеты 
osg::ref_ptr<osg::Texture2D> OculusConfig::create_rtt_texture(int width, int height,osg::Texture2D::FilterMode mode) const
{
	osg::ref_ptr<osg::Texture2D> tex(new osg::Texture2D);
	tex->setFilter(osg::Texture2D::MIN_FILTER,mode);
	tex->setFilter(osg::Texture2D::MAG_FILTER,mode);
	tex->setTextureSize(width,height);
	tex->setInternalFormat(GL_RGBA);
	return tex;
}
/// создать камеру для сцены
osg::ref_ptr<osg::Camera> OculusConfig::create_scene_camera(osg::GraphicsContext* gc,osg::Texture2D* tex) const
{
	osg::ref_ptr<osg::Camera> camera(new osg::Camera);
	camera->setGraphicsContext(gc);
	camera->setRenderOrder(osg::Camera::POST_RENDER,get_new_order());
	camera->setClearMask(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera->setReferenceFrame(osg::Camera::RELATIVE_RF);
	camera->setViewport(0,0,tex->getTextureWidth(),tex->getTextureHeight());
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	camera->attach(osg::Camera::COLOR_BUFFER,tex,0,0,false,4,4);
	camera->setClearColor(osg::Vec4(0.2f,0.2f,0.4f,1.f));
	camera->setCullMask(0xfffffffd);
	return camera;
}
/// создать камеру для 2D ortho menu
osg::ref_ptr<osg::Camera> OculusConfig::create_hud_camera(osg::GraphicsContext* gc,osg::Texture2D* tex,unsigned int width,unsigned int height) const
{
	osg::ref_ptr<osg::Camera> camera(new osg::Camera);
	camera->setGraphicsContext(gc);
	camera->setRenderOrder(osg::Camera::POST_RENDER,get_new_order());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	camera->setViewport(0,0,tex->getTextureWidth(),tex->getTextureHeight());
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	camera->attach(osg::Camera::COLOR_BUFFER,tex);//,0,0,false,4,4);

	camera->setAllowEventFocus(false);
	//float h = height*2.f;
	//camera->setProjectionMatrix(osg::Matrix::ortho2D(0,width,-0.25f*h,0.75f*h));
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0,width,0,height));
	return camera;
}
/// создать камеру для пост процесса
osg::ref_ptr<osg::Camera> OculusConfig::create_post_camera(osg::GraphicsContext* gc) const
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext(gc);
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setRenderOrder(osg::Camera::POST_RENDER,get_new_order());
	camera->setAllowEventFocus(false);
	camera->setClearMask(0);
	return camera;
}
/// создать квадрат для пост процессинга
osg::ref_ptr<osg::Geode> OculusConfig::create_post_quad() const
{
	osg::ref_ptr<osg::Geometry> geom(osg::createTexturedQuadGeometry(osg::Vec3(),osg::Vec3(1,0.f,0.f),osg::Vec3(0.f,1.f,0.f),0.f,0.f,1.f,1.f));
	osg::ref_ptr<osg::Geode> quad(new osg::Geode);
	quad->addDrawable(geom);
	int values = osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED;
	quad->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL), values);
	quad->getOrCreateStateSet()->setMode(GL_LIGHTING,values);
	quad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,values);
	return quad;
}
/// присоединить шейдер
void OculusConfig::connect_shader(osg::Geode* quad,osg::Camera* camera,osg::Texture2D* rtt_scene,OculusDevice::EyeSide eye) const
{
	osg::StateSet* state = camera->getOrCreateStateSet();
	state->setAttributeAndModes(m_shader_program, osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(0, rtt_scene, osg::StateAttribute::ON);
	//state->setTextureAttributeAndModes(1, rtt_hud, osg::StateAttribute::ON);
	//state->addUniform(new osg::Uniform("rtt_scene", 0));
	//state->addUniform(new osg::Uniform("rtt_hud", 1));

	state->addUniform(new osg::Uniform("LensCenter", m_dev->lensCenter(eye)));
	state->addUniform(new osg::Uniform("ScreenCenter", m_dev->screenCenter()));
	state->addUniform(new osg::Uniform("Scale", m_dev->scale()));
	state->addUniform(new osg::Uniform("ScaleIn", m_dev->scaleIn()));
	state->addUniform(new osg::Uniform("HmdWarpParam", m_dev->warpParameters()));
	state->addUniform(new osg::Uniform("ChromAbParam", m_dev->chromAbParameters()));
}
/// создать шейдер
osg::ref_ptr<osg::Program> OculusConfig::create_shader() const
{
	osg::ref_ptr<osg::Program> program = new osg::Program;
	osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
	vertexShader->loadShaderSourceFromFile(osgDB::findDataFile("./shaders/oculus/warp.vert"));
	osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
	fragmentShader->loadShaderSourceFromFile(osgDB::findDataFile("./shaders/oculus/warpWithChromeAb.frag"));
	program->addShader(vertexShader);
	program->addShader(fragmentShader);
	return program;
}
/// получить новый порядковый номер
unsigned int OculusConfig::get_new_order() const
{
	return m_order_value++;
}

void OculusViewConfigOrientationCallback::operator() (osg::Node* node, osg::NodeVisitor* nv)
{
	osg::Camera* mainCamera = static_cast<osg::Camera*>(node);
	osg::View* view = mainCamera->getView();

	if (view) {
		osg::Quat orient = m_device.get()->getOrientation();
		// Nasty hack to update the view offset for each of the slave cameras
		// There doesn't seem to be an accessor for this, fortunately the offsets are public
		view->findSlaveForCamera(m_l_rtt.get())->_viewOffset.setRotate(orient);
		view->findSlaveForCamera(m_r_rtt.get())->_viewOffset.setRotate(orient);
	}

	traverse(node, nv);
}