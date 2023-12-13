#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <iostream>

class FindAndUnrefTextureVisitor : public osg::NodeVisitor
{
public:
    FindAndUnrefTextureVisitor():
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
		{
			setTraversalMask(0xffffffff);
			setNodeMaskOverride(0xffffffff);
		}
    
    virtual void apply(osg::Node& node);
    virtual void apply(osg::MatrixTransform& node);
    virtual void apply(osg::Geode& geode);
    virtual void apply(osg::Group& group);
    osg::ref_ptr<osg::Texture2D> texture;
};
