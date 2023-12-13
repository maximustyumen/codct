#ifndef PROTO_OPEN_BUMP_MATERIAL_H
#define PROTO_OPEN_BUMP_MATERIAL_H

#include <memory>
#include <string>
#include "osg/ref_ptr"

namespace osg
{
	class StateSet;
	class Geometry;
	class Program;
	class Camera;
	class Shader;
	class Node;
}

//=====================================================================================//
//                                   class BumpProgram                                 //
//=====================================================================================//
class BumpProgram
{
private:
	osg::ref_ptr<osg::Program> m_program;	//	шейдерная программа

public:
	BumpProgram(const std::string& name);

public:
	/// применить программу к StateSet
	void ApplyToStateSet(osg::StateSet* state_set);

private:
	/// загрузить шейдер
	void LoadShader(osg::Shader* shader, const std::string& fileName) const;
};
//=====================================================================================//


//=====================================================================================//
//                                  class BumpMaterial                                 //
//=====================================================================================//
class BumpMaterial
{
public:
	struct BumpDesc
	{
		BumpDesc() : bump_scale(1.f), bump_texture_slot(-1), specular_texture_slot(-1), specular_factor(0) {}

		std::string material_name;
		float bump_scale;
		int bump_texture_slot;
		float specular_factor;
		int specular_texture_slot;
	};

private:
	//std::auto_ptr<BumpProgram> m_parallax;
	std::auto_ptr<BumpProgram> m_relief;

public:
	BumpMaterial();
	virtual ~BumpMaterial();

public:
	/// применить бамп к узлу по описанию экспортёра
	void ApplyToNodeByDescription(osg::Node* node);
	/// применить бамп к геометрии объекта
	void ApplyToGeometry(osg::Geometry* geometry,const BumpDesc& bump_desc);

private:
	/// пропарсить параметры описания поля Map
	void ParseMapParams(const std::string& desc,size_t from,int* texture_slot,float* factor=0);
	/// применить бамп ко всем узлам с материалом
	void ApplyToNodesWithMaterial(osg::Node* node,const BumpDesc& bump_desc);
	/// применить освещение
	void SetupLighting(osg::Geometry* geometry);
};
//=====================================================================================//

#endif