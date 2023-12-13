#ifndef INTERFACE_FOR_SKIN_ANIMATION
#define INTERFACE_FOR_SKIN_ANIMATION

#include <osgAnimation/BasicAnimationManager>

class SkinAnimationManager;
namespace osgAnimation
{
	class StackedTransformElement;
	class UpdateBone;
	class Bone;
}

class InterfaceForSkinAnimation
{
private:
	osg::ref_ptr<osg::Group> m_root;

public:
	InterfaceForSkinAnimation(osg::ref_ptr<osg::Group> root);
	virtual ~InterfaceForSkinAnimation();
	
public:
	/// загрузить модельку c анимациями
	bool load_skin(const std::string& object_name,const std::string& filename);
	/// проиграть анимацию
	void play_animation(const std::string& object_name,const std::string& animation_name,int priority = 0,float weight = 1.f);
	/// остановить анимацию
	void stop_animation(const std::string& object_name,const std::string& animation_name);
	/// перейти ко времени
	void goto_time_animation(const std::string& object_name,const std::string& animation_name,float time,float weight = 1.f);
	/// повернуть кость
	void rotate_bone(const std::string& object_name,const std::string& bone_name,const std::string& rotate_name,const osg::Vec3& axis,float angle);

private:
	/// поиск менеджера анимаций для узла
	SkinAnimationManager* find_skin_animation_manager(const std::string& object_name);
	/// поиск узла
	osg::MatrixTransform* find_node(const std::string& node_name);
	/// поиск кости
	osgAnimation::Bone* find_bone(const std::string& bone_name);
	/// найти элемент стэка трансформаций
	osgAnimation::StackedTransformElement* find_stacked_transform_element(osgAnimation::UpdateBone* update_bone,const std::string& name);
};

#endif