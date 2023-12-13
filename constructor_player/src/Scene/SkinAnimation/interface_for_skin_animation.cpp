#include "interface_for_skin_animation.h"
#include "skin_animation_manager.h"
#include "../Visitors/FindNodeByName.h"
#include <osgAnimation/StackedRotateAxisElement>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/VertexInfluence>
#include <osgAnimation/RigGeometry>
#include <osgAnimation/UpdateBone>
#include <osgAnimation/Skeleton>
#include <osgAnimation/Bone>
#include <osgDB/ReadFile>

struct AnimationManagerFinder : public osg::NodeVisitor
{
	osg::ref_ptr<osgAnimation::AnimationManagerBase> base_manager;
	
	AnimationManagerFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), base_manager(0) {}
	void apply(osg::Node& node)
	{
		if(base_manager)
			return;
		if(node.getUpdateCallback())
		{
			osgAnimation::AnimationManagerBase* b = dynamic_cast<osgAnimation::AnimationManagerBase*>(node.getUpdateCallback());
			if(b)
			{
				base_manager = b;
				return;
			}
		}
		traverse(node);
	}
};


/*struct NoCull : public osg::NodeVisitor
{
	NoCull() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
	void apply(osg::Node& node)
	{
		node.setCullingActive(false);
		traverse(node);
	}
};*/


struct FindSkeleton : public osg::NodeVisitor
{
	osgAnimation::Skeleton* skeleton;
	FindSkeleton() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), skeleton(0) {}
	void apply(osg::MatrixTransform& mt)
	{
		if(skeleton) return;
		if(skeleton = dynamic_cast<osgAnimation::Skeleton*>(&mt)) return;
		traverse(mt);
	}
};


struct RenameAllMatrixTransform : public osg::NodeVisitor
{
	std::string prefix;
	RenameAllMatrixTransform(const std::string& name)
	: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	, prefix(name+"_")
	{
	}
	void apply(osg::MatrixTransform& mt)
	{
		rename_node(mt);
		traverse(mt);
	}
	void apply(osg::Geode& geode)
	{
		rename_node(geode);

		/// rename VertexInfluence
		for(unsigned int i=0; i<geode.getNumDrawables(); i++)
		{
			if(osgAnimation::RigGeometry* rig = dynamic_cast<osgAnimation::RigGeometry*>(geode.getDrawable(i)))
			{
				if(osgAnimation::VertexInfluenceMap* influences = rig->getInfluenceMap())
				{
					osgAnimation::VertexInfluenceMap* new_influences = new osgAnimation::VertexInfluenceMap;
					for(osgAnimation::VertexInfluenceMap::iterator itr=influences->begin(); itr!=influences->end(); ++itr)
					{
						const std::string new_name = prefix + itr->first;
						osgAnimation::VertexInfluence vl = itr->second;
						vl.setName(new_name);
						new_influences->insert(std::make_pair(new_name,vl));
					}
					rig->setInfluenceMap(new_influences);
				}
			}
		}
	}
	void rename_node(osg::Node& node)
	{
		if(!node.getName().empty())
			node.setName(prefix + node.getName());
	}
};


class RigComputeBoundingBoxCallback : public osg::Drawable::ComputeBoundingBoxCallback
{
public:
	RigComputeBoundingBoxCallback(const std::string& name,double factor = 2.0) : _computed(false), _factor(factor)
	{
		setName(name);
	}
	void reset() { _computed = false; }
	virtual osg::BoundingBox computeBound(const osg::Drawable& drawable) const
	{
		const osgAnimation::RigGeometry& rig = dynamic_cast<const osgAnimation::RigGeometry&>(drawable);

		// if a valid inital bounding box is set we use it without asking more
		if (rig.getInitialBound().valid())
			return rig.getInitialBound();

		if (_computed)
			return _boundingBox;

		// if the computing of bb is invalid (like no geometry inside)
		// then dont tag the bounding box as computed
		osg::BoundingBox bb = rig.computeBoundingBox();
		if (!bb.valid())
			return bb;


		_boundingBox.expandBy(bb);
		osg::Vec3 center = _boundingBox.center();
		osg::Vec3 vec = (_boundingBox._max-center)*_factor;
		_boundingBox.expandBy(center + vec);
		_boundingBox.expandBy(center - vec);
		_computed = true;
		//        OSG_NOTICE << "build the bounding box for RigGeometry " << rig.getName() << " " << _boundingBox._min << " " << _boundingBox._max << std::endl;
		return _boundingBox;
	}
protected:
	mutable bool _computed;
	double _factor;
	mutable osg::BoundingBox _boundingBox;
};

struct SetupComputeBoundingBox : public osg::NodeVisitor
{
	std::string m_name;
	SetupComputeBoundingBox(const std::string& name) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), m_name(name) {}
	void apply(osg::Geode& geode)
	{
		for(unsigned int i=0; i<geode.getNumDrawables(); i++)
		{
			if(osgAnimation::RigGeometry* rig = dynamic_cast<osgAnimation::RigGeometry*>(geode.getDrawable(i)))
			{
				rig->setComputeBoundingBoxCallback(new RigComputeBoundingBoxCallback(m_name+"RigComputeBoundingBoxCallback",100.));
			}
		}
	}
};

/*
osg::Geode* createAxis()
{
	osg::Geode*     geode    = new osg::Geode();
	osg::Geometry*  geometry = new osg::Geometry();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec4Array* colors   = new osg::Vec4Array();

	float scale = 5.f;

	vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3(scale, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3(0.0f, scale, 0.0f));
	vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3(0.0f, 0.0f, scale));

	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));

	geometry->setVertexArray(vertices);
	geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));
	geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, false);

	geode->addDrawable(geometry);
	return geode;
}

struct AddHelperBone : public osg::NodeVisitor
{
	AddHelperBone() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
	void apply(osg::Transform& node) {
		osgAnimation::Bone* bone = dynamic_cast<osgAnimation::Bone*>(&node);
		if (bone)
			bone->addChild(createAxis());
		traverse(node);
	}
};*/



