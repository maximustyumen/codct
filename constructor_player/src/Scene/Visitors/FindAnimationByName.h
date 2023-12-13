#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <iostream>

class FindAnimationVisitor : public osg::NodeVisitor
{
public:
    FindAnimationVisitor():
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
		{
			//apc=NULL;
			setTraversalMask(0xffffffff);
			setNodeMaskOverride(0xffffffff);
		}

	//найденные анимации
	typedef std::vector< osg::ref_ptr<osg::AnimationPathCallback> > NodeList;
	NodeList _foundNodes;
	//и соответственно объекты, содержащие эти самые анимации
	typedef std::vector< osg::ref_ptr<osg::Transform> > TransformList;
    TransformList _foundTransforms;
    
    virtual void apply(osg::Transform& node);
	virtual void apply(osg::Group& group);
	/*
	virtual void apply(osg::MatrixTransform& node);
	virtual void apply(osg::AnimationPathCallback& node);
	*/

private:
	//Путь анимации
    //osg::ref_ptr<osg::AnimationPathCallback> apc;
};
