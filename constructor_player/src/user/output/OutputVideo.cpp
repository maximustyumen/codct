/***************************************************************************
OutputVideo.cpp  -  класс графического вывода локального пользователя
-------------------
begin                : 10 марта 2023
refactoring          : 1 апреля 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
// 
// Настройка камер, окна вывода и т.д.
// 
#include "OutputVideo.h"
#include "./ScreenShot.h"
#include "../../error/errorclass.h"
//#include "osgoculusviewer/oculusconfig.h"


#ifdef WIN32
	#include <windows.h>
#endif

void OutputVideoClass::PseudoFullScreenMode()
{
	///////////
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface(); 
	if (!wsi) 
	{ 
		std::cout <<"Error, no WindowSystemInterface available, cannot getWindowingSystemInterface."<<std::endl; 
		return; 
	} 
	
	#ifdef WIN32

	unsigned int width, height; 
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), 	width, height); 

	HDC screen = GetDC(0);
	int virtualWidth = GetDeviceCaps (screen, HORZRES);
	int physicalWidth = GetDeviceCaps (screen, DESKTOPHORZRES);
	ReleaseDC (0, screen);
	int DPI =  (int)(96.0f * physicalWidth / virtualWidth);
	std::cout << "DPI = " <<  DPI << std::endl;
			
	float k = 96.0f / DPI;

	//
	RECT workarea;
	// Get the current work area
	SystemParametersInfo( SPI_GETWORKAREA, 0, &workarea, 0 );
	int systembarhight = height*k - (workarea.bottom - workarea.top);

	////////
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 0; //0
	traits->y =0; //0
	traits->width = width * k ;//0
	traits->height = height* k   - systembarhight ;//2	       
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->vsync = true;
	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	viewer->getCamera()->setGraphicsContext(gc); 
	viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height)); 
	
	#endif
}


//конструктор
OutputVideoClass::OutputVideoClass(Configure* configuration, osg::ref_ptr<osgViewer::Viewer> _viewer)
{

	//информация о типе визуализациии и версии
	std::cout << "OutputVideoClass constructor.. " << osgGetLibraryName() << " " << osgGetVersion() << std::endl;

	
	m_oculus_enable = configuration->GetRecordString("#OCULUS_ON") == "ON";
	if (m_oculus_enable==true)
	{
		std::cout << "OCULUS !!!!" << std::endl;
	}
	
	m_htc_vive_enable = configuration->GetRecordString("#HTC_VIVE_ON") == "ON";
	if (m_htc_vive_enable==true)
	{
		std::cout << "HTC VIVE !!!!" << std::endl;
	}

	//если не получен уазатель на ранее созданный viewer, то создаем новый
	if (_viewer==0)
	{




		viewer = new osgViewer::Viewer();
		
		//pseudo fullscreen for video grabbing
		if ( (configuration->GetRecordString("#pseudofullscreen") == "ON") && (m_oculus_enable == false) && (m_htc_vive_enable == false) )
		{
			PseudoFullScreenMode();
		}
		///////////////

		#ifdef WIN32
		if ((configuration->GetRecordString("#windowed") != "1") && (configuration->GetRecordString("#pseudofullscreen") != "ON") && (m_oculus_enable == false) && (m_htc_vive_enable == false) )
		{
			 HDC screen = GetDC(0);
			int virtualWidth = GetDeviceCaps (screen, HORZRES);
			int physicalWidth = GetDeviceCaps (screen, DESKTOPHORZRES);
			ReleaseDC (0, screen);
			int DPI =  (int)(96.0f * physicalWidth / virtualWidth);
			std::cout << "DPI = " <<  DPI << std::endl;



			if (DPI == 96)
			{
				std::cout << "1bbb" << std::endl;
				viewer->setUpViewAcrossAllScreens();
				std::cout << "3bbb" << std::endl;
				//+
				//ГЛЮК КУРГАН!!!!
				/*
				osg::ref_ptr< osg::GraphicsContext::Traits > traits = new osg::GraphicsContext::Traits( *viewer->getCamera()->getGraphicsContext()->getTraits());
				traits->vsync = true;
				//Create a new graphics context with the modified traits
				osg::ref_ptr< osg::GraphicsContext > gc = osg::GraphicsContext::createGraphicsContext( traits.get() );
				gc->realize();
				gc->makeCurrent();
				*/
				//+

			}
			else
			{

				PseudoFullScreenMode();
			}



			// set the vertical black on by default
			osgViewer::Viewer::Windows windows;
			viewer->getWindows(windows);
			for(osgViewer::Viewer::Windows::iterator itr = windows.begin(); itr != windows.end(); ++itr) 
			{
				(*itr)->setSyncToVBlank (true);
			}
			/*
				if(!wglewIsSupported("WGL_EXT_swap_control")) 
				{
					std::cout << "VSync is not supported\n";
					return false;
				}
				int swapInterval = wglGetSwapIntervalEXT();
				std::cout << "Current swap interval: " << swapInterval << std::endl;
				// assuming display with 1 frame per swap
				wglSwapIntervalEXT(1);
			*/	

			
		}
		#endif

		if (m_oculus_enable == true)
		{
			ChangeResolution(1280,800,60);
		}
	}
	else
	{
		viewer = _viewer;
	}

	

	//Настраиваем DataBasePager
	/** Set the maximum number of OpenGL objects that the page should attempt to compile per frame.
		* Note, Lower values reduces chances of a frame drop but lower the rate that database will be paged in at.
		* Default value is 8.
	*/
	//setMaximumNumOfObjectsToCompilePerFrame
	//void setMaximumNumOfObjectsToCompilePerFrame(unsigned int num) { _maximumNumOfObjectsToCompilePerFrame = num; }	
	/**
		Set the target maximum number of PagedLOD to maintain in memory.
		* Note, if more than the target number are required for rendering of a frame then these active PagedLOD are excempt from being expiried.
		* But once the number of active drops back below the target the inactive PagedLOD will be trimmed back to the target number.
			* Default value is 300
	*/
	//dbPager->setTargetMaximumNumberOfPageLOD (10);
	viewer->getDatabasePager()->setDoPreCompile(true);

	

		
	//для screenshot'a
	osg::ref_ptr<SnapImageDrawCallback> snapImageDrawCallback = new SnapImageDrawCallback();
	viewer->getCamera()->setPostDrawCallback (snapImageDrawCallback.get());
	//

	//для статистики
	osg::ref_ptr<osgViewer::StatsHandler> stats = new osgViewer::StatsHandler;
	stats->setKeyEventTogglesOnScreenStats (osgGA::GUIEventAdapter::KEY_Pause );
	stats->setKeyEventPrintsOutStats (osgGA::GUIEventAdapter::KEY_F15);//НИКОГДА)
	viewer->addEventHandler(stats.get());


	

	
    if (configuration->GetRecordString("#ThreadingModel").find("AutomaticSelection",0)!=std::string::npos)
	{
		viewer->setThreadingModel(osgViewer::Viewer::AutomaticSelection );
	}
	else
	{
		viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded );
	}


	
	
    if (configuration->GetRecordString("#Stereo") == "ON")
	{
		viewer->getCamera()->setCullMask(0xffffffff);
		viewer->getCamera()->setCullMaskLeft(0x00000001);
		viewer->getCamera()->setCullMaskRight(0x00000002);
		// set up the use of stereo by default.
		osg::DisplaySettings::instance()->setStereo(true);

                float EyeSeparation = configuration->GetRecordFloat("#EyeSeparation");
		osg::DisplaySettings::instance()->setEyeSeparation(EyeSeparation);

                if  (configuration->GetRecordString("#StereoMode").find("ANAGLYPHIC",0)!=std::string::npos)
		{
			osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::ANAGLYPHIC);
		}
                else if (configuration->GetRecordString("#StereoMode").find("CHECKERBOARD",0)!=std::string::npos)
		{
			osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::CHECKERBOARD);
		}
                else if (configuration->GetRecordString("#StereoMode").find("HORIZONTAL_INTERLACE",0)!=std::string::npos)
		{
			osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::HORIZONTAL_INTERLACE);
		}
                else if (configuration->GetRecordString("#StereoMode").find("HORIZONTAL_SPLIT",0)!=std::string::npos)
		{
			osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
		}
                else if (configuration->GetRecordString("#StereoMode").find("QUAD_BUFFER",0)!=std::string::npos)
		{
			osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::QUAD_BUFFER);
		}
                else if (configuration->GetRecordString("#StereoMode").find("VERTICAL_INTERLACE",0)!=std::string::npos)
		{
			osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::VERTICAL_INTERLACE);
		}
                else if (configuration->GetRecordString("#StereoMode").find("VERTICAL_SPLIT",0)!=std::string::npos)
		{
			osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::VERTICAL_SPLIT);
		}
	}

        if (configuration->GetRecordString("#SpecialDisplayType").find("3DSphericalDisplay",0)!=std::string::npos)
	{
		viewer->setUpViewFor3DSphericalDisplay();
	}
        if (configuration->GetRecordString("#SpecialDisplayType").find("PanoramicSphericalDisplay",0)!=std::string::npos)
	{
		viewer->setUpViewForPanoramicSphericalDisplay();
	}
        if (configuration->GetRecordString("#SpecialDisplayType").find("WoWVxDisplay",0)!=std::string::npos)
	{
		//viewer->setUpViewForWoWVxDisplay();
	}



        if (configuration->GetRecordString("#DisplayType").find("MONITOR",0)!=std::string::npos)
	{
		osg::DisplaySettings::instance()->setDisplayType(osg::DisplaySettings::MONITOR);
	}
        if (configuration->GetRecordString("#DisplayType").find("POWERWALL",0)!=std::string::npos)
	{
		osg::DisplaySettings::instance()->setDisplayType(osg::DisplaySettings::POWERWALL);
	}
        if (configuration->GetRecordString("#DisplayType").find("REALITY_CENTER",0)!=std::string::npos)
	{
		osg::DisplaySettings::instance()->setDisplayType(osg::DisplaySettings::REALITY_CENTER);
	}
        if (configuration->GetRecordString("#DisplayType").find("HEAD_MOUNTED_DISPLAY",0)!=std::string::npos)
	{
		osg::DisplaySettings::instance()->setDisplayType(osg::DisplaySettings::HEAD_MOUNTED_DISPLAY);
	}
	


        std::string notfound = "not found";
        if (configuration->GetRecordString("#ScreenWidth") != notfound)
	{
                float _ScreenWidth = configuration->GetRecordFloat("#ScreenWidth");
		osg::DisplaySettings::instance()->setScreenWidth(_ScreenWidth);
	}

        if (configuration->GetRecordString("#ScreenHeight") != notfound)
	{
                float _ScreenHeight = configuration->GetRecordFloat("#ScreenHeight");
		osg::DisplaySettings::instance()->setScreenHeight(_ScreenHeight);
	}

        if (configuration->GetRecordString("#ScreenDistance") != notfound)
	{
                float _ScreenDistance = configuration->GetRecordFloat("#ScreenDistance");
		osg::DisplaySettings::instance()->setScreenDistance(_ScreenDistance);
	}

	//m_enable_ppu = configuration->GetRecordString("#PPU") == "ON";

	
	
	//!!!! ВКЛЮЧИТЬ
	viewer->setKeyEventSetsDone(0);

	
	if(!m_oculus_enable)
	{
		if ((configuration->GetRecordString("#windowed") == "1")&&(configuration->GetRecordString("#pseudofullscreen") != "ON"))
		{
			float l = configuration->GetRecordFloat("#window.left");
			float t = configuration->GetRecordFloat("#window.top");
			float w = configuration->GetRecordFloat("#window.width");
			float h = configuration->GetRecordFloat("#window.height");
			//viewer->setUpViewOnSingleScreen(0);
			viewer->setUpViewInWindow(l, t, w, h);   // добавить эту строчку	
		}
	}


	////////////////////
	if(m_htc_vive_enable == true)
	{
		#ifdef WIN32
		std::cout << "HTC VIVE 1" << std::endl;


		// Exit if we do not have an HMD present
		if (!OpenVRDevice::hmdPresent())
		{
			std::cout << "Error: No valid HMD present!" << std::endl;
			//return;
		}
		
		// Open the HMD
		//,0.3f,5000.0f);
		//float nearClip = 0.01f;
		//float farClip = 10000.0f;

		float nearClip = 0.3f;
		float farClip = 5000.0f;


		float worldUnitsPerMetre = 18.0f;
		int samples = 4;
		openvrDevice = new OpenVRDevice(nearClip, farClip, worldUnitsPerMetre, samples);

		// Exit if we fail to initialize the HMD device
		if (!openvrDevice->hmdInitialized())
		{
			// The reason for failure was reported by the constructor.
			std::cout << "Exit if we fail to initialize the HMD device" << std::endl;
			//return;
		}

		// Get the suggested context traits
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = openvrDevice->graphicsContextTraits();
		traits->windowName = "Player";

		

		

		// Create a graphic context based on our desired traits
		htc_vive_gc = osg::GraphicsContext::createGraphicsContext(traits);

		if (!htc_vive_gc)
		{
			std::cout << "Error, GraphicsWindow has not been created successfully" << std::endl;
			//return;
		}

		if (htc_vive_gc.valid())
		{
			htc_vive_gc->setClearColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
			htc_vive_gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		//GraphicsWindowViewer viewer(arguments, dynamic_cast<osgViewer::GraphicsWindow*>(gc.get()));
		//std::cout << "HTC VIVE 2" << std::endl;
		// Force single threaded to make sure that no other thread can use the GL context
		viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
		viewer->getCamera()->setGraphicsContext(htc_vive_gc);
		viewer->getCamera()->setViewport(0, 0, traits->width, traits->height);

		// Disable automatic computation of near and far plane
		viewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		//viewer->setCameraManipulator(cameraManipulator);

		// Things to do when viewer is realized
		openvrRealizeOperation = new OpenVRRealizeOperation(openvrDevice);
		viewer->setRealizeOperation(openvrRealizeOperation.get());

		
		//std::cout << "HTC VIVE 3" << std::endl;

		// Add statistics handler
		//viewer.addEventHandler(new osgViewer::StatsHandler);
		//viewer.addEventHandler(new OpenVREventHandler(openvrDevice));
		#endif

		#ifdef WIN32
			// Get windows handle for viewer window and set the viewer window icon 
			HWND hWnd = FindWindow(NULL,"Player" ); 
			HANDLE hIcon = (HICON) LoadImage(GetModuleHandle(NULL), "IDI_ICON1", IMAGE_ICON, 32, 32, 0);
			if( hIcon && hWnd ) 
			{ 
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon); 
			}	
		#endif

		

	}



	if(m_htc_vive_enable == false)
	{
		//viewer->realize();
	}
	
	
	////////////////////

	//viewer->setRealizeOperation( new CompileStateSets( lightSource0 ) );
	//
	viewer->realize();
	//std::cout << "GL ver. " << osg::getGLVersionNumber () << std::endl;
	//viewer->setLightingMode(osg::View::SKY_LIGHT);
	//мышь в центр
	viewer->requestWarpPointer (0,0);
	viewer->getCamera()->setName("__MAINCAMERA__");
	// enable the use of compile contexts and associated threads.
    //osg::DisplaySettings::instance()->setCompileContextsHint(true);
	//viewer->setLightingMode(osg::View::SKY_LIGHT);
	//viewer->setLightingMode(osg::View::HEADLIGHT); //osg::View::SKY_LIGHT | 
	//узнаем размеры экрана
	osgViewer::Viewer::Windows windows;
	viewer->getWindows(windows);
	osgViewer::Viewer::Windows::iterator itr = windows.begin();
	int tempx,tempy;
	(*itr)->getWindowRectangle(tempx, tempy, WINDOW_WIDTH, WINDOW_HEIGHT);

	//std::cout << "HTC VIVE 4" << WINDOW_WIDTH << "   " << WINDOW_HEIGHT << std::endl;



	////////////////////
	if(m_htc_vive_enable == true)
	{
		/*
		traits->x = 50;
		traits->y = 50;
		traits->width = 800;
		traits->height = 450;
		*/
		// По идее нужно сообщить разрешение
		//WINDOW_WIDTH = 800;
		//WINDOW_HEIGHT = 450;

	}

	//add moution blur
	//double persistence = 0.25;
	//(*itr)->add(new MotionBlurOperation(persistence));

	//std::cout << "HTC VIVE 4" << std::endl;



}

