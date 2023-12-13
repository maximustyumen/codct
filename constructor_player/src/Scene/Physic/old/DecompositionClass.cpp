/***************************************************************************
DecompositionClass.cpp  -  класс для разбиения сложных тел (trimesh) на простые "выпуклые тела" (convex) и объединения их в "состовные" объекты (composite)
-------------------
begin                : 21 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "./DecompositionClass.h"

MyConvexDecomposition::MyConvexDecomposition (btDiscreteDynamicsWorld* _dynamicsWorld, osg::ref_ptr<osg::MatrixTransform> _root)
{
	dynamicsWorld = _dynamicsWorld;
	std::cout << "Start decomposition process ... " << std::endl;
	mBaseCount=0;
	mHullCount=0;
	root = _root;
	//это группа для debug'a
	debugGl= new osg::Group();
    osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
    osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
    stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
	debugGl->setStateSet(stateset);
	//
	root->addChild (debugGl);
	//
	BigShape = new btCompoundShape();
}


void MyConvexDecomposition::ConvexDecompResult(ConvexDecomposition::ConvexResult &result)
{
		//это заготовка для нового куска 
		btTriangleMesh* trimesh = new btTriangleMesh();
		//Запоминаем указатель
		m_trimeshes.push_back(trimesh);
		//масштаб
		btVector3 localScaling(1.f,1.f,1.f);
		//debug
		std::cout << "new decomposition Convex." << std::endl;
		//calc centroid, to shift vertices around center of mass
		btVector3	centroid;
		centroid.setValue(0,0,0);
		btAlignedObjectArray<btVector3> vertices;
		for (unsigned int i=0; i<result.mHullVcount; i++)
		{
			btVector3 vertex(result.mHullVertices[i*3],result.mHullVertices[i*3+1],result.mHullVertices[i*3+2]);
			vertex *= localScaling;
			centroid += vertex;
		}
		centroid *= 1.f/(float(result.mHullVcount) );

		for (unsigned int i=0; i<result.mHullVcount; i++)
		{
			btVector3 vertex(result.mHullVertices[i*3],result.mHullVertices[i*3+1],result.mHullVertices[i*3+2]);
			vertex *= localScaling;
			vertex -= centroid ;
			vertices.push_back(vertex);
		}

		const unsigned int *src = result.mHullIndices;
		for (unsigned int i=0; i<result.mHullTcount; i++)
		{
			unsigned int index0 = *src++;
			unsigned int index1 = *src++;
			unsigned int index2 = *src++;

			btVector3 vertex0(result.mHullVertices[index0*3], result.mHullVertices[index0*3+1],result.mHullVertices[index0*3+2]);
			btVector3 vertex1(result.mHullVertices[index1*3], result.mHullVertices[index1*3+1],result.mHullVertices[index1*3+2]);
			btVector3 vertex2(result.mHullVertices[index2*3], result.mHullVertices[index2*3+1],result.mHullVertices[index2*3+2]);
			vertex0 *= localScaling;
			vertex1 *= localScaling;
			vertex2 *= localScaling;
			
			vertex0 -= centroid;
			vertex1 -= centroid;
			vertex2 -= centroid;

			index0+=mBaseCount;
			index1+=mBaseCount;
			index2+=mBaseCount;

			trimesh->addTriangle(vertex0,vertex1,vertex2);
		
			#define MYDEBUG
			#ifdef MYDEBUG
				float f =1.0;
				osg::Vec3 myCoords[] =
				{
				osg::Vec3((vertex0.x()+centroid.x())*f, (vertex0.y()+centroid.y())*f, (vertex0.z()+centroid.z())*f),
				osg::Vec3((vertex1.x()+centroid.x())*f, (vertex1.y()+centroid.y())*f, (vertex1.z()+centroid.z())*f),
				osg::Vec3((vertex2.x()+centroid.x())*f, (vertex2.y()+centroid.y())*f, (vertex2.z()+centroid.z())*f)
				};
				
				osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
				shared_colors->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
				// same trick for shared normal.
				osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
				shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
				//debug
				osg::ref_ptr<osg::Geode> geode = new osg::Geode();
				osg::ref_ptr<osg::Geometry> polyGeom = new osg::Geometry();
				int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);
				osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numCoords,myCoords);
				polyGeom->setVertexArray(vertices);
				// use the shared color array.
				polyGeom->setColorArray(shared_colors.get());
				polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
				// use the shared normal array.
				polyGeom->setNormalArray(shared_normals.get());
				polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

				polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,3));
				geode->addDrawable(polyGeom);
				debugGl->addChild(geode);
			#endif
			//
			
		}

		//создаем convexShape из trimesh
		btCollisionShape* convexShape = new btConvexTriangleMeshShape(trimesh);
		//запоминаем указатель
		collisionShapes.push_back(convexShape);
		//параметры оболочки
		convexShape->setMargin(0.01);

		//сдвигаем convexShape из начала координат...
		btTransform trans;
		trans.setIdentity();
		btVector3   convexDecompositionObjectOffset(0,0,0);
		trans.setOrigin(centroid-convexDecompositionObjectOffset);

		//добавляем в общее тело
		BigShape->addChildShape (trans,convexShape);
		
		//Вот тут можно создавать тело для каждого куска декомпозиции, например если будут нужно "развалить объект на части" ))))
		//localCreateRigidBody( mass, trans,convexShape);

		// advance the 'base index' counter.
		mBaseCount+=result.mHullVcount; 
}

btCompoundShape* MyConvexDecomposition::GetCompoundShape()
{
	return BigShape;
}

