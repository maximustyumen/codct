#ifndef INTERFACEFORSHADERS
#define INTERFACEFORSHADERS

#include <memory>
#include <iostream>
#include <osgViewer/Viewer>
#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osgDB/FileUtils>
#include <osg/TextureCubeMap>

#include "../Visitors/FindNodeByName.h"
#include "../Graphic/GraphicSceneClassObserver.h"

class BumpMaterial;

class InterfaceForShaders : public GraphicSceneClassObserver
{	
public:
	//конструктор
	InterfaceForShaders(osg::ref_ptr<osg::Group> _root,bool use_shaders);
	//
	virtual ~InterfaceForShaders();
	//сброс
	bool Reset();
	//микропрограмма микролельефа)
	bool ConnectBumpShader(std::string, std::string);
	//микропрограмма неба (одна текстура бежит по другой)
	bool ConnectSkyShader(std::string, std::string);
	//микропрограмма керамики (зеркала)
	bool ConnectCeramicShader(std::string, std::string);
	/// обновить
	//void update(osg::Camera* camera);
private:
	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;
	//
	//сервисная функция закрузки шейдеров из файла
	void LoadShaderSource( osg::ref_ptr<osg::Shader> shader, const std::string& fileName );

private:
	/// обработать завершение загрузки графического объекта
	void on_graphic_scene_class_file_loaded(osg::Node* node);

private:
	/// бамп-материал
	std::auto_ptr<BumpMaterial> m_bump_material;
};

#endif