//setCompileOnNextDraw
void OutputVideoClass::setCompileOnNextDraw()
{
    //viewer->getCamera()->getRenderer()->setCompileOnNextDraw(true);
    osg::ref_ptr<osgViewer::Renderer> rendering = dynamic_cast<osgViewer::Renderer*>(viewer->getCamera()->getRenderer());
    rendering->setCompileOnNextDraw(true);
}

//полное кеширование уже подключенных к viewer'у данных
//http://www.mail-archive.com/osg-users@lists.openscenegraph.org/msg43356.html
//
void OutputVideoClass::FullCompileScene(bool full)
{
    //#define ENABLE_WORKAROUND
    if (full==true) viewer->stopThreading();

    osg::GraphicsContext* gc = viewer->getCamera()->getGraphicsContext();
    if (gc)
    {
        osg::GraphicsContext::incrementContextIDUsageCount(gc->getState()->getContextID());

        if (full==true)
        {
            gc->makeCurrent();
            osgUtil::GLObjectsVisitor glov;
            glov.setState(gc->getState());
            //root
            viewer->getSceneData()->accept(glov);
            gc->releaseContext();
        }
    }

    if (full==true) viewer->startThreading();
}

//
OutputVideoClass::~OutputVideoClass()
{
	RestoreResolution();
	std::cout << "Destructor of Video System" << std::endl;

	////////////////////
	if(m_htc_vive_enable == true)
	{
		#ifdef WIN32
		// Need to do this here to make it happen before destruction of the OSG Viewer, which destroys the OpenGL context.
		openvrDevice->shutdown(htc_vive_gc.get());
		#endif
	}


}

