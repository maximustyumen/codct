/***************************************************************************
InterfaceForShaders.cpp  -  интерфейс для работы с шейдерами
-------------------
begin                : 28 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./InterfaceForShaders.h"
#include "BumpMaterial.h"

//конструктор
InterfaceForShaders::InterfaceForShaders(osg::ref_ptr<osg::Group> _root,bool use_shaders)
: m_bump_material(use_shaders ? new BumpMaterial : 0)
{
	root = _root;
}

InterfaceForShaders::~InterfaceForShaders()
{
	std::cout << "Destructor InterfaceForShaders."  << std::endl;
	Reset();
}

//сброс всего
bool InterfaceForShaders::Reset()
{
	return true;
}

//сервисная функция закрузки шейдеров из файла
void InterfaceForShaders::LoadShaderSource( osg::ref_ptr<osg::Shader> shader, const std::string& fileName )
{
	std::string fqFileName = osgDB::findDataFile(fileName);
    if( fqFileName.length() != 0 )
    {
        shader->loadShaderSourceFromFile( fqFileName.c_str() );
    }
    else
    {
		std::cout << "File \"" << fileName << "\" not found." << std::endl;
    }
}

//микропрограмма микролельефа)
bool InterfaceForShaders::ConnectBumpShader(std::string ObjectName, std::string filename)
{
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv.get());
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		//delete fnnv;
		return false;
	}
	osg::ref_ptr<osg::StateSet> objectState = root->getOrCreateStateSet();
    osg::ref_ptr<osg::Program> defaultProgram = new osg::Program;
    objectState->setAttributeAndModes(defaultProgram.get(), osg::StateAttribute::ON);
 	osg::ref_ptr<osg::StateSet> bumpMapState = fnnv->_foundNodes.front().get()->getOrCreateStateSet();
    osg::ref_ptr<osg::Program> bumpMapProgramObject = new osg::Program;
    osg::ref_ptr<osg::Shader> bumpVertexObject = new osg::Shader( osg::Shader::VERTEX );
    osg::ref_ptr<osg::Shader> bumpFragmentObject = new osg::Shader( osg::Shader::FRAGMENT );
    LoadShaderSource( bumpVertexObject, "./shaders/bumpmap.vert" );
    LoadShaderSource( bumpFragmentObject, "./shaders/bumpmap.frag" );
    bumpMapProgramObject->addShader( bumpFragmentObject.get() );
    bumpMapProgramObject->addShader( bumpVertexObject.get() );
    // Load a texture to be used as a normal map
    osg::ref_ptr<osg::Texture2D> BodyNormalMap = new osg::Texture2D;
    // protect from being optimized away as static state:
    BodyNormalMap->setDataVariance(osg::Object::DYNAMIC); 
    osg::ref_ptr<osg::Image> tankBody = osgDB::readImageFile(filename);
    if (!tankBody)
    {
       std::cout << "couldn't find texture for Bumping" << std::endl;
	  // delete fnnv;
       return false;
    }
    BodyNormalMap->setImage(tankBody.get());
    bumpMapState->setTextureAttributeAndModes(1,BodyNormalMap.get(),osg::StateAttribute::ON);
	osg::ref_ptr<osg::Uniform> LightPosition = new osg::Uniform( "LightPosition", osg::Vec3(0,0,1) );
	osg::ref_ptr<osg::Uniform> normalMap = new osg::Uniform( "normalMap", 1 );
	osg::ref_ptr<osg::Uniform> baseTexture = new osg::Uniform( "baseTexture", 0 );
	bumpMapState->addUniform (LightPosition.get(), osg::StateAttribute::ON);
	bumpMapState->addUniform (normalMap.get(), osg::StateAttribute::ON);
	bumpMapState->addUniform (baseTexture.get(), osg::StateAttribute::ON);
    bumpMapState->setAttributeAndModes(bumpMapProgramObject.get(), osg::StateAttribute::ON);
	//delete fnnv;
	return true;
}

//Шейдер НЕБО (одна текстура бежит по другой, filename- имя 2 текстуры
bool InterfaceForShaders::ConnectSkyShader(std::string ObjectName, std::string filename)
{
	FindNamedNodeVisitor* fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv);
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		delete fnnv;
		return false;
	}
	osg::StateSet* objectState = root->getOrCreateStateSet();
    osg::Program* defaultProgram = new osg::Program;
    objectState->setAttributeAndModes(defaultProgram, osg::StateAttribute::ON);
	osg::StateSet* skyState = fnnv->_foundNodes.front().get()->getOrCreateStateSet();
    osg::Program* skyProgramObject = new osg::Program;
    osg::Shader* skyVertexObject = new osg::Shader( osg::Shader::VERTEX );
    osg::Shader* skyFragmentObject = new osg::Shader( osg::Shader::FRAGMENT );
    LoadShaderSource( skyVertexObject, "./shaders/sky.vert" );
    LoadShaderSource( skyFragmentObject, "./shaders/sky.frag" );
    skyProgramObject->addShader( skyFragmentObject );
    skyProgramObject->addShader( skyVertexObject );
    // Load a texture to be used as sky
    osg::Texture2D* skyMap = new osg::Texture2D;
    // protect from being optimized away as static state:
    skyMap->setDataVariance(osg::Object::DYNAMIC); 
	skyMap->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
	skyMap->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
    osg::Image* skyFile = osgDB::readImageFile(filename);
    if (!skyFile)
    {
       std::cout << " couldn't find texture, quiting." << std::endl;
       delete fnnv;
       return false;
    }
    skyMap->setImage(skyFile);
    skyState->setTextureAttributeAndModes(1,skyMap,osg::StateAttribute::ON);
	osg::Uniform* baseMap = new osg::Uniform( "baseMap", 0 );
	osg::Uniform* TextureMap = new osg::Uniform( "TextureMap", 1 );
	skyState->addUniform (baseMap, osg::StateAttribute::ON);
	skyState->addUniform (TextureMap, osg::StateAttribute::ON);
    skyState->setAttributeAndModes(skyProgramObject, osg::StateAttribute::ON);
	delete fnnv;
	return true;
}

//микропрограмма керамики (зеркала)
bool InterfaceForShaders::ConnectCeramicShader(std::string ObjectName, std::string filename)
{
	/*
	bool Technique::validate(osg::State& state) const
{
    typedef std::vector<std::string> String_list;
    String_list extensions;

    getRequiredExtensions(extensions);

    for (String_list::const_iterator i=extensions.begin(); i!=extensions.end(); ++i) {
        if (!osg::isGLExtensionSupported(state.getContextID(),i->c_str())) return false;
    }

    return true;
}

			extensions.push_back("GL_ARB_vertex_program");
            extensions.push_back("GL_ARB_fragment_program");

	*/
	

	FindNamedNodeVisitor* fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv);
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		delete fnnv;
		return false;
	}
	osg::StateSet* objectState = root->getOrCreateStateSet();
    osg::Program* defaultProgram = new osg::Program;
    objectState->setAttributeAndModes(defaultProgram, osg::StateAttribute::ON);
 	osg::StateSet* bumpMapState = fnnv->_foundNodes.front().get()->getOrCreateStateSet();
    osg::Program* bumpMapProgramObject = new osg::Program;
    osg::Shader* bumpVertexObject = new osg::Shader( osg::Shader::VERTEX );
    osg::Shader* bumpFragmentObject = new osg::Shader( osg::Shader::FRAGMENT );
    LoadShaderSource( bumpVertexObject, "./shaders/ceramic.vert" );
    LoadShaderSource( bumpFragmentObject, "./shaders/ceramic.frag" );
    bumpMapProgramObject->addShader( bumpFragmentObject );
    bumpMapProgramObject->addShader( bumpVertexObject );
	// create and setup the texture object
    osg::TextureCubeMap *tcm = new osg::TextureCubeMap;
	// protect from being optimized away as static state:
    tcm->setDataVariance(osg::Object::DYNAMIC); 
    tcm->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP);
    tcm->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP);
    tcm->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP);
    tcm->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tcm->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);    
    // assign the six images to the texture object
	osg::Image* tankBody1 = osgDB::readImageFile(filename);
	if (!tankBody1)
    {
		std::cout << " couldn't find texture, quiting." << std::endl;
       	delete fnnv;
		return false;
    }
    tcm->setImage(osg::TextureCubeMap::POSITIVE_X,  tankBody1);
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_X,  tankBody1);
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Y,  tankBody1);
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y,  tankBody1);
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Z,  tankBody1);
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z,  tankBody1);
    bumpMapState->setTextureAttributeAndModes(1,tcm,osg::StateAttribute::ON);
	//
	osg::Uniform* LightPosition = new osg::Uniform( "light", osg::Vec4(1,1,1,0) );
	osg::Uniform* normalMap = new osg::Uniform( "texCube", 1 );
	osg::Uniform* baseTexture = new osg::Uniform( "tex0", 0 );
	//
	bumpMapState->addUniform (LightPosition, osg::StateAttribute::ON);
	bumpMapState->addUniform (normalMap, osg::StateAttribute::ON);
	bumpMapState->addUniform (baseTexture, osg::StateAttribute::ON);
    bumpMapState->setAttributeAndModes(bumpMapProgramObject, osg::StateAttribute::ON);
	delete fnnv;
	return true;
}

/// обработать завершение загрузки графического объекта
void InterfaceForShaders::on_graphic_scene_class_file_loaded(osg::Node* node)
{
	// apply bump mapping
	if(m_bump_material.get()) m_bump_material->ApplyToNodeByDescription(node);
}/*
/// обновить
void InterfaceForShaders::update(osg::Camera* camera)
{
	m_bump_material->update(camera);
}*/