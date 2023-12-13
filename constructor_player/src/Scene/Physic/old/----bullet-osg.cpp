
/*
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgProducer/Viewer>
#include <osg/CoordinateSystemNode>
#include <osg/PositionAttitudeTransform>

#include <btBulletDynamicsCommon.h>

float phys2world_scale = 100.0;
extern ContactAddedCallback gContactAddedCallback;

class BallUpdateCallback : public osg::NodeCallback
{
    private:
        btRigidBody *_body;

    public:
        BallUpdateCallback(btRigidBody *body) :
                _body(body)
        {}

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            btScalar m[16];

            btDefaultMotionState* myMotionState = ( btDefaultMotionState*)_body->getMotionState();
            myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);

            osg::Matrixf mat(m);

            osg::PositionAttitudeTransform *pat = dynamic_cast<osg::PositionAttitudeTransform *>(node);
            pat->setPosition(mat.getTrans());
            pat->setAttitude(mat.getRotate());

            //std::cout << mat.getTrans().x() << ", " << mat.getTrans().y() << ", " << mat.getTrans().z() << std::endl;

            traverse(node, nv);
        }
};

bool materialCombinerCallback(btManifoldPoint& cp,
                              const btCollisionObject* colObj0, int partId0,
                              int index0, const btCollisionObject* colObj1,
                              int partId1, int index1)
{
    btVector3 impact_point = cp.getPositionWorldOnA();

    if (!colObj0->isStaticObject() && colObj0->isActive())
    {
        std::cout << "Hit between " << static_cast<osg::Node *>(colObj0->getUserPointer())->getName() << " and " << static_cast<osg::Node *>(colObj1->getUserPointer())->getName() << " at [" << impact_point.getX() << ", "
        << impact_point.getY() << ", " << impact_point.getZ() << "] "
        << std::endl;
    }

    return false;
}

btRigidBody* createRigidBody(btDynamicsWorld *world, float mass,
                             const btTransform& startTransform,
                             btCollisionShape* shape)
{
    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody* body = new btRigidBody(mass, myMotionState, shape, localInertia);

    world->addRigidBody(body);

    return body;
}

int main(int argc, char **argv)
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc, argv);

    // construct the viewer.
    osgProducer::Viewer viewer(arguments);

    // set up the value with sensible default event handlers.
    viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
    viewer.setBlockOnVsync(true);

    // scenegraph root
    osg::ref_ptr<osg::Group> root = new osg::Group;

    // read the scene from the list of file specified command line args.
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("data/ball.osg");
    osg::ref_ptr<osg::PositionAttitudeTransform>
    ball_pat = new osg::PositionAttitudeTransform;
    //ball_pat->setScale(osg::Vec3(100, 100, 100));
    ball_pat->addChild(loadedModel.get());
    root->addChild(ball_pat.get());

    osg::ref_ptr<osg::Node> loadedModel2 = osgDB::readNodeFile("data/ground-plane.osg");
    osg::ref_ptr<osg::PositionAttitudeTransform>
    ground_pat = new osg::PositionAttitudeTransform;
    ground_pat->addChild(loadedModel2.get());
    ground_pat->setScale(osg::Vec3(100.0, 100.0, 0.1));
    root->addChild(ground_pat.get());

    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(root.get());

    // pass the loaded scene graph to the viewer.
    viewer.setSceneData(root.get());

    //
    // Physics
    //
    btVector3 worldAabbMin( -1000, -1000, -1000);
    btVector3 worldAabbMax(1000, 1000, 1000);
    const int maxProxies = 32766;

    // register global callback
    gContactAddedCallback = materialCombinerCallback;

    btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver;
    btAxisSweep3 *broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);

    btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    btDynamicsWorld *m_dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0, 0, -10.0));

    // ground plane
    {
        btCollisionShape *gnd_shape = new btStaticPlaneShape(btVector3(0, 0, 1), 0.5);

        btVector4 pos;
        pos.setValue(0, 0, -0.5);
        btTransform trans;
        trans.setIdentity();
        trans.setOrigin(pos);
        btScalar mass = 0.f;

        btRigidBody* gnd = createRigidBody(m_dynamicsWorld, mass, trans,
                                           gnd_shape);

        gnd->setUserPointer(loadedModel2.get());
        gnd->setCollisionFlags(gnd->getCollisionFlags()
                               | btCollisionObject::CF_KINEMATIC_OBJECT
                               | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        gnd->setActivationState(DISABLE_DEACTIVATION);
    }

    // lander
    btRigidBody* lnd;
    btCollisionShape *lnd_shape;
    {
        lnd_shape = new btBoxShape(btVector3(1, 1, 1));

        btVector4 pos;
        pos.setValue(0, 0, 20.0);
        btTransform trans;
        trans.setIdentity();
        trans.setOrigin(pos);
        btScalar mass = 1.f;

        lnd = createRigidBody(m_dynamicsWorld, mass, trans, lnd_shape);
        lnd->setUserPointer(loadedModel.get());
        lnd->setCollisionFlags(lnd->getCollisionFlags()
                               | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        lnd->setCcdSquareMotionThreshold( 0.5);
        lnd->setCcdSweptSphereRadius( 0.2 * 0.5);
    }
    ball_pat->setUpdateCallback(new BallUpdateCallback(lnd));

    // create the windows and run the threads.
    viewer.realize();

    osg::Timer_t frame_tick = osg::Timer::instance()->tick();
    while ( !viewer.done() )
    {
        // wait for all cull and draw threads to complete.
        viewer.sync();

        // Physics update
        osg::Timer_t now_tick = osg::Timer::instance()->tick();
        float dt = osg::Timer::instance()->delta_s(frame_tick, now_tick);
        frame_tick = now_tick;
        // int numSimSteps 
        m_dynamicsWorld->stepSimulation(dt); //, 10, 0.01);
        m_dynamicsWorld->updateAabbs();

        // update the scene by traversing it with the the update visitor which will
        // call all node update callbacks and animations.
        viewer.update();

        // fire off the cull and draw traversals of the scene.
        viewer.frame();
    }

    // wait for all cull and draw threads to complete.
    viewer.sync();

    // run a clean up frame to delete all OpenGL objects.
    viewer.cleanup_frame();

    // wait for all the clean up frame to complete.
    viewer.sync();

    return 0;
}

*/