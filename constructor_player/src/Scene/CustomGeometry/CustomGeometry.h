#include <osg/Geode>
#include <iostream>


// Now the OSG wrapper for the above OpenGL code, the most complicated bit is computing
// the bounding box for the above example, normally you'll find this the easy bit.
class CustomGeometry : public osg::Drawable
{
    public:
        CustomGeometry();

        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        CustomGeometry(const CustomGeometry& teapot,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):  osg::Drawable(teapot,copyop) {}

        META_Object(myTeapotApp,CustomGeometry)


        // the draw immediate mode method is where the OSG wraps up the drawing of
        // of OpenGL primitives.
        virtual void drawImplementation(osg::RenderInfo&) const;
        
        
        // we need to set up the bounding box of the data too, so that the scene graph knows where this
        // objects is, for both positioning the camera at start up, and most importantly for culling.
        virtual osg::BoundingSphere computeBound() const;

    protected:
    
        virtual ~CustomGeometry();
        
};


//+test
//osg::ref_ptr<osg::Geode> createTeapot();