//установка фокуса глаз пользователя (фокусное расстояние, мм)
void OutputVideoClass::setFocusLight(float focuslight)
{
	if ((m_oculus_enable)||(m_htc_vive_enable)) return;
       
	{
        osgViewer::Viewer::Windows windows;
        viewer->getWindows(windows);
        osgViewer::Viewer::Windows::iterator itr = windows.begin();
        int tempx,tempy;
        (*itr)->getWindowRectangle(tempx, tempy, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

	viewer->getCamera()->setProjectionMatrixAsPerspective (focuslight,(float)WINDOW_WIDTH/(float)WINDOW_HEIGHT,0.3f,5000.0f);
}

//установка курсора в окнах
void OutputVideoClass::setCursor (bool value)
{
	//выключаем курсор

	viewer->getWindows(windows);
	for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
		itr != windows.end();
			++itr)
		{
                    //(*itr)->useCursor(value);
                    if (value == true) (*itr)->setCursor(osgViewer::GraphicsWindow::RightArrowCursor);
                    if (value == false) (*itr)->setCursor(osgViewer::GraphicsWindow::NoCursor);
		}
	return;

}

//возвращает указатель на объект viewer
osg::ref_ptr<osgViewer::Viewer> OutputVideoClass::getViewer()
{
	return viewer;
}

//передает указатель на 3D данные сцены...
void OutputVideoClass::setDataToVideoViewer(osg::ref_ptr<osg::Node> root, osg::Node* menu_node, osg::Node* cursor2d_node)
{
	/*
	if(m_enable_ppu)
	{
		try
		{
			m_postProcessing.reset();
			m_postProcessing.reset(new PostProcessing("ppu/main.ppu", viewer, root));
		}
		catch(const ErrorClass&)
		{
			osg::notify(osg::WARN) << "Couldn't create PPU-pipeline" << std::endl;
		}
	}
	*/

	createHUD(root);

	/*
	if(m_oculus_enable)
	{
		m_oculus_config.reset(new OculusConfig(viewer,root,menu_node,cursor2d_node));
	}
	*/
	
	if(m_htc_vive_enable)
	{
		#ifdef WIN32
		std::cout << "HTC VIVE 11" << std::endl;
		osg::ref_ptr<OpenVRViewer> openvrViewer = new OpenVRViewer(viewer, openvrDevice, openvrRealizeOperation);
		openvrViewer->addChild(root);
		viewer->setSceneData(openvrViewer);
		std::cout << "HTC VIVE 22" << std::endl;
		#endif
	}

	if ((m_oculus_enable==false)&&(m_htc_vive_enable==false))
	{
		viewer->setSceneData(root.get());
	}
	setCursor(false);
}

void OutputVideoClass::createHUD(osg::ref_ptr<osg::Node> root)
{
	updateText = new osgText::Text;
	updateText->setFontResolution(14,14);
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setName("__DEBUG_CAMERA__");
	 // set the projection matrix
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0,WINDOW_WIDTH,0,WINDOW_HEIGHT));
    // set the view matrix    
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    // only clear the depth buffer
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    // draw subgraph after main camera view.
    camera->setRenderOrder(osg::Camera::POST_RENDER,0);
    std::string timesFont("fonts/verdana.ttf");
    // turn lighting off for the text and disable depth test to ensure its always ontop.
    osg::Vec3 position(WINDOW_WIDTH*0.7,WINDOW_HEIGHT*0.45,-89000.0f);
    // отображение пересеченных курсором объектов
    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
        geode->setName("The text label");
        geode->addDrawable( updateText.get() );
		camera->addChild(geode.get());
        updateText->setCharacterSize(28.0f*WINDOW_HEIGHT/1200);
		//updateText->setLineSpacing (0.10f/WINDOW_HEIGHT*1200);
		//updateText->setBackdropImplementation (osgText::Text::BackdropImplementation::STENCIL_BUFFER);
		//updateText->setBackdropType(osgText::Text::BackdropType::DROP_SHADOW_CENTER_LEFT);
		//updateText->setBackdropColor (osg::Vec4(1.0f,1.0f,1.0f,0.5f));
        updateText->setFont(timesFont);
        updateText->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
        updateText->setText("Gammer MAX");
        updateText->setPosition(position);
    } 
    root->asGroup()->addChild(camera.get());
}

