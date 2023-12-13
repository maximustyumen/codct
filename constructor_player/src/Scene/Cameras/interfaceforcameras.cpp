/***************************************************************************
interfaceforcameras.h  -  Определение класса дополнительных камер
                          (например, зеркало заднего вида машины)
-------------------
begin                : 23 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/


// example, osgprerendercubemap.


#include "interfaceforcameras.h"

InterfaceForCameras::InterfaceForCameras(osg::ref_ptr<osg::Group> _mainRoot, osg::ref_ptr<osg::Group> _TopRoot, int width, int height)
{
    GraphicDataRoot = _mainRoot;
    //
    TopRoot = _TopRoot;
    //
    //
    screen_width = width;
    screen_height = height;


    //CreateCamera()
    CreateCamera(0);
    CreateCamera(200);
    CreateCamera(400);
    CreateCamera(600);

}

InterfaceForCameras::~InterfaceForCameras()
{
    std::cout << "Destructor InterfaceForCameras."  << std::endl;
    Reset();
}

//сброс
bool InterfaceForCameras::Reset()
{
    return TopRoot->removeChild(0, TopRoot->getNumChildren());
}






//создание камеры
void InterfaceForCameras::CreateCamera(int dx)
{
    osg::ref_ptr<osg::Group> group = new osg::Group;

    unsigned tex_width = screen_width;
    unsigned tex_height = screen_height;
    int unit = 0;

    /////////////////
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    {
        texture->setTextureSize(tex_width, tex_height);
        texture->setInternalFormat(GL_RGB);
        texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
        texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
        texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
        texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
        texture->setBorderColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
    }
    /////////////////


    osg::Vec4 clearColor (1,0,0,1);

    osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;


    // create the camera
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setName("__ADDCAMERA__");

    camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->setClearColor(clearColor);

    // set viewport
    camera->setViewport(0,0,tex_width,tex_height);

    //!!!!!!!!!!!!!!!!!!!!!!!!
    camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF_INHERIT_VIEWPOINT);
    // set the camera to render before the main camera.
    camera->setRenderOrder(osg::Camera::PRE_RENDER);
    //camera->setRenderOrder(osg::Camera::POST_RENDER); //POST_RENDER //PRE_RENDER

    // tell the camera to use OpenGL frame buffer object where supported.
    camera->setRenderTargetImplementation(renderImplementation);

    // attach the texture and use it as the color buffer.
    camera->attach(osg::Camera::COLOR_BUFFER, texture); //, 0, i

    // add subgraph to render
    camera->addChild(GraphicDataRoot);

    // make the material black for a shadow.
    /*
    osg::StateSet* stateset = camera->getOrCreateStateSet();
    osg::Material* material = new osg::Material;
    material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,0.0f,0.0f,1.0f));
    material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,0.0f,0.0f,0.5f));
    material->setEmission(osg::Material::FRONT_AND_BACK,ambientLightColor);
    material->setShininess(osg::Material::FRONT_AND_BACK,0.0f);
    stateset->setAttribute(material,osg::StateAttribute::OVERRIDE);
    */

    group->addChild(camera);
    //???
    //viewer->addSlave(camera.get());

    // create the texgen node to project the tex coords onto the subgraph
    osg::ref_ptr<osg::TexGenNode> texgenNode = new osg::TexGenNode;
    texgenNode->setTextureUnit(unit);
    group->addChild(texgenNode);

    // set an update callback to keep moving the camera and tex gen in the right direction.
    group->setUpdateCallback(new UpdateCameraAndTexGenCallback(osg::Vec3 (0,0,0), camera, texgenNode));



    // set hud to render shadow texture, just for interest
    {
       osg::ref_ptr<osg::Geode> geode = new osg::Geode;
       osg::ref_ptr<osg::Geometry> geom = osg::createTexturedQuadGeometry(osg::Vec3(0,0,0),osg::Vec3(200.0,0.0,0.0),osg::Vec3(0.0,200.0,0.0));
       geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
       geom->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
       geode->addDrawable(geom);
       osg::ref_ptr<osg::Camera> camera = new osg::Camera;
       // set the projection matrix
       camera->setProjectionMatrix(osg::Matrix::ortho2D(0,100,0,100));
       // set the view matrix
       camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
       camera->setViewMatrix(osg::Matrix::identity());
       camera->setViewport(50+dx,50,100,100);
       // only clear the depth buffer
       camera->setClearMask(GL_DEPTH_BUFFER_BIT);
       // draw subgraph after main camera view.
       camera->setRenderOrder(osg::Camera::POST_RENDER);
       camera->addChild(geode);
       group->addChild(camera);
    }


    TopRoot->addChild(group);
    //TopRoot->addChild(camera);


    {
        osg::Matrixd myCameraMatrix;
        osg::Matrixd cameraRotation;
        osg::Matrixd cameraTrans;



        cameraRotation.makeRotate(
            osg::DegreesToRadians(0.0f), osg::Vec3(0,1,0), // roll z
            osg::DegreesToRadians(0.0f), osg::Vec3(1,0,0) , // pitch y
            osg::DegreesToRadians(-3.14f), osg::Vec3(0,0,1) ); // heading x



        cameraTrans.makeTranslate( dx/10,34,76 ); //x z y
        myCameraMatrix = cameraRotation * cameraTrans;

        osg::Matrixd i = myCameraMatrix.inverse(myCameraMatrix);

       
        camera->setViewMatrix((osg::Matrixd(i.ptr()))* osg::Matrixd::rotate( (float)dx/600.0f, 1, 0, 0 ));
    }
}

