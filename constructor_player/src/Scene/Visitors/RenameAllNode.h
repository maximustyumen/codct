#ifndef RENAMENODECLASS
#define RENAMENODECLASS

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <iostream>

class RenameNodeVisitor : public osg::NodeVisitor
{
public:
    RenameNodeVisitor(const std::string& name):
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
        _name(name) 
		{
			setTraversalMask(0xffffffff);
			setNodeMaskOverride(0xffffffff);
		}
	virtual void apply(osg::MatrixTransform& node);
    std::string _name;
};


#endif

