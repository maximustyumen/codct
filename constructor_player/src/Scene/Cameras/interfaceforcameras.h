#ifndef INTERFACEFORCAMERAS_H
#define INTERFACEFORCAMERAS_H

#include <iostream>
#include <osgViewer/Viewer>
#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osg/Camera>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/GLExtensions>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
//#include <osg/CameraNode>
#include <osg/ComputeBoundsVisitor>
#include <osg/io_utils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/TexGenNode>

#include <iostream>


class InterfaceForCameras
{
public:
    InterfaceForCameras(osg::ref_ptr<osg::Group> mainRoot, osg::ref_ptr<osg::Group> TopRoot, int width, int height);
    ~InterfaceForCameras();
    //сброс
    bool Reset();
    //создание камеры
    void CreateCamera(int dx);
    //
    void CreateCamera2 (osg::ref_ptr<osgViewer::Viewer> viewer);
private:
    //указатель на главную группу сцены
    osg::ref_ptr<osg::Group> GraphicDataRoot;
    //
    osg::ref_ptr<osg::Group> TopRoot;
    //
    //
    int screen_width;
    int screen_height;
};






class UpdateCameraAndTexGenCallback : public osg::NodeCallback
{
    public:

        UpdateCameraAndTexGenCallback(const osg::Vec3& position, osg::Camera* camera, osg::TexGenNode* texgenNode):
            _position(position),
            _camera(camera),
            _texgenNode(texgenNode)
        {
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {

            // first update subgraph to make sure objects are all moved into postion
            traverse(node,nv);

/*
            static float iii=0;
            iii+=0.001f;


            osg::Matrixd myCameraMatrix;
            osg::Matrixd cameraRotation;
            osg::Matrixd cameraTrans;

            cameraRotation.makeRotate(
            osg::DegreesToRadians(0.0f), osg::Vec3(0,1,0), // roll z
            osg::DegreesToRadians(0.0f), osg::Vec3(1,0,0) , // pitch y
            osg::DegreesToRadians(1.0f), osg::Vec3(0,0,1) ); // heading x

            cameraTrans.makeTranslate( 0,0,0 ); //x z y
            myCameraMatrix = cameraRotation * cameraTrans;

            osg::Matrixd i = myCameraMatrix.inverse(myCameraMatrix);
            _camera->setViewMatrix((osg::Matrixd(i.ptr()))* osg::Matrixd::rotate( -3.14f / 2.0, 0, 0, 0 ));


           // _camera->setReferenceFrame(osg::CameraNode::ABSOLUTE_RF);
            //_camera->setProjectionMatrixAsFrustum(-right,right,-top,top,znear,zfar);
           // _camera->setViewMatrixAsLookAt(_position,osg::Vec3(100,0,0),osg::Vec3(0.0f,1.0f,0.0f));

            std::cout << ".";
*/

         }

    protected:

        virtual ~UpdateCameraAndTexGenCallback() {}

        osg::Vec3                     _position;
        osg::ref_ptr<osg::Camera>     _camera;
        osg::ref_ptr<osg::TexGenNode> _texgenNode;
};

#endif // INTERFACEFORCAMERAS_H
