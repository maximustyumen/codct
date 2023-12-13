#ifndef OUTVIDEOCLASS
#define OUTVIDEOCLASS


#include <stdlib.h>
#include <stdio.h> 
#include <iostream>
#include <osg/Version>
#include <osg/GLExtensions>
#include <osgViewer/Viewer>
#include <osgViewer/Renderer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/DatabasePager>

#include <osgText/Font>
#include <osgText/Text>
#include <osg/LightModel>
//#include <osg/CameraNode>
#include <osgDB/ReadFile>

#include "../../Configure/Configure.h"

//#if (_MSC_VER < 1700)
//#include "../input/vr/emagin_z800/emagin_z800.h"
//#endif

//#include "../output/osgoculusviewer/oculusViewConfig.h"

//#include "../output/PostProcessing.h"

#include "../../VR/osgOpenVR/openvrviewer.h"

//class OculusConfig;

class OutputVideoClass
{
public:
	OutputVideoClass(Configure* configuration, osg::ref_ptr<osgViewer::Viewer> viewer =0);
	~OutputVideoClass();
	void setCursor (bool);
	osg::ref_ptr<osgViewer::Viewer> getViewer();
	void setFocusLight(float focuslight=50.0f);
	void setDataToVideoViewer(osg::ref_ptr<osg::Node> _root, osg::Node* menu_node, osg::Node* cursor2d_node);
	
	void ChangeResolution ( int Width , int Height , int Frequency );
	
	//изменение текста в служебном HUD
	void SetHUDText(std::string);

	//screenshot
	void takeScreenshot(std::string);

    //setCompileOnNextDraw
    void setCompileOnNextDraw();
    //полное кеширование уже подключенных к viewer'у данных
    void FullCompileScene(bool);

	void update();

	osg::ref_ptr<OpenVRDevice> GetOpenvrDevice () 
	{
		return openvrDevice;
	}
	

private:
    //std::auto_ptr<PostProcessing> m_postProcessing;
	//bool m_enable_ppu;

	osg::ref_ptr<osgViewer::Viewer>  viewer;
	osgViewer::Viewer::Windows windows;
	//osg::ref_ptr<OculusViewConfig> oculusViewConfig;
	//std::auto_ptr<OculusConfig> m_oculus_config;

	//
	void RestoreResolution ();
	//
	bool ChangedSettings;
	bool DoStereoVision;

	//ширина и высота экрана
	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;

	bool m_oculus_enable;
	bool m_htc_vive_enable;

	//служебный HUD
	void createHUD(osg::ref_ptr<osg::Node> root);
	osg::ref_ptr<osgText::Text> updateText;


	//htc
	osg::ref_ptr<OpenVRDevice> openvrDevice;
	osg::ref_ptr<OpenVRRealizeOperation> openvrRealizeOperation;
	osg::ref_ptr<osg::GraphicsContext> htc_vive_gc;


	//dpi fix
	void PseudoFullScreenMode();

};

#endif

