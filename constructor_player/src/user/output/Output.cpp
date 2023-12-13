/***************************************************************************
Output.cpp  -  класс всего вывода локального пользователя
-------------------
begin                : 10 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
// 
//

#include "Output.h"

//конструктор
UserOutputClass::UserOutputClass( Configure* configuration, osg::ref_ptr<osgViewer::Viewer> viewer)
{
	//инициализация графического вывода
	VideoOutput = new OutputVideoClass(configuration, viewer);
	AudioOutput = new OutputAudioClass(configuration);
}

UserOutputClass::~UserOutputClass()
{
	delete AudioOutput;
	delete VideoOutput;
}

//возврат указателя на viewer
osg::ref_ptr<osgViewer::Viewer> UserOutputClass::getViewer()
{
	return VideoOutput->getViewer();
}
//
void UserOutputClass::setUserFocusLight(float focus)
{
	VideoOutput->setFocusLight (focus);
}
//
void UserOutputClass::setDataToVideoViewer(osg::ref_ptr<osg::Node> root, osg::Node* menu_node, osg::Node* cursor2d_node)
{
	VideoOutput->setDataToVideoViewer (root, menu_node, cursor2d_node);
}

//Изменение текста спец. HUD
void UserOutputClass::SetHUDText(std::string text)
{
	VideoOutput->SetHUDText (text);
}

//update audio 
void UserOutputClass::UpdateAudio(float posx,float posy, float posz, float Yaw, float Pitch, float Roll)
{
	AudioOutput->Update(posx, posy,  posz,  Yaw,  Pitch,  Roll);
	VideoOutput->update();
}


//screenshot
void UserOutputClass::takeScreenshot(std::string filaneme)
{
	VideoOutput->takeScreenshot(filaneme);
}

//setCompileOnNextDraw
void UserOutputClass::setCompileOnNextDraw()
{
    VideoOutput->setCompileOnNextDraw();
}

//полное кеширование уже подключенных к viewer'у данных
void UserOutputClass::FullCompileScene(bool full)
{
    VideoOutput->FullCompileScene(full);
}


