#include "BumpMaterial.h"

#include <osgDB/FileUtils>
#include <osgUtil/TangentSpaceGenerator>
#include <osg/LightSource>

//=====================================================================================//
//                                   class BumpProgram                                 //
//=====================================================================================//
BumpProgram::BumpProgram(const std::string& name)
: m_program(new osg::Program())
{
	m_program->setName(name);

	osg::Shader* vertex_shader = new osg::Shader(osg::Shader::VERTEX);
	LoadShader(vertex_shader, std::string("shaders/bump/") + name + ".vert");
	m_program->addShader(vertex_shader);

	osg::Shader* pixel_shader = new osg::Shader(osg::Shader::FRAGMENT);
	LoadShader(pixel_shader, std::string("shaders/bump/") + name + ".frag");
	m_program->addShader(pixel_shader);

	m_program->addBindAttribLocation("tangent", 6);
	m_program->addBindAttribLocation("binormal", 7);
}

/// применить программу к StateSet
void BumpProgram::ApplyToStateSet(osg::StateSet* state_set)
{
	state_set->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}
/// загрузить шейдер
void BumpProgram::LoadShader(osg::Shader* shader, const std::string& fileName) const
{
	std::string fqFileName = osgDB::findDataFile(fileName);
	if( fqFileName.length() != 0 )
	{
		shader->loadShaderSourceFromFile(fqFileName.c_str());
		std::cout << "Shader \"" << fileName << "\" loaded." << std::endl;
	}
	else std::cout << "File \"" << fileName << "\" not found." << std::endl;
}

//=====================================================================================//
//                                  class BumpMaterial                                 //
//=====================================================================================//
BumpMaterial::BumpMaterial()
//: m_parallax(new BumpProgram("parallax"))
: m_relief(new BumpProgram("relief"))
{
}

BumpMaterial::~BumpMaterial()
{
}
/// применить бамп к узлу по описанию экспортёра
void BumpMaterial::ApplyToNodeByDescription(osg::Node* node)
{
	for(unsigned int i=0;i<node->getNumDescriptions();i++)
	{
		const std::string& desc = node->getDescription(i);
		if(desc.compare("# osgmaxexp material data"))
		{
			BumpDesc bump_desc;
			bool bump = false;

			int pos = 0;
			while(true)
			{
				pos = desc.find_first_of("\n",pos);
				if(std::string::npos == pos) break;
				++pos;

				std::string::size_type key_end = desc.find_first_of("\t",pos);
				if(std::string::npos == key_end) break;

				int end_pos = desc.find_first_of("\n",pos);
				if(std::string::npos == end_pos)
				{
					end_pos = desc.length();
				}

				std::string key(desc.substr(pos,key_end - pos));
				int value_pos = key_end + 1;
				if("Name" == key)
				{
                    //if(bump) ApplyToNodesWithMaterial(node,bump_desc);
                    bump_desc.material_name = desc.substr(value_pos,end_pos - value_pos);
                    //bump = false;
				}
				else if("Map" == key)
				{
					int map_type_end = desc.find_first_of("\t",value_pos);
					std::string map_type(desc.substr(value_pos,map_type_end - value_pos));

					if("Bump" == map_type)
					{
						ParseMapParams(desc,map_type_end,&bump_desc.bump_texture_slot,&bump_desc.bump_scale);
						bump = true;
					}
					else if("Specular" == map_type)
					{
						ParseMapParams(desc,map_type_end,&bump_desc.specular_texture_slot,&bump_desc.specular_factor);
					}
				}
			}

			if(bump) ApplyToNodesWithMaterial(node,bump_desc);
		}
	}
}
/// пропарсить параметры описания поля Map
void BumpMaterial::ParseMapParams(const std::string& desc,size_t from,int* texture_slot,float* factor)
{
	int texture_slot_end = desc.find_first_of("\t",from+1);
	if(texture_slot)
	{
		std::string str_texture_slot(desc.substr(from+1,texture_slot_end - (from+1)));
		*texture_slot = atoi(str_texture_slot.c_str());
	}

	int factor_end = desc.find_first_of("\t",texture_slot_end+1);
	if(factor)
	{
		std::string str_factor(desc.substr(texture_slot_end+1,factor_end - (texture_slot_end+1)));
		*factor = atof(str_factor.c_str());
	}
}