//-------------------------------


void InterfaceForCameras::CreateCamera2 (osg::ref_ptr<osgViewer::Viewer> viewer)
{
    /*
   int tex_width = 300;
   int tex_height = 200;
   int unit = 0;

   osg::ref_ptr<osg::Camera> camera = new osg::Camera;
   // set up the background color and clear mask.
   camera->setClearColor(osg::Vec4(0.1f,0.1f,0.3f,1.0f));
   camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // set viewport
   camera->setGraphicsContext(viewer->getCamera()->getGraphicsContext());
   camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   camera->setViewport(0,0,tex_width,tex_height);
   double ratio = tex_height * 1.0 /tex_width;
   camera->setProjectionMatrixAsOrtho(-1, 1,-ratio, ratio, -10000, 10000);
   camera->setViewMatrix(view);

   osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
   camera->setRenderTargetImplementation(renderImplementation);


   /////////////////
   osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
   {
       texture->setTextureSize(tex_width, tex_height);
       texture->setInternalFormat(GL_RGB);
       texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
       texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
       texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
       texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
       texture->setBorderColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
   }
   /////////////////


   // attach the image so its copied on each frame.
   camera->attach(osg::Camera::COLOR_BUFFER, texture);

   viewer->addSlave(camera.get());




   osg::ref_ptr<osg::Group> group = new osg::Group;


   // create the texgen node to project the tex coords onto the subgraph
   osg::ref_ptr<osg::TexGenNode> texgenNode = new osg::TexGenNode;
   texgenNode->setTextureUnit(unit);
   group->addChild(texgenNode);

   // set an update callback to keep moving the camera and tex gen in the right direction.
   group->setUpdateCallback(new UpdateCameraAndTexGenCallback(osg::Vec3 (0,0,0), camera, texgenNode));



   // set hud to render shadow texture, just for interest
   {
      osg::ref_ptr<osg::Geode> geode = new osg::Geode;
      osg::ref_ptr<osg::Geometry> geom = osg::createTexturedQuadGeometry(osg::Vec3(0,0,0),osg::Vec3(200.0,0.0,0.0),osg::Vec3(0.0,200.0,0.0));
      geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
      geom->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
      geode->addDrawable(geom);
      osg::ref_ptr<osg::CameraNode> camera = new osg::CameraNode;
      // set the projection matrix
      camera->setProjectionMatrix(osg::Matrix::ortho2D(0,100,0,100));
      // set the view matrix
      camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      camera->setViewMatrix(osg::Matrix::identity());
      camera->setViewport(1000,50,100,100); //DX
      // only clear the depth buffer
   //camera->setClearMask(GL_DEPTH_BUFFER_BIT);
      // draw subgraph after main camera view.
   //camera->setRenderOrder(osg::CameraNode::POST_RENDER);
      camera->addChild(geode);
      group->addChild(camera);
   }


   TopRoot->addChild(group);
   */
}




