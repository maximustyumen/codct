#ifndef FINDLIGHTBYNAME
#define FINDLIGHTBYNAME true

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <iostream>

// LightSource - used to hold osg::Lights
#include <osg/LightSource>
// Light - represents the functionality of the OpenGL light model
#include <osg/Light>

class FindLightVisitor : public osg::NodeVisitor
{
public:
    FindLightVisitor():
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
		{
			LS.clear();
			setTraversalMask(0xffffffff);
			setNodeMaskOverride(0xffffffff);
		}

	virtual void apply(osg::Group& group);
	virtual void apply(osg::LightSource& ls);
	virtual void apply(osg::MatrixTransform& node);

	//osg::LightSource* LS;
	std::vector <osg::LightSource*> LS;
};

#endif