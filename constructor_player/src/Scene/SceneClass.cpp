/***************************************************************************
SceneClass.cpp  - класс описания сцены
-------------------
begin                : 21 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "SceneClass.h"
#include "../Configure/Configure.h"



//ТЕСТ!!!
bool SceneClass::TESTTEST()
{
    /*
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	// add the teapot to the geode.
	osg::ref_ptr<CustomGeometry> newGeometry = new CustomGeometry;
	geode->addDrawable( newGeometry.get() );
	// add a StateSet to the teapot.     
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	geode->setStateSet(stateset.get());
	//
	osg::ref_ptr<osg::MatrixTransform>  newtest = new osg::MatrixTransform();
	newtest->addChild (geode.get());
	newtest->setName ("TESTTEST");
	//   
	root->addChild(newtest.get());
	//
	return true;
    */

       double radius = 10.0f;
       osg::Vec3 center = osg::Vec3(0.0f,0.0f,0.0f);

       osg::ref_ptr<osg::PagedLOD> plod = new osg::PagedLOD();

       plod->setName("plod for ");
       plod->setRangeMode(osg::LOD::DISTANCE_FROM_EYE_POINT);

       //lod->setFileName(0, terrain_filenames[i]);
       //lod->setFilename(1, pagedLod0Child0Filename);
       //lod->setPriorityOffset(0, 1);
       //lod->setPriorityScale(0, 1.0f);
       //lod->setCenter(center);
       //lod->setRadius(radius);
       //lod->setRange(0, 0.0f, radius * 6.0); // FIXME

       std::string Filename1 = "/home/maximum2000/LABS/PLOD/osg/1.osg";
       std::string Filename2 = "/home/maximum2000/LABS/PLOD/osg/2.osg";
       std::string Filename3 = "/home/maximum2000/LABS/PLOD/osg/3.osg";
       std::string Filename4 = "/home/maximum2000/LABS/PLOD/osg/4.osg";



       plod->setFileName (0, Filename1);
       plod->setRange    (0, 0.0f, 100.0f);
       plod->setFileName (1, Filename2);
       plod->setRange    (1, 100.0f, 200.0f);
       plod->setFileName (2, Filename3);
       plod->setRange    (2, 200.0f, 300.0f);
       plod->setFileName (3, Filename4);
       plod->setRange    (3, 300.0f, 1000.0f);


       //plod->addChild( loadedModel2, 100.0f, 200.0f, Filename2 );
       //plod->addChild( loadedModel3, 200.0f, 300.0f, Filename3 );
       //plod->addChild( loadedModel4, 400.0f, 500.0f, Filename4 );

       plod->setNumChildrenThatCannotBeExpired(1);

       root->addChild(plod);
       osgDB::writeNodeFile(*root, "/home/maximum2000/LABS/PLOD/osg/saved.osg");

      // dbPager->registerPagedLODs(plod) ;
    return true;

}

