/***************************************************************************
PhysicBULLET.cpp  -  интерфейс для работы с физикой на основе BULLET
-------------------
begin                : 7 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

//   http://codesuppository.blogspot.com/2006/04/approximate-convex-decomposition.html
//   gimpact see http://gimpact.sourceforge.net/reference_html/programmers_guide.html

#include "./PhysicBULLET.h"

//Callback
extern ContactAddedCallback gContactAddedCallback;
bool InterfaceForPhysic::materialCombinerCallback(btManifoldPoint& cp,
                              const btCollisionObject* colObj0, int partId0,
                              int index0, const btCollisionObject* colObj1,
                              int partId1, int index1)
{
    btVector3 impact_point = cp.getPositionWorldOnA();
    if (!colObj0->isStaticObject() && colObj0->isActive())
    {
		//static_cast<osg::Node *>(colObj0->getUserPointer())->getName()
        std::cout << "Hit between " << static_cast<osg::MatrixTransform *>(colObj0->getUserPointer())->getName() << " and " << static_cast<osg::MatrixTransform *>(colObj1->getUserPointer())->getName() << " at [" << impact_point.getX() << ", "
        << impact_point.getY() << ", " << impact_point.getZ() << "] "
        << std::endl;
    }
    return false;
}

//конструктор
InterfaceForPhysic::InterfaceForPhysic(osg::ref_ptr<osg::Group> _root)
{
	std::cout << "BULLET version " << btGetVersion() << std::endl;
	//запоминаем указатель на группу сцены
	root = _root;

	//Инициализация
	//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();
	//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	//the maximum size of the collision world. Make sure objects stay within these boundaries
	//Don't make the world AABB size too large, it will harm simulation quality and performance
	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);
	int	maxProxies = 1024;
	overlappingPairCache = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,0,-9.8*10));
	dynamicsWorld->getDispatchInfo().m_enableSPU = true;

	// register global callback
    gContactAddedCallback = materialCombinerCallback;
	
	//Включить если нужен GIMPACT (mesh vs mesh)
	//btCollisionDispatcher * GIMPACTdispatcher = static_cast<btCollisionDispatcher *>(dynamicsWorld->getDispatcher());
	//btGImpactCollisionAlgorithm::registerAlgorithm(GIMPACTdispatcher);

	//создаем "землю", чтобы объекты не проваливались...
	AddGroundBody();
}

//Связь point to point constraint (ball socket)
bool InterfaceForPhysic::AddBallSocket(std::string ObjectName1, std::string ObjectName2)
{
	//проверка на наличие необходимый объектов
	if ((bodies.count(ObjectName1)==0) || (bodies.count(ObjectName2)==0) )
	{
		std::cout << "Warning: Physic object " << ObjectName1 << " or " << ObjectName2 << " NOT exsist!" << std::endl;
		return false;
	}

	btVector3 pivotInA = bodies[ObjectName1].rigidBody->getCenterOfMassPosition();   // .inverse()(body0->getCenterOfMassTransform()(pivotInA)) : pivotInA;
	btVector3 pivotInB = bodies[ObjectName2].rigidBody->getCenterOfMassPosition();
	//?
	//dynamicsWorld->removeRigidBody (bodies[ObjectName1].rigidBody);
	//dynamicsWorld->removeRigidBody (bodies[ObjectName2].rigidBody);
	//?
	btTypedConstraint* p2p = new btPoint2PointConstraint(*bodies[ObjectName1].rigidBody,*bodies[ObjectName2].rigidBody,pivotInA,pivotInB);
	//
	//  attach piler1 to the plate
    //  btVector3 pivA1(3., 3., 3.);
    //  btVector3 pivB1(0., -3., 0.);
    //  btTypedConstraint* p2p1 = new btPoint2PointConstraint(*pPlate,*spPiler1, pivA1, pivB1);
	//

	dynamicsWorld->addConstraint (p2p);
	return true;
}

/*
//point to point constraint (ball socket)
	{
		btRigidBody* body0 = localCreateRigidBody( mass,trans,shape);
		trans.setOrigin(btVector3(2*CUBE_HALF_EXTENTS,20,0));

		mass = 1.f;
		btRigidBody* body1 = 0;//localCreateRigidBody( mass,trans,shape);
		//body1->setActivationState(DISABLE_DEACTIVATION);
		//body1->setDamping(0.3,0.3);

		btVector3 pivotInA(CUBE_HALF_EXTENTS,-CUBE_HALF_EXTENTS,-CUBE_HALF_EXTENTS);
		btVector3 axisInA(0,0,1);

		btVector3 pivotInB = body1 ? body1->getCenterOfMassTransform().inverse()(body0->getCenterOfMassTransform()(pivotInA)) : pivotInA;
		btVector3 axisInB = body1? 
			(body1->getCenterOfMassTransform().getBasis().inverse()*(body1->getCenterOfMassTransform().getBasis() * axisInA)) : 
		body0->getCenterOfMassTransform().getBasis() * axisInA;

		//btTypedConstraint* p2p = new btPoint2PointConstraint(*body0,*body1,pivotInA,pivotInB);
		//btTypedConstraint* hinge = new btHingeConstraint(*body0,*body1,pivotInA,pivotInB,axisInA,axisInB);
		btHingeConstraint* hinge = new btHingeConstraint(*body0,pivotInA,axisInA);
		
		//use zero targetVelocity and a small maxMotorImpulse to simulate joint friction
		//float	targetVelocity = 0.f;
		//float	maxMotorImpulse = 0.01;
		float	targetVelocity = 1.f;
		float	maxMotorImpulse = 1.0f;
		hinge->enableAngularMotor(true,targetVelocity,maxMotorImpulse);

		m_dynamicsWorld->addConstraint(hinge);//p2p);

	}

	
	//create a slider, using the generic D6 constraint
	{
		mass = 1.f;
		btVector3 sliderWorldPos(0,10,0);
		btVector3 sliderAxis(1,0,0);
		btScalar angle=0.f;//SIMD_RADS_PER_DEG * 10.f;
		btMatrix3x3 sliderOrientation(btQuaternion(sliderAxis ,angle));
		trans.setIdentity();
		trans.setOrigin(sliderWorldPos);
		//trans.setBasis(sliderOrientation);
		sliderTransform = trans;

		d6body0 = localCreateRigidBody( mass,trans,shape);
		d6body0->setActivationState(DISABLE_DEACTIVATION);
		btRigidBody* fixedBody1 = localCreateRigidBody(0,trans,0);
		m_dynamicsWorld->addRigidBody(fixedBody1);

		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInB = btTransform::getIdentity();

		bool useLinearReferenceFrameA = false;//use fixed frame B for linear limits
		btGeneric6DofConstraint* slider = new btGeneric6DofConstraint(*d6body0,*fixedBody1,frameInA,frameInB,useLinearReferenceFrameA);
		slider->setLinearLowerLimit(lowerSliderLimit);
		slider->setLinearUpperLimit(hiSliderLimit);

		//range should be small, otherwise singularities will 'explode' the constraint
		slider->setAngularLowerLimit(btVector3(10,0,0));
		slider->setAngularUpperLimit(btVector3(0,0,0));

		m_dynamicsWorld->addConstraint(slider);

	}
*/

