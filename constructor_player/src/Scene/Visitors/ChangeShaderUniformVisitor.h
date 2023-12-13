#ifndef CHANGEShaderUniformVisitor
#define CHANGEShaderUniformVisitor
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <iostream>
#include <osg/BlendFunc>
#include <osg/TexEnvCombine>

class ChangeShaderUniformVisitor : public osg::NodeVisitor
{
public:
    ChangeShaderUniformVisitor(std::string uniformname, float value);
    
    virtual void apply(osg::Node& node);

    virtual void apply(osg::MatrixTransform& node);
	
	virtual void apply(osg::Geode& geode);
     
	virtual void apply(osg::Group& group);



private:

	std::string uniformname;
	float value;
};


#endif