/// поиск геометрии по имени материала
class FindGeometryByMaterialName : public osg::NodeVisitor
{
public:
	typedef std::vector<osg::Geometry*> geometry_t;

private:
	std::string m_material_name;
	geometry_t m_geometry;		//	список геометрии

public:
	FindGeometryByMaterialName(const std::string& material_name) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), m_material_name(material_name)
	{
	}

	virtual void apply(osg::Node& node)
	{
		traverse(node);
	}
	virtual void apply(osg::MatrixTransform& node)
	{
		apply((osg::Node&)node);
	}
	virtual void apply(osg::Group& group)
	{
		traverse(group);
	}

	virtual void apply(osg::Geode& geode)
	{
		for(unsigned int i=0;i<geode.getNumDrawables();i++)
		{
			if(osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i)))
			{
                //if(strcmp(m_material_name.c_str(),geometry->getOrCreateStateSet()->getName())==0)
                //if(m_material_name.compare(geometry->getOrCreateStateSet()->getName())==0)
                size_t pos = m_material_name.find(geometry->getOrCreateStateSet()->getName());
                if((pos != std::string::npos)&&((m_material_name.length()- geometry->getOrCreateStateSet()->getName().length()) <= 1 ))
                {
                    m_geometry.push_back(geometry);
                }
			}

		}
	}

	/// запросить список геометрии
	const geometry_t& GetGeometry() const
	{
		return m_geometry;
	}	 
};

/// применить бамп к материалу
void BumpMaterial::ApplyToNodesWithMaterial(osg::Node* node,const BumpDesc& bump_desc)
{
    FindGeometryByMaterialName visitor(bump_desc.material_name);
	node->accept(visitor);
	
	for(FindGeometryByMaterialName::geometry_t::const_iterator it=visitor.GetGeometry().begin(),
		it_end=visitor.GetGeometry().end();it!=it_end;++it)
	{
		ApplyToGeometry(*it,bump_desc);
    }
}

/// применить бамп к геометрии объекта
void BumpMaterial::ApplyToGeometry(osg::Geometry* geometry,const BumpDesc& bump_desc)
{
	osgUtil::TangentSpaceGenerator* tsg = new osgUtil::TangentSpaceGenerator();
	tsg->generate(geometry, bump_desc.bump_texture_slot);

	geometry->setVertexAttribArray (6, tsg->getTangentArray());
	geometry->setVertexAttribArray (7, tsg->getBinormalArray());
	geometry->setVertexAttribBinding (6, osg::Geometry::BIND_PER_VERTEX);
	geometry->setVertexAttribBinding (7, osg::Geometry::BIND_PER_VERTEX);
	
	// create copy state, because we bind various light parameters and could not see good result in 'share stateset'
	osg::StateSet* state_set = geometry->getStateSet();
	if(state_set) state_set = new osg::StateSet(*state_set);
	else state_set = new osg::StateSet;	
	geometry->setStateSet(state_set);

	// now bind uniforms
	state_set->addUniform(new osg::Uniform("colorMap", 0));
	state_set->addUniform(new osg::Uniform("normalMap", bump_desc.bump_texture_slot));
	if(bump_desc.specular_texture_slot >= 0)
	{
		state_set->addUniform(new osg::Uniform("specularMap", bump_desc.specular_texture_slot));
		state_set->addUniform(new osg::Uniform("specular_factor", bump_desc.specular_factor));
	}
	else state_set->addUniform(new osg::Uniform("specular_factor", 0.f));
	
	state_set->addUniform(new osg::Uniform("bump_depth", 0.005f*bump_desc.bump_scale));
	
	/*if("mat_pogreb_ground" == bump_desc.material_name)
		__asm nop;*/
	// setup lighting
	SetupLighting(geometry);

	m_relief->ApplyToStateSet(state_set);

	state_set->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF);
	state_set->setMode(GL_BLEND, osg::StateAttribute::OFF);
	state_set->setRenderingHint(osg::StateSet::DEFAULT_BIN);
}

//=====================================================================================//
/// поиск источников света вверх по иерархии
class FindLights : public osg::NodeVisitor
{
public:
	typedef std::vector<osg::LightSource*> lights_t;

private:
	//=====================================================================================//
	/// поиск источников света среди соседних чилдернов в группе (но не ниже по иерархии)
	class FindInChildren : public osg::NodeVisitor
	{
	private:
		lights_t& m_lights;
		bool m_traverse;