SceneClass::SceneClass(Configure* configuration)
: skin_animation(0)
, HUD(0)
{
	std::cout << "SceneClass constructor ..." << std::endl;
        TopRoot = new osg::Group;


//#define ADDSSAO true

		////////
#ifdef ADDSSAO
		osg::ref_ptr<osg::LightSource> lightSource_SunShadow;
		lightSource_SunShadow = new osg::LightSource;
		if ( lightSource_SunShadow.valid() ) 
		{
			osg::Vec4 lightpos(10, 10, 100,1.0f);
			lightSource_SunShadow->getLight()->setLightNum(1);
			lightSource_SunShadow->getLight()->setPosition(lightpos);
			lightSource_SunShadow->getLight()->setSpecular(osg::Vec4(0.10,0.10,0.10,1.0));
			lightSource_SunShadow->getLight()->setAmbient(osg::Vec4(0.18,0.18,0.175,1.0));
			lightSource_SunShadow->getLight()->setDiffuse(osg::Vec4(0.4,0.4,0.4,1.0));
		}
		//
		TopRoot->addChild(lightSource_SunShadow.get());
		TopRoot->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
		TopRoot->getOrCreateStateSet()->setMode(GL_LIGHT1, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
		//
		// PHSSAO
		root = new osg::Group;
		//root->addChild(loadedNode.get());
		osg::ref_ptr<SSAO> ssaoPE = new SSAO(root.get(),lightSource_SunShadow.get(),"");
		ssaoPE->_ssao_type = SSAO::POLY_ESTIMATED_SSAO;
		ssaoPE->addChild(root.get());
		osg::ref_ptr<osg::Group> grpPESSAO = new osg::Group;
		grpPESSAO->addChild(ssaoPE.get());
		grpPESSAO->addDescription("Algorithm: PB-AO");

		osg::ref_ptr<osg::Group> node = grpPESSAO.get();
		TopRoot->addChild(node);
#endif
		///////////////////


	#ifndef SHADOW
		#ifndef ADDSSAO
		root = new osg::Group;
		#endif
	#else
		root = new osgShadow::ShadowedScene;
		ReceivesShadowTraversalMask = 0x1;
		CastsShadowTraversalMask = 0x2;
		root->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
		root->setCastsShadowTraversalMask(CastsShadowTraversalMask);
		//

                /*
                int mapcount = 3;
                osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,mapcount);
                int mapres = 1024;
                pssm->setTextureResolution(mapres);
                */


                osg::ref_ptr<osgShadow::MinimalShadowMap> sm = NULL;
                sm = new osgShadow::LightSpacePerspectiveShadowMapDB;
                if( sm.valid() )
                {
                    //while( arguments.read("--debugHUD") ) sm->setDebugDraw( true );

                    float minLightMargin = 10.f;
                    float maxFarPlane = 0;
                    unsigned int texSize = 1024;
                    unsigned int baseTexUnit = 0;
                    unsigned int shadowTexUnit = 2;

                    sm->setMinLightMargin( minLightMargin );
                    sm->setMaxFarPlane( maxFarPlane );
                    sm->setTextureSize( osg::Vec2s( texSize, texSize ) );
                    sm->setShadowTextureCoordIndex( shadowTexUnit );
                    sm->setShadowTextureUnit( shadowTexUnit );
                    sm->setBaseTextureCoordIndex( baseTexUnit );
                    sm->setBaseTextureUnit( baseTexUnit );
                }


                //
                osg::Vec4 lightpos;
                lightpos.set(0.5f,0.25f,0.5f,0.0f);
                osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
                ls->getLight()->setPosition(lightpos);
                ls->getLight()->setAmbient(osg::Vec4(0.2,0.2,0.2,1.0));
                ls->getLight()->setDiffuse(osg::Vec4(0.1,0.1,0.1,1.0));


                //

                //int mapres = 1024;
                //sm->setTextureSize(osg::Vec2s(mapres,mapres));
                //osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
		//osg::ref_ptr<osgShadow::SoftShadowMap> sm = new osgShadow::SoftShadowMap;
		//osg::ref_ptr<osgShadow::ShadowTexture> st = new osgShadow::ShadowTexture;
                root->setShadowTechnique(sm.get());
                root->addChild(ls.get());

	#endif
        osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("./ppu/dummy.osg");
        if (loadedModel.valid())
        {
                root->addChild(loadedModel);
        }
        else
        {
            std::cout << "dummy.osg not found" << std::endl;
        }

		#ifndef ADDSSAO
			TopRoot->addChild(root);
		#endif

	//если звук выключен
	std::string notfound_ = "not found";
	bool SOUND_OFF = false;
	if (configuration->GetRecordString("#MasterVolume") != notfound_)
	{
			float MasterVolume = configuration->GetRecordFloat("#MasterVolume");
			if (MasterVolume == 0) SOUND_OFF = true;
	}
	//если звук выключен



	Graphic = new GraphicSceneClass(root);
	Audio = new AudioSceneClass(root, SOUND_OFF);
	//	Characters = new InterfaceForCharacters (root);
	skin_animation = new InterfaceForSkinAnimation(root);
	//HUD - инициализируется позже в setViewResolution
	OLD = new InterfaceForOLD(root);
	Texts = new InterfaceForText(root);
	Light = new InterfaceForLight(root);
	Shaders = new InterfaceForShaders(root,configuration->GetRecordString("#Shaders") == "ON");
	Graphic->set_observer(Shaders);
	Video = new InterfaceForVideo (root);
	Particles = new InterfaceForParticle(root);
	//остальные создаются при вызове setViewResolution....

	//HUD3D.reset(new InterfaceForHUD3D(root));
	{
		osg::ref_ptr<osg::MatrixTransform> hud3D = new osg::MatrixTransform();
		hud3D->setName("HUD3D");
		root->asGroup()->addChild(hud3D);
	}
	//Загрузка курсора
    {
        osg::ref_ptr<osg::Node> Cursor = osgDB::readNodeFile("./Cursor.osg");
        if (!Cursor.valid())
        {
			std::cout << "Cursor.osg not load."  << std::endl;
        }
		Cursor->setName("__CURSOR__");
        /*
        osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
        at->addChild(Cursor);
        at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
        at->setAutoScaleToScreen(true);
        at->setMinimumScale(1);
        at->setMaximumScale(1000);
        */
        root->asGroup()->addChild(Cursor.get()); // at
        osg::ref_ptr<osg::StateSet> stateset = Cursor->getOrCreateStateSet();
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		//Это для 3D HUD MENU
		stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		stateset->setRenderBinDetails( 200, "RenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
	}

	//Загрузка курсора - руки для VR
	{
		osg::ref_ptr<osg::Node> HandCursor = osgDB::readNodeFile("./HandCursor.fbx");
		if (!HandCursor.valid())
		{
			std::cout <<  "HandCursor.fbx not load." << std::endl;
		}	
		else
		{
			HandCursor->setName("__HAND__CURSOR__AIR");
			osg::ref_ptr<osg::StateSet> stateset = HandCursor->getOrCreateStateSet();
			stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			//Это для 3D HUD MENU
			//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			stateset->setRenderBinDetails( 199, "RenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
			root->asGroup()->addChild(HandCursor.get()); 
		}
	}


	//Physic = new InterfaceForPhysic(root);
	//Physic2 = new InterfaceForPhysic2(root);
#ifdef PHYSX
	#if (_MSC_VER < 1700)
	_pPhysic = cPhysic::create();
	_pPhysicScene = _pPhysic->createScene( root, -9.81f );
	#endif
#endif
	
}

SceneClass::~SceneClass()
{
	std::cout << "Destructor SceneClass."  << std::endl;
	//очистка всех данных ...
	Reset();
#ifdef PHYSX
	#if (_MSC_VER < 1700)
	_pPhysic->destroyScene( _pPhysicScene );
	_pPhysic->destroy();
#endif
#endif

//	delete Characters;
	delete HUD;
	delete OLD;
	delete Texts;
	delete Light;
	Graphic->set_observer(0);
	delete Shaders;
	delete Video;
	delete Particles;
	//delete Physic;
	delete Graphic;
	delete Audio;
}

//передача указателя на группу сцены для Viewer'a
osg::ref_ptr<osg::Group> SceneClass::GetDataToVideoViewer()
{
        return TopRoot;
}

//необходимо для создания проекций (HUD)
void SceneClass::setViewResolution(int width, int height)
{
	HUD = new InterfaceForHUD(root,width,height);
        //Cameras = new InterfaceForCameras(root, TopRoot,width,height);
}

//очистка всех данных ...
bool SceneClass::Reset()
{
#ifdef PHYSX
	#if (_MSC_VER < 1700)
	_pPhysicScene->reset();
	#endif
#endif

	Video->Reset();
	Shaders->Reset();
	Light->Reset();
//	Characters->Reset();
	if(HUD) HUD->Reset();
	Graphic->Reset();
	Particles->Reset ();
	Audio->Reset();
	OLD->Reset();
	Texts->Reset();

	//Чистим "КОРЕНЬ"
	std::cout << "--- RESET SceneClass ---" << std::endl;

	std::cout << "Object in root: " << root->getNumChildren() << std::endl;
	//Объекты, которые нельзя удалять
	//Object in root: 4
	//MatrixTransform: __NullPlane__
	//Camera: __HUDCAMERA__
	//Camera: 
	//MatrixTransform: __CURSOR__
	for (unsigned int i=root->getNumChildren()-1; i > 0 ; i--)
	{
		if (osg::ref_ptr<osg::MatrixTransform> temp  = dynamic_cast<osg::MatrixTransform*>(root->getChild(i)) )
		{
			std::cout << "delete MatrixTransform: " << temp->getName();
                        if ((temp->getName()!="__NullPlane__")&&(temp->getName()!="__CURSOR__")&&(temp->getName()!="__CURSORFORHUD__AIR")&&(temp->getName()!="CURSOR_AIR"))
			{
				if (root->removeChild(i)==true)
				{
					std::cout << " .. removed ok." << std::endl;
				}
				else
				{
					std::cout << " .. removed false." << std::endl;
				}
			}
			else
			{
				std::cout << " is system object!" << std::endl;
			}
		}
		else if (osg::ref_ptr<osg::Camera>  temp = dynamic_cast<osg::Camera*>( root->getChild(i) ) )
		{
			std::cout << "delete Camera: " << temp->getName();
                        if ((temp->getName()!="__HUDCAMERA__")&&(temp->getName()!="__DEBUG_CAMERA__") &&(temp->getName()!="__CURSOR_CAMERA__"))
			{
				if (root->removeChild(i)==true)
				{
					std::cout << " .. removed ok." << std::endl;
				}
				else
				{
					std::cout << " .. removed false." << std::endl;
				}
			}
			else
			{
				std::cout << " is system object!" << std::endl;
			}
		}
		else
		{
			std::cout << "delete Node: " << root->getChild(i)->getName();
			if (root->removeChild(i)==true)
			{
				std::cout << " .. removed ok." << std::endl;
			}
			else
			{
				std::cout << " .. removed false." << std::endl;
			}
		}
	}

        //если это был звук
        std::cout << "FindNamedNodeVisitor::ClearCache()" << std::endl;
        FindNamedNodeVisitor::ClearCache();
        std::cout << "Audio->CheckCache();" << std::endl;
        Audio->CheckCache();


	std::cout << "Object in root: " << root->getNumChildren() << std::endl;
	std::cout << "--- RESET SceneClass Completed ---" << std::endl;
	//

	return true;
}

//Удаление объекта из сцены
bool SceneClass::UnloadObject(std::string ObjectName)
{
	Graphic->UnloadObject (ObjectName);
    //если это был звук
    Audio->CheckCache();
	//
	return true;
}

//update...
bool SceneClass::Update(float userX, float userY, float userZ)
{
#ifdef PHYSX
	#if (_MSC_VER < 1700)
	float fDeltaTime = 0.f;
	_pPhysicScene->update( fDeltaTime );
	#endif
#endif

	//
	Audio->Update(userX, userY, userZ);
	return true;
}


