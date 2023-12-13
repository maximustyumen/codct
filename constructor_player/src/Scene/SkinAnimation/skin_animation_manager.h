#ifndef SKIN_ANIMATION_MANAGER
#define SKIN_ANIMATION_MANAGER

#include <osgAnimation/BasicAnimationManager>

class SkinAnimationManager : public osgAnimation::BasicAnimationManager
{
private:
	typedef osgAnimation::AnimationMap animations_t;
	animations_t m_animations;

	struct GotoTimeUpdate
	{
		GotoTimeUpdate(osgAnimation::Animation* _animation,float _time,float _weight)
		: animation(_animation), weight(_weight), time(_time)
		{}
		
		osgAnimation::Animation* animation;
		float weight;
		float time;
	};

	typedef std::vector<GotoTimeUpdate> goto_update_t;
	goto_update_t m_goto_update;

public:
	SkinAnimationManager(const AnimationManagerBase& base, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	virtual ~SkinAnimationManager();
	
public:
	/// проиграть анимацию
	void play(const std::string& name,int priority,float weight);
	/// остановить анимацию
	void stop(const std::string& name);
	/// перейти ко времени
	void goto_time(const std::string& name,float time,float weight);

public:
	/// обновление
	virtual void update(double t);

private:
	/// найти анимацию
	osgAnimation::Animation* find_animation(const std::string& name);
};

#endif