	public:
		FindInChildren(lights_t& lights) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), m_traverse(true), m_lights(lights)
		{
		}
		virtual void apply(osg::Node& node)
		{	// empty is true
		}
		virtual void apply(osg::Geode& node)
		{	// empty is true
		}
		virtual void apply(osg::MatrixTransform& node)
		{
			if(m_traverse)	apply((osg::Group&)node);
		}
		virtual void apply(osg::Group& group)
		{
			m_traverse = false;
			traverse(group);
			m_traverse = true;
		}
		virtual void apply(osg::LightSource& light)
		{
			m_lights.push_back(&light);
		}
	};
	//=====================================================================================//

	lights_t m_lights;		// список обнаруженных источников света
	FindInChildren m_find_in_children_visitor;

public:
	FindLights() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_PARENTS), m_find_in_children_visitor(m_lights)
	{
	}

public:
	/// получить список обнаруженных источников света
	const lights_t& GetLights() const
	{
		return m_lights;
	}

public:
	virtual void apply(osg::Node& node)
	{
		traverse(node);
	}
	virtual void apply(osg::MatrixTransform& node)
	{
		apply((osg::Group&)node);
	}
	virtual void apply(osg::Group& group)
	{
		// поиск в соседних чилдренах ноды
		m_find_in_children_visitor.traverse(group);

		// now traverse to parent
		traverse(group);
	}
	virtual void apply(osg::Geode& geode)
	{
		traverse(geode);
	}
	/*virtual void apply(osg::LightSource& light)
	{
		// never using this apply
		//m_lights.push_back(&light);
		std::cout << "find light!!! uncomment!!!" << std::endl;
	}*/
};
//=====================================================================================//


//=====================================================================================//
// Visitor to return the world coordinates of a node.
// It traverses from the starting node to the parent.
// The first time it reaches a root node, it stores the world coordinates of 
// the node it started from.  The world coordinates are found by concatenating all 
// the matrix transforms found on the path from the start node to the root node.

class WorldCoordOfNodeVisitor : public osg::NodeVisitor 
{
private:
	osg::Matrix m_matrix;
	bool m_done;

public:
	WorldCoordOfNodeVisitor() : osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS), m_done(false)
	{
	}
	virtual void apply(osg::Node &node)
	{
		if(!m_done)
		{
			if(0 == node.getNumParents()) // no parents
			{
				m_matrix.set( osg::computeLocalToWorld(this->getNodePath()) );
				m_done = true;
			}
			traverse(node);
		}
	}
	const osg::Matrix& GetMatrix() const
	{
	  return m_matrix;
	}
};
//=====================================================================================//

/// применить освещение
void BumpMaterial::SetupLighting(osg::Geometry* geometry)
{
	osg::StateSet* state_set = geometry->getOrCreateStateSet();
	
	FindLights visitor;
	osg::Node* node = geometry->getParent(0);
	if(!node) std::cout << "Bump: error to find geometry parent node" << std::endl;
	else
	{
		// ищем источники света
		node->accept(visitor);

		const int max_count_lights = 4;

		// создаём юниформы для точечных источников света
		osg::Uniform* uni_pos = new osg::Uniform(osg::Uniform::FLOAT_VEC3,"omni_lights_positions",max_count_lights);
		osg::Uniform* uni_color = new osg::Uniform(osg::Uniform::FLOAT_VEC4,"omni_lights_color",max_count_lights);

		const FindLights::lights_t& lights = visitor.GetLights();
		for(size_t i=0; i<max_count_lights; i++)
		{
			if(i < lights.size())
			{
				osg::LightSource* ls = lights[i];
				const osg::Light* light = ls->getLight();
				const osg::Vec4& color = light->getDiffuse();

				float radius = 140;
				uni_color->setElement(i,osg::Vec4(color.x(),color.y(),color.z(),radius));

				// set light world coordinate position
				WorldCoordOfNodeVisitor world_coord_visitor;
				ls->accept(world_coord_visitor);
				uni_pos->setElement(i,osg::Vec3(world_coord_visitor.GetMatrix().getTrans()));
			}
			else
			{
				// слот света не используется
				uni_color->setElement(i,osg::Vec4(0.f,0.f,0.f,0.f));
				uni_pos->setElement(i,osg::Vec3(0.f,0.f,0.f));
			}
		}

		state_set->addUniform(uni_color);
		state_set->addUniform(uni_pos);
	}
}