//сброс всей физики
bool InterfaceForPhysic::Reset()
{
	/*
	//cleanup in the reverse order of creation/initialization
	int i;
	//removed/delete constraints
	for (i=m_dynamicsWorld->getNumConstraints()-1; i>=0 ;i--)
	{
		btTypedConstraint* constraint = m_dynamicsWorld->getConstraint(i);
		m_dynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}
	//remove the rigidbodies from the dynamics world and delete them
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}
	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}
	//delete dynamics world
	delete m_dynamicsWorld;
	//delete solver
	delete m_constraintSolver;
	//delete broadphase
	delete m_overlappingPairCache;
	//delete dispatcher
	delete m_dispatcher;
	delete m_collisionConfiguration;
	} // SliderConstraintDemo::~SliderConstraintDemo()
	*/
	return true;
}

//один шаг физики
bool InterfaceForPhysic::PhysicStep()
{
	static osg::Timer_t tick1 = osg::Timer::instance()->tick();
	double D_T = osg::Timer::instance()->delta_s(tick1,osg::Timer::instance()->tick());
	tick1 = osg::Timer::instance()->tick();
	if (D_T <= 0.0f) return true;
	
	dynamicsWorld->stepSimulation(D_T,10);
	
	/*
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	std::cout << "numManifolds=" << numManifolds << std::endl;;
	for (i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold = collisionWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
		//obB->getUserPointer
	
		int numContacts = contactManifold->getNumContacts();
		for (int j=0;j<numContacts;j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
	}
	*/

	//Проходим все ЖЕСТКИЕ тела
	if (bodies.empty() == true) return false;
	//
	for(std::map<std::string,OneBodyClass>::iterator hitr=bodies.begin();hitr!=bodies.end();	++hitr)
	{
		if (hitr->second.rigidBody )
		{
			//динамический объект
			if (hitr->second.rigidBody->getMotionState())
			//if ((hitr->second.mass >0)&&(hitr->second.rigidBody->getMotionState()))
			{
				btScalar m[16];
				//std::cout << "Physic for " << hitr->first << std::endl;
				btDefaultMotionState* myMotionState = ( btDefaultMotionState*)hitr->second.rigidBody->getMotionState();
				myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
				osg::Matrixf mat(m);
				//
				hitr->second.matrix->setMatrix (mat);
			}
		}
	}
    return true;
}

