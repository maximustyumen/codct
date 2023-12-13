#ifndef OCULUS_CONFIG
#define OCULUS_CONFIG

#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include "oculusdevice.h"

class OculusConfig
{
private:
	osg::ref_ptr<osg::Program> m_shader_program;
	mutable unsigned int m_order_value;
	osg::ref_ptr<OculusDevice> m_dev;
	float m_near_clip;
	float m_far_clip;

public:
	OculusConfig(osgViewer::Viewer* viewer,osg::Node* scene_node,osg::Node* menu_node,osg::Node* cursor2d_node);
	virtual ~OculusConfig();

private:
	/// настроить главную камеру
	void configure_main_camera(osg::Camera* camera,osg::GraphicsContext* gc,int width, int height) const;
	/// создать контекст рендера
	osg::ref_ptr<osg::GraphicsContext> create_gc(int width, int height) const;
	/// создать рендер таргеты 
	osg::ref_ptr<osg::Texture2D> create_rtt_texture(int width, int height,osg::Texture2D::FilterMode mode) const;
	/// создать камеру для сцены
	osg::ref_ptr<osg::Camera> create_scene_camera(osg::GraphicsContext* gc,osg::Texture2D* tex) const;
	/// создать камеру для 2D ortho menu
	osg::ref_ptr<osg::Camera> create_hud_camera(osg::GraphicsContext* gc,osg::Texture2D* tex,unsigned int width,unsigned int height) const;
	/// создать камеру для пост процесса
	osg::ref_ptr<osg::Camera> create_post_camera(osg::GraphicsContext* gc) const;
	/// создать квадрат для пост процессинга
	osg::ref_ptr<osg::Geode> create_post_quad() const;
	/// создать шейдер
	osg::ref_ptr<osg::Program> create_shader() const;
	/// присоединить шейдер
	void connect_shader(osg::Geode* quad,osg::Camera* camera,osg::Texture2D* rtt_scene,OculusDevice::EyeSide eye) const;
	/// настроить вывод
	osg::Camera* configure_eye(osgViewer::Viewer* viewer,OculusDevice::EyeSide eye,osg::ref_ptr<osg::GraphicsContext> gc,
		osg::Node* scene_node,osg::ref_ptr<osg::Node> hud_node,osg::Node* cursor2d_node,unsigned int x_viewport,unsigned int width,unsigned int height);
	/// получить новый порядковый номер
	unsigned int get_new_order() const;
};

class OculusViewConfigOrientationCallback :  public osg::NodeCallback
{
public:
	OculusViewConfigOrientationCallback(osg::ref_ptr<osg::Camera> l_rtt, osg::ref_ptr<osg::Camera> r_rtt, osg::observer_ptr<OculusDevice> device) : m_l_rtt(l_rtt), m_r_rtt(r_rtt), m_device(device) {};
	virtual void operator() (osg::Node* node, osg::NodeVisitor* nv);
protected:
	osg::observer_ptr<osg::Camera> m_l_rtt, m_r_rtt;
	osg::observer_ptr<OculusDevice> m_device;
};

#endif