InterfaceForSkinAnimation::InterfaceForSkinAnimation(osg::ref_ptr<osg::Group> root)
: m_root(root)
{
}
/// загрузить модельку c анимациями
bool InterfaceForSkinAnimation::load_skin(const std::string& object_name,const std::string& filename)
{
	osg::ref_ptr<osg::Group> node(dynamic_cast<osg::Group*>(osgDB::readNodeFile(filename)));
	if(!node.valid())
	{
		std::cout << "Couldn't read skin animation: " << filename << std::endl;
		return false;
	}

	osg::ref_ptr<osg::MatrixTransform> mt(new osg::MatrixTransform());
	mt->setName(object_name);
	mt->addChild(node);
	m_root->addChild(mt);

	AnimationManagerFinder finder;
	node->accept(finder);
	if(!finder.base_manager) std::cout << "Couldn't find osgAnimation::AnimationManagerBase: " << filename << std::endl;
	else
	{
		osg::ref_ptr<SkinAnimationManager> manager(new SkinAnimationManager(*finder.base_manager));
		mt->setUpdateCallback(manager.get());
	}

	FindSkeleton find_skeleton;
	node->accept(find_skeleton);
	if(find_skeleton.skeleton)
	{
		RenameAllMatrixTransform rename(object_name);
		find_skeleton.skeleton->accept(rename);
	}

	//NoCull nocull;
	//mt->accept(nocull);

	SetupComputeBoundingBox compute_bb(object_name);
	mt->accept(compute_bb);

	//AddHelperBone add_helper_bone;
	//node->accept(add_helper_bone);

	return true;
}
InterfaceForSkinAnimation::~InterfaceForSkinAnimation()
{
}
/// проиграть анимацию
void InterfaceForSkinAnimation::play_animation(const std::string& object_name,const std::string& animation_name,int priority,float weight)
{
	SkinAnimationManager* manager = find_skin_animation_manager(object_name);
	if(manager)	manager->play(animation_name,priority,weight);
}
/// остановить анимацию
void InterfaceForSkinAnimation::stop_animation(const std::string& object_name,const std::string& animation_name)
{
	SkinAnimationManager* manager = find_skin_animation_manager(object_name);
	if(manager)	manager->stop(animation_name);
}
/// перейти ко времени
void InterfaceForSkinAnimation::goto_time_animation(const std::string& object_name,const std::string& animation_name,float time,float weight)
{
	SkinAnimationManager* manager = find_skin_animation_manager(object_name);
	if(manager)	manager->goto_time(animation_name,time,weight);
}
/// поиск узла
osg::MatrixTransform* InterfaceForSkinAnimation::find_node(const std::string& node_name)
{
	FindNamedNodeVisitor finder(node_name);
	m_root->accept(finder);
	if(finder._foundNodes.empty())
	{
		std::cout << "Find objects in Root - FALSE! " << node_name << std::endl;
		return 0;
	}
	if(finder._foundNodes.size() > 1)
		std::cout << "Warning: Duplicate names - " << node_name << std::endl;
	return finder._foundNodes.front().get();
}
/// поиск менеджера анимаций для узла
SkinAnimationManager* InterfaceForSkinAnimation::find_skin_animation_manager(const std::string& object_name)
{
	if(osg::MatrixTransform* node = find_node(object_name))		return dynamic_cast<SkinAnimationManager*>(node->getUpdateCallback());
	return 0;
}
/// поиск кости
osgAnimation::Bone* InterfaceForSkinAnimation::find_bone(const std::string& bone_name)
{
	return dynamic_cast<osgAnimation::Bone*>(find_node(bone_name));
}
/// повернуть кость
void InterfaceForSkinAnimation::rotate_bone(const std::string& object_name,const std::string& bone_name,const std::string& rotate_name,const osg::Vec3& axis,float angle)
{
	const std::string full_bone_name(object_name+"_"+bone_name);
	if(osgAnimation::Bone* bone = find_bone(full_bone_name))
	{
		if(osgAnimation::UpdateBone* update_bone = dynamic_cast<osgAnimation::UpdateBone*>(bone->getUpdateCallback()))
		{
			if(osgAnimation::StackedTransformElement* transform = find_stacked_transform_element(update_bone,rotate_name))
			{
				if(osgAnimation::StackedRotateAxisElement* rotation = dynamic_cast<osgAnimation::StackedRotateAxisElement*>(transform))
				{
					rotation->setAngle(angle);
					rotation->setAxis(axis);
				}
				else std::cout << "is not StackedRotateAxisElement: " << rotate_name << " bone: " << full_bone_name << std::endl;
			}
			else update_bone->getStackedTransforms().push_back(new osgAnimation::StackedRotateAxisElement(rotate_name,axis,angle));
		}
	}
}
/// найти элемент стэка трансформаций
osgAnimation::StackedTransformElement* InterfaceForSkinAnimation::find_stacked_transform_element(osgAnimation::UpdateBone* update_bone,const std::string& name)
{
	osgAnimation::StackedTransform& stacked = update_bone->getStackedTransforms();
	for(osgAnimation::StackedTransform::const_iterator it=stacked.begin(), end=stacked.end(); it!=end; ++it)
	{
		osgAnimation::StackedTransformElement* te = it->get();
		if(te->getName() == name)
			return te;
	}
	return 0;
}