//создает "землю", чтобы объекты не проваливались...
bool InterfaceForPhysic::AddGroundBody()
{
	//BOX "земли"
	//btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(1.),btScalar(50.))); 
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(650.),btScalar(650.),btScalar(1))); 
	//btAlignedObjectArray<btCollisionShape*> collisionShapes;
	collisionShapes.push_back(groundShape);
	//координаты земли
	btTransform groundTransform;
	groundTransform.setIdentity();
	//Координаты
	groundTransform.setOrigin(btVector3(0,0,3));
	//Если масса =0 то неподвижный
	btScalar mass(0.);
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0,0,0);
	if (isDynamic) 
	{
		groundShape->calculateLocalInertia(mass,localInertia);
	}
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	NullPlane = new osg::MatrixTransform();
	root->addChild(NullPlane);
	NullPlane->setName("__NullPlane__");
	body->setUserPointer(NullPlane);
	//?
	//body->setActivationState(ISLAND_SLEEPING);
	//body->applyForce (...
	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);
	return true;
}




bool InterfaceForPhysic::AddSphereBody(std::string _ObjectName, float mass)
{
	//проверка на совпадение имен с уже имеющимися
	if (bodies.count(_ObjectName)!=0) 
	{
		std::cout << "Warning: Physic '" << _ObjectName << "' already exsist!" << std::endl;
		return false;
	}
	//
	OneBodyClass temp;
	//задаем массу
	temp.mass = mass;
	
	//1. проверяем есть ли такой объект в OSG?
	std::string ObjectName = _ObjectName;
	FindNamedNodeVisitor* fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv);
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		//Запоминаем матрицу...
		temp.matrix = fnnv->_foundNodes.front().get();
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		delete fnnv;
		return false;
	}
	

	
	//create a dynamic rigidbody
	//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
	temp.bodyShape = new btSphereShape(btScalar(5.));
	collisionShapes.push_back(temp.bodyShape);
	/// Create Dynamic Objects
	
	
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (temp.mass != 0.f);
	btVector3 localInertia(0,0,0);
	if (isDynamic)
	{
		temp.bodyShape->calculateLocalInertia(temp.mass,localInertia);
	}
	//стартовые координаты
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(fnnv->accumulated.getTrans().x(),fnnv->accumulated.getTrans().y(),fnnv->accumulated.getTrans().z()));
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(temp.mass,myMotionState,temp.bodyShape,localInertia);
	temp.rigidBody = new btRigidBody(rbInfo);
	if (!isDynamic)
	{
		temp.rigidBody->setCollisionFlags(temp.rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); //
	}
	//!!!!!!!!!!!!!
	temp.rigidBody->setActivationState(DISABLE_DEACTIVATION);
	//!!!!!!!!!!
	temp.rigidBody->setUserPointer(temp.matrix);
	temp.rigidBody->setCollisionFlags(temp.rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	//
	dynamicsWorld->addRigidBody(temp.rigidBody);
	//Сохраняем
	bodies[ObjectName] = temp;
	//
	delete fnnv;
	return true;
}