//изменение текста в служебном HUD
void OutputVideoClass::SetHUDText(std::string newtext)
{
	 if(updateText) updateText->setText(newtext);
}


///////////////////////////////////////////////////////////////
void OutputVideoClass::ChangeResolution ( int Width , int Height , int Frequency ) 
{
#ifdef WIN32
	//#if (_MSC_VER < 1700)
    DEVMODE dmScreenSettings;  // Device Mode
    if ( 0 == EnumDisplaySettings ( 
                NULL , ENUM_CURRENT_SETTINGS , & dmScreenSettings ) ) 
    { 
		std::cout << "Failed to retreive screen settings." << std::endl;
        return ; 
    } 
    ChangedSettings = false ; 
    dmScreenSettings.dmFields = 0 ;
    // Change Display refresh rate if StereoVision 
    if ( ( DoStereoVision ) && 
         ( dmScreenSettings.dmDisplayFrequency != Frequency ) ) 
    { 
        ChangedSettings = true ; 
    	  dmScreenSettings.dmDisplayFrequency = Frequency ;
        dmScreenSettings.dmFields = DM_DISPLAYFREQUENCY ;
    } 
    if ( ( dmScreenSettings.dmPelsWidth  != Width ) ||
         ( dmScreenSettings.dmPelsHeight != Height ) )
    { 
        ChangedSettings = true ; 
	  dmScreenSettings.dmPelsWidth  = Width ;  // Screen Width
	  dmScreenSettings.dmPelsHeight = Height ; // Screen Height
	  dmScreenSettings.dmFields  |= DM_PELSWIDTH | DM_PELSHEIGHT ;
    } 
    if ( ChangedSettings ) 
    {
	// Try To Set Selected Mode And Get Results.  
      // NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
    	if ( DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings ( & dmScreenSettings, CDS_FULLSCREEN ) )
	{
         std::cout << "Failed to change settings and/or frequency."  << std::endl;
	} 
    } 
    else  
		std::cout << "No change to screen settings needed." << std::endl ;  
	//#endif
#endif
} 

