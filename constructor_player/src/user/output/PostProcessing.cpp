#include "./PostProcessing.h"
#include "../../error/errorclass.h"

#include <osgPPU/UnitInResampleOut.h>
#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitOut.h>
#include <osgPPU/Visitor.h>
#include <osgPPU/Camera.h>
#include <osgDB/ReadFile>

#include <osg/GLExtensions>
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/Camera>

//=====================================================================================//
//                       Event handler to react on resize events                       //
//=====================================================================================//
class ResizeEventHandler : public osgGA::GUIEventHandler
{
private:
	osgPPU::Processor* m_processor;
	osg::Camera* m_camera;

public:
	ResizeEventHandler(osgPPU::Processor* proc, osg::Camera* cam) : m_processor(proc), m_camera(cam) {}

	bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
	{
		if(ea.getEventType() == osgGA::GUIEventAdapter::RESIZE)            
		{
			osgPPU::Camera::resizeViewport(0,0, ea.getWindowWidth(), ea.getWindowHeight(), m_camera);
			m_processor->onViewportChange();
			return true;
		}
		return false;
	}
};

//=====================================================================================//
//                                class PostProcessing                                 //
//=====================================================================================//
PostProcessing::PostProcessing(const std::string& ppu_filename, osgViewer::Viewer* viewer, osg::Node* node)
: m_processor(dynamic_cast<osgPPU::Processor*>(osgDB::readObjectFile(ppu_filename)))
, m_event_resize(new ResizeEventHandler(m_processor.get(), viewer->getCamera()))
, m_root(node->asGroup())
, m_viewer(viewer)
{
	if (!m_processor.get())	throw (ErrorClass (__FILE__, __LINE__ , "PostProcessing::PostProcessing()", "PPU-file does not contain a valid pipeline" , 1));
	m_processor->useColorClamp(false);

	// setup viewers camera
	setupCamera(viewer->getCamera());

	// add processor to the scene
	m_root->addChild(m_processor);

	// add resize event
	viewer->addEventHandler(m_event_resize);

	// update uniforms
	updateGlobalUniforms();
}
PostProcessing::~PostProcessing()
{
	m_processor->getCamera()->setRenderTargetImplementation(m_rt_impl);
	m_viewer->removeEventHandler(m_event_resize);
	m_root->removeChild(m_processor);
}
/// настроить камеру
void PostProcessing::setupCamera(osg::Camera* camera)
{
	// set up the background color and clear mask.
	camera->setClearColor(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// tell the camera to use OpenGL frame buffer object where supported
	m_rt_impl = camera->getRenderTargetImplementation();
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

	// create texture to render to
	osg::Viewport* vp = camera->getViewport();
	osg::ref_ptr<osg::Texture2D> colorTexture = createRenderTarget(vp->width(), vp->height());
	osg::ref_ptr<osg::Texture2D> depthTexture = createDepthTarget(vp->width(), vp->height());

	// attach the texture and use it as the color buffer.
	camera->attach(osg::Camera::COLOR_BUFFER, colorTexture);
	camera->attach(osg::Camera::DEPTH_BUFFER, depthTexture);

	m_processor->setCamera(camera);
}
/// создать рендер таргет
osg::ref_ptr<osg::Texture2D> PostProcessing::createRenderTarget(int width, int height)
{
	// create simple 2D texture
	osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
	texture->setTextureSize(width, height);
	texture->setResizeNonPowerOfTwoHint(false);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP);
	texture->setInternalFormat(GL_RGBA16F_ARB);
	texture->setSourceFormat(GL_RGBA);
	texture->setSourceType(GL_FLOAT);
	return texture;
}
/// создать текстуру глубины
osg::ref_ptr<osg::Texture2D> PostProcessing::createDepthTarget(int width, int height)
{
	osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
	texture->setTextureSize(width, height);
	texture->setResizeNonPowerOfTwoHint(false);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP);
	texture->setInternalFormat(GL_DEPTH_COMPONENT);
	return texture;
}
/// обновить глобальные переменные
void PostProcessing::updateGlobalUniforms()
{
	osg::Camera* camera = m_viewer->getCamera();

	double left, right, bottom, top, near, far;
	camera->getProjectionMatrixAsFrustum(left, right, bottom, top, near, far);
	
	osg::Viewport* vp = camera->getViewport();

	osg::Vec3 eye, center, up;
	camera->getViewMatrixAsLookAt(eye,center,up);

	double fovy,aspectRatio,zNear,zFar;
	osg::Matrix matProjection = camera->getProjectionMatrix();

	matProjection.getPerspective(fovy,aspectRatio,zNear,zFar);

	
	/*float angleRad = osg::DegreesToRadians(fovy);
	osg::Vec2 metricNearFarPlanePixelResolution
	(
		tan(angleRad)*near / (float)(camera->getViewport()->height()),
		tan(angleRad)*far  / (float)(camera->getViewport()->height())
	);*/

	//double fovy,aspectRatio,zNear,zFar;
	//camera->getProjectionMatrix().getPerspective(fovy,aspectRatio,zNear,zFar);

	//osg::Viewport* vp = camera->getViewport();

	std::vector<std::string> vUnitNames;
	vUnitNames.push_back("SSAO");
	vUnitNames.push_back("BlurSSAO");
	vUnitNames.push_back("BlurSSAO_X");
	vUnitNames.push_back("BlurSSAO_Y");
	vUnitNames.push_back("Final");
	//vUnitNames.push_back("SSAO_Blur_Lighting");

	//float ssao_radius = 4.f;
	//float ssao_inv_radius = 1.f/ssao_radius;
	//float ssao_sqr_radius = ssao_radius*ssao_radius;

	//float ssao_angle_bias = 32.f * osg::PI / 180.f;

	//osg::Vec2 ssao_resolution(vp->width()*0.5f, vp->height()*0.5f);
	//osg::Vec2 ssao_inv_resolution(1.f/ssao_resolution.x(), 1.f/ssao_resolution.y());

	osg::Vec2 ssao_focal_len;
	float fov = osg::DegreesToRadians(fovy);
	ssao_focal_len[1] = 1.f / tanf(fov * 0.5f);
	ssao_focal_len[0] = ssao_focal_len.y() * vp->height()/vp->width();

	osg::Vec2 ssao_inv_focal_len(1.f/ssao_focal_len.x(), 1.f/ssao_focal_len.y());

	osg::Vec2 texel_size(1.f/vp->width(), 1.f/vp->height());

	for(size_t i=0; i<vUnitNames.size(); i++)
	{
		if(osgPPU::Unit* unit = m_processor->findUnit(vUnitNames[i]))
		{
			osg::StateSet* state_set = unit->getGeode()->getStateSet();
			
			osg::Uniform* screen_size = state_set->getOrCreateUniform("g_screen_size",osg::Uniform::FLOAT_VEC2);
			screen_size->set(osg::Vec2(vp->width()/2.f, vp->height()/2.f));

			//osg::Uniform* uniform_eye = state_set->getOrCreateUniform("g_eye",osg::Uniform::FLOAT_VEC3);
			//if(uniform_eye)	uniform_eye->set(eye);

			//osg::Uniform* uniform_metricNearFarPlanePixelResolution = state_set->getOrCreateUniform("metricNearFarPlanePixelResolutionUniform", osg::Uniform::FLOAT_VEC2);
			//if(uniform_metricNearFarPlanePixelResolution)
			//	uniform_metricNearFarPlanePixelResolution->set(metricNearFarPlanePixelResolution);
			

			//osg::StateSet* state_set = unit->getGeode()->getStateSet();
			//state_set->getOrCreateUniform("g_R",osg::Uniform::FLOAT)->set(ssao_radius);
			//state_set->getOrCreateUniform("g_inv_R",osg::Uniform::FLOAT)->set(ssao_inv_radius);
			//state_set->getOrCreateUniform("g_sqr_R",osg::Uniform::FLOAT)->set(ssao_sqr_radius);
			//state_set->getOrCreateUniform("g_AngleBias",osg::Uniform::FLOAT)->set(ssao_angle_bias);
			//state_set->getOrCreateUniform("g_FocalLen",osg::Uniform::FLOAT_VEC2)->set(ssao_focal_len);
			state_set->getOrCreateUniform("g_InvFocalLen",osg::Uniform::FLOAT_VEC2)->set(ssao_inv_focal_len);
			//state_set->getOrCreateUniform("g_SSAOResolution",osg::Uniform::FLOAT_VEC2)->set(ssao_resolution);
			//state_set->getOrCreateUniform("g_SSAOInvResolution",osg::Uniform::FLOAT_VEC2)->set(ssao_inv_resolution);

			state_set->getOrCreateUniform("matProjection",osg::Uniform::FLOAT_MAT4)->set(matProjection);

			state_set->getOrCreateUniform("zNear",osg::Uniform::FLOAT)->set(static_cast<float>(near));
			state_set->getOrCreateUniform("zFar",osg::Uniform::FLOAT)->set(static_cast<float>(far));

			// g_depth_to_z_a = zFar / ( zFar - zNear );
			state_set->getOrCreateUniform("g_depth_to_z_a",osg::Uniform::FLOAT)->set(static_cast<float>(far/(far - near)));
			// g_depth_to_z_b = zFar * zNear / ( zNear - zFar );
			state_set->getOrCreateUniform("g_depth_to_z_b",osg::Uniform::FLOAT)->set(static_cast<float>(far*near/(near - far)));

			state_set->getOrCreateUniform("g_texel_size",osg::Uniform::FLOAT_VEC2)->set(texel_size);



			//
			float blur_radius = 4.f;
			float sigma = (blur_radius + 1.f)/2.f;
			float inv_sigma2 = 1.f / (2.f*sigma*sigma);

			state_set->getOrCreateUniform("g_BlurRadius",osg::Uniform::FLOAT)->set(blur_radius);
			state_set->getOrCreateUniform("g_BlurFalloff",osg::Uniform::FLOAT)->set(inv_sigma2);

			state_set->getOrCreateUniform("g_Sharpness",osg::Uniform::FLOAT)->set(0.1f);

			osg::Vec2 inv_resolution(1.f/(vp->width()*0.5f), 1.f/(vp->height()*0.5f));
			state_set->getOrCreateUniform("g_InvResolution",osg::Uniform::FLOAT_VEC2)->set(inv_resolution);
		}
	}

	/*
	std::vector<std::string> vBlurUnitNames;
	vBlurUnitNames.push_back("SSAO_Blur_X");
	vBlurUnitNames.push_back("SSAO_Blur_Y");

	// SSAO_Blur
	for(size_t i=0; i<vBlurUnitNames.size(); i++)
	{
		if(osgPPU::Unit* unit = m_processor->findUnit(vBlurUnitNames[i]))
		{
			osg::StateSet* state_set = unit->getGeode()->getStateSet();

			float blur_radius = 7.f;
			float sigma = (blur_radius + 1.f)/2.f;
			float inv_sigma2 = 1.f / (2.f*sigma*sigma);

			state_set->getOrCreateUniform("g_BlurRadius",osg::Uniform::FLOAT)->set(blur_radius);
			state_set->getOrCreateUniform("g_BlurFalloff",osg::Uniform::FLOAT)->set(inv_sigma2);

			state_set->getOrCreateUniform("g_Sharpness",osg::Uniform::FLOAT)->set(3.f);

			osg::Vec2 inv_resolution(1.f/vp->width(), 1.f/vp->height());
			state_set->getOrCreateUniform("g_InvResolution",osg::Uniform::FLOAT_VEC2)->set(inv_resolution);

			state_set->getOrCreateUniform("zNear",osg::Uniform::FLOAT)->set(static_cast<float>(near));
			state_set->getOrCreateUniform("zFar",osg::Uniform::FLOAT)->set(static_cast<float>(far));
		}
	}*/
	
}
/// обновить
void PostProcessing::update()
{
	updateGlobalUniforms();
}