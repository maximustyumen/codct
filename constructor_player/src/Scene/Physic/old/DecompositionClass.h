#ifndef DECOMPOSITIONCLASS
#define DECOMPOSITIONCLASS
//это для std::cout
#include <iostream>
//это для связи с OSG
#include <osg/MatrixTransform>
#include "../Visitors/FindNodeByName.h"
#include "../Visitors/FindGeometry.h"
//это для Bullet'a
#include <btBulletDynamicsCommon.h>
#include <ConvexBuilder.h>
//Необходимы при графическом отображении получаемого физического тела (Debug)
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonMode>

class MyConvexDecomposition : public ConvexDecomposition::ConvexDecompInterface
{
	public:
	//конструктор
	MyConvexDecomposition (btDiscreteDynamicsWorld* _dynamicsWorld, osg::ref_ptr<osg::MatrixTransform> _root);
	//эта функция вызывается для каждого нового "куска" тела
	virtual void ConvexDecompResult(ConvexDecomposition::ConvexResult &result);
	//Возврат указателя на созданный CompoundShape...
	btCompoundShape* GetCompoundShape();

private:
	int   	mBaseCount;
	int		mHullCount;

	//хранит указатели на все созданные куски trimesh
	btAlignedObjectArray<btTriangleMesh*> m_trimeshes;
	//хранит указатели на созданные "физицческие" описания объектов
	btAlignedObjectArray<btCollisionShape*> collisionShapes;


	//Ссылка на группу сцены
	osg::ref_ptr<osg::MatrixTransform> root;
	//это ссылка на мир физики (bullet)
	btDiscreteDynamicsWorld* dynamicsWorld;
	//это группа для debug'a
	osg::ref_ptr<osg::Group> debugGl;
	//
	btCompoundShape* BigShape;
	
};

#endif