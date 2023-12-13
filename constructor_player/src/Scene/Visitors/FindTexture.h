#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <iostream>

class FindTextureVisitor : public osg::NodeVisitor
{
public:
    FindTextureVisitor(unsigned int channel=0):
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
		{
			Channel = channel;
			setTraversalMask(0xffffffff);
			setNodeMaskOverride(0xffffffff);
			Sucessful=false;
		}
    
    virtual void apply(osg::Node& node);

    virtual void apply(osg::MatrixTransform& node);
	
	virtual void apply(osg::Geode& geode);
     
	virtual void apply(osg::Group& group);

	unsigned int Channel;
	osg::ref_ptr<osg::Texture2D> texture;
	osg::ref_ptr<osg::Image> image;
	osg::ref_ptr<osg::Material> material;
	osg::ref_ptr<osg::StateSet> stateset;
	osg::ref_ptr<osg::Drawable> drawable;
private:
	bool Sucessful;

};