//Code to restore the screen settings is much simpler, as shown below: 

void OutputVideoClass::RestoreResolution ( ) 
{
#ifdef WIN32
	//#if (_MSC_VER < 1700)
    if ( ChangedSettings ) 
    	 ChangeDisplaySettings ( NULL, 0 ) ; 
	//#endif
#endif
} 


//делает screenshot
void OutputVideoClass::takeScreenshot(std::string filename)
{
	osg::ref_ptr<SnapImageDrawCallback> snapImageDrawCallback = dynamic_cast<SnapImageDrawCallback*> (viewer->getCamera()->getPostDrawCallback());
	if(snapImageDrawCallback.get()) 
	{
		std::cout << "make screenshot" << std::endl;
		snapImageDrawCallback->setFileName(filename);
		snapImageDrawCallback->setSnapImageOnNextFrame(true);
	}
	else
	{
		std::cout << "Warning: no make screenshot" << std::endl;
	}
}


//////////////////////////////////////////////////////////////

/// обновить
void OutputVideoClass::update()
{
	//if(m_postProcessing.get())	m_postProcessing->update();
}







class MotionBlurOperation: public osg::Operation
{
public:
    MotionBlurOperation(double persistence):
        osg::Operation("MotionBlur",true),
        cleared_(false),
        persistence_(persistence)
    {
    }

    virtual void operator () (osg::Object* object)
    {
        osg::GraphicsContext* gc = dynamic_cast<osg::GraphicsContext*>(object);
        if (!gc) return;
    
        double t = gc->getState()->getFrameStamp()->getSimulationTime();

        if (!cleared_)
        {
            // clear the accumulation buffer
            glClearColor(0, 0, 0, 0);
            glClear(GL_ACCUM_BUFFER_BIT);
            cleared_ = true;
            t0_ = t;
        }

        double dt = fabs(t - t0_);
        t0_ = t;

        // compute the blur factor
        double s = powf(0.2, dt / persistence_);

        // scale, accumulate and return
        glAccum(GL_MULT, s);
        glAccum(GL_ACCUM, 1 - s);
        glAccum(GL_RETURN, 1.0f);
    }

private:
    bool cleared_;
    double t0_;
    double persistence_;
};