//----------------------------------








/*

void InterfaceForCameras::addScreenShootCamera(osgViewer::Viewer* viewer, osg::NodeCallback* callback, const osg::Matrix& view, const std::string& filename)
{
   int tex_width = 300;
   int tex_height = 200;
   osg::ref_ptr<osg::Camera> camera = new osg::Camera;
   // set up the background color and clear mask.
   camera->setClearColor(osg::Vec4(0.1f,0.1f,0.3f,1.0f));
   camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // set viewport
   camera->setGraphicsContext(viewer->getCamera()->getGraphicsContext());
   camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   camera->setViewport(0,0,tex_width,tex_height);
   double ratio = tex_height * 1.0 /tex_width;
   camera->setProjectionMatrixAsOrtho(-1, 1,-ratio, ratio, -10000, 10000);
   camera->setViewMatrix(view);

   osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;

   camera->setRenderTargetImplementation(renderImplementation);
   osg::Image* image = new osg::Image;
   image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
   image->setFileName(filename);
   // attach the image so its copied on each frame.
   camera->attach(osg::Camera::COLOR_BUFFER, image);
   viewer->addSlave(camera.get());
   //AdaptProjectionForSceneSizeForFrontView* callback_casted = dynamic_cast<AdaptProjectionForSceneSizeForFrontView*>(callback);
   //callback_casted->useImage(image);
   //callback_casted->setCamera(camera.get());
   //camera->setUpdateCallback(callback);
}


#include <osg/GLExtensions>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/CameraNode>
#include <osg/ComputeBoundsVisitor>
#include <osg/io_utils>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>

#include <osgUtil/SmoothingVisitor>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <iostream>

struct PSDrawCallback : public osg::Camera::DrawCallback
{
public:
  mutable osg::ref_ptr<osg::Image> _image;
  PSDrawCallback(osg::Image* image): _image(image) {}

  void operator () (osg::RenderInfo& ri) const
  {
    osg::Camera* camera = ri.getCurrentCamera();
    if (camera) {
      osgDB::writeImageFile(*_image, _image->getFileName());
      camera->setPostDrawCallback(0); // delete the callback
    } else
      std::cout << "Error: Camera::DrawCallback called without valid camera." << std::endl;
  }
};

// Cedric Pinson mornifle@plopbyte.net
struct AdaptProjectionForSceneSizeForFrontView : public osg::NodeCallback
{
  osgViewer::Viewer* _viewer;
  bool _needUpdate;
  osg::ref_ptr<osg::Image> _image;
  osg::ref_ptr<osg::Camera> _camera;
  AdaptProjectionForSceneSizeForFrontView(osgViewer::Viewer* viewer) : _viewer(viewer), _needUpdate (false) {}
  void useImage(osg::Image* image) { _image = image; }
  void setCamera(osg::Camera* camera) { _camera = camera;}
  void needUpdate(bool state) { _needUpdate = state; }
  virtual void updateProjection(osg::Camera* cam, const osg::BoundingBox& sceneSize)
  {
    double ratio = _image->t() * 1.0/ _image->s();
    float width = sceneSize._max[0] - sceneSize._min[0];
    float height = sceneSize._max[2] - sceneSize._min[2];
    if (height > width * ratio)
      width = height / ratio;
    height = width * ratio;

    width *= 0.5;
    height *= 0.5;
    std::cout << "front" << std::endl;
    std::cout << -width + sceneSize.center()[0] << " " << width + sceneSize.center()[0] << " "
              << -height + sceneSize.center()[2] << " " << height + sceneSize.center()[2] << std::endl;
    cam->setProjectionMatrixAsOrtho(-width + sceneSize.center()[0],
                                    width + sceneSize.center()[0],
                                    -height + sceneSize.center()[2],
                                    height + sceneSize.center()[2],
                                    -10000,
                                    10000);
  }
  void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    if (_needUpdate && nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && _viewer->getSceneData()) {
      osg::BoundingBox sceneSize;
      osg::ref_ptr<osg::ComputeBoundsVisitor> bb = new osg::ComputeBoundsVisitor;
      _viewer->getSceneData()->accept(*bb);
      sceneSize = bb->getBoundingBox();
      osg::Camera* cam = dynamic_cast<osg::Camera*>(node);
      if (cam) {
        std::cout << cam << " " << _camera.get() << " Scene size " << bb->getBoundingBox()._min << " "  << bb->getBoundingBox()._max << std::endl;
        updateProjection(cam, sceneSize);
        _camera->setPostDrawCallback(new PSDrawCallback(_image.get()));
        _needUpdate = false;
      }
    }
    traverse(node,nv);
  }
};

struct AdaptProjectionForSceneSizeForTopView : public AdaptProjectionForSceneSizeForFrontView
{
  AdaptProjectionForSceneSizeForTopView(osgViewer::Viewer* viewer) : AdaptProjectionForSceneSizeForFrontView(viewer) {}
  virtual void updateProjection(osg::Camera* cam, const osg::BoundingBox& sceneSize)
  {
    std::cout << "top" << std::endl;
    double ratio = _image->t() * 1.0/ _image->s();
    float width = sceneSize._max[0] - sceneSize._min[0];
    float height = sceneSize._max[1] - sceneSize._min[1];
    if (height > width * ratio)
      width = height / ratio;
    height = width * ratio;

    width *= 0.5;
    height *= 0.5;

    std::cout << -width + sceneSize.center()[0] << " " << width + sceneSize.center()[0] << " "
              << -height + sceneSize.center()[1] << " " << height + sceneSize.center()[1] << std::endl;
    cam->setProjectionMatrixAsOrtho(-width + sceneSize.center()[0],
                                    width + sceneSize.center()[0],
                                    -height + sceneSize.center()[1],
                                    height + sceneSize.center()[1],
                                    -10000,
                                    10000);
  }
};

void addScreenShootCamera(osgViewer::Viewer* viewer, osg::NodeCallback* callback, const osg::Matrix& view, const std::string& filename)
{
   int tex_width = 300;
   int tex_height = 200;
   osg::ref_ptr<osg::Camera> camera = new osg::Camera;
   // set up the background color and clear mask.
   camera->setClearColor(osg::Vec4(0.1f,0.1f,0.3f,1.0f));
   camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // set viewport
   camera->setGraphicsContext(viewer->getCamera()->getGraphicsContext());
   camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   camera->setViewport(0,0,tex_width,tex_height);
   double ratio = tex_height * 1.0 /tex_width;
   camera->setProjectionMatrixAsOrtho(-1, 1,-ratio, ratio, -10000, 10000);
   camera->setViewMatrix(view);

   osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
#if 0 // other method to draw
    while (arguments.read("--fbo")) { renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT; }
    while (arguments.read("--pbuffer")) { renderImplementation = osg::Camera::PIXEL_BUFFER; }
    while (arguments.read("--pbuffer-rtt")) { renderImplementation = osg::Camera::PIXEL_BUFFER_RTT; }
    while (arguments.read("--fb")) { renderImplementation = osg::Camera::FRAME_BUFFER; }
#endif
   camera->setRenderTargetImplementation(renderImplementation);
   osg::Image* image = new osg::Image;
   image->allocateImage(tex_width, tex_height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
   image->setFileName(filename);
   // attach the image so its copied on each frame.
   camera->attach(osg::Camera::COLOR_BUFFER, image);
   viewer->addSlave(camera.get());
   AdaptProjectionForSceneSizeForFrontView* callback_casted = dynamic_cast<AdaptProjectionForSceneSizeForFrontView*>(callback);
   callback_casted->useImage(image);
   callback_casted->setCamera(camera.get());
   camera->setUpdateCallback(callback);
}


class MyHandler : public osgGA::GUIEventHandler
{
public:
  osgViewer::Viewer* _viewer;
  MyHandler(osgViewer::Viewer* viewer) {
    _viewer = viewer;
  }
  virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&) {
    switch(ea.getEventType()) {
    case(osgGA::GUIEventAdapter::KEYUP):
      {
        int numSlaves = _viewer->getNumSlaves();
        for (int i = 0; i < numSlaves; i++) {
          AdaptProjectionForSceneSizeForFrontView* callback = dynamic_cast<AdaptProjectionForSceneSizeForFrontView*>(_viewer->getSlave(i)._camera->getUpdateCallback());
          if (callback)
            callback->needUpdate(true);
        }
        return true;
      }
      break;
    }
    return false;
  }
};

int main( int argc, char **argv )
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    // construct the viewer.
    osgViewer::Viewer viewer(arguments);
    viewer.setUpViewInWindow(0,0,800,600);

    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
    //  viewer->setThreadingModel(osgViewer::Viewer::CullDrawThreadPerContext);
    //    viewer->setThreadingModel(osgViewer::Viewer::CullThreadPerCameraDrawThreadPerContext);

    // add stats
    viewer.addEventHandler( new osgViewer::StatsHandler() );

    // add the threading handler
    //    viewer.addEventHandler( new osgViewer::ThreadingHandler() );

    // if user request help write it out to cout.
    if (arguments.read("-h") || arguments.read("--help"))
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }


    // load the nodes from the commandline arguments.
    osg::Node* loadedModel = osgDB::readNodeFiles(arguments);

    // if not loaded assume no arguments passed in, try use default mode instead.
    if (!loadedModel) loadedModel = osgDB::readNodeFile("cessna.osg");

    if (!loadedModel)
    {
        return 1;
    }

    // create a transform to spin the model.
    osg::MatrixTransform* loadedModelTransform = new osg::MatrixTransform;
    loadedModelTransform->addChild(loadedModel);

    MyHandler* handler = new MyHandler(&viewer);
    viewer.addEventHandler( handler );

    // add model to the viewer.
    viewer.setSceneData( loadedModelTransform );
    //    viewer.realize();


    addScreenShootCamera(&viewer, new AdaptProjectionForSceneSizeForFrontView(&viewer), osg::Matrix::lookAt(osg::Vec3(0,-1000, 0), osg::Vec3(0,0,0), osg::Vec3(0,0,1)),"front.png");
    addScreenShootCamera(&viewer, new AdaptProjectionForSceneSizeForTopView(&viewer) , osg::Matrix::lookAt(osg::Vec3(0,0, 1000), osg::Vec3(0,0,0), osg::Vec3(0,1,0)),"top.png");


    std::cout << "threading model " <<  viewer.getThreadingModel() << std::endl;
    std::cout << "SingleThreaded " << osgViewer::ViewerBase::SingleThreaded << std::endl;
    std::cout << "CullDrawThreadPerContext " << osgViewer::ViewerBase::CullDrawThreadPerContext << std::endl;
    std::cout << "ThreadPerContext " << osgViewer::ViewerBase::ThreadPerContext << std::endl;
    std::cout << "DrawThreadPerContext " << osgViewer::ViewerBase::DrawThreadPerContext << std::endl;
    std::cout << "CullThreadPerCameraDrawThreadPerContext " << osgViewer::ViewerBase::CullThreadPerCameraDrawThreadPerContext << std::endl;
    std::cout << "ThreadPerCamera " << osgViewer::ViewerBase::ThreadPerCamera << std::endl;
    std::cout << "AutomaticSelection " << osgViewer::ViewerBase::AutomaticSelection << std::endl;


    return viewer.run();
}
*/
