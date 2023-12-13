#include "skin_animation_manager.h"

SkinAnimationManager::SkinAnimationManager(const AnimationManagerBase& base, const osg::CopyOp& copyop)
: osgAnimation::BasicAnimationManager(base,copyop)
{
	for(osgAnimation::AnimationList::const_iterator it = getAnimationList().begin(); it != getAnimationList().end(); it++)
		m_animations[(*it)->getName()] = *it;
}
SkinAnimationManager::~SkinAnimationManager()
{
}
/// проиграть анимацию
void SkinAnimationManager::play(const std::string& name,int priority,float weight)
{
	if(osgAnimation::Animation* animation = find_animation(name))	playAnimation(animation,priority,weight);
}
/// остановить анимацию
void SkinAnimationManager::stop(const std::string& name)
{
	if(osgAnimation::Animation* animation = find_animation(name))	stopAnimation(animation);
}
/// перейти ко времени
void SkinAnimationManager::goto_time(const std::string& name,float time,float weight)
{
	if(osgAnimation::Animation* animation = find_animation(name))	m_goto_update.push_back(GotoTimeUpdate(animation,time,weight));
}
/// найти анимацию
osgAnimation::Animation* SkinAnimationManager::find_animation(const std::string& name)
{
	animations_t::iterator it = m_animations.find(name);
	if(it != m_animations.end())	return it->second;

	std::cout << "couldn't find animation: " << name << std::endl;
	return 0;
}
/// обновление
void SkinAnimationManager::update(double t)
{
	for(goto_update_t::iterator it=m_goto_update.begin();it!=m_goto_update.end();++it)
	{
		it->animation->setWeight(it->weight);

		float start_time = t - it->time;
		it->animation->setStartTime(start_time);
		it->animation->update(t);
	}
	m_goto_update.clear();

	osgAnimation::BasicAnimationManager::update(t);
}