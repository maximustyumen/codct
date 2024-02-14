#ifndef OUTSOUNDCLASS
#define OUTSOUNDCLASS

#include "../../Configure/Configure.h"

#include "OutputVideo.h"
#include "OutputAudio.h"

class UserOutputClass
{
public:
	UserOutputClass(Configure* configuration, osg::ref_ptr<osgViewer::Viewer> viewer = 0);
	~UserOutputClass();
	
	//возвращаем указатель на viewer (const)
	osg::ref_ptr<osgViewer::Viewer> getViewer();

	//установка фокусного расстояния камеры - наблядателя
	void setUserFocusLight(float focus=50.0f);

	//Передача, что визуализировать на камере - наблюдателе
	void setDataToVideoViewer(osg::ref_ptr<osg::Node>, osg::Node* menu_node, osg::Node* cursor2d_node);

	//Изменение текста спец. HUD
	void SetHUDText(std::string);

	//update audio 
	void UpdateAudio(float posx,float posy, float posz, float Yaw, float Pitch, float Roll);

	//screenshot
	void takeScreenshot(std::string);

    //setCompileOnNextDraw
    void setCompileOnNextDraw();
    //полное кеширование уже подключенных к viewer'у данных
    void FullCompileScene(bool);

	#ifdef WIN32
	osg::ref_ptr<OpenVRDevice> GetOpenvrDevice () 
	{
		return VideoOutput->GetOpenvrDevice();
	}
	#endif

private:
	OutputVideoClass* VideoOutput;
	OutputAudioClass* AudioOutput;
};

#endif