bool InterfaceForPhysic::AddDecompositionBody(std::string ObjectName , std::string MeshObjectName , float mass, unsigned int _depth)
{
	//ObjectName - это объект, на который будет действовать физика
	//MeshObjectName - это объект, с которого будет строится физическая форма

	//проверка на совпадение имен с уже имеющимися
	if (bodies.count(ObjectName)!=0) 
	{
		std::cout << "Warning: Physic '" << ObjectName << "' already exsist!" << std::endl;
		return false;
	}
	
	//создаем тело
	OneBodyClass temp;
	//задаем массу
	temp.mass = mass;

	//Находим объект
	osg::ref_ptr<FindNamedNodeVisitor> fnnv = new FindNamedNodeVisitor(ObjectName);
	osg::ref_ptr<FindNamedNodeVisitor> fnnv2 = new FindNamedNodeVisitor(MeshObjectName);
    root->accept(*fnnv);
	root->accept(*fnnv2);
	//Если такой объект есть, ищем вертексы
	if ((!fnnv->_foundNodes.empty())&&(!fnnv2->_foundNodes.empty()))
	{
		//для этого объекта возможно уже создана физика
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		if (fnnv2->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << MeshObjectName << std::endl;
		
		//Запоминаем матрицу...
		temp.matrix = fnnv->_foundNodes.front().get();

		//а геометрию берем с MeshObjectName
		osg::ref_ptr<FindGeometryVisitor> zzz = new FindGeometryVisitor();
		fnnv2->_foundNodes.front().get()->accept (*zzz);
		//Если геометрия найдена
		if (!zzz->geom)
		{
			std::cout << "geometry NOT found";
			//delete zzz;
			//delete fnnv;
			//delete fnnv2;
			return false;
		}
		
		//получаем данные по вершмнам
		osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*> (zzz->geom->getVertexArray()) ;
			
		//
		float* DecompVertices = new float[vertices->size()*3];
		unsigned int* DecompIndices = new unsigned int[vertices->size()];
		for (unsigned int _count =0; _count < vertices->size() ; _count++)
		{
			DecompVertices[_count*3+0] = vertices->at(_count).x();
			DecompVertices[_count*3+1] = vertices->at(_count).y();
			DecompVertices[_count*3+2] = vertices->at(_count).z();
		}
		for (unsigned int _count =0; _count < vertices->size() ; _count++)
		{
			DecompIndices[_count] = _count;
		}

		
		std::cout << "Original mesh num triangles = " << vertices->size()/3 << std::endl;
		//структура с параметрами разбиения и данными trimesh
		DecompDesc desc;

		unsigned int depth = _depth; //5..10
		float cpercent     = 5;
		float ppercent     = 15;
		unsigned int maxv  = 16;
		float skinWidth    = 0.0;		

		//количество вертексов
		desc.mVcount       = vertices->size();
		//указатель на массив вертексов
		desc.mVertices     = DecompVertices;
		//количество треугольников
		desc.mTcount       = vertices->size()/3;
		//указатель на массив индексов
		desc.mIndices      = DecompIndices;
		//
		desc.mDepth        = depth;
		desc.mCpercent     = cpercent;
		desc.mPpercent     = ppercent;
		desc.mMaxVertices  = maxv;
		desc.mSkinWidth    = skinWidth;

		MyConvexDecomposition	convexDecomposition (dynamicsWorld, temp.matrix);
		desc.mCallback = &convexDecomposition;
		//
		ConvexBuilder cb(desc.mCallback);
		cb.process(desc);


		//Получаем результат декомпозиции (композит из всех получившихся кусочков)...
		temp.bodyShape = convexDecomposition.GetCompoundShape ();
		//
		collisionShapes.push_back(temp.bodyShape);

		

		//смещение этого тела
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(fnnv->accumulated.getTrans().x(),fnnv->accumulated.getTrans().y(),fnnv->accumulated.getTrans().z()));
		trans.setRotation (btQuaternion( fnnv->accumulated.getRotate().x(),fnnv->accumulated.getRotate().y(),fnnv->accumulated.getRotate().z(),fnnv->accumulated.getRotate().w()));
		//trans.setFromOpenGLMatrix ((const btScalar*)fnnv->accumulated.);
		
		//и его создание
		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0,0,0);
		if (isDynamic)
		{
			temp.bodyShape->calculateLocalInertia(mass,localInertia);
		}
		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
		btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,temp.bodyShape,localInertia);
		temp.rigidBody = new btRigidBody(cInfo);
		if (!isDynamic)
		{
			temp.rigidBody->setCollisionFlags(temp.rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); 
		}
		dynamicsWorld->addRigidBody(temp.rigidBody);
		//Сохраняем ссылку на матрицу OSG объекта
		temp.rigidBody->setUserPointer(temp.matrix);
		//флаги
		temp.rigidBody->setCollisionFlags(temp.rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		//
		temp.rigidBody->setActivationState(DISABLE_DEACTIVATION);
		//Сохраняем
		bodies[ObjectName] = temp;
		
		//Чистим мусор
		delete[] DecompVertices;
		delete[] DecompIndices;
		//delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << " or " << MeshObjectName << std::endl;
		//Чистим мусор
		//delete fnnv;
		//delete fnnv2;
		return false;
	}
	//Чистим мусор
	//delete fnnv;
	//delete fnnv2;

	return true;
}

