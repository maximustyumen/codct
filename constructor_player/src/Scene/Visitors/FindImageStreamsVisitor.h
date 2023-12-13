#ifndef FindImageStreamsVisitorClass
#define FindImageStreamsVisitorClass

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <iostream>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/TextureCubeMap>
#include <osg/TexMat>
#include <osg/CullFace>
#include <osg/ImageStream>
#include <osg/io_utils>

class FindImageStreamsVisitor : public osg::NodeVisitor
{
public:
	typedef std::vector< osg::observer_ptr<osg::ImageStream> > ImageStreamList;

    FindImageStreamsVisitor(ImageStreamList& imageStreamList):
        _imageStreamList(imageStreamList) {}
        
    virtual void apply(osg::Geode& geode)
    {
        apply(geode.getStateSet());

        for(unsigned int i=0;i<geode.getNumDrawables();++i)
        {
            apply(geode.getDrawable(i)->getStateSet());
        }
    
        traverse(geode);
    }

    virtual void apply(osg::Node& node)
    {
        apply(node.getStateSet());
        traverse(node);
    }

	virtual void apply(osg::MatrixTransform& node)
    {
        apply(node.getStateSet());
        traverse(node);
    }

	virtual void apply(osg::Group & node)
    {
        apply(node.getStateSet());
        traverse(node);
    }
    
    inline void apply(osg::StateSet* stateset)
    {
        if (!stateset) return;
        
        osg::StateAttribute* attr = stateset->getTextureAttribute(0,osg::StateAttribute::TEXTURE);
        if (attr)
        {
            osg::Texture2D* texture2D = dynamic_cast<osg::Texture2D*>(attr);
            if (texture2D) apply(dynamic_cast<osg::ImageStream*>(texture2D->getImage()));

            osg::TextureRectangle* textureRec = dynamic_cast<osg::TextureRectangle*>(attr);
            if (textureRec) apply(dynamic_cast<osg::ImageStream*>(textureRec->getImage()));
        }
    }
    
    inline void apply(osg::ImageStream* imagestream)
    {
        if (imagestream)
        {
            _imageStreamList.push_back(imagestream); 
            s_imageStream = imagestream;
        }
    }
    
	
    ImageStreamList& _imageStreamList;
	osg::ImageStream* s_imageStream;//=0
};


#endif

