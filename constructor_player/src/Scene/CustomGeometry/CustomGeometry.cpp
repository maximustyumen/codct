/***************************************************************************
CustomGeometry.cpp  - класс описания OpenGL wrapper (интеграция читого кода OpenGL)
-------------------
begin                : 9 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./CustomGeometry.h"

static void PureGL()
{
	glPushAttrib(GL_ENABLE_BIT | GL_EVAL_BIT);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBegin(GL_POLYGON);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 10.0f, 0.0f);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(10.0f, 10.0f, 0.0f);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(10.0f, 0.0f, 0.0f);
	glEnd();

	
	glPopAttrib();
}


CustomGeometry::CustomGeometry()
{
}


void CustomGeometry::drawImplementation(osg::RenderInfo&) const
{
    //disableAllVertexArrays()
	//disable VBO? выключить списки отображения
    // just call the OpenGL code.
	PureGL();
}
        
        
        // we need to set up the bounding box of the data too, so that the scene graph knows where this
        // objects is, for both positioning the camera at start up, and most importantly for culling.
osg::BoundingSphere CustomGeometry::computeBound() const
{
    osg::BoundingSphere bbox;
	//
    bbox.expandBy(osg::Vec3(0,0,0));
    bbox.expandBy(osg::Vec3(100,100,100));
    //
    return bbox;
}

    
    
CustomGeometry::~CustomGeometry() 
{
	std::cout << "Destructor CustomGeometry."  << std::endl;
}
        