bool InterfaceForPhysic::AddMeshBody(std::string ObjectName , float mass)
{
	//проверка на совпадение имен с уже имеющимися
	if (bodies.count(ObjectName)!=0) 
	{
		std::cout << "Warning: Physic '" << ObjectName << "' already exsist!" << std::endl;
		return false;
	}
	
	//создаем тело
	OneBodyClass temp;
	//задаем массу
	temp.mass = mass;

	//Находим объект
	FindNamedNodeVisitor* fnnv = new FindNamedNodeVisitor(ObjectName);
    root->accept(*fnnv);
	//Если такой объект есть, ищем вертексы
	if (!fnnv->_foundNodes.empty())
	{
		if (fnnv->_foundNodes.size()>1) std::cout << "Warning: Duplicate names - " << ObjectName << std::endl;
		
		//Запоминаем матрицу...
		temp.matrix = fnnv->_foundNodes.front().get();

		FindGeometryVisitor* zzz = new FindGeometryVisitor();
		fnnv->_foundNodes.front().get()->accept (*zzz);
		//Если геометрия найдена
		if (!zzz->geom)
		{
			std::cout << "geometry NOT found";
			delete zzz;
			delete fnnv;
			return false;
		}

		
		//получаем данные
		
		osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*> (zzz->geom->getVertexArray()) ;
		
		//проходим все треугольники
		unsigned int i;
		btTriangleMesh *triangleMesh = new btTriangleMesh();
		for (i=0; i < vertices->size()/3 ; i+=1)
		{
			//std::cout <<  << " | " << vertices->at(i).y() << " | " << vertices->at(i).z() <<std::endl;
			//по три точки на треугольник
			btVector3 vertex1 = btVector3(vertices->at(i*3+0).x(), vertices->at(i*3+0).y(), vertices->at(i*3+0).z());
			btVector3 vertex2 = btVector3(vertices->at(i*3+1).x(), vertices->at(i*3+1).y(), vertices->at(i*3+1).z());
			btVector3 vertex3 = btVector3(vertices->at(i*3+2).x(), vertices->at(i*3+2).y(), vertices->at(i*3+2).z());
			
			triangleMesh->addTriangle(vertex1, vertex2, vertex3);
		}

		bool useQuantizedAabbCompression = true;
		temp.bodyShape = new btBvhTriangleMeshShape(triangleMesh, useQuantizedAabbCompression); 


		//btConvexShape* tmpConvexShape = new btConvexTriangleMeshShape(triangleMesh);
		//temp.bodyShape = tmpConvexShape;

		collisionShapes.push_back(temp.bodyShape);
		std::cout << "test 2" << std::endl;
		
		// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();
		//задаем массу
		temp.mass = mass;
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (temp.mass != 0.f);
		btVector3 localInertia(0,0,0);
		if (isDynamic)
		{
			temp.bodyShape->calculateLocalInertia(temp.mass,localInertia);
		}
		//стартовые координаты
		startTransform.setOrigin(btVector3(0,0,0));
		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(temp.mass,myMotionState,temp.bodyShape,localInertia);
		temp.rigidBody = new btRigidBody(rbInfo);
		if (!isDynamic)
		{
			temp.rigidBody->setCollisionFlags(temp.rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); //
		}
		//!!!!!!!!!!!!!
		temp.rigidBody->setActivationState(DISABLE_DEACTIVATION);
		//!!!!!!!!!!
		temp.rigidBody->setUserPointer(temp.matrix);
		temp.rigidBody->setCollisionFlags(temp.rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		//
		dynamicsWorld->addRigidBody(temp.rigidBody);
		//Сохраняем
		bodies[ObjectName] = temp;
		std::cout << "test 2.5" << std::endl;
		
		//Чистим мусор
		delete zzz;
	}
	else
	{
		std::cout << "Find objects in Root - FALSE! " << ObjectName << std::endl;
		delete fnnv;
		return false;
	}
	//Чистим мусор
	delete fnnv;
	std::cout << "test 3.0" << std::endl;

	return true;
}




bool InterfaceForPhysic::RMoveBodyTo(std::string ObjectName , float X, float Y, float Z)
{
	//проверка на наличие такого объекта
	if (bodies.count(ObjectName)==0) 
	{
		std::cout << "Warning: Physic '" << ObjectName << "' not exsist!" << std::endl;
		return false;
	}
	
	//это для динамики
	bodies[ObjectName].rigidBody->translate (btVector3(X,Z,Y));
	bodies[ObjectName].rigidBody->setLinearVelocity(btVector3(X,Z,Y));

	//это для статики
	btTransform newTrans;
	bodies[ObjectName].rigidBody->getMotionState()->getWorldTransform(newTrans);
	newTrans.getOrigin().setX(newTrans.getOrigin().getX() + X);
	newTrans.getOrigin().setY(newTrans.getOrigin().getY() + Y);
	newTrans.getOrigin().setZ(newTrans.getOrigin().getZ() + Z);
	bodies[ObjectName].rigidBody->getMotionState()->setWorldTransform(newTrans);

	return true;
}



InterfaceForPhysic::~InterfaceForPhysic()
{

	//cleanup in the reverse order of creation/initialization
	//remove the rigidbodies from the dynamics world and delete them
	for (int i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<collisionShapes.size();j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();

	//
	root->removeChild (NullPlane);

	std::cout << "Destroy BULLET" << std::endl;
}





