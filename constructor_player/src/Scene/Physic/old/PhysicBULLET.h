#ifndef PHYSICBULLET
#define PHYSICBULLET

#include <iostream>
//
#include <osg/MatrixTransform>
#include "../Visitors/FindNodeByName.h"
#include "../Visitors/FindGeometry.h"
//
#include <btBulletDynamicsCommon.h>

#include "./DecompositionClass.h"
//
#include <osg/Timer>



class InterfaceForPhysic
{	
public:
	//конструктор
	InterfaceForPhysic(osg::ref_ptr<osg::Group> _root);
	~InterfaceForPhysic();
	//сброс
	bool Reset();
	//шаг имитации
	bool PhysicStep();
	//создает физическое тело аппроксимируя его сферой
	bool AddSphereBody(std::string ObjectName , float mass);
	//создает физическое тело аппроксимируя его сеткой из егоже геометрии
	bool AddMeshBody(std::string ObjectName , float mass);
	//
	bool AddDecompositionBody (std::string ObjectName , std::string MeshObjectName , float mass, unsigned int _depth=5);
	
	//Перемещает физическое тело (кинематического типа)
	bool RMoveBodyTo(std::string ObjectName , float X, float Y, float Z);

	//Связь point to point constraint (ball socket)
	bool AddBallSocket(std::string ObjectName1, std::string ObjectName2);
	

private:

	//создает "землю", чтобы объекты не проваливались...
	bool AddGroundBody();

	//указатель на главную группу сцены
	osg::ref_ptr<osg::Group> root;

	//собственно "мир" физики
	btDiscreteDynamicsWorld* dynamicsWorld;
	//решатель
	btSequentialImpulseConstraintSolver* solver;
	//Пределы мира
	btAxisSweep3* overlappingPairCache;
	//collision dispatcher
	btCollisionDispatcher* dispatcher;
	//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration;

	//Это типа группа где находятся все "физицческие" описания объектов
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	
	class OneBodyClass
	{
	public:
		//std::string name; //osg::matrix ?
		btCollisionShape* bodyShape;
		btRigidBody* rigidBody;
		btScalar	mass;
		osg::ref_ptr<osg::MatrixTransform> matrix;
	};

	std::map<std::string,OneBodyClass> bodies;

	//callback
	static bool materialCombinerCallback(btManifoldPoint& cp,
                              const btCollisionObject* colObj0, int partId0,
                              int index0, const btCollisionObject* colObj1,
                              int partId1, int index1);
	//Это для пола, чтобы у него было имя (getUserPointer())->getName())
	osg::ref_ptr<osg::MatrixTransform>  NullPlane;
	
};



#endif